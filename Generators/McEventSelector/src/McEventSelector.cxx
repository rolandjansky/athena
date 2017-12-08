/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	EventSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//  Description: The EventSelector component is able
//              to produce a list of event references given a set of "selection
//               criteria".
//
//	Author     : P. Calafiura
//      Created    : 2/29/00
//      Changes    : 11/8/01 new storegate -- I Hinchliffe
//
//
//====================================================================

// Include files
//stdlib
#include <vector>
//own
#include "McEventSelector/McEventSelector.h"
#include "McEventSelector/McAddress.h"
#include "EventInfo/EventInfo.h"

//framework
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

class McContext : public IEvtSelector::Context {
private:
  const McEventSelector*        m_pSelector;
  std::string                   m_criteria;

  unsigned int m_initRunNo;
  uint64_t     m_initEventNo;
  uint64_t     m_eventsP;
  unsigned int m_initLBNo;
  unsigned int m_eventsPLB;

  unsigned int m_initTimeStamp;
  unsigned int m_timeStampInt;

  unsigned int m_runNo;
  uint64_t     m_eventNo;
  unsigned int m_LBNo;
  unsigned int m_timeStamp;
  unsigned int m_LBTick;
  int          m_nevt;

  bool m_firstEvent;


public:
  /// Standard constructor
  McContext(const McEventSelector* pSelector);
  McContext(const McEventSelector* pSelector,
	    const unsigned int &initRunNo, const uint64_t &initEventNo,
	    const uint64_t &evPR,
	    const unsigned int &initLBNo, const unsigned int &evPLB,
	    const unsigned int &initTimeStamp,
	    const unsigned int &timeStampInt);

  /// Standard destructor
  virtual ~McContext();

  virtual void* identifier() const {
    return (void*)m_pSelector;
  }
  void setCriteria(const std::string& crit) {
    m_criteria = crit;
  }

  void rewind() {

    // Question: should this rewind to before the first event, or to
    // it? ie, will next() be called right after rewind()?
    // if so, then should set m_firstEvent to true;

    // CGL 2011.6.9: by process of experimental programming, we have
    // determined that next() is called right after rewind() for the
    // first event, so set m_firstEvent to true.

    m_firstEvent = true;

    m_eventNo    = m_initEventNo;
    m_runNo      = m_initRunNo;
    m_LBNo       = m_initLBNo;
    m_timeStamp  = m_initTimeStamp;
    m_nevt       = 0;
  }

  void next() {
    m_nevt++; // advance serial event number

    if (m_firstEvent) {
      m_firstEvent = false;

      m_eventNo   = m_initEventNo;
      m_runNo     = m_initRunNo;
      m_LBNo      = m_initLBNo;
      m_timeStamp = m_initTimeStamp;
      m_LBTick=0;

      return;
    }

    // Andy Buckley, 10 Feb 2014: commenting out to reduce Athena evgen noise
    //std::cout <<  "evt no, per run " << m_eventNo << "/" << m_eventsP << std::endl;

    if(m_eventNo>=m_eventsP){
      m_runNo++;
      m_eventNo=0;
      m_LBNo=0; // lumiblock starts at 1 by default
      m_LBTick=0;
    }
    m_eventNo++;
    if (++m_LBTick >= m_eventsPLB) {
      m_LBTick=0;
      ++m_LBNo;
    }
    m_timeStamp += m_timeStampInt;
  }

  void previous() {
    m_nevt--; // decrement serial event number
    m_eventNo--;
    m_timeStamp -= m_timeStampInt;
    if (--m_LBTick <=0) {
      m_LBTick=m_eventsPLB;
      --m_LBNo;
    }
  }

  unsigned int runNumber() const { return m_runNo; }
  uint64_t     eventNumber() const { return m_eventNo; }
  unsigned int lumiBlock() const { return m_LBNo; }
  unsigned int timeStamp() const { return m_timeStamp; }
  int          serialEventNumber() const { return m_nevt; }



  void setRunNumber( unsigned int runNo ) { m_runNo = runNo; }
  void setEventNumber( uint64_t eventNo ) { m_eventNo = eventNo ; }
  void setEventsPerRun( uint64_t eventsP) { m_eventsP = eventsP ; }
  void setTimeStamp( unsigned int time ) { m_timeStamp = time; }
  void setTimeStampInterval( unsigned int intvl ) { m_timeStampInt = intvl; }

};

