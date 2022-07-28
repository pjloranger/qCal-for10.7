#include "G4UImanager.hh"
#include "G4RunManagerFactory.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "qCalActionInitialization.hh"
#include "qCalPhysicsList.hh"
#include "qCalDetectorConstruction.hh"



int main(int argc, char** argv)
{

	//For storing the macro name argument
	G4String macro;
	G4String mainOutputFileName = "outputFile";

	//For the session argument
	G4String session = "win32";

	//declaring detector construction variables
	G4String sAbs = "Fe"; //material of the absorber
	G4double fCubeWidth = 10.0 * cm; //width of a quartz cube
	G4double fCubeDepth = 10.0 * cm; //depth of a quartz cube
	G4double fSiPMDepth = 0.3 * cm; //depth of a SiPM
	G4double fSiPMWidth = 5.0 * cm; //width of a SiPM
	G4double fAbsRadLen = 1.905 * cm; //1.905 cm is the depth of one of the iron plates in the lab
	G4int nXAxis = 3; //number of quartz cubes in X direction
	G4int nYAxis = 3; //number of quartz cubes in Y direction
	G4int nZAxis = 3; //number of quartz cubes in Z direction

	bool b_xAxisEntetredByUser = false;
	bool b_yAxisEntetredByUser = false;
	bool b_zAxisEntetredByUser = false;
	bool b_absLengthEntered = false;

	//declaring action initialization variables
	G4String sParticle = "mu-";
	G4int sEnergy = 1 * GeV;
	G4double fZPosition = ((fCubeDepth / 2 + (0.025 * cm) + fSiPMDepth / 2 + fAbsRadLen / 2) * nZAxis) + 5 * cm;

	//loop through arguments and edit variables

	G4cout << "If you receive a segmentation fault here, you probably entered the wrong arguments." << G4endl;
	for (G4int i = 1; i < argc; i = i + 2)
	{
		if (G4String(argv[i]) == "-m") macro = argv[i + 1];
		else if (G4String(argv[i]) == "-u") session = argv[i + 1];
		else if (G4String(argv[i]) == "-qw") fCubeWidth = atof(argv[i + 1]);
		else if (G4String(argv[i]) == "-x") { nXAxis = atoi(argv[i + 1]); b_xAxisEntetredByUser = true; }
		else if (G4String(argv[i]) == "-y") { nYAxis = atoi(argv[i + 1]); b_yAxisEntetredByUser = true; }
		else if (G4String(argv[i]) == "-z") { nZAxis = atoi(argv[i + 1]); b_zAxisEntetredByUser = true; }
		else if (G4String(argv[i]) == "-e") sEnergy = (atoi(argv[i + 1]) * GeV);
		else if (G4String(argv[i]) == "-p") sParticle = argv[i + 1];
		else if (G4String(argv[i]) == "-a") sAbs = argv[i + 1];
		else if (G4String(argv[i]) == "-qd") fCubeDepth = atof(argv[i + 1]);								//quartzDepth in centimeters
		else if (G4String(argv[i]) == "-sd") fSiPMDepth = atof(argv[i + 1]);									//PMTDepth in centimeters
		else if (G4String(argv[i]) == "-sw") fSiPMWidth = atof(argv[i + 1]);   							//DetectorWidth in centimeters
		else if (G4String(argv[i]) == "-ad") { fAbsRadLen = atof(argv[i + 1]); }  //Absorber depth in centimeters
		else {
			G4cout << "Incorrect Usage, arguments read are: " << G4endl;
			G4cout << "nXAxis: " << nXAxis << G4endl;
			G4cout << "nYAxis: " << nYAxis << G4endl;
			G4cout << "nZAxis: " << nZAxis << G4endl;
			G4cout << "startingEnergy: " << sEnergy << G4endl;
			G4cout << "sAbs: " << sAbs << G4endl;
			G4cout << "Macro: " << macro << G4endl;
			G4cout << "Session: " << session << G4endl;
			return 1;
		}
	}

	//create the absorber material, used for calculations below
	G4Material* absorberMat = nist->FindOrBuildMaterial("G4_" + sAbs);
	G4double moliereRad = 1.719*cm //This is the moliere radius for IRON, must be changed if using lead as absorber mat, for example

	if (b_xAxisEntetredByUser == false) {
		nXAxis = int(2 * moliereRad / fCubeWidth);
	}
	if (b_yAxisEntetredByUser == false) {
		nYAxis = int(2 * moliereRad / fCubeWidth);
	}
	if (b_zAxisEntetredByUser == false) {
		nZAxis = int(9 * absorberMat->GetRadlen() / cm / fAbsRadLen); //total depth of absorbers should = 9*radlen, will capture 95% of shower
	}

	auto ui = new G4UIExecutive(argc, argv, session);

	auto runManager = G4RunManagerFactory::CreateRunManager();

	//declare and initialize detector construction
	auto detConstruction = new qCalDetectorConstruction(nXAxis, nYAxis, nZAxis, sAbs, fAbsRadLen, fCubeWidth, fCubeDepth, fSiPMWidth, fSiPMDepth);
	runManager->SetUserInitialization(detConstruction);

	//declare and configure physics list
	auto physicsList = new FTFP_BERT;

	physicsList->ReplacePhysics(new G4EmStandardPhysics_option4);

	G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();

	physicsList->RegisterPhysics(opticalPhysics);

	//opticalparams setup
	auto opticalParams = G4OpticalParameters::Instance();
	opticalParams->SetWLSTimeProfile("delta");
	opticalParams->SetCerenkovMaxPhotonsPerStep(100);
	opticalParams->SetCerenkovMaxBetaChange(10.0);
	opticalParams->SetCerenkovTrackSecondariesFirst(true);

	//initialize physics list
	runManager->SetUserInitialization(physicsList);

	auto actInitial = new qCalActionInitialization(sParticle, sEnergy, fZPosition, fCubeWidth);
	runManager->SetUserInitialization(actInitial);
	
	runManager->Initialize();

	//instantiation and initialization of the visualization manager
	auto visManager = new G4VisExecutive();
	visManager->Initialize();

	auto UIManager = G4UImanager::GetUIpointer();

	if (macro.size())
	{
		//executes macro
		G4String command = "/control/execute";
		UIManager->ApplyCommand(command + " " + macro);
	}
	else
	{
		//interactive mode
		UIManager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete ui;
	}

	delete runManager;
	delete visManager;
	
	return 0;
}

