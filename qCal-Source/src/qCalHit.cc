#include "qCalHit.hh"

#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"

#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include <iomanip>

G4ThreadLocal G4Allocator<qCalHit>* qCalSiPMHitAllocator=0;

qCalHit::qCalHit(G4ThreeVector i, G4double id, G4double t, G4double E)
:G4VHit(), fTime(t), fSiPMNumber(id), fEnergy(E), fPhysVol(0), fCoord(i), fPhotons(0), fDrawit(false)
{

}

qCalHit::~qCalHit()
{
   
}

void qCalHit::Draw()
{
}

void qCalHit::Print()
{
}















