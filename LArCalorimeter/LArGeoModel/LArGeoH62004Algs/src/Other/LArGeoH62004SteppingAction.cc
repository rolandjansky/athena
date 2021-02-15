/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004Algs/LArGeoH62004SteppingAction.h"
//#include "LArG4TBEmecHec/LArTBLeakHit.h"
//#include "LArG4TBEmecHec/LArTBDeadHit.h"
//#include "LArG4TBEmecHec/LArTBHECHit.h"
//#include "LArG4TBEmecHec/LArG4TBLeakSD.h"
//#include "LArG4TBEmecHec/LArG4TBHECSD.h"
//#include "LArG4TBEmecHec/LArG4TBDeadSD.h"
//#include "LArG4TBEmecHec/TBEmecHecRunOptions.h"
//#include "LArG4TBEndcap/LArG4TBEndcapEvFill.h"
// #include "LArG4EC/LArWheelCalculator.h"
// #include "LArG4HEC/LArHECCalculator.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4SD/LArG4SD.h"
#include "LArG4Code/LArG4Identifier.h"
#include "G4Track.hh"
#include "G4TouchableHistory.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"
#include "G4TrackStatus.hh"
#include "G4StepPoint.hh"


static LArGeoH62004SteppingAction sa("LArGeoH62004SteppingAction");

