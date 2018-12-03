/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_BKGSTREAMSCONCENTRICCACHE_H
# define PILEUPTOOLS_BKGSTREAMSCONCENTRICCACHE_H
/** @file BkgStreamsConcentricCache.h
 * @brief Optimized in memory cache for pileup events
 *        This implementation controls how events are reused in the cache
 *        events that have been used in the "inner rings" of the cache
 *        can be used in the outer rings, but not viceversa
 *        The idea is that events in the inner rings will have all data
 *        objects in them read from disk, while events in the outer rings
 *        will only need to read hits from one or two detectors (LAr and MDT) 
 *
 * $Id: BkgStreamsConcentricCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include <string>
#include <vector>
#include <boost/function.hpp>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/VectorMap.h"
#include "PileUpTools/PileUpStream.h"
#include "PileUpTools/IBkgStreamsCache.h"
#include "EventInfo/PileUpTimeEventIndex.h" /* needed for PileUpType */
#include "AthenaKernel/MsgStreamMember.h"

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


/** @class BkgStreamsConcentricCache
 * @brief Optimized in-memory cache for pileup events
 */
class BkgStreamsConcentricCache :
  public extends<AthAlgTool, IBkgStreamsCache>
{
public:
  BkgStreamsConcentricCache( const std::string&, const std::string&, const IInterface*);
  virtual ~BkgStreamsConcentricCache();
  
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
     @param BCID          bunch-crossing ID of signal bunch crossing
     @param loadEventProxies should we load the event proxies or not.   
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
				PileUpEventInfo& overEvent,
				int t0BinCenter, bool loadEventProxies, unsigned int /*BCID*/);
  /// how many stores in cache
  virtual unsigned int nStores() const { return m_nStores; }

  /// meant to be used (mainly) via m_f_collDistr
  long collXing() { return m_collXing * m_collXingSF; }
  long collXingPoisson();

/** @class BkgStreamsConcentricCache::Ring
 * @brief A closed range in beam xings. Events initially used for a Ring
 * can be reused for an outer Ring but not for an inner Ring.
 */
  class Ring {
  public:
    ///takes a string of the form "lowXing:hiXing". @throws if bad format
    Ring(const std::string&);
    /// args are xing indices, not times in ns, 0 being the index of the t0
    Ring(int lowXing=-9999, int hiXing=9999) :
      m_lowXing(lowXing), m_hiXing(hiXing) {}
    Ring(const Ring& rhs) : 
      m_lowXing(rhs.m_lowXing), m_hiXing(rhs.m_hiXing) {}
    /// does this ring contain iXing
      bool contains(int iXing) { 
#ifdef DEBUG_PILEUP
	std::cout << "contains: " << m_lowXing << ' ' << iXing << ' ' << m_hiXing << std::endl;
#endif
	return (m_lowXing <= iXing) && (iXing <= m_hiXing) ; 
      }
    ///@throws if rings are not concentric
    bool operator <(const Ring& rhs) const;
    Ring& operator =(const Ring& rhs)
    {
      if (this == &rhs) {return *this;}   // Handle self assignment
      m_lowXing = rhs.m_lowXing;
      m_hiXing = rhs.m_hiXing;
      return *this;
    }
    int lowXing() const { return m_lowXing; }
    int hiXing() const { return m_hiXing; }
    void adjust(int offset) {
      m_lowXing += offset;
      m_hiXing += offset;
    }

  private:
    int m_lowXing;
    int m_hiXing;
  };
private:
  
  /// get next bkg event for xing iXing from cache
  const EventInfo* nextEvent(unsigned int iXing);
  /// as nextEvent except don't actually load anything
  StatusCode nextEvent_passive(unsigned int iXing);
  /// get current (last selected) stream
  PileUpStream* current();
  
  unsigned int setNEvtsXing(unsigned int iXing);
  unsigned int nEvtsXing(unsigned int iXing) const;
  
  typedef std::vector<PileUpStream> StreamVector;
  bool canUse(StreamVector::size_type iS, unsigned currentRing);
  ActiveStoreSvc* p_activeStore;
  StreamVector::iterator m_cursor;
  StreamVector m_streams;
  unsigned int m_nXings;
  unsigned int m_nStores;
  std::vector<unsigned int> m_nEvtsXing;

  /// these rings will be sorted in the property callback 
  std::vector<Ring> m_rings;

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
  FloatProperty m_readDownscale;
  /// IAtRndmGenSvc controlling the distribution of bkg events/xing
  ServiceHandle<IAtRndmGenSvc> m_atRndmSvc;
  /// the IAtRndmGenSvc stream to generate number of collisions/xing
  StringProperty m_randomStreamName;
  /// the type of events in this cache
  UnsignedShortProperty m_pileUpEventTypeProp;
  void PileUpEventTypeHandler(Property&);
  /// the type of events in this cache
  PileUpTimeEventIndex::PileUpType m_pileUpEventType;
  void RingsPropHandler(Property&);
  /// the list of rings to be used in the form "lowXing:hiXing"
  StringArrayProperty m_ringsProp;
  /// Allow events in inner rings to be used in outer rings. 
  /// When false every ring is effectively a separate cache
  BooleanProperty m_allowRingMigProp;
  //@}
  /// read a new event every downscaleFactor accesses
  CLHEP::RandFlat* m_readEventRand;
  /// pickup an event store at random from the cache
  CLHEP::RandFlat* m_chooseEventRand;
  /// set number of collisions/xing (if Poisson distribution chosen)
  CLHEP::RandPoisson* m_collXingPoisson;
  /// function returning number of collisions/xing 
  boost::function0< long > m_f_collDistr;
  /// float scaling number of collisions/xing 
  float m_collXingSF;
  /// bool apply scaling number of collisions/xing ?
  BooleanProperty m_ignoreSF;
};

#endif // PILEUPTOOLS_BKGSTREAMSCACHE_H
