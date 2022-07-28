#include "G4VSensitiveDetector.hh"

#include "qCalSD.hh"
#include "qCalHit.hh"
#include "qCalDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include <map>


//Constructor
qCalSD::qCalSD(G4String SDname, G4double fTotalCubeWidth, G4double fTotalCubeDepth, G4double fAbsLen)
: G4VSensitiveDetector(SDname), fSiPMHitCollection(0),fSiPMPositionsX(0),fSiPMPositionsY(0),fSiPMPositionsZ(0) //is this declaring variables?
{
   collectionName.insert("SiPMHitCollection");
   p_fTotalCubeWidth = fTotalCubeWidth*2; //needs to be full length
   p_fTotalCubeDepth = fTotalCubeDepth*2;
   p_fAbsLen = fAbsLen*2;

}

//Destructor
qCalSD::~qCalSD() = default;
//{}
//Initialize (Required)
void qCalSD::Initialize(G4HCofThisEvent* hitsCE)
{
   fSiPMHitCollection = new qCalSiPMHitsCollection(SensitiveDetectorName, collectionName[0]);
   //Store collection with event and keep ID

   G4int hitCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

   hitsCE->AddHitsCollection( hitCID, fSiPMHitCollection );
}


//Process Hits (Required)
G4bool qCalSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
   //if there's no photon hits, return false
   if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
   {
      return false;
   }
   
   G4StepPoint* preStepPoint = step->GetPreStepPoint();
   //G4Track *aTrack = step->GetTrack(); //necessary?

   auto touchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());

   //gets the position of the hit as a vector
   G4ThreeVector copyNoPos = touchable->GetTranslation();

   G4double fTempX = copyNoPos.getX() / p_fTotalCubeWidth;
   G4double fTempY = copyNoPos.getY() / p_fTotalCubeWidth;
   G4double fTempZ = copyNoPos.getZ() / (p_fTotalCubeDepth + p_fAbsLen);

   //takes coordinate position of the hit and converts it to the position of the cube it was in
   G4double fCubeNumX = fTempX - fmod(fTempX, 1);
   G4double fCubeNumY = fTempY - fmod(fTempY, 1);
   G4double fCubeNumZ = fTempZ - fmod(fTempZ, 1);

   //argument variables for adding a new qCalHit later
   G4ThreeVector cubeVector = G4ThreeVector(fCubeNumX, fCubeNumY, fCubeNumZ);
   //THE SIPM NUMBER WILL BREAK IF NOT 4x4x4
   G4double SiPMNumber = (fCubeNumX + fCubeNumY * 4 + fCubeNumZ * 16); //takes cube position and turns it into SiPM id, counting the SiPMs layer-wise from the bottom left (in an X-Y plane)
   G4double hitTime              = preStepPoint->GetGlobalTime();
   G4double photonWavelength     = 4.15e-15*3e8/(step->GetTrack()->GetTotalEnergy()/eV)*1e9;
  

   qCalHit* hit = nullptr;

   //simulating the AFBR-S4N33C013 Broadcom
   //https://docs.broadcom.com/doc/AFBR-S4N33C013-DS

   G4int isHit = 0;
   if (photonWavelength >= 300 && photonWavelength < 350 && (rand() % 100) < 42.5) isHit = 1;
   else if (photonWavelength >= 350 && photonWavelength < 400 && (rand() % 100) < 47.5) isHit = 1;
   else if (photonWavelength >= 400 && photonWavelength < 450 && (rand() % 100) < 52.5) isHit = 1;
   else if (photonWavelength >= 450 && photonWavelength < 500 && (rand() % 100) < 45) isHit = 1;
   else if (photonWavelength >= 500 && photonWavelength < 550 && (rand() % 100) < 37.5) isHit = 1;
   else if (photonWavelength >= 550 && photonWavelength < 600 && (rand() % 100) < 32.5) isHit = 1;
   else if (photonWavelength >= 600 && photonWavelength < 700 && (rand() % 100) < 35) isHit = 1;
   else if (photonWavelength >= 700 && photonWavelength < 800 && (rand() % 100) < 15) isHit = 1;
   else if (photonWavelength >= 800 && photonWavelength < 900 && (rand() % 100) < 7.5) isHit = 1;

   if (isHit == 1)
   {
      hit = new qCalHit(cubeVector, SiPMNumber, hitTime, photonWavelength);
      fSiPMHitCollection->insert(hit);
      hit->SetDrawit(true);
      hit->IncPhotonCount();
   }
   //aTrack->SetTrackStatus(fStopAndKill); //necessary?

   return false;
}

void qCalSD::EndOfEvent(G4HCofThisEvent*) {
   
   //((qCalDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->SetCoordOffsetZ(p_offsetZ);


}

//Clear hit markings
void qCalSD::Clear()
{
   
}

//Draw hit markings
void qCalSD::DrawAll()
{
   
}

//Draw hit markings
void qCalSD::PrintAll()
{
   
}