void LArGeoH62004SteppingAction::SteppingAction(const G4Step * theStep)
{ 
 static G4int h_num,kk;
 static G4int pcode, trackid, parid;
 static G4ThreeVector xyz;
 static G4String vname;
 static const char *ces_ime;
 static G4Track* theTrack;
// static LArTBLeakHit* theLeakHit; 
// static LArTBLeakHitsCollection *LeakCollection; 
 static G4String thePrePVname;
// static LArWheelCalculator *whcalc;
 static LArVCalculator *whcalc;


// Printing for geantino scan
// if(TBEmecHecRunOptions::GetInstance()->GetStep()) {

   
  G4StepPoint* tacka=theStep->GetPreStepPoint();
  G4StepPoint* tackab=theStep->GetPostStepPoint();
  G4TouchableHistory* theTouchable = 
                            (G4TouchableHistory*) (tacka->GetTouchable());
  xyz = tacka->GetPosition();

  std::cout<<"----------------------------------------------------"<<std::endl;
  std::cout<<"Step: X= "<<xyz.x()<<" Y= "<<xyz.y()<<" Z= "<<xyz.z()<<std::endl;
   /*
   G4ThreeVector localPoint = 
            G4TransportationManager::GetTransportationManager()
                                    ->GetNavigatorForTracking()
                                    ->GetGlobalToLocalTransform()
				    .TransformPoint(xyz);
				    */
   G4ThreeVector localPoint = theStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(xyz);
   
   G4cout<<"Local: X= "<<localPoint.x()<<" Y= "<<localPoint.y()<<
                     " Z= "<<localPoint.z()<<G4endl;
   vname = tacka->GetPhysicalVolume()->GetName();
   G4cout<<"Name: "<<vname<< "  Copy n.: "<<tacka->GetPhysicalVolume()->GetCopyNo()<<G4endl;
   G4cout<<"Material: "<<tacka->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName()<<G4endl;
   std::cout<<"-------------------"<<std::endl;
  G4ThreeVector xyzb = tackab->GetPosition();

  std::cout<<"Post Step: X= "<<xyzb.x()<<" Y= "<<xyzb.y()<<" Z= "<<xyzb.z()<<std::endl;
  /*
   G4ThreeVector localPointb =
            G4TransportationManager::GetTransportationManager()
                                    ->GetNavigatorForTracking()
                                    ->ComputeLocalPoint(xyzb);
				    */
   G4ThreeVector localPointb = theStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(xyzb);
   std::cout<<"Post Local: X= "<<localPointb.x()<<" Y= "<<localPointb.y()<<
                     " Z= "<<localPointb.z()<<std::endl;
   vname = tackab->GetPhysicalVolume()->GetName();
   std::cout<<"Post Name: "<<vname<< "  Copy n.: "<<tackab->GetPhysicalVolume()->GetCopyNo()<<std::endl;
   std::cout<<"Material: "<<tackab->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName()<<std::endl;
   std::cout<<"-------------------"<<std::endl;
   std::cout<<"Step length: "<<theStep->GetStepLength()<<std::endl;
   G4cout<<"Energy dep.: "<<theStep->GetTotalEnergyDeposit()<<G4endl;
 
   
   LArG4SD* lsd = 0;
   if((!strncmp(vname,"LArMgr::LAr::EMEC",17)) || (!strncmp(vname,"LArMgr::LAr::FCAL",17)) || (!strncmp(vname,"LArMgr::LAr::HEC::Module",24)) ) {
//   if((!strncmp(vname,"LAr::EMEC",9)) || (!strncmp(vname,"LAr::Endcap",11)) ) {
      lsd=(LArG4SD*)tacka->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector();
   }
//   if(!strncmp(vname,"LAr::HEC::Module",16) ) {
//       lsd=(LArG4SensitiveDetector*)tacka->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector();
//   }
      if(lsd) {
        std::cout<<"LArG4SD: "<<lsd->GetName()<<std::endl;
        whcalc = lsd->GetCalculator();
	if(whcalc){
           if(whcalc->Process(theStep) ){
               LArG4Identifier id = whcalc->identifier();
 	       if(!strncmp(vname,"LArMgr::LAr::FCAL",17)) 
                  std::cout<<" Stepping sampling: "<<id[3]<<" etaBin: "<<id[4]<<" phiBin: "<<id[5]<<std::endl;
	       else
                  std::cout<<" Stepping sampling: "<<id[3]<<" reg.: "<<id[4]<<" etaBin: "<<id[5]<<" phiBin: "<<id[6]<<std::endl;
	   }
	}
        std::cout<<"-------------------"<<std::endl;
      }
   
    
   G4int dubina=theTouchable->GetHistoryDepth();
         for( kk=0; kk<=dubina; kk++ )
            {
              h_num=theTouchable->GetVolume(kk)->GetCopyNo();    
              vname=theTouchable->GetVolume(kk)->GetName(); 
              G4cout<<vname<<" "<<h_num<<G4endl;
            }
	    
   G4cout<<"----------------------------------------------------"<<G4endl;
// }

  h_num = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
/* 
    if(h_num > 10000) {
//       G4G4cout<<"LArGeoH62004SteppingAction: Find leakage hit: "<<theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()<<G4G4endl;
      theTrack = theStep->GetTrack();
      trackid = theTrack->GetTrackID();
      parid = theTrack->GetParentID();
      for(kk=0; kk<m_tracks.size(); ++kk) {
	 if(m_tracks[kk] == trackid || m_tracks[kk] == parid) {
	    // old particle
	    break;
	 }
      }
      if(kk==m_tracks.size()) {
        theLeakHit = new LArTBLeakHit(h_num-10000);
        m_tracks.push_back(trackid);
        theLeakHit->SetEtot(theTrack->GetTotalEnergy());
        theLeakHit->SetEkin(theTrack->GetKineticEnergy());
        pcode = theTrack->GetDefinition()->GetPDGEncoding();
        ces_ime = theTrack->GetDefinition()->GetParticleName().data(); // for checking
        if(pcode == 0) {
           if(ces_ime[0] == 'a') pcode = 9003;
           else if(ces_ime[0] == 'd') pcode = 9001;
             else if(ces_ime[0] == 't') pcode = 9002;
               else if(ces_ime[0] == 'H') pcode = 9004;
           }
        theLeakHit->SetCode(pcode);
//---------------  store all in collection ------------------      
        LeakCollection = ((LArG4TBLeakSD *) G4SDManager::GetSDMpointer()->
                             FindSensitiveDetector("LeakSD"))->GetLeakColl();
        LeakCollection->insert(theLeakHit);
//-------------- we are not stopping the track ----
//        theTrack->SetTrackStatus(fStopAndKill);
       } 
    }
*/
    // Addition to find the step lengths in LAr and absorber 
    /*
    if(TBEmecHecRunOptions::GetInstance()->GetSteps()) {
      G4String matName = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
     if(matName == "LAr::EMEC::OuterWheel") {
//       cout<<"Adding step: "<<theStep->GetStepLength()<<" to LiquidArgon"<<endl;
       // we are adding the step length to leak hit
       G4StepPoint* tacka=theStep->GetPreStepPoint();
       G4StepPoint* tackab=theStep->GetPostStepPoint();
       theLeakHit = new LArTBLeakHit(NUM_LEAK+1,1,theStep->GetStepLength(),0.);
       LeakCollection = ((LArG4TBLeakSD *) G4SDManager::GetSDMpointer()->
                             FindSensitiveDetector("LeakSD"))->GetLeakColl();
       LeakCollection->insert(theLeakHit);

       LArTBHECHit* theLeakHitx = new LArTBHECHit(0,0,0,0,0);
       theLeakHitx->SetEdep(tacka->GetPosition().x());
        LArTBHECHit* theLeakHitbx = new  LArTBHECHit(1,0,0,0,0);
       theLeakHitbx->SetEdep(tackab->GetPosition().x());
        LArTBHECHit* theLeakHity = new  LArTBHECHit(0,1,0,0,0);
       theLeakHity->SetEdep(tacka->GetPosition().y());
        LArTBHECHit* theLeakHitby = new  LArTBHECHit(1,1,0,0,0);
       theLeakHitby->SetEdep(tackab->GetPosition().y());
        LArTBHECHit* theLeakHitz = new  LArTBHECHit(0,2,0,0,0);
       theLeakHitz->SetEdep(tacka->GetPosition().z());
        LArTBHECHit* theLeakHitbz = new  LArTBHECHit(1,2,0,0,0);
       theLeakHitbz->SetEdep(tackab->GetPosition().z());
        LArTBHECHitsCollection* LarCollection =
                             ((LArG4TBHECSD *) G4SDManager::GetSDMpointer()->
                             FindSensitiveDetector("/HEC/HLar"))->GetColl();
       LarCollection->insert(theLeakHitx);
       LarCollection->insert(theLeakHity);
       LarCollection->insert(theLeakHitz);
       LarCollection->insert(theLeakHitbx);
       LarCollection->insert(theLeakHitby);
       LarCollection->insert(theLeakHitbz);
    } else if ( matName == "LAr::EMEC::OuterWheel::Absorber" ) {
//              cout<<"Adding step: "<<theStep->GetStepLength()<<" to Absorber"<<endl;
       // we are adding the step length to leak hit
       G4StepPoint* tacka=theStep->GetPreStepPoint();
       G4StepPoint* tackab=theStep->GetPostStepPoint();
       theLeakHit = new LArTBLeakHit(NUM_LEAK+1,2,theStep->GetStepLength(),0.);
       LeakCollection = ((LArG4TBLeakSD *) G4SDManager::GetSDMpointer()->
                             FindSensitiveDetector("LeakSD"))->GetLeakColl();
       LeakCollection->insert(theLeakHit);

       LArTBHECHit* theLeakHitx = new LArTBHECHit(-1,0,-1,0,0);
       theLeakHitx->SetEdep(tacka->GetPosition().x());
       LArTBHECHit* theLeakHitbx = new LArTBHECHit(-1,0,-1,1,0);
       theLeakHitbx->SetEdep(tackab->GetPosition().x());
       LArTBHECHit* theLeakHity = new LArTBHECHit(-1,1,-1,0,0);
       theLeakHity->SetEdep(tacka->GetPosition().y());
       LArTBHECHit* theLeakHitby = new LArTBHECHit(-1,1,-1,1,0);
       theLeakHitby->SetEdep(tackab->GetPosition().y());
       LArTBHECHit* theLeakHitz = new LArTBHECHit(-1,2,-1,0,0);
       theLeakHitz->SetEdep(tacka->GetPosition().z());
       LArTBHECHit* theLeakHitbz = new LArTBHECHit(-1,2,-1,1,0);
       theLeakHitbz->SetEdep(tackab->GetPosition().z());
       LArTBHECHitsCollection* CuCollection =
                             ((LArG4TBHECSD *) G4SDManager::GetSDMpointer()->
                             FindSensitiveDetector("/HEC/Cu"))->GetColl();
       CuCollection->insert(theLeakHitx);
       CuCollection->insert(theLeakHity);
       CuCollection->insert(theLeakHitz);
       CuCollection->insert(theLeakHitbx);
       CuCollection->insert(theLeakHitby);
       CuCollection->insert(theLeakHitbz);
    }
  } 
 */
    // Another addition, summing all energy 
/*    
    if(TBEmecHecRunOptions::GetInstance()->GetSteps()) {
       if(theStep->GetTotalEnergyDeposit() > 0.) {
         LArTBDeadHitsCollection* DeadColl = ((LArG4TBDeadSD*)G4SDManager::GetSDMpointer()->
                                    FindSensitiveDetector("DeadSD"))->GetDeadColl();
       DeadColl->insert(new LArTBDeadHit(22222,theStep->GetTotalEnergyDeposit()));
	     }
    }
*/    
 return;
}







