/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_BKGSTREAMSSTEPCACHE_H
# define PILEUPTOOLS_BKGSTREAMSSTEPCACHE_H
/** @file BkgStreamsStepCache.h
 * @brief In memory cache for pileup events - stepping through the bunch crossings
 *
 * $Id: BkgStreamsStepCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Will Buttinger - ATLAS Collaboration
 */

#include <string>
#include <vector>
#include <deque>
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
class IEvtSelector;
class IAtRndmGenSvc;
class IBeamIntensity;
class PileUpEventInfo;
namespace CLHEP {
  class RandFlat;
  class RandPoisson;
}


/** @class BkgStreamsStepCache
 * @brief In-memory cache for pileup events
 */
class BkgStreamsStepCache :
  public extends<AthAlgTool, IBkgStreamsCache>
{
public:
  BkgStreamsStepCache( const std::string&, const std::string&, const IInterface*);
  virtual ~BkgStreamsStepCache();

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  /**
      @param nXings bunch Xings to be processed
      @param firstStore id of first store in cache
  */
  virtual StatusCode setup(int firstXing,
                           unsigned int nXings,
                           unsigned int firstStore,
                           IBeamIntensity*) override final;
  /// inform cache that we start overlaying a new event
  virtual void newEvent() override final;
  /// reset scale factor at new run/lumiblk
  virtual void resetEvtsPerXingScaleFactor(float sf) override final;
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
                                PileUpEventInfo& overlaidEvent,
                                int t0BinCenter) override final;
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns
     @param BCID          bunch-crossing ID of signal bunch crossing
     @param loadEventProxies should we load the event proxies or not.
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
                                PileUpEventInfo& overEvent,
                                int t0BinCenter, bool loadEventProxies, unsigned int /*BCID*/) override final;
  /// how many stores in cache
  virtual unsigned int nStores() const override final { return m_nStores; }

  /// meant to be used (mainly) via f_collDistr
  long collXing() { return m_collXing; }
  long collXingPoisson();
  /// meant to be used via f_numberOfBackgroundForBunchCrossing
  unsigned int numberOfBkgForBunchCrossingIgnoringBeamIntensity(unsigned int iXing) const;
  unsigned int numberOfBkgForBunchCrossingDefaultImpl(unsigned int iXing) const;
  unsigned int numberOfCavernBkgForBunchCrossing(unsigned int iXing) const;
private:
  /// get next bkg event from cache
  const EventInfo* nextEvent();
  /// as nextEvent except don't actually load anything
  StatusCode nextEvent_passive();
  /// get current (last asked) stream
  PileUpStream* current();
  /// apply m_beamInt normalization to random number of events
  //unsigned int normEventsXing(unsigned int iXing) const;

  unsigned int setNEvtsXing(unsigned int iXing);
  unsigned int nEvtsXing(unsigned int iXing) const;

  typedef std::vector<PileUpStream> StreamVector;
  bool alreadyInUse(StreamVector::size_type iStream);
  ActiveStoreSvc* p_activeStore;
  StreamVector::iterator m_cursor;
  StreamVector m_streams;
  std::vector<bool> m_usedStreams;
  std::deque<StreamVector::size_type> m_streamUseOrder; //holds order in which events should be loaded
        //we use a deque to easily pop early events off the front, pushing new events on to the back
  std::deque<StreamVector::size_type>::iterator m_useCursor; //will use to know which event to load next
  bool m_firstEvent; //For very first event of job we need to setup a full set of event counts
  unsigned int m_currentXing; //the xing of the current event being simulated

  unsigned int m_nXings;
  unsigned int m_nStores;
  std::vector<unsigned int> m_nEvtsXing;

  /// @name Properties
  //@{
  /// # of collisions/xings (~beam intensity)
  FloatProperty m_collXing;
  /// The maximum fraction of bunch-crossings which will be occupied.
  FloatProperty m_occupationFraction;
  /// select collision distribution
  StringProperty m_collDistrName;
  ServiceHandle<IEvtSelector> m_selecName;
  /// read downscale factor (average number of times a min bias is reused)
  IntegerProperty m_readDownscale;
  /// IAtRndmGenSvc controlling the distribution of bkg events/xing
  ServiceHandle<IAtRndmGenSvc> m_atRndmSvc;
  /// the IAtRndmGenSvc stream to generate number of collisions/xing
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
  //@}
  /// read a new event every downscaleFactor accesses
  CLHEP::RandFlat* m_readEventRand;
  /// pickup an event store at random from the cache
  CLHEP::RandFlat* m_chooseEventRand;
  /// set number of collisions/xing (if Poisson distribution chosen)
  CLHEP::RandPoisson* m_collXingPoisson;
  /// function returning the number of collisions per bunch crossing
  /// before bunch structure modulation
  boost::function0< long > m_f_collDistr;
  /// function returning the number of bkg events per bunch crossing
  /// after bunch structure modulation
  boost::function1< unsigned int, unsigned int > m_f_numberOfBackgroundForBunchCrossing;
  /// float scaling number of collisions/xing
  float m_collXingSF;
  /// bool apply scaling number of collisions/xing ?
  BooleanProperty m_ignoreSF;
  /// offset of BC=0 xing
  int m_zeroXing;
  /// pointer to the IBeamIntensity distribution tool
  IBeamIntensity* m_beamInt;
};

#endif // PILEUPTOOLS_BKGSTREAMSCACHE_H
