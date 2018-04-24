/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_SPLITBKGSTREAMSCACHE_H
# define PILEUPTOOLS_SPLITBKGSTREAMSCACHE_H
/** @file SplitBkgStreamsCache.h
 * @brief In memory cache for pileup events
 *
 * $Id: SplitBkgStreamsCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include <string>
#include <vector>
#include <boost/function.hpp>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "PileUpTools/PileUpStream.h"
#include "PileUpTools/IBkgStreamsCache.h"
#include "EventInfo/PileUpTimeEventIndex.h" /* needed for PileUpType */


class ActiveStoreSvc;
class EventInfo;
class IAtRndmGenSvc;
class IEvtSelector;
class IBeamIntensity;
class PileUpEventInfo;
namespace CLHEP {
  class RandFlat;
  class RandPoisson;
}


/** @class SplitBkgStreamsCache
 * @brief In-memory cache for pileup events
 */
class SplitBkgStreamsCache :
  public extends<AthAlgTool, IBkgStreamsCache>
{
public:
  SplitBkgStreamsCache( const std::string&, const std::string&, const IInterface*);
  virtual ~SplitBkgStreamsCache();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  /** 
      @param nXings bunch Xings to be processed
      @param firstStore id of first store in cache 
  */
  virtual StatusCode setup(int firstXing,
			   unsigned int nXings,  
			   unsigned int firstStore,
			   IBeamIntensity*); 
  /// inform cache that we start overlaying a new event
  virtual void newEvent();
  /// reset scale factor at new run/lumiblk       
  virtual void resetEvtsPerXingScaleFactor(float sf);
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns      
  */
  virtual StatusCode addSubEvts(unsigned int iXing, 
				PileUpEventInfo& overlaidEvent,
				int t0BinCenter);
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns      
     @param loadEventProxies should we load the event proxies or not.   
     @param BCID          bunch-crossing ID of signal bunch crossing
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
				PileUpEventInfo& overEvent,
				int t0BinCenter, bool loadEventProxies, unsigned int /*BCID*/);
  /// how many stores in this cache
  virtual unsigned int nStores() const { return (m_nStores1 + m_nStores2); }

  /// meant to be used (mainly) via f_collDistr
  long collXing() { return m_meanCollisionsPerBunchCrossing; }
  long collXingPoisson();
  /// meant to be used via m_f_numberOfBackgroundForBunchCrossing
  unsigned int numberOfBkgForBunchCrossingIgnoringBeamIntensity(unsigned int iXing) const;
  unsigned int numberOfBkgForBunchCrossingDefaultImpl(unsigned int iXing) const;
  unsigned int numberOfCavernBkgForBunchCrossing(unsigned int iXing) const;
private:
  /// get next bkg event from cache
  const EventInfo* nextEvent(bool isCentralBunchCrossing);
  /// as nextEvent except don't actually load anything
  StatusCode nextEvent_passive(bool isCentralBunchCrossing);
  /// get current (last asked) stream
  PileUpStream* current();

  unsigned int pickNumberOfBkgForBunchCrossing(unsigned int iXing);
  unsigned int getNumberOfBkgForBunchCrossing(unsigned int iXing) const;
  void calculateCacheSizes();

  typedef std::vector<PileUpStream> StreamVector;
  bool alreadyInUse(StreamVector::size_type iStream, unsigned int cacheForThisEvent);
  ActiveStoreSvc* p_activeStore;
  StreamVector::iterator m_cursor;
  unsigned int m_nXings;

  //Properties for Cache1
  StreamVector m_streams1;
  std::vector<bool> m_usedStreams1;
  unsigned int m_nStores1;

  //Properties for Cache2
  StreamVector m_streams2;
  std::vector<bool> m_usedStreams2;
  unsigned int m_nStores2;


  std::vector<unsigned int> m_numberOfBkgForBunchCrossing;

  /// @name Properties
  //@{
  /// # of collisions per bunch crossing (~beam intensity)
  FloatProperty m_meanCollisionsPerBunchCrossing;
  /// Fraction of cache1 collisions
  FloatProperty m_fractionOfCache1Collisions;
  /// The maximum fraction of bunch-crossings which will be occupied.
  FloatProperty m_occupationFraction;
  /// select collision distribution
  StringProperty m_collDistrName;
  ServiceHandle<IEvtSelector> m_selecName1;
  ServiceHandle<IEvtSelector> m_selecName2;
  /// read downscale factor for cache1 (average number of times a min bias is reused)
  FloatProperty m_readDownscale1;
  /// read downscale factor for cache2 (average number of times a min bias is reused)
  FloatProperty m_readDownscale2;
  /// IAtRndmGenSvc controlling the distribution of bkg events per bunch crossing
  ServiceHandle<IAtRndmGenSvc> m_atRndmSvc;
  /// the IAtRndmGenSvc stream to generate number of bkg events per bunch crossing
  StringProperty m_randomStreamName;
  /// the type of events in this cache
  UnsignedShortProperty m_pileUpEventTypeProp;
  void PileUpEventTypeHandler(Property&);
  /// the type of events in this cache
  PileUpTimeEventIndex::PileUpType m_pileUpEventType;
  /// subtract from number of events at bunch xing = 0
  UnsignedShortProperty m_subtractBC0;
  /// ignore the PileUpEventLoopMgr beam intensity tool
  BooleanProperty m_ignoreBM;
  /// bool apply scaling number of collisions per bunch crossing ?
  BooleanProperty m_ignoreSF;
  /// Force events used in the central bunch crossing to be refreshed
  BooleanProperty m_forceReadForBC0;
  //@}
  /// read a new event every downscaleFactor accesses from cache1
  CLHEP::RandFlat* m_readEventRand1;
  /// read a new event every downscaleFactor accesses from cache2
  CLHEP::RandFlat* m_readEventRand2;
  /// pickup an event store at random from cache1
  CLHEP::RandFlat* m_chooseEventRand1;
  /// pickup an event store at random from cache2
  CLHEP::RandFlat* m_chooseEventRand2;
  /// pick whether event will come from cache 1 or cache2
  CLHEP::RandFlat* m_chooseCacheRand;
  /// set number of collisions per bunch crossing (if Poisson distribution chosen)
  CLHEP::RandPoisson* m_collXingPoisson;
  /// function returning the number of collisions per bunch crossing
  /// before bunch structure modulation
  boost::function0< long > m_f_collDistr;
  /// function returning the number of bkg events per bunch crossing
  /// after bunch structure modulation
  boost::function1< unsigned int, unsigned int > m_f_numberOfBackgroundForBunchCrossing;
  /// float scaling number of collisions per bunch crossing 
  float m_collXingSF;
  /// offset of BC=0 xing
  int m_zeroXing;
  /// pointer to the IBeamIntensity distribution tool
  IBeamIntensity* m_beamInt;
  
};

#endif // PILEUPTOOLS_SPLITBKGSTREAMSCACHE_H
