#include "qCalDetectorConstruction.hh"
#include "qCalSD.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include <vector>
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4OpticalSurface.hh"
#include "G4Material.hh"
#include "G4VSensitiveDetector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"


qCalDetectorConstruction::qCalDetectorConstruction(G4int nXAxis,                                        //number of cubes in the X-Axis
                                                   G4int nYAxis,                                        //number of cubes in the Y-Axis
                                                   G4int nZAxis,                                        //number of cubes in the Z-Axis
                                                   G4String sAbs,                                       //absorber Periodic Table symbol
                                                   G4double fAbsLen,                                    //absorber radiation length
                                                   G4double fCubeWidth,                                 //width of a single cube
                                                   G4double fCubeDepth,                                 //depth of a single cube
                                                   G4double fSiPMWidth,                                 //width of the SiPM
                                                   G4double fSiPMDepth)                                 //depth of the SiPM

      :G4VUserDetectorConstruction()
{
   p_nXAxis = nXAxis;                                                                                   //number of cubes in the X-Axis
   p_nYAxis = nYAxis;                                                                                   //number of cubes in the Y-Axis
   p_nZAxis = nZAxis;                                                                                   //number of cubes in the Z-Axis
  
   p_fAbsLen = (fAbsLen/2);                                                                             //1/2 length of the absorber plate in Z direction
   p_sAbs = sAbs;                                                                                       //absorber element
  
   p_fCubeWidth = (fCubeWidth)/2;                                                                       //1/2 width of quartz cube
   p_fCubeDepth = (fCubeDepth)/2;                                                                       //1/2 depth of quartz cube
   
   p_fWrapSize = 0.025*cm;                                                                              //width of the tyvek wrapping

   p_fSiPMWidth = (fSiPMWidth)/2;                                                                       //1/2 width of SiPM
   p_fSiPMDepth = (fSiPMDepth)/2;                                                                       //1/2 depth of SiPM

   p_fOffset = 2 * cm;                                                                                  //how much world volume is offset from detector, used in p_fWorldX, Y, and Z

   p_fWorldX = ((fCubeWidth / 2 + p_fWrapSize) * nXAxis) + p_fOffset;                                   //1/2 size of world volume in X direction
   p_fWorldY = ((fCubeWidth / 2 + p_fWrapSize) * nYAxis) + p_fOffset;                                   //1/2 size of world volume in Y direction
   p_fWorldZ = ((fCubeDepth / 2 + p_fWrapSize + fSiPMDepth / 2 + fAbsLen / 2) * nZAxis) + p_fOffset;    //1/2 size of world volume in Z direction

   p_fTotalCubeDepth = (fCubeDepth / 2 + p_fWrapSize + fSiPMDepth / 2);                                 //1/2 depth of the total cube (quartz + SiPM + tyvek wrapping)
   p_fTotalCubeWidth = (fCubeWidth / 2 + p_fWrapSize);                                                  //1/2 width of the total cube (quartz + SiPM + tyvek wrapping)

   p_fscaleZ = (p_fAbsLen + p_fTotalCubeWidth)/cm;                                                      //used in rawcordstoSiPMnumber method

   negHalfDetSize = ((fCubeDepth / 2 + p_fWrapSize + fSiPMDepth / 2 + fAbsLen / 2) * nZAxis);           //half the depth of the detector? used in setting the Z coord of the particle gun

   G4cout << "************************************" << G4endl;
   G4cout << "***    Detector Construction     ***" << G4endl;
   G4cout << "************************************" << G4endl;
   G4cout << "World volume is " << p_fWorldX*2 / cm << " cm * " << p_fWorldY*2 / cm << " cm * " << p_fWorldZ*2 / cm << " cm" << G4endl;
 
   G4cout << "************************************" << G4endl;
   G4cout << "************************************" << G4endl;

}

qCalDetectorConstruction::~qCalDetectorConstruction() = default;
//{}

