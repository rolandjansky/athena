/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_BKGSTREAMSCACHE_H
# define PILEUPTOOLS_BKGSTREAMSCACHE_H
/** @file BkgStreamsCache.h
 * @brief In memory cache for pileup events
 *
 * $Id: BkgStreamsCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include <string>
#include <vector>
#include <boost/function.hpp>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "PileUpTools/PileUpStream.h"
#include "PileUpTools/IBkgStreamsCache.h"

class ActiveStoreSvc;
class IEvtSelector;
class IAtRndmGenSvc;
class IBeamIntensity;
namespace CLHEP {
  class RandFlat;
  class RandPoisson;
}


/** @class BkgStreamsCache
 * @brief In-memory cache for pileup events
 */
class BkgStreamsCache :
  public extends<AthAlgTool, IBkgStreamsCache>
{
public:
  BkgStreamsCache( const std::string&, const std::string&, const IInterface*);
  virtual ~BkgStreamsCache();

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
                                xAOD::EventInfo* overlaidEvent,
                                int t0BinCenter) override final;
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns
     @param loadEventProxies should we load the event proxies or not.
     @param BCID          bunch-crossing ID of signal bunch crossing
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
                                xAOD::EventInfo* overEvent,
                                int t0BinCenter, bool loadEventProxies, unsigned int /*BCID*/) override final;
  /// how many stores in this cache
  virtual unsigned int nStores() const override final { return m_nStores; }

  /// meant to be used (mainly) via m_f_collDistr
  long collXing() { return m_collXing; }
  long collXingPoisson();
  /// meant to be used via m_f_numberOfBackgroundForBunchCrossing
  unsigned int numberOfBkgForBunchCrossingIgnoringBeamIntensity(unsigned int iXing) const;
  unsigned int numberOfBkgForBunchCrossingDefaultImpl(unsigned int iXing) const;
  unsigned int numberOfCavernBkgForBunchCrossing(unsigned int iXing) const;
private:
  /// get next bkg event from cache
  const xAOD::EventInfo* nextEvent(bool isCentralBunchCrossing);
  /// as nextEvent except don't actually load anything
  StatusCode nextEvent_passive(bool isCentralBunchCrossing);
  /// get current (last asked) stream
  PileUpStream* current();

  unsigned int setNEvtsXing(unsigned int iXing);
  unsigned int nEvtsXing(unsigned int iXing) const;

  typedef std::vector<PileUpStream> StreamVector;
  bool alreadyInUse(StreamVector::size_type iStream);
  ActiveStoreSvc* p_activeStore;
  StreamVector::iterator m_cursor;
  StreamVector m_streams;
  std::vector<bool> m_usedStreams;
  unsigned int m_nXings;
  unsigned int m_nStores;
  std::vector<unsigned int> m_nEvtsXing;

  /// @name Properties
  //@{
  /// # of collisions per bunch crossing (~beam intensity)
  Gaudi::Property<float> m_collXing;
  /// The maximum fraction of bunch-crossings which will be occupied.
  Gaudi::Property<float> m_occupationFraction;
  /// select collision distribution
  Gaudi::Property<std::string> m_collDistrName;
  ServiceHandle<IEvtSelector> m_selecName;
  /// read downscale factor (average number of times a min bias is reused)
  Gaudi::Property<float> m_readDownscale;
  /// IAtRndmGenSvc controlling the distribution of bkg events per bunch crossing
  ServiceHandle<IAtRndmGenSvc> m_atRndmSvc;
  /// the IAtRndmGenSvc stream to generate number of bkg events per bunch crossing
  Gaudi::Property<std::string> m_randomStreamName;
  /// the type of events in this cache
  Gaudi::CheckedProperty<unsigned short> m_pileUpEventTypeProp;
  void PileUpEventTypeHandler(Property&);
  /// the type of events in this cache
  xAOD::EventInfo::PileUpType m_pileUpEventType;
  /// subtract from number of events at bunch xing = 0
  Gaudi::Property<unsigned short> m_subtractBC0;
  /// ignore the PileUpEventLoopMgr beam intensity tool
  Gaudi::Property<bool> m_ignoreBM;
  //@}
  /// read a new event every downscaleFactor accesses
  CLHEP::RandFlat* m_readEventRand;
  /// pickup an event store at random from the cache
  CLHEP::RandFlat* m_chooseEventRand;
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
  /// bool apply scaling number of collisions per bunch crossing ?
  Gaudi::Property<bool> m_ignoreSF;
  /// offset of BC=0 xing
  int m_zeroXing;
  /// pointer to the IBeamIntensity distribution tool
  IBeamIntensity* m_beamInt;
  /// Force events used in the central bunch crossing to be refreshed
  Gaudi::Property<bool> m_forceReadForBC0;

};

#endif // PILEUPTOOLS_BKGSTREAMSCACHE_H
