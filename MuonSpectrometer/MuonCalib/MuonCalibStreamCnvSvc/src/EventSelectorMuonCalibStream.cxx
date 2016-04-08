/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	EventSelectorMuonCalibStream.cxx
//====================================================================
// 
// Include files.
#include "MuonCalibStreamCnvSvc/EventSelectorMuonCalibStream.h"
#include "MuonCalibStreamCnvSvc/EventContextMuonCalibStream.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamInputSvc.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddress.h"
#include "MuonCalibStreamCnvSvc/IMuonCalibStreamDataProviderSvc.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

#include "EventInfo/EventInfo.h" 
#include "StoreGate/StoreGate.h"

 
// Instantiation of a Service Factory static class to generate 
// instances of the EventSelectorMuonCalibStream class.

// static const SvcFactory<EventSelectorMuonCalibStream> s_factory;
// const ISvcFactory& EventSelectorMuonCalibStreamFactory = s_factory;

// Constructor.
EventSelectorMuonCalibStream::EventSelectorMuonCalibStream(const string& name, ISvcLocator* svcloc)
  : AthService(name, svcloc),
    m_beginIter(0), m_endIter(0),
    m_eventSource(0), m_SkipEvents(0), m_NumEvents(0) {

  // cout<<"in the Selector constructor: "<<name<<endl;

  m_eventSourceName="MuonCalibStreamFileInputSvc";
  m_SkipEvents=0;
  declareProperty("MuonCalibStreamInputSvc", m_eventSourceName);
  declareProperty("SkipEvents", m_SkipEvents);
}

// Destructor.
EventSelectorMuonCalibStream::~EventSelectorMuonCalibStream() {
  //if(m_eventSource) m_eventSource->release() ;
  if(m_beginIter) delete m_beginIter;
  if(m_endIter)   delete m_endIter;
}

// EventSelectorMuonCalibStream::initialize().
StatusCode EventSelectorMuonCalibStream::initialize() {
  // Check MuonCalibStreamCnvSvc
  IService* svc;
  ATH_CHECK( serviceLocator()->getService(m_eventSourceName, svc) ); 

  m_eventSource = dynamic_cast<MuonCalibStreamInputSvc*> (svc); 
  if(m_eventSource == 0 ) {
    ATH_MSG_ERROR( "  Cannot cast to MuonCalibStreamInputSvc " ); 
    return StatusCode::FAILURE;
  } 
  m_eventSource->addRef();    
  
  ATH_CHECK( service("MuonCalibStreamDataProviderSvc", m_dataProvider) ); 

  // Create the begin and end iterators for this selector.
  m_beginIter = new EventContextMuonCalibStream(this); 
  // increment to get the new event in. 
  //     ++(*m_beginIter);   ??? 
  m_endIter   = new EventContextMuonCalibStream(0); 

  return StatusCode::SUCCESS;
}

StatusCode EventSelectorMuonCalibStream::createContext(IEvtSelector::Context*& it) const {
  it = new EventContextMuonCalibStream(this);
  return(StatusCode::SUCCESS);
}

// Implementation of IEvtSelector::next().
StatusCode EventSelectorMuonCalibStream::next(IEvtSelector::Context& it) const {
  for(;;) { 
    const LVL2_MUON_CALIBRATION::CalibEvent* pre = m_eventSource->nextEvent();
    // cout << " NextPackedCalibEvent "<<pre<<endl; 
    if(!pre) { 
     // End of file 
      it = *m_endIter; 
      return StatusCode::FAILURE; 
    }
    ++m_NumEvents; 

   // Check if we are skipping events
    if(m_NumEvents>m_SkipEvents) {
      break;
    } else {    
      ATH_MSG_INFO( " Skipping event "<< m_NumEvents-1 ); 
    }
  } 

  return StatusCode::SUCCESS; 
}

// Implementation of IEvtSelector::next() with a "jump" parameter 
// (to skip over a certain number of events?)
StatusCode EventSelectorMuonCalibStream::next(IEvtSelector::Context& ctxt, int jump) const {
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::previous(IEvtSelector::Context& /*it*/) const {
  ATH_MSG_ERROR( "EventSelectorMuonCalibStream::previous() not implemented" );
  return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::previous(IEvtSelector::Context& it,int /*jump*/) const {
  return(previous(it));
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::last(IEvtSelector::Context& it) const {
  if (it.identifier() == m_endIter->identifier()) {
    ATH_MSG_DEBUG( "last(): Last event in InputStream." );
    return(StatusCode::SUCCESS);
  }
  return (StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::resetCriteria(const std::string& /*criteria*/, IEvtSelector::Context& /*ctxt*/) const {
  return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::rewind(IEvtSelector::Context& /*it*/) const {
  ATH_MSG_ERROR( "EventSelectorMuonCalibStream::rewind() not implemented" );
  return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::createAddress(const IEvtSelector::Context& /*it*/,
						       IOpaqueAddress*& iop) const {

  const LVL2_MUON_CALIBRATION::CalibEvent* pre = m_eventSource->currentEvent();
  m_dataProvider->setNextEvent(pre); 
  ATH_MSG_DEBUG( "Calib Event cached in Data Provider " ); 

  iop = new MuonCalibStreamAddress(ClassID_traits<EventInfo>::ID() ,
				"MuonCalibStreamEventInfo",""); 
  ATH_MSG_DEBUG( "MuonCalibStreamAddress created " ); 

  return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorMuonCalibStream::releaseContext(IEvtSelector::Context*& /*it*/) const {
  return(StatusCode::SUCCESS);
}

// Implementation of IInterface::queryInterface.
StatusCode EventSelectorMuonCalibStream::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (riid == IEvtSelector::interfaceID()) {
    *ppvInterface = (IEvtSelector*) this;
  } else if (riid == IProperty::interfaceID()) {
    *ppvInterface = (IProperty*) this;
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }

  addRef();
  return StatusCode::SUCCESS; 
}
