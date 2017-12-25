/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*#define DEBUG_PILEUP 1*/
#include <cassert>
#include <cmath>      /*ceil,sqrt*/
#include <functional> /* mem_fun_ref*/
#include <stdexcept>  /*runtime_error*/
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/ActiveStoreSvc.h" 
#include "StoreGate/StoreGateSvc.h" /*to print name() */
#include "GaudiKernel/IEvtSelector.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "EventInfo/EventID.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "BkgStreamsConcentricCache.h"

using namespace std;

BkgStreamsConcentricCache::BkgStreamsConcentricCache( const std::string& type, 
						      const std::string& name,
						      const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  p_activeStore(0),
  m_cursor(),
  m_streams(),
  m_nXings(0),
  m_nStores(0),
  m_rings(),
  m_collXing(23.0),
  m_occupationFraction(1.0),
  m_collDistrName("Poisson"),
  m_selecName("FakeEventSelector", name),
  m_readDownscale(150),
  m_atRndmSvc("AtRndmGenSvc", name),
  m_randomStreamName("PileUpCollXingStream"),
  m_pileUpEventTypeProp(0),
  m_pileUpEventType(PileUpTimeEventIndex::Signal),
  m_ringsProp(),
  m_allowRingMigProp(false),
  m_readEventRand(0),
  m_chooseEventRand(0),
  m_collXingPoisson(0),
  m_f_collDistr(0),
  m_collXingSF(1.0),
  m_ignoreSF(false)
{   
  declareProperty("CollPerXing", m_collXing, "(average) number of collisions per beam crossing");  
  declareProperty("OccupationFraction", m_occupationFraction, "The maximum fraction of bunch-crossings which will be occupied."); 
  declareProperty("CollDistribution", m_collDistrName, "nEvts/Xings can be either Fixed at CollPerXing or Poisson with average CollPerXing");
  declareProperty("EventSelector", m_selecName);
  declareProperty("ReadDownscaleFactor", m_readDownscale, "read one event every downscaleFactor accesses (asymptotically -> number of times an event in the cache will be reused)"); 
  declareProperty("RndmGenSvc", m_atRndmSvc, "IAtRndmGenSvc controlling the distribution of bkg events/xing");
  declareProperty("RndmStreamName", m_randomStreamName, "IAtRndmGenSvc stream used as engine for our various random distributions, including the CollPerXing one ");  
  declareProperty("PileUpEventType", m_pileUpEventTypeProp, "Type of the pileup events in this cache: 0:Signal, 1:MinimumBias, 2:Cavern, 3:HaloGas, 4:ZeroBias");
  m_pileUpEventTypeProp.verifier().setUpper(PileUpTimeEventIndex::NTYPES-2);
  m_pileUpEventTypeProp.declareUpdateHandler(&BkgStreamsConcentricCache::PileUpEventTypeHandler, this);
  declareProperty("NonIntersectingRings", m_ringsProp,
		  "Array of rings specified in the form lowXing:hiXing. Rings must not intersect (e.g. -7:7, -3:0, -1:0) or an exception will be thrown");
  m_ringsProp.declareUpdateHandler(&BkgStreamsConcentricCache::RingsPropHandler, this);
  //add default (huge) ring to make cache work if NonIntersectingRings never set
  m_rings.push_back(Ring());
  declareProperty("AllowRingMigration", m_allowRingMigProp, "Allow events in inner rings to be used in outer rings. When false every ring is effectively a separate cache");
  declareProperty("IgnoreBeamLumi", m_ignoreSF, "Default=False, set to True to ignore the PileUpEventLoopMgr beam luminosity tool in setting the number of events per xing.");
}

BkgStreamsConcentricCache::~BkgStreamsConcentricCache() {
  delete m_collXingPoisson;
  delete m_chooseEventRand;
  delete m_readEventRand;
}

StatusCode
BkgStreamsConcentricCache::queryInterface(const InterfaceID& riid, void** ppvif) {
  if ( riid == IBkgStreamsCache::interfaceID() ) {
    *ppvif = (IBkgStreamsCache*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthAlgTool::queryInterface( riid, ppvif );
}

void
BkgStreamsConcentricCache::PileUpEventTypeHandler(Property&) {
  m_pileUpEventType=PileUpTimeEventIndex::ushortToType(m_pileUpEventTypeProp.value());
}

void
BkgStreamsConcentricCache::RingsPropHandler(Property&) {
  const vector<string>& rRings = m_ringsProp.value();
  if (!rRings.empty()) {
    vector<string>::const_iterator iSR(rRings.begin()), eSR(rRings.end());
    while (iSR!=eSR) m_rings.push_back(Ring(*iSR++));
    //this will throw an exception if the rings are intersecting
    sort(m_rings.begin(), m_rings.end());
  }
}

StatusCode
BkgStreamsConcentricCache::setup(int firstXing,
				 unsigned int nXings, 
				 unsigned int firstStore,
				 IBeamIntensity*)
{
  assert (0 < nXings);
  m_nXings = nXings;

  //readjust all rings so that they count from 0 as the code expects
  vector<Ring>::iterator iR(m_rings.begin()), eR(m_rings.end());
  while (iR!=eR) (iR++)->adjust(-firstXing); 


  //ceil (cmath) rounds up to the nearest integer ceil(1.5)= 2
  float occupiedCrossings = ceil(static_cast<float>(m_nXings) * m_occupationFraction);
  m_nStores = static_cast<unsigned int>(ceil( m_collXing * occupiedCrossings ));
  if (m_collDistrName.value() == "Poisson") {
    //allow for fluctuations in # of overlaid events 
    //allow for one sided migration among rings
    //FIXME just a guess this needs to be optimized
    m_nStores += ( m_allowRingMigProp.value() ?
		   static_cast<unsigned int>(6.0 * m_rings.size() * sqrt(static_cast<float>(m_nStores))) :
		   static_cast<unsigned int>(6.0 * sqrt(static_cast<float>(m_nStores))));
  }
  if (m_allowRingMigProp.value() && m_collDistrName.value() == "Fixed") {
    //allow for one sided migration among rings
    //FIXME just a guess this needs to be optimized
    m_nStores += static_cast<unsigned int>(m_rings.size() * sqrt(static_cast<float>(m_nStores)));
  }


  //get bkg selectors, stores and iterators
  m_nEvtsXing.reserve(m_nStores);
  m_streams.reserve(m_nStores);

  const std::string& selecName(m_selecName.name());
  for (unsigned int i=0; i < m_nStores; ++i) {
    std::stringstream bufName;
    bufName << "BkgEvent_" << i + firstStore;
    const std::string& streamName(bufName.str());    
    try {
      m_streams.push_back(PileUpStream(streamName,serviceLocator(),selecName)); 
    } catch (const std::runtime_error& e) {
      ATH_MSG_ERROR ( "Exception thrown while creating PileUpStream " 
		      << streamName << " : " << e.what() );     
      return StatusCode::FAILURE;
    }
    
    if(!(m_streams.back().setupStore())) {
      ATH_MSG_ERROR ( "Can not setup bkg evt store for stream " 
		      << streamName );
      return StatusCode::FAILURE;
    }
  }
  m_cursor = m_streams.begin();

  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(0 == collEng ) {
    ATH_MSG_ERROR (  "can not get random stream " << m_randomStreamName.value() );
    return StatusCode::FAILURE;
  }
  //setup generator to pickup an event store at random from the cache
  //notice how we pass collEng by reference. If ! CLHEP will take ownership...
  m_chooseEventRand = new CLHEP::RandFlat(*(collEng),
					  0.0, double(m_nStores));

  return StatusCode::SUCCESS;
}

void BkgStreamsConcentricCache::resetEvtsPerXingScaleFactor(float sf) {
  if (!m_ignoreSF) m_collXingSF = sf;
}

long BkgStreamsConcentricCache::collXingPoisson(){ 
  if (!m_collXingPoisson) return collXing();
  return m_collXingPoisson->fire(m_collXing * m_collXingSF); 
}

void BkgStreamsConcentricCache::newEvent() { 
  ATH_MSG_DEBUG (  "newEvent called resetting used event set" );
  unsigned int totEvts(0);
  do {
    totEvts = 0;
    for (unsigned int iXing=0; iXing<m_nXings; ++iXing) {
      totEvts += setNEvtsXing(iXing);
      if (totEvts > m_nStores) {
	ATH_MSG_WARNING ( "newEvent: number of required evts exceeds number of available stores "
			  << m_nStores << ". Regenerating bkg sequence for this event \n"
			  << " the total number of bkg events may not exceed average by more than 6 sigmas"
			  );
	break;
      }
    }
  } while(totEvts > m_nStores);

  for_each(m_streams.begin(), m_streams.end(), 
	   mem_fun_ref(&PileUpStream::resetUsed));
}

const EventInfo* BkgStreamsConcentricCache::nextEvent(unsigned int iXing) { 
  const EventInfo* pNextEvt(0);
  //find the first ring containing iXing
  unsigned int iRing(0);
  while (iRing<m_rings.size() && (!m_rings[iRing++].contains(iXing))) { }
  --iRing;
#ifdef DEBUG_PILEUP
  cout <<"nextEvent " << iXing << ' ' << iRing << ' ' << m_rings.size() << endl; 
#endif
  if (iRing<m_rings.size()) {
    StreamVector::size_type iS(0);
    unsigned int attempts(100*m_nStores);
    do { 
      iS = (StreamVector::size_type)m_chooseEventRand->fire();
    } while ((--attempts != 0) && !this->canUse(iS,iRing));
    
    if (0 == attempts) 
      throw GaudiException("BkgStreamsConcentricCache::nextEvent: can not find a suitable stream",
			   name(),
			   StatusCode::FAILURE);

    //set current store to iS
    m_cursor = m_streams.begin();
    std::advance(m_cursor, iS);
    ATH_MSG_DEBUG (  "using stream/store " << iS );
    PileUpStream* pCurrStream(current());
    if (0 != pCurrStream) {
      p_activeStore->setStore(&(pCurrStream->store()));
      //read a new event every downscaleFactor accesses
      bool readEvent(m_readEventRand->fire()<1.0);
      pNextEvt=pCurrStream->nextEventPre(readEvent);
    }
  } //ring found
  return pNextEvt;
}

StatusCode BkgStreamsConcentricCache::nextEvent_passive(unsigned int iXing) { 
  //find the first ring containing iXing
  unsigned int iRing(0);
  while (iRing<m_rings.size() && (!m_rings[iRing++].contains(iXing))) { }
  --iRing;
#ifdef DEBUG_PILEUP
  cout <<"nextEvent " << iXing << ' ' << iRing << ' ' << m_rings.size() << endl; 
#endif
  if (iRing<m_rings.size()) {
    StreamVector::size_type iS(0);
    unsigned int attempts(100*m_nStores);
    do { 
      iS = (StreamVector::size_type)m_chooseEventRand->fire();
    } while ((--attempts != 0) && !this->canUse(iS,iRing));
    
    if (0 == attempts) 
      throw GaudiException("BkgStreamsConcentricCache::nextEvent: can not find a suitable stream",
			   name(),
			   StatusCode::FAILURE);

    //set current store to iS
    m_cursor = m_streams.begin();
    std::advance(m_cursor, iS);
    ATH_MSG_DEBUG (  "using stream/store " << iS );
    PileUpStream* pCurrStream(current());
    if (0 != pCurrStream) {
      p_activeStore->setStore(&(pCurrStream->store()));
      //read a new event every downscaleFactor accesses
      bool readEvent(m_readEventRand->fire()<1.0);
     if(pCurrStream->nextEventPre_Passive(readEvent)) return StatusCode::SUCCESS;
    }
  } //ring found
  return StatusCode::FAILURE;
}

bool BkgStreamsConcentricCache::canUse(StreamVector::size_type iS, unsigned currentRing) {
  assert(currentRing<m_rings.size());
  //can use a stream if belongs to a ring inside the current one
  //if the stream has no ring assigned yet we can of course use it
  PileUpStream& stream(m_streams[iS]);
  bool canUse(!stream.used() && 
	       ( !stream.hasRing() ||
		 (m_allowRingMigProp.value() ?
		  currentRing>=stream.originalIRing() :
		  currentRing==stream.originalIRing()) ) );
#ifndef NDEBUG
  ATH_MSG_VERBOSE ( "canUse: stream " << iS  
		    << (canUse ? " can" : " can not") << " be used in ring " << currentRing
		    << "\n Stream " <<(stream.used() ? "already" : "not yet") 
		    << " used in this event. Stream originally used for ring "
		    << stream.originalIRing()
		    );
#endif
  if (canUse) {
    stream.setUsed();
    stream.setOriginalIRing(currentRing);
#ifdef DEBUG_PILEUP
    cout << "canUse: new stream status : used " << stream.used() << " original ring " << stream.originalIRing() << endl;
#endif
  }    
  return canUse;
} 

PileUpStream* BkgStreamsConcentricCache::current() { 
  if (m_cursor != m_streams.end()) return &*m_cursor;
  //      m_cursor->isNotEmpty()) return &*m_cursor;
  else return 0; //FIXME should reomve empty stream and keep going
}
 
StatusCode BkgStreamsConcentricCache::initialize() { 
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO ( "Initializing " << name()
		 << " - cache for events of type " 
		 << PileUpTimeEventIndex::typeName(m_pileUpEventType)
		 << " - package version " << PACKAGE_VERSION ) ;
  PileUpEventTypeHandler(m_pileUpEventTypeProp);
  //locate the ActiveStoreSvc and initialize our local ptr
  if (!(sc = service("ActiveStoreSvc", p_activeStore)).isSuccess() )  
    {
      ATH_MSG_ERROR (  "Error retrieving ActiveStoreSvc." );
      return sc;
    }

  //create random number generators
  if (!(m_atRndmSvc.retrieve()).isSuccess()) {
    ATH_MSG_ERROR (  "can not get IAtRndmGenSvc " << m_atRndmSvc );
    return StatusCode::FAILURE;
  }
  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(0 == collEng ) {
    ATH_MSG_ERROR (  "can not get random stream " << m_randomStreamName.value() );
    return StatusCode::FAILURE;
  }

  //setup distribution to read a new event every downscaleFactor accesses
  //notice how we pass collEng by reference. If ! CLHEP will take ownership...
  m_readEventRand = new CLHEP::RandFlat(*(collEng),
					0.0, double(m_readDownscale));

  // select collision distribution function
  if (m_collDistrName.value() == "Fixed") 
    m_f_collDistr = boost::bind(&BkgStreamsConcentricCache::collXing, this);      
  else if (m_collDistrName.value() == "Poisson") {
    //pass collEng by reference. If Not CLHEP will take ownership...
    m_collXingPoisson = new CLHEP::RandPoisson(*(collEng), m_collXing);
    // m_f_collDistr will call m_collXingPoisson->fire(m_collXing)  USED TO BE boost::bind(&CLHEP::RandPoisson::fire, m_collXingPoisson); 
    m_f_collDistr =  boost::bind(&BkgStreamsConcentricCache::collXingPoisson, this);
  } else {
    ATH_MSG_ERROR (  m_collDistrName 
		     << " is not a know collision distribution function" );
    sc = StatusCode::FAILURE;
  }
  return sc;
}

unsigned int BkgStreamsConcentricCache::nEvtsXing(unsigned int iXing) const {
  return (iXing + 1 > m_nEvtsXing.size()) ? 0 : m_nEvtsXing[iXing];
}

unsigned int BkgStreamsConcentricCache::setNEvtsXing(unsigned int iXing) {
  if (iXing + 1 > m_nEvtsXing.size())  m_nEvtsXing.resize(2 * iXing + 1);
  m_nEvtsXing[iXing] = m_f_collDistr();
  return m_nEvtsXing[iXing];
}

StatusCode BkgStreamsConcentricCache::addSubEvts(unsigned int iXing,
						 PileUpEventInfo& overEvent,
						 int t0BinCenter) {
  return this->addSubEvts(iXing, overEvent, t0BinCenter, true, 0);
}

StatusCode BkgStreamsConcentricCache::addSubEvts(unsigned int iXing,
						 PileUpEventInfo& overEvent,
						 int t0BinCenter, bool loadEventProxies, unsigned int /*BCID*/) {
  for (unsigned int iEvt=0; iEvt<nEvtsXing(iXing); ++iEvt) {
    StoreGateSvc* pBkgStore(0);
    
    // increment event iterators
    if(!loadEventProxies)
      {
	return this->nextEvent_passive(iXing);
      }
    const EventInfo* pBkgEvent(nextEvent(iXing));

    //check input selector is not empty
    PileUpStream* currStream(current());
    if (0 == pBkgEvent || 0 == currStream) {
      // This is the end of the loop. No more events in the selection
      ATH_MSG_INFO ( "end of loop: background cache has no more events" );
      return StatusCode::FAILURE;
    } else {
      pBkgStore = &(currStream->store());
      ATH_MSG_DEBUG ( "added event " <<  pBkgEvent->event_ID()->event_number() 
		      << " run " << pBkgEvent->event_ID()->run_number()
		      << " from store " 
		      << pBkgStore->name()
		      << " @ Xing " << iXing );
    }
    
    //  register as sub event of the overlaid
    //    ask if sufficient/needed
    overEvent.addSubEvt(t0BinCenter,
			m_pileUpEventType,
			pBkgEvent, pBkgStore);//,BCID); FIXME:Changes needed to PileUpEventInfo to do this.
#ifdef DEBUG_PILEUP
    const EventInfo* pStoreInfo(0);
    if (pBkgStore->retrieve(pStoreInfo).isSuccess() && pStoreInfo && 
	pBkgEvent->event_ID()->event_number() != pStoreInfo->event_ID()->event_number()) {
      ATH_MSG_ERROR ( "added event " <<  pBkgEvent->event_ID()->event_number() 
		      << " run " << pBkgEvent->event_ID()->run_number()
		      << " differ from current store " 
		      << pBkgStore->name()
		      << " event " <<  pStoreInfo->event_ID()->event_number() 
		      << " run " << pStoreInfo->event_ID()->run_number()
		      );
      assert(1);
    }
#endif
  } //loop over evts in xing
  return StatusCode::SUCCESS;
}

StatusCode BkgStreamsConcentricCache::finalize() { 
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO ( "Finalizing " << name()
		 << " - cache for events of type " 
		 << PileUpTimeEventIndex::typeName(m_pileUpEventType)
		 << " - package version " << PACKAGE_VERSION ) ;
  while (sc.isSuccess() && m_streams.size()>0) {
    sc=m_streams.back().finalize();
    m_streams.pop_back();
  }
  return sc;
}
bool 
BkgStreamsConcentricCache::Ring::operator <(const BkgStreamsConcentricCache::Ring& rhs) const {
  //this is not a typo: we want the rings with higher m_lowXing to be
  //less than (inside of) the others
  bool lessLHS(this->m_lowXing > rhs.m_lowXing);

  //throw if the rings are not concentric
  if ((lessLHS && (this->m_hiXing > rhs.m_hiXing)) ||
      ((m_lowXing < rhs.m_lowXing) && (this->m_hiXing < rhs.m_hiXing))){
    cerr << "rings not concentric:\n" 
	 << "this " << this->m_lowXing << ':' << this->m_hiXing << '\n'
	 << "rhs " << rhs.m_lowXing << ':' << rhs.m_hiXing 
	 << endl;
    throw runtime_error("rings not concentric");
  }
  return lessLHS;
}
BkgStreamsConcentricCache::Ring::Ring(const string& prop) :
  m_lowXing(-9999),
  m_hiXing(9999)
{
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(":");
  tokenizer tokens(prop, sep);                    
  bool OK = (distance(tokens.begin(), tokens.end()) == 2);
  if (OK) {
    tokenizer::iterator token(tokens.begin());
    try {
      m_lowXing = boost::lexical_cast<long>(*token++);
      m_hiXing = boost::lexical_cast<long>(*token);
    } catch (const boost::bad_lexical_cast& e) {
      OK = false;
    }
  }
  OK = OK && (m_lowXing <= m_hiXing); 
  if (!OK) {
    cerr << "Badly formatted ring property string: " << prop 
	 << " please use format lowXing:hiXing " << endl;
    throw runtime_error("Badly formatted ring property string");
  }

}
