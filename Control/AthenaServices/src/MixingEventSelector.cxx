/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MixingEventSelector.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "EventInfo/MergedEventInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/getMessageSvc.h"

#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GenericAddress.h"
#include "CLHEP/Random/RandFlat.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <string>

using namespace std;
using boost::lexical_cast;
using boost::tokenizer;
using boost::char_separator;
using SG::DataProxy;

#ifdef DEBUG_OUTPUT_STATUS
ofstream outfile2("status.txt");
#endif

MixingEventSelector::MixingEventSelector(const string& name, ISvcLocator* svc) :
  extends<AthService, IEvtSelector, IConverter>(name,svc),
  m_helperTools(this),
  m_trigList(), m_pCurrentTrigger(m_trigList.end()),
  m_eventPos(0), 
  m_pEventStore( "StoreGateSvc", name ),
  m_atRndmSvc("AtRndmGenSvc", name),
  m_randomStreamName("MixingEventSelectorStream"),
  m_chooseRangeRand(nullptr)
{
  declareProperty("TriggerList", m_triggerListProp,
		  "list of triggers (streams) to be used. Format is SelectorType/SelectorName:firstEventToUse:lastEventToUse. One assumes events are consecutively numbered.");
  m_triggerListProp.declareUpdateHandler(&MixingEventSelector::setUpTriggerList, this);
  declareProperty("OutputRunNumber", m_outputRunNumber=123456789);
  declareProperty("EventNumbers", m_eventNumbers, 
		  "list of event numbers to be used for output stream. If list empty or not long enough, event numbers are assigned consucutively after last one in list");
  declareProperty("StreamStatusFileName", m_statusFileName=string(),
		  "Name of the file recording the last event used and how many were available for each stream. Default is to produce no file." ); //no out
  declareProperty("MergedEventInfoKey", 
		  m_mergedEventInfoKey=string("MergedEventInfo"),
		  "StoreGate key for output (merged) event info object. Default is MergedEventInfo ");
  declareProperty("RndmGenSvc", m_atRndmSvc, "IAtRndmGenSvc controlling the order with which events are takes from streams");
  declareProperty("RndmStreamName", m_randomStreamName, "IAtRndmGenSvc stream used as engine for our random distributions");   
  declareProperty("HelperTools", m_helperTools, "a collection of selector tools");

}

MixingEventSelector::~MixingEventSelector() { 
}

StatusCode
MixingEventSelector::initialize() {
  ATH_MSG_INFO ("Initializing " << name()
		<< " - package version " << PACKAGE_VERSION);
// defer this (it triggers a init loop via PPS 
//   if (!m_pEventStore.retrieve().isSuccess()) 
//     return StatusCode::FAILURE;

  //setup random stream
  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(nullptr == collEng ) {
    ATH_MSG_ERROR ("can not get random stream " << m_randomStreamName.value());
    return StatusCode::FAILURE;
  }
  //flat distribution in [0,1] range
  m_chooseRangeRand = new CLHEP::RandFlat(*(collEng), 0.0, 1.0);

  StatusCode sc= m_helperTools.retrieve();
  std::vector<ToolHandle<IAthenaSelectorTool> >::iterator 
    i(m_helperTools.begin()), iE(m_helperTools.end());
  while (sc.isSuccess() && (i != iE)) sc = (*i++)->postInitialize();

  return sc;
}

StatusCode
MixingEventSelector::finalize() {
  ATH_MSG_DEBUG ("Finalizing " << name()
		 << " - package version " << PACKAGE_VERSION);

  StatusCode sc(StatusCode::SUCCESS);
  std::vector<ToolHandle<IAthenaSelectorTool> >::iterator 
    i(m_helperTools.begin()), iE(m_helperTools.end());
  while (i != iE) ((*i++)->preFinalize()).ignore();

  const std::string& fname(m_statusFileName.value());
  ofstream outfile(fname.c_str());
  if ( outfile ) {
    TriggerList::const_iterator i(m_trigList.begin());
    TriggerList::const_iterator iEnd(m_trigList.end());
    while (i != iEnd) outfile << (*i++).toString();
  }  else if (!fname.empty()) {
    ATH_MSG_WARNING("unable to open trigger list status file " << fname);
  }

  m_helperTools.release().ignore();

  return sc;

}


void
MixingEventSelector::setUpTriggerList(Property&) {
  m_trigList = TriggerList();
  for_each(m_triggerListProp.value().begin(), m_triggerListProp.value().end(),  
	   bind1st(mem_fun(&MixingEventSelector::decodeTrigger), this));
  setCurrentTrigger();
}

