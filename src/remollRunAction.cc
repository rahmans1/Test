#include "remollRunAction.hh"

#include "G4GenericMessenger.hh"
#include "G4RunManager.hh"
#include "G4Timer.hh"

#include "remollAnalysis.hh" // Implementing Analysis Manager - Sakib
#include "remollIO.hh"
#include "remollRun.hh"
#include "remollRunData.hh"
#include "remollBeamTarget.hh"

#include "G4Threading.hh"
#include "G4AutoLock.hh"
//namespace { G4Mutex remollRunActionMutex = G4MUTEX_INITIALIZER; }

remollRunAction::remollRunAction()
{
  // Create messenger to set the seed with single long int
  fMessenger = new G4GenericMessenger(this,"/remoll/","Remoll properties");
  fMessenger->DeclareMethod(
      "seed",
      &remollRunAction::UpdateSeed,
      "Set random engine seed")
      .SetParameterName("seed", false)
      .SetStates(G4State_PreInit,G4State_Idle);

  // Create timer
  fTimer = new G4Timer();

  auto analysisManager = G4AnalysisManager::Instance(); //Implementing Analysis Manager-Sakib


}

remollRunAction::~remollRunAction()
{
  delete G4AnalysisManager::Instance(); //Implementing Analysis Manager-Sakib
  delete fMessenger;
  delete fTimer;
}

void remollRunAction::UpdateSeed(const G4long seed)
{
  G4Random::setTheSeed(seed);
  remollRun::GetRunData()->SetSeed(seed);
  G4cout << "Random seed set to " << seed << G4endl;
}

G4Run* remollRunAction::GenerateRun()
{
  return new remollRun();
}

void remollRunAction::BeginOfRunAction(const G4Run* run)
{
  // Cast into remollRun
  const remollRun* aRun = static_cast<const remollRun*>(run);

  // Print progress
  G4int interval = 100; // Print this many progress points (i.e. 100 -> every 1%)
  G4int evts_to_process = aRun->GetNumberOfEventToBeProcessed();
  G4RunManager::GetRunManager()->SetPrintProgress((evts_to_process > interval)
                                                  ? evts_to_process/interval
                                                  : 1);

  if (IsMaster())
  {
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

    fTimer->Start();

  //  G4AutoLock lock(&remollRunActionMutex);

    
    
    //remollIO* io = remollIO::GetInstance();
    //io->InitializeTree();

    remollRunData *rundata = remollRun::GetRunData();
    rundata->SetNthrown( aRun->GetNumberOfEventToBeProcessed() );
    rundata->Print();
  } else{
    
  /*-----Implementing analysis manager-Sakib-----------------*/ 
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetFirstHistoId(1);
    analysisManager->SetFirstNtupleId(1);
    
    analysisManager->CreateNtuple("T", "T");
    analysisManager->CreateNtupleDColumn("x");
    analysisManager->CreateNtupleDColumn("y");
    analysisManager->CreateNtupleDColumn("z");
    analysisManager->CreateNtupleDColumn("rate");
    analysisManager->CreateNtupleDColumn("asymmetry");
    analysisManager->CreateNtupleDColumn("theta_com");
    analysisManager->CreateNtupleDColumn("theta");
    analysisManager->CreateNtupleDColumn("det");
    analysisManager->CreateNtupleDColumn("vx");
    analysisManager->CreateNtupleDColumn("vy");
    analysisManager->CreateNtupleDColumn("vz");
    analysisManager->CreateNtupleDColumn("hitenergy");
    analysisManager->CreateNtupleDColumn("Q2");
    analysisManager->FinishNtuple();
    analysisManager->OpenFile();
    /*----------------------------------------------------*/
    }
}

void remollRunAction::EndOfRunAction(const G4Run* run)
{
  // Cast into remollRun
  const remollRun* aRun = static_cast<const remollRun*>(run);

  if (IsMaster())
  {
      fTimer->Stop();

      G4cout << "### Run " << aRun->GetRunID() << " ended "
             << "(" << fTimer->GetUserElapsed() << "s)." << G4endl;

      //G4AutoLock lock(&remollRunActionMutex);
      //remollIO* io = remollIO::GetInstance();
      //io->WriteTree();

  
  }else{

    /*-----------------Implementing Analysis Manager-Sakib----------------*/
      auto analysisManager = G4AnalysisManager::Instance();
      analysisManager->Write();
      analysisManager->CloseFile();
      /*-------------------------------------------------------------------*/
  }
}

