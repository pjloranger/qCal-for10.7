#include "qCalEventAction.hh"
#include "qCalHit.hh"
#include "qCalDetectorConstruction.hh"
#include "qCalRunAction.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"
#include "g4root.hh"
#include "G4HCofThisEvent.hh"


#include <iomanip>

#include <algorithm>


//constructor
qCalEventAction::qCalEventAction():
   G4UserEventAction(),
   fSiPMCollID(-1),
   eventDetector((qCalDetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction()),
   SDVolume(eventDetector->GetVolume()),
   fphotonCount{std::vector<G4double>(SDVolume, 0)},
   fSiPMNums{std::vector<G4double>(SDVolume, -1)},
   fHitTimes{ std::vector<G4double>(SDVolume, -1)},
   fInitialHitTimes{ std::vector<G4double>(SDVolume, -1)},
   fFinalHitTimes{ std::vector<G4double>(SDVolume, -1)},
   iSiPMCoordsX{ std::vector<G4int>(SDVolume, -1)},
   iSiPMCoordsY{ std::vector<G4int>(SDVolume, -1)},
   iSiPMCoordsZ{ std::vector<G4int>(SDVolume, -1)}

{
   //Set Print Progress to every event.
   G4RunManager::GetRunManager()->SetPrintProgress(1);
}

//destructor
qCalEventAction::~qCalEventAction() = default;
//{}


void qCalEventAction::BeginOfEventAction(const G4Event*)
{
    G4cout << "BeginOfEventAction START" << G4endl;
    G4cout << "BeginOfEventAction END" << G4endl;
    
}


void qCalEventAction::EndOfEventAction(const G4Event* event)
{
    G4cout << "EndOfEventAction START" << G4endl;

    //If fSiPMCollID not defined yet, define it.
    //We only need to do this once per run.
    if (fSiPMCollID == -1)
    {
        fSiPMCollID = G4SDManager::GetSDMpointer()->GetCollectionID("SiPMHitCollection");

    }

    auto SiPMHitCollection = static_cast<qCalSiPMHitsCollection*>(event->GetHCofThisEvent()->GetHC(fSiPMCollID));

    // Number of Hits
    //G4cout << "get number of hits" << G4endl;
    G4int n_hit = SiPMHitCollection->entries();
    //G4cout << n_hit << G4endl;

	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetVerboseLevel(1);

    // The hits are iterated to give them SiPMNumbers (IDs) and extract photon counts and hit times:
    for (G4int i = 0; i < n_hit; ++i) {
        qCalHit* hit = (*SiPMHitCollection)[i];
        // Set hit integer coordinates and SiPM number from scale factor:
        G4int IDofHit = hit->GetSiPMNumber();
        // Increment the hit count for the hit with the current SiPM number:
        fphotonCount[IDofHit] += hit->GetPhotonCount();
        // Set initial and final times for hits at the current SiPM number:

    }

    // Single hit times are calculated for each SiPM that fill the hit times ntuple:
    // Create histograms of hit times for each SiPM and extract the peak value which is the mode of hit times for that SiPM.
    // An initial estimate of the histogram bin width is 0.015 - 0.020 nsecs:
    G4int i_totalPhotonCount = 0;
    G4double timeBinWidth = 0.015;

    for (G4int id = 0; id < SDVolume; ++id)
    {
        i_totalPhotonCount += (int)fphotonCount[id];
    }
    // Adds the events photon count to the histogram:
    analysisManager->FillH1(1, i_totalPhotonCount);
    //analysisManager->FillH1(1, 1);

    // Update all the root ntuples:
    //analysisManager->AddNtupleRow();

    // Reset the SiPM counts and Hit Times to zero for a new event.
    for (int i = 0; i < SDVolume; i++) {
        fphotonCount[i] = 0.;
    }

    G4cout << "EndOfEventAction END" << G4endl;
}