void 
MixingEventSelector::decodeTrigger(string triggDescr) {
  //all this would be much more readable without error reporting...
  typedef  tokenizer<char_separator<char> > Tokenizer;
  Tokenizer tokens(triggDescr, char_separator<char>(" :"));
  //we need exactly three tokens: selector name, first and last event 2 B read
  if ( (distance(tokens.begin(), tokens.end()) == 3) ||
       (distance(tokens.begin(), tokens.end()) == 3) ){
    Tokenizer::iterator iToken(tokens.begin());
    try {
      ListItem selTN(*iToken++);
      const bool CREATEIF(true);
      //get selector
      IEvtSelector* pSelector(nullptr);
      if ((serviceLocator()->service(selTN.type(), selTN.name(), pSelector, CREATEIF)).isSuccess()) {
	//must be an address provider
	if (nullptr != dynamic_cast<IEvtSelector*>(pSelector) ) {
	  //FIXME	  if (!pSelector.done()) {
	    //try to add to trig list
	    unsigned int firstEvt(boost::lexical_cast<unsigned int>(*iToken++));
	    unsigned int lastEvt(boost::lexical_cast<unsigned int>(*iToken));
	    if (m_trigList.add(Trigger(pSelector, firstEvt, lastEvt))) {
	      if (msgLvl(MSG::DEBUG)) {
		INamedInterface *pNamed(dynamic_cast<INamedInterface*>(pSelector));
		if (nullptr != pNamed) {
		  msg() << "decodeTrigger: added selector " << pNamed->name() 
			<< " first event to be read " << firstEvt
			<< " last event to be read " << lastEvt << endmsg;
		}
	      }
	    } else {
	      ATH_MSG_ERROR 
		("decodeTrigger: Selector [" 
		 << selTN.type() << '/' << selTN.name() 
		 << "] not added");
	    } //can add to range
// 	  } else {
// 	    log << MSG::ERROR << "decodeTrigger: Selector [" 
// 		<< selTN.type() << '/' << selTN.name() 
// 		<< "] is empty, not added" << endmsg;
// 	  } //selector empty
	} else {
	  ATH_MSG_ERROR
	    ("decodeTrigger: Selector [" 
	     << selTN.type() << '/' << selTN.name() 
	     << "] not an IEvtSelector, not added");
	} //is address provider
      } else {
        ATH_MSG_ERROR 
	  ("decodeTrigger: Selector [" 
	   << selTN.type() << '/' << selTN.name() 
	   << "] can not be found or created");
      } //selector available
    } catch (boost::bad_lexical_cast e) {
      ATH_MSG_ERROR
	("decodeTrigger: Can't cast ["<< *iToken 
	 << "] to double(frequency). SKIPPING");
    } //can cast to frequency
  } else {
    ATH_MSG_ERROR
      ("decodeTrigger: Badly formatted descriptor [" 
       << triggDescr << "]. SKIPPING");
  } //can parse property string
}

#ifndef NDEBUG
#define FORWARD___DEBUG( METHOD )\
  { ATH_MSG_VERBOSE ("forwarding " << #METHOD << " to service "\
  << (validTrigger() ? currentTrigger()->name() : "NONE")); }
#else
#define FORWARD___DEBUG( METHOD )
#endif

StatusCode MixingEventSelector::createContext(IEvtSelector::Context*& pctxt) const {
  pctxt=nullptr;
  if (!validTrigger()) return StatusCode::FAILURE;
  TriggerList::iterator iTr(m_trigList.begin());
  StatusCode sc(StatusCode::FAILURE);
  while (iTr != m_trigList.end() && (sc=(iTr++)->createContext(pctxt)).isSuccess()) ;
  return sc;
}

StatusCode
MixingEventSelector::next(IEvtSelector::Context& /*c*/) const {
  StatusCode sc(StatusCode::FAILURE);
  do {
#ifdef DEBUG_OUTPUT_STATUS
    TriggerList::const_iterator i(m_trigList.begin()); 
    TriggerList::const_iterator iEnd(m_trigList.end());
    while (i != iEnd) outfile2 << (*i++).toString();  //output status
#endif
    setCurrentTrigger();
    sc = currentTrigger()->next();
    if (sc.isFailure()){
      m_trigList.remove(m_pCurrentTrigger);
      continue;  // we have to add  "continue;" after "m_trigList.remove(m_pCurrentTrigger);
    }
  } while ((sc.isFailure() || currentTrigger()->done()) &&  !m_trigList.empty() );
  // use !m_trigList.empty() to replace validTrigger(), which is not working well

  std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator 
    i(m_helperTools.begin()), iE(m_helperTools.end());
  while (i != iE) ((*i++)->preNext()).ignore();

  ++m_eventPos;
  FORWARD___DEBUG( next );  

  i=m_helperTools.begin(); iE=m_helperTools.end();
  while (sc.isSuccess() && (i != iE)) {
    sc =(*i)->postNext();
    if (sc.isRecoverable()) 
      ATH_MSG_INFO("Request skipping event from: " << (*i)->name());
    else if (sc.isFailure()) 
      ATH_MSG_WARNING((*i)->name() << ":postNext failed");
    ++i;
  }
  return sc;
}


