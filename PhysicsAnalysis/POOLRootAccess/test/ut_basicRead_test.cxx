/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODEventInfo/EventInfo.h"
#include "POOLRootAccess/TEvent.h"

#include "TStopwatch.h"
#include "TChain.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include "AsgMessaging/MessageCheck.h"

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

int main(int argc, char* argv[]) {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;
   

   std::string whatToRead = "$ASG_TEST_FILE_MC";
   if(argc>1) whatToRead = argv[1];

   std::cout << "reading: " << whatToRead << std::endl; 

   const xAOD::EventInfo_v1* evtInfo = 0;


   ANA_CHECK (xAOD::Init());
   TChain* c = new TChain("CollectionTree"); 
   c->Add(whatToRead.c_str());
   xAOD::TEvent evt2;
   ANA_CHECK (evt2.readFrom(c));

   int maxEvt2 = evt2.getEntries(); 
   TStopwatch st2;
   st2.Start();
   for(int i=0; i< maxEvt2; i++) {
      evt2.getEntry(i);
      ANA_CHECK (evt2.retrieve( evtInfo , "EventInfo" ));
      evtInfo->eventNumber();
   }
   st2.Stop();
   st2.Print();
   std::cout << "xAODRootAccess Event rate = " << double(maxEvt2)/st2.RealTime() << " Hz " << std::endl;


   POOL::TEvent evt(POOL::TEvent::kPOOLAccess);
   evt.readFrom(whatToRead.c_str()).ignore();
   //evt.setEvtSelProperty("CollectionTree","POOLContainer/basic");
   //evt.setEvtSelProperty( "OutputLevel" , 3 );

   int maxEvt = evt.getEntries(); evt.getEntry(0);

   //just a quick example of retrieving metadata
   float beam_energy(0);
   AAH::retrieveMetadata("/TagInfo","beam_energy",beam_energy,evt.inputMetaStore()).ignore();
   std::cout << "Beam energy = " << beam_energy << std::endl;

   TStopwatch st;
   st.Start();
   for(int i=0; i< maxEvt; i++) {
      evt.getEntry(i);
      evt.retrieve( evtInfo , "EventInfo" ).ignore();
      evtInfo->eventNumber();
   }
   st.Stop();
   st.Print();
   std::cout << "POOLRootAccess Event rate = " << double(maxEvt)/st.RealTime() << " Hz" << std::endl;



   return 0;

}
