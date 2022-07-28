#ifndef qCalDetectorConstruction_h
#define qCalDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"
#include "G4Cache.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

class G4PhysicalVolume;
class G4LogicalVolume;

class qCalDetectorConstruction : public G4VUserDetectorConstruction
{
public:
   qCalDetectorConstruction(G4int nXAxis = 1,
                            G4int nYAxis = 1,
                            G4int nZAxis = 1,
                            G4String sAbs = "Fe",
                            G4double fAbsLen = 1.0,
                            G4double fCubeWidth = 1.0,
                            G4double fQuartzDepth = 1.0,
                            G4double fPMTDepth = 1.0,
                            G4double fDetecWidth = 1.0
   );
   virtual ~qCalDetectorConstruction();

   virtual G4VPhysicalVolume* Construct();
   virtual void ConstructSDandField();

   G4double GetVolume() { return p_nXAxis * p_nYAxis * p_nZAxis; };
   G4int GetXAxis() { return nXAxis; };
   G4int GetYAxis() { return nYAxis; };
   G4int GetZAxis() { return nZAxis; };

private:
   G4int p_nXAxis;
   G4int p_nYAxis;
   G4int p_nZAxis;
   G4String p_sAbs;
   G4double p_fAbsLen;
   G4double p_fCubeWidth;
   G4double p_fCubeDepth;
   G4double p_fWorldX;
   G4double p_fWorldY;
   G4double p_fWorldZ;
   G4double p_fWrapSize;
   G4double p_fSiPMDepth;
   G4double p_fSiPMWidth;
   std::vector<G4double> p_vDetectorAxisValues;
   G4double p_fTotalCubeDepth;
   G4double p_fTotalCubeWidth;
   G4double p_fOffsetZ;
   G4double p_fOffset; //the other offset!!!!
   G4double p_fscaleZ;
   G4LogicalVolume* logicSiPM;
   G4double negHalfDetSize;
   std::vector<G4ThreeVector> fSiPMPositions;
};
#endif
