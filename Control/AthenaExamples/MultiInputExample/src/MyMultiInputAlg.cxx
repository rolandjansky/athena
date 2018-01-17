/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MultiInputExample/MyMultiInputAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "EventInfo/PileUpEventInfo.h"  
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TagInfo.h"

#include "AthenaPoolExampleData/ExampleClass.h"
#include "AthenaPoolExampleData/ExampleHitContainer.h"

/////////////////////////////////////////////////////////////////////////////

MyMultiInputAlg::MyMultiInputAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_mergeSvc("PileUpMergeSvc", name)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode MyMultiInputAlg::initialize()
{
  ATH_CHECK( m_mergeSvc.retrieve() );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode MyMultiInputAlg::execute()
{
  //Put all available ExampleClass with key "MyData" in alist
  typedef PileUpMergeSvc::TimedList<ExampleClass>::type ExampleClassList;
  ExampleClassList alist;

  ATH_CHECK( m_mergeSvc->retrieveSubEvtsData("MyData", alist) );
  ATH_MSG_INFO( alist.size() 
                << " ExampleClassList with key " << "MyData"
                << " found"  );

  //Now alist.begin() is the first input "SimpleRootFile1.root"
  //Now alist.end() is the second (in this example only) input "SimpleRootFile2.root"
  //ExampleClassList::iterator has type pair
  ExampleClassList::iterator icls(alist.begin());
  ExampleClassList::iterator endcls(alist.end());
  int k=1;
  while (icls != endcls) {
    //Get a pointer to access 
    const ExampleClass* pcls(icls->second);
    ATH_MSG_INFO("MyData in Input File #"<<k<<" contains getRun()="<<pcls->getRun()<<" getEvent()="
                 <<pcls->getEvent()<<" getText()="
                 <<pcls->getText()<<" " );
    ++icls;++k;
  }


  //Put all available ExampleHitContainer with key "MyHits" in blist
  typedef PileUpMergeSvc::TimedList<ExampleHitContainer>::type ExampleHitContainerList;
  ExampleHitContainerList blist;

  ATH_CHECK( m_mergeSvc->retrieveSubEvtsData("MyHits", blist) );
  ATH_MSG_INFO( blist.size() 
                << " ExampleHitContainerList with key " << "MyHits"
                << " found"  );

  //Now blist.begin() is the first input "SimpleRootFile1.root"
  //Now blist.end() is the second (in this example only) input "SimpleRootFile2.root"
  //ExampleHitContainerList::iterator has type pair
  ExampleHitContainerList::iterator ihit(blist.begin());
  ExampleHitContainerList::iterator endhit(blist.end());
  k=1;
  while (ihit != endhit) {
    //Get a pointer to access 
    const ExampleHitContainer* cont(ihit->second);
    ATH_MSG_INFO("MyHits in Input File #"<<k<<" contains:" );
    
    for (ExampleHitContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
      ATH_MSG_INFO( "Hit x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " detector = " << (*obj)->getDetector()  );
    }
    
    ++ihit;++k;
  }

  /*
  const PileUpEventInfo* pEvent;
  if (p_overStore->contains<PileUpEventInfo>("OverlayEvent")) p_overStore->retrieve(pEvent);
  
  
  if (pEvent) {
    // access the sub events DATA objects...
    PileUpEventInfo::SubEvent::const_iterator iEvt = pEvent->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator endEvt = pEvent->endSubEvt();
    int i=0;


    //    while (iEvt != endEvt) {
    assert(iEvt->pSubEvtSG);
    assert(iEvt->pSubEvt);
    
    assert(endEvt->pSubEvtSG);
    assert(endEvt->pSubEvt);
    

    StatusCode sc;
    log<<MSG::INFO<<"HAHAHAHA1"<<endmsg;
    if (iEvt->pSubEvtSG->contains<ExampleClass>("MyData")) {
      log<<MSG::INFO<<"HAHAHAHA2"<<endmsg;
      const DataHandle<ExampleClass> ec1;
      log<<MSG::INFO<<"HAHAHAHA3"<<endmsg;
      sc = iEvt->pSubEvtSG->retrieve(ec1, "MyData");
      log<<MSG::INFO<<"HAHAHAHA4"<<endmsg;
      if (!sc.isSuccess()) {
	log<<MSG::INFO<<"HAHAHAHA5"<<endmsg;
	log << MSG::ERROR << "Could not find ExampleClass/MyData" << endmsg;
	return(StatusCode::FAILURE);
      }
      log << MSG::INFO<<"HAHAHAHA6"<<endmsg;
      log << MSG::INFO << "Run = " << ec1->getRun() << " event = " << ec1->getEvent() << " :: " << ec1->getText() << endmsg;
    }

    log<<MSG::INFO<<"GOGOHAHAHAHA1"<<endmsg;    
    if (endEvt->pSubEvtSG->contains<ExampleClass>("MyData")) {
      log<<MSG::INFO<<"GOGOHAHAHAHA2"<<endmsg;    
      const DataHandle<ExampleClass> ec2;
      log<<MSG::INFO<<"GOGOHAHAHAHA3"<<endmsg;    
      sc = endEvt->pSubEvtSG->retrieve(ec2, "MyData");
      log<<MSG::INFO<<"GOGOHAHAHAHA4"<<endmsg;    
      if (!sc.isSuccess()) {
	log<<MSG::INFO<<"GOGOHAHAHAHA5"<<endmsg;    
	log << MSG::ERROR << "Could not find ExampleClass/MyData" << endmsg;
	return(StatusCode::FAILURE);
      }
    log<<MSG::INFO<<"GOGOHAHAHAHA6"<<endmsg;    
    log << MSG::INFO << "Run = " << ec2->getRun() << " event = " << ec2->getEvent() << " :: " << ec2->getText() << endmsg;
    }
    
  *//*      
      log << MSG::INFO << "DUMPDUMPDUMP\t"<<i<<"\n"<<iEvt->pSubEvtSG->dump() <<endmsg;

      const EventInfo * evt = 0;
      StatusCode sc = iEvt->pSubEvtSG->retrieve( evt );
      if ( sc.isFailure() ) {
	log << MSG::ERROR << "  Could not get event info" << endmsg;      
	return StatusCode::FAILURE;
      }
      else {
	log << MSG::DEBUG << "Event ID: ["
	    << evt->event_ID()->run_number()   << ","
	    << evt->event_ID()->event_number() << ":"
	    << evt->event_ID()->time_stamp() << "] "
	    << endmsg;
	log << MSG::DEBUG << "Event type: user type "
	    << evt->event_type()->user_type() << " weight "
	    << evt->event_type()->mc_event_weight() 
	    << endmsg;
      }
      
      iEvt++;i++;
      
    }
    */
  //  }

  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode MyMultiInputAlg::finalize()
{
  ATH_MSG_INFO( "finalize()"  );
  return StatusCode::SUCCESS;
}
