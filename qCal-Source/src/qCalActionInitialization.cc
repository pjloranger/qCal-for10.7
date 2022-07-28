#include "qCalActionInitialization.hh"
#include "qCalDetectorConstruction.hh"
#include "qCalPrimaryGeneratorAction.hh"
#include "qCalEventAction.hh"
#include "qCalRunAction.hh"

#include "G4ThreeVector.hh"
#include "G4RunManager.hh"

//constructor
qCalActionInitialization::qCalActionInitialization(G4String sParticle, G4int nEnergy, G4double fZPosition, G4double fCubeSize)
: G4VUserActionInitialization()
{
	p_fZPosition = fZPosition;
	p_fCubeSize = fCubeSize/2;
	startingParticle = sParticle;
	startingEnergy = nEnergy;

}

//destructor
qCalActionInitialization::~qCalActionInitialization() = default;

//Build method
void qCalActionInitialization::Build()const
{
	G4ThreeVector gunMomentum = G4ThreeVector(0, 0, -1);
	
	//initialize a new PrimaryGeneratorAction
	SetUserAction(new qCalPrimaryGeneratorAction(startingParticle, startingEnergy, p_fZPosition, p_fCubeSize, gunMomentum));

	//initialize/set eventAction and runAction?
	auto eventAction = new qCalEventAction;
	SetUserAction(eventAction);
	SetUserAction(new qCalRunAction(eventAction));
	

}

void qCalActionInitialization::BuildForMaster() const
{
	
	auto eventAction = new qCalEventAction;
	SetUserAction(new qCalRunAction(eventAction));
	
}