McContext::McContext(const McEventSelector* pSelector):
  m_pSelector(pSelector),
  m_initRunNo(0),
  m_initEventNo(0),
  m_eventsP(2000000000), // <-- match default property of McEventSelector
  m_initLBNo(0),
  m_eventsPLB(1000),     // <-- ditto
  m_initTimeStamp(0),
  m_timeStampInt(0),
  m_runNo(0),
  m_eventNo(0),
  m_LBNo(0),
  m_timeStamp(0),
  m_LBTick(0),
  m_nevt(0),
  m_firstEvent(true)
{}

McContext::McContext(const McEventSelector* pSelector,
		     const unsigned int &initRunNo,
		     const uint64_t     &initEventNo,
		     const uint64_t     &evPR,
		     const unsigned int &initLBNo,
		     const unsigned int &evPLB,
		     const unsigned int &initTimeStamp,
		     const unsigned int &timeStampInt):
  m_pSelector(pSelector),
  m_initRunNo(initRunNo), m_initEventNo(initEventNo), m_eventsP(evPR),
  m_initLBNo(initLBNo), m_eventsPLB(evPLB),
  m_initTimeStamp(initTimeStamp), m_timeStampInt(timeStampInt),
  m_runNo(0),
  m_eventNo(0),
  m_LBNo(0),
  m_timeStamp(0),
  m_LBTick(0),
  m_nevt(0),
  m_firstEvent(true)
{}


McContext::~McContext() {
}


// IInterface::queryInterface
StatusCode McEventSelector::queryInterface(const InterfaceID& riid,
					   void** ppvIf) {
  if ( riid == IEvtSelector::interfaceID() )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return SUCCESS;
  } else if (riid == IEvtSelectorSeek::interfaceID() ) {
    *ppvIf = dynamic_cast<IEvtSelectorSeek*>(this);
    addRef();
    return SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );
}



McEventSelector::McEventSelector( const std::string& name, ISvcLocator* svcloc ) :
        AthService( name, svcloc),
        m_ctx(0)
{

  declareProperty( "RunNumber",           m_runNo = 0 );
  m_runNo.verifier().setLower( 0 );
  declareProperty( "EventsPerRun",        m_eventsPerRun = 2000000000 );
  m_eventsPerRun.verifier().setLower( 0 );
  declareProperty( "FirstEvent",          m_firstEventNo = 0 );
  m_firstEventNo.verifier().setLower( 0 );
  declareProperty( "FirstLB",             m_firstLBNo = 0);
  m_firstLBNo.verifier().setLower(0);
  declareProperty( "EventsPerLB",         m_eventsPerLB = 1000 );
  m_eventsPerLB.verifier().setLower( 0 );
  declareProperty( "InitialTimeStamp",    m_initTimeStamp = 0);
  m_initTimeStamp.verifier().setLower( 0 );
  declareProperty( "TimeStampInterval",   m_timeStampInterval = 0 );

  /// Flags to indicate override of run/event/time
  ///  These are always true and are here for consistency
  ///  with other event selectors which only optionally
  ///  override these numbers.
  declareProperty( "OverrideRunNumber",   m_overrideRunNumber = true );
  declareProperty( "OverrideEventNumber", m_overrideEventNumber = true );
  declareProperty( "OverrideTimeStamp",   m_overrideTimeStamp = true );
  declareProperty( "OverrideLBNumber",    m_overrideLBNumber = true );

}

McEventSelector::~McEventSelector() {
  // FIXME: who deletes the Context?
}

StatusCode
McEventSelector::createContext(Context*& refpCtxt) const
{
  McContext* ctx =  new McContext(this,
				  (unsigned int) m_runNo.value( ),
				  (uint64_t)     m_firstEventNo.value( ),
				  (uint64_t)     m_eventsPerRun.value( ),
				  (unsigned int) m_firstLBNo.value( ),
				  (unsigned int) m_eventsPerLB.value( ),
				  (unsigned int) m_initTimeStamp.value( ),
				  (unsigned int) m_timeStampInterval.value( )
				  );
  refpCtxt = ctx;

  m_ctx = ctx;

  return StatusCode::SUCCESS;
}

StatusCode McEventSelector::initialize()     {
  ATH_MSG_INFO (" Enter McEventSelector Initialization ");
  StatusCode sc = AthService::initialize();
  msg().setLevel( m_outputLevel.value() );
  if( sc.isSuccess() ) {
    setProperties().ignore();
  } else {
    ATH_MSG_ERROR ("Unable to initialize service ");
    return sc;
  }

  ATH_MSG_DEBUG (" McEventSelector Initialized Properly ... ");

  return sc;
}

