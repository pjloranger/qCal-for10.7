#ifndef qCalPrimaryGeneratorAction_h
#define qCalPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "qCalDetectorConstruction.hh"


class G4ParticleDefinition;
class G4Event;
class G4ParticleGun;

class qCalPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
   qCalPrimaryGeneratorAction(
      const G4String& particleName = "mu-",
      G4double energy = 120.*GeV,
      G4double zPosition = 0,
      G4double cubeSize = 1*cm,
      G4ThreeVector momentumDirection = G4ThreeVector(0,0,1));
   
   virtual ~qCalPrimaryGeneratorAction();
   virtual void GeneratePrimaries(G4Event*);
   
private:
   G4ParticleGun* gun;
   qCalDetectorConstruction* eventDetector;
   G4double p_fZPosition;
   G4double p_fCubeSize;
    
};
#endif

