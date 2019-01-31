#include "remollEventAction.hh"
#include "remollGenericDetectorHit.hh"
#include "remollGenericDetectorSum.hh"
#include "remollPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4PhysicalConstants.hh"
#include "G4TwoVector.hh"

#include "remollAnalysis.hh"
#include "remollIO.hh"
#include "remollEvent.hh"
#include "remollRun.hh" //sakib
#include "remollRunData.hh" //sakib

#include "G4Threading.hh"
#include "G4AutoLock.hh"
//namespace { G4Mutex remollEventActionMutex = G4MUTEX_INITIALIZER; }

remollEventAction::remollEventAction()
  : fPrimaryGeneratorAction(0),fEventSeed("") { }

remollEventAction::~remollEventAction() { }

void remollEventAction::BeginOfEventAction(const G4Event* event) { }

void remollEventAction::EndOfEventAction(const G4Event* aEvent)
{
  // We collect all interaction with remollIO in this thread for as
  // little locking as possible. This means that all the thread local
  // information must be retrieved from here.

  // Lock mutex
  //G4AutoLock lock(&remollEventActionMutex);
  
  /*-----------------Implementing Analysis Manager-Sakib----------------*/
  auto analysisManager = G4AnalysisManager::Instance();
  /*-------------------------------------------------------------------*/



  //remollIO* io = remollIO::GetInstance();

  // Store random seed
  //fEventSeed = aEvent->GetRandomNumberStatus();
  //io->SetEventSeed(fEventSeed);

  // Get primary event action information
  const remollEvent* event = fPrimaryGeneratorAction->GetEvent();
  //io->SetEventData(event);
 
  // Implmenting analysis manager -Sakib //
  G4String genname=remollRun::GetRunData()->GetGenName(); //getting generator info
  int maxtr=1;
  if(genname=="moller"){ maxtr=2; }
  // ------------------------------------//
 
  // Traverse all hit collections, sort by output type
  G4HCofThisEvent *HCE = aEvent->GetHCofThisEvent();
  for (int hcidx = 0; hcidx < HCE->GetCapacity(); hcidx++) {
    G4VHitsCollection* thiscol = HCE->GetHC(hcidx);
    if (thiscol){ // This is NULL if nothing is stored

      // Dynamic cast to test types, process however see fit and feed to IO

      ////  Generic Detector Hits ///////////////////////////////////
      if (remollGenericDetectorHitCollection *thiscast = dynamic_cast<remollGenericDetectorHitCollection*>(thiscol)) {
        for (unsigned int hidx = 0; hidx < thiscast->GetSize(); hidx++) {
      //    io->AddGenericDetectorHit((remollGenericDetectorHit *) thiscast->GetHit(hidx));

   /*-----------------Implementing Analysis Manager-Sakib----------------*/
    	  remollGenericDetectorHit_t thishit= (((remollGenericDetectorHit *) thiscast->GetHit(hidx))->GetGenericDetectorHitIO());
          //int detcut = thishit.det==27||thishit.det==26||thishit.det==25||thishit.det==24||thishit.det==28||thishit.det==29;
          if(true){// detcut && thishit.trid<=maxtr && thishit.pid==11){ //hits plane detector, is electron, is moller, elastic or inelastic
         
            analysisManager->FillNtupleDColumn(1,0,thishit.x);
            analysisManager->FillNtupleDColumn(1,1,thishit.y);
            analysisManager->FillNtupleDColumn(1,2,thishit.z);
            analysisManager->FillNtupleDColumn(1,3,event->fRate*s);
            analysisManager->FillNtupleDColumn(1,4,(event->fAsym)*1e9);
            if (thishit.trid==1){
		analysisManager->FillNtupleDColumn(1,5, (event->fThCoM)*180/pi);
                }else{
                analysisManager->FillNtupleDColumn(1,5, 180-(event->fThCoM)*180/pi);
                }
            analysisManager->FillNtupleDColumn(1,6, event->fPartRealMom[thishit.trid-1].theta());
       	    analysisManager->FillNtupleDColumn(1,7,thishit.det);
	    analysisManager->FillNtupleDColumn(1,8,event->fPartPos[thishit.trid-1].x());
	    analysisManager->FillNtupleDColumn(1,9,event->fPartPos[thishit.trid-1].y());
	    analysisManager->FillNtupleDColumn(1,10,event->fPartPos[thishit.trid-1].z());
	    analysisManager->FillNtupleDColumn(1,11, thishit.e);
	    analysisManager->FillNtupleDColumn(1,12, event->fQ2);
            analysisManager->FillNtupleDColumn(1,13, thishit.pid);
	    analysisManager->FillNtupleDColumn(1,14, thishit.trid);
            analysisManager->AddNtupleRow(1);
          
	   }
        
  /*-------------------------------------------------------------------*/


        }
      }

      ////  Generic Detector Sum ////////////////////////////////////
      /*
      if (remollGenericDetectorSumCollection *thiscast =
          dynamic_cast<remollGenericDetectorSumCollection*>(thiscol)) {
        for (unsigned int hidx = 0; hidx < thiscast->GetSize(); hidx++) {
          io->AddGenericDetectorSum((remollGenericDetectorSum *)
                                    thiscast->GetHit(hidx));
        }
      }*/

    }
  }

  // Fill tree and reset buffers
  //io->FillTree();
  //io->Flush();
}