G4VPhysicalVolume* qCalDetectorConstruction::Construct()
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Creating the materials and initial values
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Get nist material manager
   G4NistManager* nist = G4NistManager::Instance();

   // Option to switch on/off checking of volumes overlaps
   const G4bool checkOverlaps = true;

   //Creating The Materials for Quartz, and the air surrounding it.
   G4double atomicNumber, atomicWeight, density;
   G4int nElements, nAtoms;

   //Absorber Material
   const G4int nEntries = 32;

   //Photon Energies for Optical Photon calculations
   G4double PhotonEnergyQ[nEntries] = {
         0.44*eV, 0.623125*eV, 0.80625*eV, 0.989375*eV,
         1.1725*eV, 1.355625*eV, 1.53875*eV, 1.721875*eV,
         1.905*eV, 2.088125*eV, 2.27125*eV, 2.454375*eV,
         2.6375*eV, 2.820625*eV, 3.00375*eV, 3.186875*eV,
         3.37*eV, 3.553125*eV, 3.73625*eV, 3.919375*eV,
         4.1025*eV, 4.285625*eV, 4.46875*eV, 4.651875*eV,
         4.835*eV, 5.018125*eV, 5.20125*eV, 5.384375*eV,
         5.5675*eV, 5.750625*eV, 5.93375*eV, 6.3*eV };

   G4Element* N   = new G4Element("Nitrogen",   "N",  atomicNumber = 7,    atomicWeight = ((14.01*g) / mole));
   G4Element* O   = new G4Element("Oxygen",     "O",  atomicNumber = 8,    atomicWeight = ((16.00*g) / mole));
   G4Element* Ar  = new G4Element("Argon",      "Ar", atomicNumber = 18,   atomicWeight = ((39.95*g) / mole));
   G4Element* Si  = new G4Element("Silicon",    "Si", atomicNumber = 14,   atomicWeight = ((28.09*g) / mole));

   //Define Air Material
   G4Material* airMat = new G4Material("Air", density = ((1.29*mg) / cm3), nElements = 3);
   airMat->AddElement(N, 78.08*perCent);
   airMat->AddElement(O, 20.95*perCent);
   airMat->AddElement(Ar, 0.97*perCent);
   G4double RefractiveIndexAir[nEntries] =
         { 1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293,
           1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293,
           1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293,
           1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293, 1.000293,
           1.000293, 1.000293, 1.000293, 1.000293 };

   G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
   airMPT->AddProperty("RINDEX", PhotonEnergyQ, RefractiveIndexAir, nEntries);
   airMat->SetMaterialPropertiesTable(airMPT);

   //Define Quartz Material
   G4Material* quartzMat = new G4Material("quartzCrystal", density = ((2.648*g) / cm3), nElements = 2);
   quartzMat->AddElement(Si, nAtoms = 1);

   quartzMat->AddElement(O, nAtoms = 4);

   G4double RefractiveIndexQ[nEntries] =
         { 1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585,
           1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585,
           1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585,
           1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585, 1.4585,
           1.4585, 1.4585, 1.4585, 1.4585 };

   G4double AbsorptionQ[nEntries] =
         { 0.07*m, 0.069375*m, 0.06875*m, 0.068125*m,
           0.0675*m, 0.066875*m, 0.06625*m, 0.065625*m,
           0.065*m, 0.064375*m,  0.06375*m, 0.063125*m,
           0.0625*m, 0.061875*m, 0.06125*m, 0.060625*m,
           0.06*m, 0.059375*m, 0.05875*m, 0.058125*m,
           0.0575*m, 0.056875*m, 0.05625*m, 0.055625*m,
           0.055*m, 0.054375*m, 0.05375*m, 0.053125*m,
           0.0525*m, 0.051875*m, 0.05125*m, 0.05*m };

   G4MaterialPropertiesTable* quartzMPT = new G4MaterialPropertiesTable();
   quartzMPT->AddProperty("RINDEX", PhotonEnergyQ, RefractiveIndexQ, nEntries);
   quartzMPT->AddProperty("ABSLENGTH", PhotonEnergyQ, AbsorptionQ, nEntries);
   quartzMat->SetMaterialPropertiesTable(quartzMPT);

   //Define the User selected Absorber Material
   G4Material* absMat = nist->FindOrBuildMaterial("G4_" + p_sAbs);

   G4Material* sipmMat           = quartzMat;

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Define the world (needs full detector + full absorber + extra space)
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //declare solid volume for the world
   G4Box* solidWorld = new G4Box("World",       //name
                                 p_fWorldX,     //half size in X direction
                                 p_fWorldY,     //half size in Y direction
                                 p_fWorldZ);    //half size in Z direction

   //declare logical volume for the world
   G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,    //solid volume
                                                     airMat,        //material of volume
                                                     "World");      //name
   //place the world
   G4VPhysicalVolume* physWorld = new G4PVPlacement(nullptr,           //no rotation
                                                    G4ThreeVector(),   //at (0,0,0)
                                                    logicWorld,        //its logical volume
                                                    "World",           //its name
                                                    nullptr,           //its mother  volume
                                                    false,             //no boolean operation
                                                    0,                 //copy number
                                                    checkOverlaps);    //overlaps checking

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Create the Quartz
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //declare solid volume of the quartz cube
   G4Box* solidQuartz = new G4Box("solidQuartz",    //name
                                  p_fCubeWidth,     //half size in X direction
                                  p_fCubeWidth,     //half size in Y direction
                                  p_fCubeDepth);    //half size in Z direction

   //declare logical volume of the quartz cube
   G4LogicalVolume* logicQuartz = new G4LogicalVolume(solidQuartz,      //solid volume
                                                      quartzMat,        //material of quartz cube
                                                      "logicQuartz");   //name

   //placement coordinates for the quartz cube, will be used to place it in the larger detector volume
   G4double quartzZCoord = 0 + (p_fTotalCubeDepth - (p_fCubeDepth + p_fWrapSize));
   G4ThreeVector quartzPos = G4ThreeVector(0,0,quartzZCoord);

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Create the SiPM
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //declare solid volume of the SiPM
   G4Box* solidSiPM = new G4Box("solidSiPM",    //name
                                p_fSiPMWidth,   //half size in X direction 
                                p_fSiPMWidth,   //half size in Y direction
                                p_fSiPMDepth);  //half size in Z direction

   //declare logical volume of the SiPM
   //for some reason, the G4LogicalVolume logicSiPM is declared in qCalDetectorConstruction.hh
   logicSiPM = new G4LogicalVolume(solidSiPM,       //corresponding solid volume
                                   sipmMat,         //material of SiPM
                                   "logicSiPM");    //name

   //placement coordinates for the quartz cube, will be used to place it in the larger detector volume
   G4double SiPMZCoord = 0 - (p_fTotalCubeDepth - (p_fSiPMDepth));
   G4ThreeVector SiPMPos = G4ThreeVector(0,0,SiPMZCoord);

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Define the whole detector(tyvec wrapping,quartz,SiPM,backing of PMT) - "Pixel"
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //declare solid volume of a single detector cube (encompases tyvek wrapping, SiPM & quartz cube
   G4Box* solidDetector = new G4Box("SingleDetectorTest",     //name
                                    p_fTotalCubeWidth,        //half size in X direction
                                    p_fTotalCubeWidth,        //half size in Y direction
                                    p_fTotalCubeDepth);       //half size in Z direction
 
   //declare solid volume of a set of detector cubes in the X direction, will be used for replicating the solidDetector
   G4Box* solidDetectorX = new G4Box("DetectorXLayer",              //name
                                     p_fTotalCubeWidth*p_nXAxis,    //half size in X direction
                                     p_fTotalCubeWidth,             //half size in Y direction
                                     p_fTotalCubeDepth);            //half size in Z direction
  
   //declare solid volume of a 'sheet' of detector cubes in both the X and Y directions, will be used for replicating the solidDetector
   G4Box* solidDetectorXY = new G4Box("DetectorXYLayer",            //name
                                      p_fTotalCubeWidth*p_nXAxis,   //half size in X direction
                                      p_fTotalCubeWidth*p_nYAxis,   //half size in Y direction
                                      p_fTotalCubeDepth);           //half size in Z direction

   //declaring logical volumes for the detector volumes
   G4LogicalVolume* logicDetector   = new G4LogicalVolume(solidDetector,   airMat, "logicDetector");
   G4LogicalVolume* logicDetectorX  = new G4LogicalVolume(solidDetectorX,  airMat, "logicDetectorX");
   G4LogicalVolume* logicDetectorXY = new G4LogicalVolume(solidDetectorXY, airMat, "logicDetectorXY");

   //placement of the quartz cube within the solidDetector volume
   G4VPhysicalVolume* quartzPlace = new G4PVPlacement(nullptr,             //no rotation
                                                      quartzPos,           //at (0,0,0)
                                                      logicQuartz,         //its logical volume
                                                      "quartzOfDetector",  //its name
                                                      logicDetector,       //its mother  volume
                                                      false,               //no boolean operation
                                                      2,                   //copy number
                                                      checkOverlaps);      //overlaps checking

   //placement of the SiPM within the solidDetector volume
   //for some reason, the placement of the SiPM doesn't need to declare an associated G4VPhysicalVolume
   new G4PVPlacement(nullptr,                //no rotation
                     SiPMPos,                //at (0,0,0)
                     logicSiPM,              //its logical volume
                     "sipmOfDetector",       //its name
                     logicDetector,          //its mother  volume
                     false,                  //no boolean operation
                     3,                      //copy number
                     checkOverlaps);         //overlaps checking

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Replicate pixel in XDir
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //replicates the single detector cube in the X direction
   new G4PVReplica("detectorLayerX",        //Name
                   logicDetector,           //Logical Volume
                   logicDetectorX,          //Mother volume
                   kXAxis,                  //Axis of replication
                   (p_nXAxis),              //Number of replica
                   p_fTotalCubeWidth*2);    //Width of replica

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Replicate pixel in XYDir
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //replicates the set of detector cubes already replicate din the X direction in the Y direction
   new G4PVReplica("detecLayerXY",           //Name
                   logicDetectorX,           //Logical Volume
                   logicDetectorXY,          //Mother volume
                   kYAxis,                   //Axis of replication
                   (p_nYAxis),               //Number of replica
                   p_fTotalCubeWidth*2);     //Width of replica

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Create an absorber behind the XYDir pixel
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //declare the solid volume of an absorber plate, will be placed in the solidFinalAbsorber
   G4Box* solidAbsorber = new G4Box("SingleAbsorber",                   //name
                                    (p_fTotalCubeWidth)*(p_nXAxis),     //half size in X direction
                                    (p_fTotalCubeWidth)*(p_nYAxis),     //half size in Y direction
                                    p_fAbsLen);                         //half size in Z direction

   //declare the solid volume of a single 'full layer' of the detector (absorber + quartz + SiPM + tyvek)
   //these variable names need to be changed
   G4Box* solidFinalAbsorber = new G4Box("FullAbs",                         //name
                                         (p_fTotalCubeWidth)*(p_nXAxis),    //half size in X direction
                                         (p_fTotalCubeWidth)*(p_nYAxis),    //half size in Y direction
                                         p_fTotalCubeDepth + p_fAbsLen);    //half size in Z direction

   //declare the solid volume of the full detector, will be used to replicte the solidFinalAbsorber in the Z direction
   G4Box* solidFullDetector = new G4Box("FullDetectorSolid",                            //name
                                        (p_fTotalCubeWidth) * (p_nXAxis),               //half size in X direction
                                        (p_fTotalCubeWidth) * (p_nYAxis),               //half size in Y direction
                                        (p_fTotalCubeDepth + p_fAbsLen) * p_nZAxis);    //half size in Z direction

   //declare the logical volume of the solidAbsorber, solidFinalAbsorber, and solidFullDetector
   G4LogicalVolume* logicAbsorber = new G4LogicalVolume(solidAbsorber, absMat, "logicAbsorber");
   G4LogicalVolume* logicFinal = new G4LogicalVolume(solidFinalAbsorber, airMat, "logicFinal");
   G4LogicalVolume* logicFullDetector = new G4LogicalVolume(solidFullDetector, airMat, "logicFullDetector");

   //placement coordinates for the absorber plate
   G4ThreeVector absPos = G4ThreeVector(0, 0, -((p_fTotalCubeDepth + p_fAbsLen) - p_fAbsLen));

   //placement coordinates for the detector cube (solidDetector)
   G4ThreeVector detPos = G4ThreeVector(0, 0, ((p_fTotalCubeDepth + p_fAbsLen) - p_fTotalCubeDepth));

   //placement of the absorber within the 'full layer' volume
   new G4PVPlacement(nullptr,               //no rotation
                     absPos,                //at (0,0,0)
                     logicAbsorber,         //its logical volume
                     "absorberOfDetector",  //its name
                     logicFinal,            //its mother  volume
                     false,                 //no boolean operation
                     4,                     //copy number
                     checkOverlaps);        //overlaps checking

   //placement of the detector 'sheet' in the 'full layer' volume
   new G4PVPlacement(nullptr,               //no rotation
                     detPos,                //at (0,0,0)
                     logicDetectorXY,       //its logical volume
                     "DetectorPortion",     //its name
                     logicFinal,            //its mother  volume
                     false,                 //no boolean operation
                     4,                     //copy number
                     checkOverlaps);        //overlaps checking

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Replicate pixel and absorber in XYZDir, and place it in the world
   ////////////////////////////////////////////////////////////////////////////////////////////////

   //replicates the 'full layer' volume in the Z direction through the entire detector volume
   new G4PVReplica("FullDetectorXYZ",                   //Name
                   logicFinal,                          //Logical Volume
                   logicFullDetector,                   //Mother volume
                   kZAxis,                              //Axis of replication
                   (p_nZAxis),                          //Number of replica
                   2*(p_fTotalCubeDepth + p_fAbsLen));  //Width of replica //+0.25

   //places the full detector volume in the world
   new G4PVPlacement(nullptr,                   //no rotation
                     G4ThreeVector(0, 0, 0),    //at (0,0,0)
                     logicFullDetector,         //its logical volume
                     "Final",                   //its name
                     logicWorld,                //its mother  volume
                     false,                     //no boolean operation
                     5,                         //copy number
                     checkOverlaps);            //overlaps checking

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //DEBUGGING: color/visible settings for quartz, sipm, and absorber to check for collisions
   ////////////////////////////////////////////////////////////////////////////////////////////////
   
   //declaring color values
   G4Color brown(0.7, 0.4, 0.1);
   G4Color red(1.0, 0.0, 0.0);
   G4Color blue(0.0, 0.0, 1.0);
   G4Color green(0.0, 1.0, 0.0);
   G4Color yellow(1.0, 1.0, 0.0);

   //G4VisAttributes* yellowColor = new G4VisAttributes(yellow);
   G4VisAttributes* redColor = new G4VisAttributes(red);
   G4VisAttributes* blueColor = new G4VisAttributes(blue);
   G4VisAttributes* greenColor = new G4VisAttributes(green);

   //set some containers to invisible so we dont see their outlines
   logicFinal->SetVisAttributes(new G4VisAttributes(false));
   logicDetector->SetVisAttributes(new G4VisAttributes(false));
   logicDetectorX->SetVisAttributes(new G4VisAttributes(false));
   logicDetectorXY->SetVisAttributes(new G4VisAttributes(false));
   logicFinal->SetVisAttributes(new G4VisAttributes(false));
   logicFullDetector->SetVisAttributes(new G4VisAttributes(false));

   //change the color of the outlines of some containers
   logicSiPM->SetVisAttributes(redColor);               //SiPMs are red
   logicFullDetector->SetVisAttributes(greenColor);     //the entire detector volume is green
   logicDetector->SetVisAttributes(blueColor);          //'detector cubes' (SiPM + tyvek + quartz) are blue

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //Set the Quartz Surface
   ////////////////////////////////////////////////////////////////////////////////////////////////
   G4OpticalSurface *quartzWrap = new G4OpticalSurface("QuartzWrap");
   //G4LogicalSkinSurface *quartzSurface = new G4LogicalSkinSurface("QuartzSurface", logicQuartz, quartzWrap);
   quartzWrap->SetType(dielectric_metal); 
   quartzWrap->SetModel(unified);
   quartzWrap->SetFinish(polishedtyvekair);
   //UnusedLogicalSkinSurface? Compiling mentions that quartzSurface is never used.

   new G4LogicalBorderSurface("QuartzWrap",
                              quartzPlace,
                              physWorld,
                              quartzWrap);

   G4double pp[2]             = { 2.0*eV, 3.5*eV };
   G4double reflectivity[2]   = { 1., 1. };
   G4double efficiency[2]     = { 0.0, 0.0 };

   G4MaterialPropertiesTable* quartzWrapMPT = new G4MaterialPropertiesTable();
   quartzWrapMPT->AddProperty("REFLECTIVITY",   pp, reflectivity, 2);
   quartzWrapMPT->AddProperty("EFFICIENCY",     pp, efficiency,   2);

   quartzWrap->SetMaterialPropertiesTable(quartzWrapMPT);

   //aaand done!
   return physWorld;
}


void qCalDetectorConstruction::ConstructSDandField()
{
    G4SDManager* sdMan = G4SDManager::GetSDMpointer();
    G4String SDname;

    G4VSensitiveDetector* sipm = new qCalSD(SDname = "/SiPM", p_fTotalCubeWidth, p_fTotalCubeDepth, p_fAbsLen);
    sdMan->AddNewDetector(sipm);
    logicSiPM->SetSensitiveDetector(sipm);
}