StatusCode
MixingEventSelector::createAddress(const IEvtSelector::Context& /*c*/, 
				   IOpaqueAddress*& pAddr) const { 
  pAddr=nullptr;
  if (!validTrigger()) return StatusCode::FAILURE;

  FORWARD___DEBUG( createAddress )
  return (currentTrigger()->createAddress(pAddr).isSuccess() &&
          pAddr != nullptr) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode
MixingEventSelector::preLoadAddresses(StoreID::type /*storeID*/,
                                     IAddressProvider::tadList& /*tads*/  ) {
    return StatusCode::SUCCESS;
}

StatusCode
MixingEventSelector::loadAddresses(StoreID::type  storeID,
                                   IAddressProvider::tadList& /*tads*/  )
{
  if (storeID != StoreID::EVENT_STORE && storeID != StoreID::PILEUP_STORE)
    return StatusCode::SUCCESS;

  CLID mclid = ClassID_traits<MergedEventInfo>::ID();
  if (!m_pEventStore->contains (mclid, m_mergedEventInfoKey)) {
    // We create the DataProxy here rather than relying on ProxyProvideSvc
    // to do it because we want to set a non-default dataloader on the proxy.
    SG::TransientAddress tad (mclid,
                              m_mergedEventInfoKey,
                              new GenericAddress (0, mclid),
                              false);
    auto dp = std::make_unique<SG::DataProxy> (std::move(tad),
                                               this,
                                               true);
    ATH_CHECK( m_pEventStore->addToStore (mclid, dp.release()) );
  }
  return StatusCode::SUCCESS;
}

StatusCode
MixingEventSelector::updateAddress(StoreID::type,SG::TransientAddress*,
                                   const EventContext&) {
  return StatusCode::FAILURE;
}

#undef FORWARD___DEBUG

unsigned long MixingEventSelector::getEventNo() const {
  return ( (m_eventNumbers.value().size() > m_eventPos) ?
	   m_eventNumbers.value()[m_eventPos] :
	   m_eventPos );	  
}

//remember that m_pCurrentTrigger and m_trigList are mutable
MixingEventSelector::TriggerList::iterator 
MixingEventSelector::setCurrentTrigger() const { 
  //we look for the first trigger which is not done
  do {
    //if called before initialize, choose the last trigger in list
    unsigned int iTrig = (nullptr != m_chooseRangeRand) ?
      (unsigned int)(m_chooseRangeRand->fire() * double(m_trigList.todo())) :
      m_trigList.todo();
    m_pCurrentTrigger = m_trigList.elementInRange(iTrig);
  } while (validTrigger() && m_pCurrentTrigger->done() && 
	   m_trigList.remove(m_pCurrentTrigger)); //remove a done trigger

  //printouts
  if (!validTrigger()) {
    ATH_MSG_INFO ("setCurrentTrigger: end of input");
  } else {
    ATH_MSG_DEBUG ("setCurrentTrigger: now using selector " 
		   << currentTrigger()->name());
  }

  return m_pCurrentTrigger;
}

MixingEventSelector::TriggerList::iterator 
MixingEventSelector::currentTrigger() const { 
  return ( validTrigger() ? m_pCurrentTrigger : setCurrentTrigger() ); 
}

bool
MixingEventSelector::TriggerList::add(const Trigger& trig) {
  bool newElem(end() == find(begin(), end() , trig));
  if (newElem) {
    m_todo += trig.todo();
    m_trigs.push_back(trig);
    m_rangeEnd.push_back(m_todo);
#ifndef NDEBUG
    MsgStream log(Athena::getMessageSvc(), "MixingEventSelector::TriggerList");
    if (log.level() <= MSG::DEBUG) {
      log << MSG::DEBUG << "add: trigger " 
	  << trig.name() << ". New trigger table: \n" << toString() <<endmsg;
    }
#endif
  }
  return newElem;
}

bool
MixingEventSelector::TriggerList::remove(iterator iTrig) {
  bool removed(false);
  unsigned int nElem(m_trigs.size());
  if (nElem) {
    unsigned int trigTodo(iTrig->todo());
    unsigned int trigIndex(iTrig - begin());
#ifndef NDEBUG
    string trigName(iTrig->name());
#endif
    m_trigs.erase(iTrig);
    removed = nElem > m_trigs.size();
    if (removed) {
      //this means we erased iTrig. Remove its todo count from total 
      m_todo -= trigTodo;
      //and from each of the subsequent ranges
      for (unsigned int i=trigIndex+1; i<m_rangeEnd.size(); ++i) 
	m_rangeEnd[i] -= trigTodo;
      //finally erase the corresponding rangeEnd element
      m_rangeEnd.erase(m_rangeEnd.begin()+trigIndex);
#ifndef NDEBUG
      MsgStream log(Athena::getMessageSvc(), "MixingEventSelector::TriggerList"); //FIXME
      if (log.level() <= MSG::DEBUG) {
	log << MSG::DEBUG << "remove: trigger " 
	    << trigName
	    << ". New trigger table: \n" << toString() <<endmsg;
      }
#endif
    }      
  }
  return removed;
}

string
MixingEventSelector::TriggerList::toString() const {
  ostringstream os;
  for (unsigned int i=0; i<m_trigs.size(); ++i) {
    //cant do    os << m_trigs[i].toString();
    os << m_trigs[i].name() << ", already read=" << m_trigs[i].read()
       << ", to do=" << m_trigs[i].todo() 
       << (m_trigs[i].done() ? " done " : " ") 
       << " -  endRange: " << m_rangeEnd[i] << '\n';
  }
  os << endl;
  return string(os.str()); 
}

MixingEventSelector::TriggerList::iterator 
MixingEventSelector::TriggerList::elementInRange(unsigned int i) {
  iterator iTrig(m_trigs.begin()), eTrig(m_trigs.end());
  std::vector<unsigned int>::const_iterator iRange(m_rangeEnd.begin());
  std::vector<unsigned int>::const_iterator eRange(m_rangeEnd.end());
  while (eRange != iRange && eTrig != iTrig && i > *iRange){
    ++iRange; ++iTrig;
  }
#ifndef NDEBUG
  MsgStream log(Athena::getMessageSvc(), "MixingEventSelector::TriggerList"); 
  if (log.level() <= MSG::VERBOSE) {
    if (eRange != iRange) log << MSG::VERBOSE 
			      << "elementInRange: rangeEnd=" << *iRange 
			      << " matches i=" << i << endmsg;
  }
#endif
  return iTrig;
}

MixingEventSelector::Trigger::Trigger(IEvtSelector* pSel,
                                      unsigned int firstEvt,
                                      unsigned int lastEvt): 
  m_pSelector(pSel), m_firstEvent(firstEvt), 
  m_toRead(lastEvt+1), m_todo(m_toRead-firstEvt), m_reads(0),
  m_current(nullptr)
  {   }

IEvtSelector::Context& MixingEventSelector::Trigger::currentContext() const {
  //make sure we have a context!
  if (nullptr == m_current) {
    IEvtSelector::Context* pTmp(nullptr);
    if ((createContext(pTmp)).isFailure()) 
      throw GaudiException("MixingEventSelector::Trigger::currentContext(): can't create context",
			   name(),StatusCode::FAILURE);
  }
  return *m_current;
}

std::string
MixingEventSelector::Trigger::toString() const { 
  ostringstream os;
  os << name() << ", already read=" << m_reads 
     << ", to do=" << todo() << endl;
  return string(os.str()); 
}

StatusCode
MixingEventSelector::Trigger::next() const{
  StatusCode sc(StatusCode::FAILURE);
  if (!done()) {
    sc= selector().next(currentContext());
    while (sc.isSuccess() && (++m_reads < m_firstEvent))  
      sc = selector().next(currentContext());// m_reads, m_current are mutable
  }
  //  cout << "calling next on Trigger " << this << " " << dynamic_cast<IService&>(selector()).name() << endl;
  return sc;
}

StatusCode
MixingEventSelector::Trigger::previous() const{
  return selector().previous(currentContext());
}

StatusCode
MixingEventSelector::Trigger::createAddress(IOpaqueAddress*& pIOA) const {
  return (done() ? 
	  StatusCode::FAILURE :
	  selector().createAddress(currentContext(), pIOA) ); 
}

StatusCode
MixingEventSelector::Trigger::createContext(IEvtSelector::Context*& pCtxt) const{
  StatusCode sc(StatusCode::FAILURE);
  if (!done()) {
    sc = selector().createContext(pCtxt);
    m_current = pCtxt;
  }
  return sc;
}

bool 
MixingEventSelector::Trigger::done() const { 
  //  cout << "calling done on Trigger " << this << " " << dynamic_cast<IService&>(selector()).name() << endl;
  //  cout << refCtxt << " " << selector().end() << endl;
  //  cout << toRead() << " " << m_reads << endl;
  //  cout << "done returns " << ((m_current && (currentContext() == *selector().end())) || (toRead() <= m_reads)) << endl; 
  return  (toRead() <= m_reads);
}


StatusCode MixingEventSelector::last(IEvtSelector::Context&) const {
  return StatusCode::FAILURE;
}
StatusCode MixingEventSelector::next(IEvtSelector::Context&, int) const {
  return StatusCode::FAILURE;
}
StatusCode MixingEventSelector::previous(IEvtSelector::Context&) const {
  return StatusCode::FAILURE;
}
StatusCode MixingEventSelector::previous(IEvtSelector::Context&,int) const {
  return StatusCode::FAILURE;
}
StatusCode MixingEventSelector::rewind(IEvtSelector::Context&) const {
  return StatusCode::FAILURE;
}
StatusCode MixingEventSelector::releaseContext(IEvtSelector::Context*&) const {
  return  StatusCode::FAILURE;
}
StatusCode MixingEventSelector::resetCriteria(const std::string&,
					      IEvtSelector::Context&)const {
  return StatusCode::FAILURE;
}


// IConverter implementation.
const CLID& MixingEventSelector::objType() const
{
  return ClassID_traits<EventInfo>::ID();
}
long MixingEventSelector::repSvcType() const
{
  return 0;
}


/** Create the transient representation of an object.
    The transient representation is created by loading the
    persistent object using the source information
    contained in the address.
    @return    Status code indicating success or failure
    @param     pAddress   Opaque address information to retrieve the
    requested object from the store in order to
    produce the transient object.
    @param     refpObject Reference to location of pointer of the
    created object.
*/
StatusCode MixingEventSelector::createObj(IOpaqueAddress* /*pAddress*/,
                                          DataObject*& refpObject)
{
  const EventInfo* pEInfo(nullptr);
  if (!(m_pEventStore->retrieve(pEInfo)).isSuccess()) {
    ATH_MSG_ERROR 
      ("addMergedEventInfo: event store does not contain "\
       "an EventInfo object!");
    return StatusCode::RECOVERABLE;
  }

  // copy the original event to a new MergedEventInfo
  auto mei = std::make_unique<MergedEventInfo> (*pEInfo,
                                                m_outputRunNumber,
                                                getEventNo());
  refpObject = SG::asStorable (std::move (mei));


  //finally slam a new EventID in place of the old! HACK!!!  FIXME!!
  (const_cast<EventInfo*>(pEInfo))->setEventID(
			         new EventID(m_outputRunNumber, getEventNo()));

  return StatusCode::SUCCESS;
}


// IConverter dummies.
StatusCode MixingEventSelector::setDataProvider(IDataProviderSvc* /*pService*/)
{
  std::abort();
}
SmartIF<IDataProviderSvc>& MixingEventSelector::dataProvider() const
{
  std::abort();
}
StatusCode MixingEventSelector::setConversionSvc(IConversionSvc* /*pService*/)
{
  std::abort();
}
SmartIF<IConversionSvc>& MixingEventSelector::conversionSvc()    const
{
  std::abort();
}
StatusCode MixingEventSelector::setAddressCreator(IAddressCreator* /*creator*/)
{
  std::abort();
}
SmartIF<IAddressCreator>& MixingEventSelector::addressCreator()    const
{
  std::abort();
}
StatusCode MixingEventSelector::fillObjRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
{
  std::abort();
}
StatusCode MixingEventSelector::updateObj(IOpaqueAddress* /*pAddress*/, DataObject* /*refpObject*/)
{
  std::abort();
}
StatusCode MixingEventSelector::updateObjRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
{
  std::abort();
}
StatusCode MixingEventSelector::createRep(DataObject* /*pObject*/, IOpaqueAddress*& /*refpAddress*/)
{
  std::abort();
}
StatusCode MixingEventSelector::fillRepRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
{
  std::abort();
}
StatusCode MixingEventSelector::updateRep(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/) 
{
  std::abort();
}
StatusCode MixingEventSelector::updateRepRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
{
  std::abort();
}
