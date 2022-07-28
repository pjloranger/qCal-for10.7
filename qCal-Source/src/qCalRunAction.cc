#include "qCalRunAction.hh"
#include "qCalDetectorConstruction.hh"
#include "qCalEventAction.hh"
#include "qCalRunMessenger.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "g4root.hh"



qCalRunAction::qCalRunAction(qCalEventAction* eventAction)
: G4UserRunAction()
{
   p_runActionOutputFileName = "qCalOutputFile.root";
   G4RunManager::GetRunManager()->SetPrintProgress(1);
   
   auto analysisManager = G4AnalysisManager::Instance();
   
   analysisManager->SetVerboseLevel(4);

   //creating histograms
   analysisManager->CreateH1("TimingHist", "HitTimesPerSiPM", 10, 0, 10000);
   analysisManager->CreateH1("PhotonCountHist","# of Photons per Event", 100*100, 0., 1000);

   analysisManager->CreateNtuple("qCal", "Simulation Data");
   analysisManager->CreateNtupleIColumn("SiPMPositionX", eventAction->GetSiPMCoordsX()); // ID 0
   analysisManager->CreateNtupleIColumn("SiPMPositionY", eventAction->GetSiPMCoordsY()); // ID 1
   analysisManager->CreateNtupleIColumn("SiPMPositionZ", eventAction->GetSiPMCoordsZ()); // ID 2
   analysisManager->CreateNtupleDColumn("SiPMNumbers", eventAction->GetSiPMNums()); // ID 3
   analysisManager->CreateNtupleDColumn("PhotonCounts", eventAction->GetPhotonCount()); // ID 4
   analysisManager->CreateNtupleDColumn("HitTimes", eventAction->GetHitTimes()); // ID 5
   analysisManager->FinishNtuple();

   p_fRunMessenger = new qCalRunMessenger(this);

}

qCalRunAction::~qCalRunAction()
{
	//i think not deleting the analysismanager might be important
	delete G4AnalysisManager::Instance();
}

void qCalRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
   G4cout << "BeginOfRunAction START" << G4endl;
   // Get analysis manager
   auto analysisManager = G4AnalysisManager::Instance();
   analysisManager->SetVerboseLevel(4);

   // Open an output file
   analysisManager->OpenFile(p_runActionOutputFileName);
   G4cout << "BeginOfRunAction END" << G4endl;
}



void qCalRunAction::EndOfRunAction(const G4Run* /*run*/)
{
   G4cout << "EndOfRunAction START" << G4endl;
   auto analysisManager = G4AnalysisManager::Instance();
   analysisManager->SetVerboseLevel(4);

   // save histograms & ntuple
   analysisManager->Write(); //Crashes here
   analysisManager->CloseFile();
   G4cout << "EndOfRunAction END" << G4endl;
}

void qCalRunAction::SetOutputFileName(G4String outputFileNameByCmd)
{
   p_runActionOutputFileName = outputFileNameByCmd;
}
