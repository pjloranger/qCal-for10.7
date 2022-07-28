#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"
#include "qCalDetectorConstruction.hh"

#include "qCalPrimaryGeneratorAction.hh"

qCalPrimaryGeneratorAction::qCalPrimaryGeneratorAction(const G4String& particleName,
                                                       G4double energy,
                                                       G4double fZPosition,
                                                       G4double fCubeSize,
                                                       G4ThreeVector momentumDirection
                                                       ) :  G4VUserPrimaryGeneratorAction(),
                                                            gun(nullptr)
                                                            eventDetector((qCalDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction()),
{
    G4int nOfParticles = 1;
    gun = new G4ParticleGun(nOfParticles);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
    gun->SetParticleDefinition(particle);
    gun->SetParticleEnergy(energy);
    gun->SetParticleMomentumDirection(momentumDirection);
    p_fZPosition = fZPosition;
    p_fCubeSize = fCubeSize;

    G4int nXAxis = eventDetector->GetXAxis();
    G4int nYAxis = eventDetector->GetYAxis();

    if (nXAxis % 2 = 0) {
        G4double x0 = ((int(G4UniformRand() * 2) * 2) - 1) * p_fCubeSize;
    }
    else {
        G4double x0 = 0;
    }

    if (nYAxis % 2 = 0) {
        G4double y0 = ((int(G4UniformRand() * 2) * 2) - 1) * p_fCubeSize;
    }
    else {
        G4double y0 = 0;
    }
    //G4double x0 = int((G4UniformRand() * 2) - 1) * p_fCubeSize;
    //G4double y0 = int((G4UniformRand() * 2) - 1) * p_fCubeSize;

    G4double z0 = p_fZPosition;

    G4cout << " " << G4endl;
    G4cout << "The Gun is positioned at: " << G4endl;
    G4cout << "X: " << x0 << G4endl;
    G4cout << "Y: " << y0 << G4endl;
    G4cout << "Z: " << z0 / cm << G4endl;
    G4cout << " " << G4endl;

    gun->SetParticlePosition(G4ThreeVector(x0, y0, z0));

}
    
// destructor
qCalPrimaryGeneratorAction::~qCalPrimaryGeneratorAction()
{
   delete gun;
}
    
//invoked once each event
void qCalPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

    gun->GeneratePrimaryVertex(event);
}


