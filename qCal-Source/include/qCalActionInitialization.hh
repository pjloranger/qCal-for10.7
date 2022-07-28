#ifndef qCalActionInitialization_h
#define qCalActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "qCalDetectorConstruction.hh"

class qCalDetectorConstruction;
//class qCalRecorderBase;

class qCalActionInitialization : public G4VUserActionInitialization
{
public:
    qCalActionInitialization(G4String sParticle, G4int nEnergy, G4double fZPosition, G4double fCubeSize);
    virtual ~qCalActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

private:

   G4double p_fZPosition;
   G4double p_fCubeSize;
   G4String startingParticle;
   G4int startingEnergy;
};
#endif

