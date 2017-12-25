/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <cmath>      /*ceil,sqrt*/
#include <stdexcept>  /*runtime_error*/
#include <string>

#include <boost/bind.hpp>

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/ActiveStoreSvc.h" 
#include "StoreGate/StoreGateSvc.h" /*to print name() */
#include "GaudiKernel/IEvtSelector.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "EventInfo/EventID.h"
#include "PileUpTools/IBeamIntensity.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "SplitBkgStreamsCache.h"

#ifdef DEBUG_PILEUP
#include <algorithm> 
#endif


SplitBkgStreamsCache::SplitBkgStreamsCache( const std::string& type, 
					    const std::string& name,
					    const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  p_activeStore(0),
  m_nXings(0),
  m_nStores1(0),
  m_nStores2(0),
  m_meanCollisionsPerBunchCrossing(23.0),
  m_fractionOfCache1Collisions(1.0),
  m_occupationFraction(1.0),
  m_collDistrName("Poisson"),
  m_selecName1("FakeEventSelector1", name),
  m_selecName2("FakeEventSelector2", name),
  m_readDownscale1(150),
  m_readDownscale2(150),
  m_atRndmSvc("AtRndmGenSvc", name),
  m_randomStreamName("PileUpCollXingStream"),
  m_pileUpEventTypeProp(0),
  m_pileUpEventType(PileUpTimeEventIndex::Signal),
  m_subtractBC0(0),
  m_ignoreBM(false),
  m_ignoreSF(false),
  m_forceReadForBC0(true),
  m_readEventRand1(0),
  m_readEventRand2(0),
  m_chooseEventRand1(0),
  m_chooseEventRand2(0),
  m_chooseCacheRand(0),
  m_collXingPoisson(0),  
  m_f_collDistr(0),
  m_f_numberOfBackgroundForBunchCrossing(0),
  m_collXingSF(1.0),
  m_zeroXing(-1),
  m_beamInt(0)
{   
  declareProperty("CollPerXing", m_meanCollisionsPerBunchCrossing, "(average) number of collisions per beam crossing");  
  declareProperty("OccupationFraction", m_occupationFraction, "The maximum fraction of bunch-crossings which will be occupied."); 
  declareProperty("CollDistribution", m_collDistrName, "nEvts/Xings can be either Fixed at CollPerXing or Poisson with average CollPerXing");
  declareProperty("Cache1EventSelector", m_selecName1, "The EventSelector for Cache 1.");
  declareProperty("Cache2EventSelector", m_selecName2, "The EventSelector for Cache 2.");
  declareProperty("PileUpEventType", m_pileUpEventTypeProp, "Type of the pileup events in this cache: 0:Signal, 1:MinimumBias, 2:Cavern, 3:HaloGas, 4:ZeroBias. Default=0 (Signal, Invalid)");
  declareProperty("FractionOfCache1Collisions", m_fractionOfCache1Collisions, "The fraction of the collisions in each bunch crossing that should come from cache1");
  declareProperty("Cache1ReadDownscaleFactor", m_readDownscale1, "read one event every downscaleFactor accesses (asymptotically -> number of times an event in the cache will be reused)"); 
  declareProperty("Cache2ReadDownscaleFactor", m_readDownscale2, "read one event every downscaleFactor accesses (asymptotically -> number of times an event in the cache will be reused)"); 
  declareProperty("RndmGenSvc", m_atRndmSvc, "IAtRndmGenSvc controlling the distribution of bkg events/xing");
  declareProperty("RndmStreamName", m_randomStreamName, "IAtRndmGenSvc stream used as engine for our various random distributions, including the CollPerXing one ");  
  declareProperty("SubtractBC0", m_subtractBC0, "reduce the number of events at bunch xing t=0 by m_subtractBC0. Default=0, set to 1 when using the same type of events (e.g. minbias) for original and background streams");
  m_pileUpEventTypeProp.verifier().setUpper(PileUpTimeEventIndex::NTYPES-2);
  m_pileUpEventTypeProp.declareUpdateHandler(&SplitBkgStreamsCache::PileUpEventTypeHandler, this);
  declareProperty("IgnoreBeamInt", m_ignoreBM, "Default=False, set to True to ignore the PileUpEventLoopMgr beam intensity tool in setting the number of events per xing.");
  declareProperty("IgnoreBeamLumi", m_ignoreSF, "Default=False, set to True to ignore the PileUpEventLoopMgr beam luminosity tool in setting the number of events per xing.");
  declareProperty("ForceReadForBC0",m_forceReadForBC0,"Force events used in the central bunch crossing to be refreshed");
}

SplitBkgStreamsCache::~SplitBkgStreamsCache() 
{
  delete m_collXingPoisson;
  delete m_chooseEventRand1;
  delete m_chooseEventRand2;
  delete m_chooseCacheRand;
  delete m_readEventRand1;
  delete m_readEventRand2;
}

StatusCode
SplitBkgStreamsCache::queryInterface(const InterfaceID& riid, void** ppvif) 
{
  if ( riid == IBkgStreamsCache::interfaceID() ) 
    {
      *ppvif = (IBkgStreamsCache*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  return AthAlgTool::queryInterface( riid, ppvif );
}

void
SplitBkgStreamsCache::PileUpEventTypeHandler(Property&) 
{
  m_pileUpEventType=PileUpTimeEventIndex::ushortToType(m_pileUpEventTypeProp.value());
}

void SplitBkgStreamsCache::calculateCacheSizes()
{
  unsigned int nStores(0);
  float largestElement( ( m_beamInt==0 || m_ignoreBM.value() || m_collDistrName.value() == "Fixed" ) ? 1.0 : m_beamInt->largestElementInPattern() );
  //ceil (cmath) rounds up to the nearest integer ceil(1.5)= 2
  float occupiedCrossings = ceil(static_cast<float>(m_nXings) * m_occupationFraction);
  nStores = static_cast<unsigned int>(ceil( m_meanCollisionsPerBunchCrossing * occupiedCrossings * largestElement));
  if (m_collDistrName.value() == "Poisson") 
    {
      //allow for fluctuations in # of overlaid events
      nStores += static_cast<unsigned int>(6.0 * sqrt(static_cast<float>(nStores)));
    }
 
  m_nStores1 = static_cast<unsigned int>(ceil(m_fractionOfCache1Collisions * static_cast<float>(nStores)));
  m_nStores2 = static_cast<unsigned int>(ceil((1.0-m_fractionOfCache1Collisions) * static_cast<float>(nStores)));

  ATH_MSG_DEBUG ( "Set up " << (m_nStores1 + m_nStores2) << " stores" );

  return;
}

StatusCode
SplitBkgStreamsCache::setup(int firstXing,
			    unsigned int nXings, 
			    unsigned int firstStore,
			    IBeamIntensity* iBM)
{
  assert (0 < nXings);
  m_nXings = nXings;
  m_zeroXing = -firstXing;
  if (m_zeroXing >= static_cast<int>(nXings)) { m_zeroXing = -99; }
  m_beamInt = (m_ignoreBM.value() ? 0 : iBM);

  calculateCacheSizes();

  m_numberOfBkgForBunchCrossing.reserve(nXings);
  m_streams1.reserve(m_nStores1);
  m_streams2.reserve(m_nStores2);
  m_usedStreams1.reserve(m_nStores1);
  m_usedStreams1.assign(m_nStores1, false);
  m_usedStreams2.reserve(m_nStores2);
  m_usedStreams2.assign(m_nStores2, false);

  //Cache 1
  const std::string& selecName1(m_selecName1.name());
  for (unsigned int i=0; i < m_nStores1; ++i) 
    {
      std::stringstream bufName;
      bufName << "BkgEvent_" << i + firstStore;
      const std::string& streamName(bufName.str());    
      try 
	{
	  m_streams1.push_back(PileUpStream(streamName,serviceLocator(),selecName1)); 
	} 
      catch (const std::runtime_error& e) 
	{
	  ATH_MSG_ERROR ( "Exception thrown while creating PileUpStream " 
			  << streamName << " : " << e.what() );     
	  return StatusCode::FAILURE;
	}
    
      if(!(m_streams1.back().setupStore())) 
	{
	  ATH_MSG_ERROR ( "Can not setup bkg evt store for stream " 
			  << streamName );
	  return StatusCode::FAILURE;
	}
    }

  //Cache 2
  const std::string& selecName2(m_selecName2.name());
  for (unsigned int i=0; i < m_nStores2; ++i) 
    {
      std::stringstream bufName;
      bufName << "BkgEvent_" << i + firstStore + m_nStores1;
      const std::string& streamName(bufName.str());    
      try 
	{
	  m_streams2.push_back(PileUpStream(streamName,serviceLocator(),selecName2)); 
	} 
      catch (const std::runtime_error& e) 
	{
	  ATH_MSG_ERROR ( "Exception thrown while creating PileUpStream " 
			  << streamName << " : " << e.what() );     
	  return StatusCode::FAILURE;
	}
    
      if(!(m_streams2.back().setupStore())) 
	{
	  ATH_MSG_ERROR ( "Can not setup bkg evt store for stream " 
			  << streamName );
	  return StatusCode::FAILURE;
	}
    }

  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(0 == collEng ) 
    {
      ATH_MSG_ERROR (  "can not get random stream " << m_randomStreamName.value() );
      return StatusCode::FAILURE;
    }
  //setup generator to pickup an event store at random from the cache
  //notice how we pass collEng by reference. If ! CLHEP will take ownership...
  m_chooseEventRand1 = new CLHEP::RandFlat(*(collEng),
					   0.0, double(m_nStores1));
  m_chooseEventRand2 = new CLHEP::RandFlat(*(collEng),
					   0.0, double(m_nStores2));
  return StatusCode::SUCCESS;
}

void SplitBkgStreamsCache::resetEvtsPerXingScaleFactor(float sf) 
{
  if (!m_ignoreSF) { m_collXingSF = sf; }
  return;
}

long SplitBkgStreamsCache::collXingPoisson()
{ 
  if (!m_collXingPoisson) { return collXing(); }
  return m_collXingPoisson->fire(m_meanCollisionsPerBunchCrossing * m_collXingSF); 
}

void SplitBkgStreamsCache::newEvent() 
{ 
  ATH_MSG_DEBUG (  "newEvent called resetting used event set" );
  unsigned int totalBkgEvtsPicked(0);
  do 
    {
      totalBkgEvtsPicked = 0;
      for (unsigned int iXing=0; iXing<m_nXings; ++iXing) 
	{
	  totalBkgEvtsPicked += pickNumberOfBkgForBunchCrossing(iXing);
	  if ( totalBkgEvtsPicked > (m_nStores1+m_nStores2) )
	    {
	      ATH_MSG_WARNING ( "newEvent: number of required evts (" << totalBkgEvtsPicked << ") exceeds number of available stores "
				<< (m_nStores1+m_nStores2) << ". Regenerating bkg sequence for this event \n"
				<< " the total number of bkg events may not exceed average by more than 6 sigmas"
				);
	      break;
	    }
	}
    } while( (totalBkgEvtsPicked*m_fractionOfCache1Collisions > m_nStores1) || (totalBkgEvtsPicked*(1.0-m_fractionOfCache1Collisions) > m_nStores2) ); //FIXME this is insufficient 
  m_usedStreams1.assign(m_streams1.size(), false); 
  m_usedStreams2.assign(m_streams2.size(), false); 
}

const EventInfo* SplitBkgStreamsCache::nextEvent(bool isCentralBunchCrossing) 
{ 
  const EventInfo* pNextEvt(NULL);
  StreamVector::size_type iS(0);
  CLHEP::RandFlat* pChooseEventRand(NULL);
  CLHEP::RandFlat* pReadEventRand(NULL);

  const unsigned int attemptlimit(10*(m_nStores1+m_nStores2));
  unsigned int retries(0);
  unsigned int cacheForThisEvent(1);
  bool retry(false);
  do
    {
      retry = false;
      if ( m_chooseCacheRand->fire() >= m_fractionOfCache1Collisions ) { cacheForThisEvent = 2; }
      else { cacheForThisEvent = 1; }
      if (cacheForThisEvent==1)
	{
	  pChooseEventRand = m_chooseEventRand1;
	  pReadEventRand = m_readEventRand1;
	  m_cursor = m_streams1.begin();
	}
      else
	{
	  pChooseEventRand = m_chooseEventRand2;
	  pReadEventRand = m_readEventRand2;
	  m_cursor = m_streams2.begin();
	}

      unsigned int attempts(0);
      do 
	{ 
	  iS = (StreamVector::size_type)pChooseEventRand->fire();
	  ++attempts;
	} while (alreadyInUse(iS, cacheForThisEvent) && attempts<attemptlimit);

      if (attempts>=attemptlimit) {
	ATH_MSG_WARNING (  "nextEvent() Failed to find an unused bkg event in cache " << cacheForThisEvent << ". After " << attemptlimit << " attempts." ); 
	++retries;
	retry=true;
      }
    } while (retry && retries<5);
    
  if(retries>=5)
    {
      ATH_MSG_FATAL (  "nextEvent() Failed to find an unsed bkg event after " << 5*attemptlimit << " attempts. Bailing out" );
      return pNextEvt;
    }

  //set current store to iS
  std::advance(m_cursor, iS);
  ATH_MSG_VERBOSE (  "nextEvent: Using Cache"<<cacheForThisEvent<<" event from store " << iS );
     
  PileUpStream* pCurrStream(current());
  if (0 != pCurrStream) 
    {
      p_activeStore->setStore(&(pCurrStream->store()));
      //read a new event every downscaleFactor accesses
      //force reading of new event if the event is being used for in-time pile-up
      //FIXME a more careful strategy would have the PileUpStreams knowing if
      //they have been used for the central bunch-crossing already.
      bool readEvent(isCentralBunchCrossing || (pReadEventRand->fire()<1.0));
      pNextEvt=pCurrStream->nextEventPre(readEvent);
    }
  return pNextEvt;
}

StatusCode SplitBkgStreamsCache::nextEvent_passive(bool isCentralBunchCrossing) 
{ 
  StreamVector::size_type iS(0);
  CLHEP::RandFlat* pChooseEventRand(NULL);
  CLHEP::RandFlat* pReadEventRand(NULL);

  const unsigned int attemptlimit(10*(m_nStores1+m_nStores2));
  unsigned int retries(0);
  unsigned int cacheForThisEvent(1);
  bool retry(false);
  do
    {
      retry = false;
      if ( m_chooseCacheRand->fire() >= m_fractionOfCache1Collisions ) { cacheForThisEvent = 2; }
      else { cacheForThisEvent = 1; }
      if (cacheForThisEvent==1)
	{
	  pChooseEventRand = m_chooseEventRand1;
	  pReadEventRand = m_readEventRand1;
	  m_cursor = m_streams1.begin();
	}
      else
	{
	  pChooseEventRand = m_chooseEventRand2;
	  pReadEventRand = m_readEventRand2;
	  m_cursor = m_streams2.begin();
	}

      unsigned int attempts(0);
      do 
	{ 
	  iS = (StreamVector::size_type)pChooseEventRand->fire();
	  ++attempts;
	} while (alreadyInUse(iS, cacheForThisEvent) && attempts<attemptlimit);

      if (attempts>=attemptlimit) {
	ATH_MSG_WARNING ( "nextEvent() Failed to find an unused bkg event in cache " << cacheForThisEvent << ". After " << attemptlimit << " attempts." ); 
	++retries;
	retry=true;
      }
    } while (retry && retries<5);
    
  if(retries>=5)
    {
      ATH_MSG_FATAL (  "nextEvent() Failed to find an unsed bkg event after " << 5*attemptlimit << " attempts. Bailing out" );
      return StatusCode::FAILURE;
    }

  //set current store to iS
  std::advance(m_cursor, iS);
  ATH_MSG_VERBOSE (  "nextEvent: Using Cache"<<cacheForThisEvent<<" event from store " << iS );
     
  PileUpStream* pCurrStream(current());
  if (0 != pCurrStream) 
    {
      p_activeStore->setStore(&(pCurrStream->store()));
      //read a new event every downscaleFactor accesses
      //force reading of new event if the event is being used for in-time pile-up
      //FIXME a more careful strategy would have the PileUpStreams knowing if
      //they have been used for the central bunch-crossing already.
      bool readEvent(isCentralBunchCrossing || (pReadEventRand->fire()<1.0));
      if(pCurrStream->nextEventPre_Passive(readEvent)) return StatusCode::SUCCESS;
    }
  return StatusCode::FAILURE;
}

bool SplitBkgStreamsCache::alreadyInUse(StreamVector::size_type iS, unsigned int cacheForThisEvent) 
{
  bool inUse(true);
  if( cacheForThisEvent == 1)
    {
      assert(iS<m_usedStreams1.size());
      inUse = (m_usedStreams1[iS]);
      if (!inUse) { m_usedStreams1[iS] = true; }
    }
  else if ( cacheForThisEvent == 2)
    {
      assert(iS<m_usedStreams2.size());
      inUse = (m_usedStreams2[iS]);
      if (!inUse) { m_usedStreams2[iS] = true; }
    }
  else 
    {
      ATH_MSG_ERROR ( "Unknown Cache number: " << cacheForThisEvent << " specified!" );     
    }
#ifndef NDEBUG
  ATH_MSG_VERBOSE (  "alreadyInUse: store " << iS  << ' '
		     << (inUse ? "already" : "not yet") << " in use" );
#endif
  return inUse;
} 

PileUpStream* SplitBkgStreamsCache::current() 
{ 
  if ( m_cursor != m_streams1.end() && m_cursor != m_streams2.end() ) { return &*m_cursor; }
  else return 0; //FIXME should remove empty stream and keep going
}
 
StatusCode SplitBkgStreamsCache::initialize() { 
  StatusCode sc(StatusCode::SUCCESS);
  // configure our MsgStream
  ATH_MSG_DEBUG (  "Initializing " << name()
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
  if (!(m_atRndmSvc.retrieve()).isSuccess()) 
    {
      ATH_MSG_ERROR (  "can not get IAtRndmGenSvc " << m_atRndmSvc );
      return StatusCode::FAILURE;
    }
  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(0 == collEng ) 
    {
      ATH_MSG_ERROR (  "can not get random stream " << m_randomStreamName.value() );
      return StatusCode::FAILURE;
    }

  //setup distribution to read a new event every downscaleFactor accesses
  //notice how we pass collEng by reference. If ! CLHEP will take ownership...
  m_readEventRand1 = new CLHEP::RandFlat(*(collEng),
					 0.0, double(m_readDownscale1));
  m_readEventRand2 = new CLHEP::RandFlat(*(collEng),
					 0.0, double(m_readDownscale2));
  //TODO add comment here!!!
  m_chooseCacheRand = new CLHEP::RandFlat(*(collEng), 0.0, 1.0);

  // select collision distribution functions
  if (m_collDistrName.value() == "Fixed") 
    {
      m_f_collDistr = boost::bind(&SplitBkgStreamsCache::collXing, this);
      if(m_ignoreBM.value()) 
	{
	  m_f_numberOfBackgroundForBunchCrossing = boost::bind(&SplitBkgStreamsCache::numberOfBkgForBunchCrossingIgnoringBeamIntensity, this, _1);
	} else 
	{
	  m_f_numberOfBackgroundForBunchCrossing = boost::bind(&SplitBkgStreamsCache::numberOfCavernBkgForBunchCrossing, this, _1);
	}
    }
  else if (m_collDistrName.value() == "Poisson") 
    {
      //pass collEng by reference. If Not CLHEP will take ownership...
      m_collXingPoisson = new CLHEP::RandPoisson(*(collEng), m_meanCollisionsPerBunchCrossing);
      // m_f_collDistr will call m_collXingPoisson->fire(m_meanCollisionsPerBunchCrossing)  USED TO BE boost::bind(&CLHEP::RandPoisson::fire, m_collXingPoisson); 
      m_f_collDistr = boost::bind(&SplitBkgStreamsCache::collXingPoisson, this);      
      if(m_ignoreBM.value()) 
	{
	  m_f_numberOfBackgroundForBunchCrossing = boost::bind(&SplitBkgStreamsCache::numberOfBkgForBunchCrossingIgnoringBeamIntensity, this, _1);
	} else 
	{
	  m_f_numberOfBackgroundForBunchCrossing = boost::bind(&SplitBkgStreamsCache::numberOfBkgForBunchCrossingDefaultImpl, this, _1);
	}
    } else 
    {
      ATH_MSG_ERROR (  m_collDistrName 
		       << " is not a know collision distribution function" );
      sc = StatusCode::FAILURE;
    }
  return sc;
}

unsigned int SplitBkgStreamsCache::getNumberOfBkgForBunchCrossing(unsigned int iXing) const 
{
  return (iXing + 1 > m_numberOfBkgForBunchCrossing.size()) ? 0 : m_numberOfBkgForBunchCrossing[iXing];
}

unsigned int SplitBkgStreamsCache::numberOfBkgForBunchCrossingIgnoringBeamIntensity(unsigned int) const 
{
  return m_f_collDistr();
}
unsigned int SplitBkgStreamsCache::numberOfBkgForBunchCrossingDefaultImpl(unsigned int iXing) const 
{
  return static_cast<unsigned int>(m_beamInt->normFactor(iXing-m_zeroXing)*static_cast<float>(m_f_collDistr()));
}
unsigned int SplitBkgStreamsCache::numberOfCavernBkgForBunchCrossing(unsigned int iXing) const 
{
  return static_cast<unsigned int>(m_beamInt->normFactor(iXing-m_zeroXing)>0.0)*m_f_collDistr();
}

unsigned int SplitBkgStreamsCache::pickNumberOfBkgForBunchCrossing(unsigned int iXing) {
  if (iXing + 1 > m_numberOfBkgForBunchCrossing.size()) { m_numberOfBkgForBunchCrossing.resize(2 * iXing + 1); }
  unsigned int nEvts(m_f_numberOfBackgroundForBunchCrossing(iXing));
  //this is done mainly to handle the case in which original and backround
  //events belong to the same stream. Of course we do not want m_numberOfBkgForBunchCrossing[m_zeroXing]<0 
  if ((int)iXing==m_zeroXing) 
    {
      unsigned int subValue(m_subtractBC0.value());
      while (nEvts<subValue) nEvts = m_f_numberOfBackgroundForBunchCrossing(iXing);
      nEvts -= subValue;
#ifndef NDEBUG
      ATH_MSG_VERBOSE ( "Subtracted " <<   m_subtractBC0.value() 
			<< " events from BC=0 Xing " << iXing << " - Events at BC=0 "
			<< nEvts 
			);
#endif
    }
  ATH_MSG_VERBOSE ( "Will pile-up " <<  nEvts
		    << " events for BCID " << iXing );

  m_numberOfBkgForBunchCrossing[iXing] = nEvts;
  return nEvts;
}

StatusCode SplitBkgStreamsCache::addSubEvts(unsigned int iXing,
					    PileUpEventInfo& overEvent,
					    int t0BinCenter) 
{
  return this->addSubEvts(iXing, overEvent, t0BinCenter, true, 0);
}

StatusCode SplitBkgStreamsCache::addSubEvts(unsigned int iXing,
					    PileUpEventInfo& overEvent,
					    int t0BinCenter, bool loadEventProxies, unsigned int BCID) 
{
  for (unsigned int iEvt=0; iEvt<getNumberOfBkgForBunchCrossing(iXing); ++iEvt) 
    {
      StoreGateSvc* pBkgStore(0);
    
      // check if we're picking events for the central bunch-crossing, 
      // so we can choose to only use fresh events here.
      bool isCentralBunchCrossing((0==t0BinCenter)&&m_forceReadForBC0);
      // increment event iterators
      if(!loadEventProxies)
	{
	  return this->nextEvent_passive(isCentralBunchCrossing);
	}
      const EventInfo* pBkgEvent(nextEvent(isCentralBunchCrossing));

      //check input selector is not empty
      PileUpStream* currStream(current());
      if (0 == pBkgEvent || 0 == currStream) 
	{
	  // This is the end of the loop. No more events in the selection
	  if (m_cursor == m_streams1.end())
	    {
	      ATH_MSG_INFO (  "end of loop: background cache 1 has no more events" );
	    }
	  else if (m_cursor == m_streams2.end())
	    {
	      ATH_MSG_INFO (  "end of loop: background cache 2 has no more events" );
	    }
	  else
	    {
	      ATH_MSG_INFO (  "end of loop: background cache has no more events" );
	    }
	  return StatusCode::FAILURE;
	} else 
	{
	  pBkgStore = &(currStream->store());
	  ATH_MSG_VERBOSE ( "addSubEvts: added event " <<  pBkgEvent->event_ID()->event_number() 
			    << " run " << pBkgEvent->event_ID()->run_number()
			    << " from store " 
			    << pBkgStore->name()
			    << " @ Xing " << iXing );
	}
    
      //  register as sub event of the overlaid
      //    ask if sufficient/needed
      overEvent.addSubEvt(t0BinCenter, BCID,
			  m_pileUpEventType,
			  *pBkgEvent, pBkgStore);
#ifdef DEBUG_PILEUP
      const EventInfo* pStoreInfo(0);
      if (pBkgStore->retrieve(pStoreInfo).isSuccess() && pStoreInfo && 
	  pBkgEvent->event_ID()->event_number() != pStoreInfo->event_ID()->event_number()) 
	{
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

StatusCode SplitBkgStreamsCache::finalize() 
{ 
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_DEBUG (  "Finalizing " << name()
		   << " - cache for events of type " 
		   << PileUpTimeEventIndex::typeName(m_pileUpEventType)
		   << " - package version " << PACKAGE_VERSION ) ;
  while (sc.isSuccess() && m_streams1.size()>0) 
    {
      sc=m_streams1.back().finalize();
      m_streams1.pop_back();
    }
  while (sc.isSuccess() && m_streams2.size()>0) 
    {
      sc=m_streams2.back().finalize();
      m_streams2.pop_back();
    }
  return sc;
}
