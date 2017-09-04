/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//example of reading xAOD in fast xAOD mode with POOL::TEvent
//author: Will Buttinger


#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleContainer.h"
#include "POOLRootAccess/TEvent.h"

#include "TStopwatch.h"
#include "TChain.h"

#include "TFile.h"
#include "TH1F.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

int main(int argc, char* argv[]) {

   xAOD::TEvent::EAuxMode accessMode2 = xAOD::TEvent::kClassAccess;
   POOL::TEvent::EReadMode accessMode = POOL::TEvent::kClassAccess;

   std::string whatToRead;
   if (getenv("ASG_TEST_FILE_MC") != nullptr)
     whatToRead = "$ASG_TEST_FILE_MC";
   else
     whatToRead = "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915862._000100.pool.root.1";
   if(argc>1) whatToRead = argv[1];

   std::cout << "reading: " << whatToRead << std::endl; 

   const xAOD::EventInfo* evtInfo = 0;
   const xAOD::IParticleContainer* els = 0; //electrons
   const xAOD::IParticleContainer* mus = 0; //mus
   const xAOD::IParticleContainer* jets = 0; //jets

   xAOD::Init().ignore();
   TChain* c = new TChain("CollectionTree"); 
   c->Add(whatToRead.c_str());
   xAOD::TEvent evt2(accessMode2);
   evt2.readFrom(c).ignore();
  
  /* the following is an older way of setting up for xAOD fast reading
   POOL::TEvent::Init("POOLRootAccess/basicxAOD.opts"); //prepare for fast xAOD reading
   POOL::TEvent evt;
   evt.setEvtSelProperty("AccessMode",int(accessMode)); 
   */
   POOL::TEvent evt(accessMode);

   evt.readFrom(whatToRead.c_str());

   int maxEvt2 = evt2.getEntries();
   int maxEvt = evt.getEntries();
   
   if(maxEvt != maxEvt2) {
    std::cout << "mismatch in getEntries: " << maxEvt << " vs " << maxEvt2 << std::endl; return -1;
   }
   std::cout << "Found " << maxEvt << " events." <<std::endl;

/*
    std::cout << "doing preloop..." << std::endl;
   //do a preloop loop through just to load the file for fair comparisons
   for(int i=0;i<maxEvt2;i++) {
    evt2.getEntry(i);
    evt2.retrieve( evtInfo , "EventInfo" ).ignore();
    evt2.retrieve( els, "Electrons" ).ignore();
    evt2.retrieve( mus, "Muons" ).ignore();
    evt2.retrieve( jets, "AntiKt4LCTopoJets" ).ignore();
   }
   std::cout << "...done.... now doing test..." << std::endl;
   */
   std::cout << "doing xAODRootAccess test (using kClassAccess mode)...." <<std::endl;
   evt2.getEntry(0);
   TStopwatch st2;
   st2.Start();
   long val2[4] = {0,0,0,0};
   for(int i=0; i< maxEvt2; i++) {
     std::cout << int(100*i/maxEvt2) << "%...\r";
     std::cout.flush();
      evt2.getEntry(i);
      evt2.retrieve( evtInfo , "EventInfo" ).ignore();
      val2[0] += evtInfo->eventNumber();
      evt2.retrieve( els, "Electrons" ).ignore();
      val2[1] += els->size();
      evt2.retrieve( mus, "Muons" ).ignore();
      val2[2] += mus->size();
      evt2.retrieve( jets, "AntiKt4LCTopoJets" ).ignore();
      val2[3] += jets->size();
   }
   std::cout << std::endl;
   st2.Stop();
   st2.Print();

   long val[4] = {0,0,0,0};

   std::cout << "doing POOLRootAccess test (using kClassAccess mode)...." <<std::endl;
   evt.getEntry(0);
   TStopwatch st;
   st.Start();
   for(int i=0; i< maxEvt; i++) {
      evt.getEntry(i);
      evt.retrieve( evtInfo , "EventInfo" );
      val[0] += evtInfo->eventNumber();
      evt.retrieve( els, "Electrons" ).ignore();
      val[1] += els->size();
      evt.retrieve( mus, "Muons" ).ignore();
      val[2] += mus->size();
      evt.retrieve( jets, "AntiKt4LCTopoJets" ).ignore();
      val[3] += jets->size();
   }
   st.Stop();
   st.Print();
   std::cout << "xAODRootAccess Event rate = " << double(maxEvt2)/st2.RealTime() << " Hz " << std::endl;
   std::cout << "POOLRootAccess Event rate = " << double(maxEvt)/st.RealTime() << " Hz" << std::endl;
   
   for(int i=0;i<4;i++) {
    if(val[i] != val2[i]) {
      std::cout << "mismatch in validation " << i << ": " << val[i] << " vs " << val2[i] << std::endl; return -1;
    }
  }

  TFile f1("ut_basicxAODRead_test.results.root","RECREATE");
  TH1F* speed1 = new TH1F("speed1","xAODRootAccess Speed [Hz]",1,0,1);speed1->Sumw2();
  TH1F* speed2 = new TH1F("speed2","POOLRootAccess Speed [Hz]",1,0,1);speed2->Sumw2();
  speed1->SetBinContent(1,double(maxEvt2)/st2.RealTime());speed1->SetBinError(1,0.0001);
  speed2->SetBinContent(1,double(maxEvt)/st.RealTime());speed2->SetBinError(1,0.0001);
  speed1->Write();speed2->Write();
  f1.Close();
  


   return 0;

}