StatusCode McEventSelector::stop()     {
  ATH_MSG_DEBUG ("............. stop .............");

  const bool createIf = true;
  IIncidentSvc* incSvc = 0;
  if ( !service("IncidentSvc", incSvc, createIf).isSuccess() || 0 == incSvc ) {
    ATH_MSG_ERROR ("Could not retrieve IncidentSvc ");
    return StatusCode::FAILURE;
  }
  Incident lastInputIncident(name(), "LastInput");
  incSvc->fireIncident(lastInputIncident);

  return StatusCode::SUCCESS;
}

StatusCode McEventSelector::finalize()     {
  ATH_MSG_INFO ("finalize");

  return StatusCode::SUCCESS;
}

// IEvtSelector::next
StatusCode
McEventSelector::next(Context& ctxt) const {
  ATH_MSG_DEBUG ("............. Next Event .............");

  McContext* ct = dynamic_cast<McContext*>(&ctxt);
  StatusCode sc(StatusCode::FAILURE);

  if (ct != 0 ) {
    ct->next();
    sc = StatusCode::SUCCESS;
  } else {
    ATH_MSG_ERROR ("Could not dcast to McContext");
  }

  m_ctx = ct;

  return sc;

}
StatusCode
McEventSelector::next(Context& ctxt,int jump) const
{
  ATH_MSG_DEBUG ("............. Next (" << jump << ") .............");

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


// IEvtSelector::previous
StatusCode
McEventSelector::previous(Context& ctxt) const {
  ATH_MSG_DEBUG ("............. previous .............");

  McContext* ct = dynamic_cast<McContext*>(&ctxt);

  StatusCode sc(StatusCode::FAILURE);

  if (ct != 0 ) {
    ct->previous();
    sc = StatusCode::SUCCESS;
  } else {
    ATH_MSG_ERROR ("Could not dcast to McContext");
  }

  m_ctx = ct;

  return sc;

}

StatusCode
McEventSelector::previous(Context& ctxt, int jump) const
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode
McEventSelector::last(Context& /*ctxt*/) const {
  ATH_MSG_ERROR ("............. Last Event Not Implemented .............");
  return StatusCode::FAILURE;
}


StatusCode
McEventSelector::rewind(Context& ctxt) const
{
  ATH_MSG_DEBUG ("............. rewind .............");

  McContext* ct = dynamic_cast<McContext*>(&ctxt);
  StatusCode sc(StatusCode::FAILURE);

  if (ct != 0 ) {
    ct->rewind();
    sc = StatusCode::SUCCESS;
  } else {
    ATH_MSG_ERROR ("Could not dcast to McContext");
  }

  m_ctx = ct;
  return sc;

}


StatusCode
McEventSelector::createAddress(const Context& refCtxt,
			       IOpaqueAddress*& addr) const {

  ATH_MSG_DEBUG ("............. createAddress .............");

  const McContext* ctx = dynamic_cast<const McContext*>( &refCtxt );

  if (ctx != 0) {
    addr = new McAddress(EventInfo::classID(), "McEventInfo",
			 ctx->runNumber(), ctx->eventNumber(),
			 ctx->lumiBlock(),ctx->timeStamp());
    m_ctx = const_cast<McContext*> (ctx);
  } else {
    ATH_MSG_ERROR ("casting to a McContext");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode
McEventSelector::releaseContext(Context*& /*refCtxt*/) const {

  // Nothing to do.
  return StatusCode::SUCCESS;

}

StatusCode
McEventSelector::resetCriteria(const std::string&, Context& ) const {

  ATH_MSG_ERROR ("............. resetCriteria Not Implemented .............");
  return StatusCode::FAILURE;

}

StatusCode
McEventSelector::seek(Context& /*refCtxt*/, int evtNum) const {

  ATH_MSG_DEBUG ("............. seek(" << evtNum << ") .............");

  m_ctx->rewind();

  for (int i=0; i < evtNum; ++i) {
    m_ctx->next();
  }

  return StatusCode::SUCCESS;
}

int
McEventSelector::curEvent (const Context& /*refCtxt*/) const {

  ATH_MSG_DEBUG ("............. curEvent .............");


  return m_ctx->serialEventNumber();
}

int
McEventSelector::size (Context& /*refCtxt*/) const
{
  return -1;
}

