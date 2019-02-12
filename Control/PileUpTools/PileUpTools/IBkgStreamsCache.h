/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file IBkgStreamsCache.h
 * @brief Interface to in-memory cache for pileup events
 *
 * $Id: IBkgStreamsCache.h,v 1.4 2008-04-23 20:41:17 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#ifndef PILEUPTOOLS_IBKGSTREAMSCACHE_H
#define PILEUPTOOLS_IBKGSTREAMSCACHE_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODEventInfo/EventInfo.h"

class IBeamIntensity;

/** @class IBkgStreamsCache
 *  @brief Interface to in-memory cache for pileup events
 */

class IBkgStreamsCache : virtual public IAlgTool {
public:
  /**
     @param firstXing index of first xing to be processed (0=t0)
     @param nXings number of bunch Xings to be processed
     @param firstStore id of first store in cache
  */
  virtual StatusCode setup(int firstXing,
                           unsigned int nXings,
                           unsigned int firstStore,
                           IBeamIntensity* iBM)=0;
  /// inform concrete cache that we start overlaying a new event
  virtual void newEvent() = 0;
  /// rescale number of events per crossing
  virtual void resetEvtsPerXingScaleFactor(float sf) = 0;
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns
     @param BCID          bunch-crossing ID of signal bunch crossing
     @param loadEventProxies should we load the event proxies or not.
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
                                xAOD::EventInfo* overEvent,
                                int t0BinCenter, bool loadEventProxies, unsigned int BCID) = 0;
  /**
     @brief Read input events in bkg stores and link them to overlay store
     @param iXing         offset to first xing number (=0 first Xing, =nXings for last xing)
     @param overlaidEvent reference to resulting overlaid event
     @param t0BinCenter   time wrto t0 of current bin center in ns
  */
  virtual StatusCode addSubEvts(unsigned int iXing,
                                xAOD::EventInfo* overlaidEvent,
                                int t0BinCenter) = 0;
  /// how many stores in cache
  virtual unsigned int nStores() const = 0;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IBkgStreamsCache, 1, 0 );
};
#endif // PILEUPTOOLS_IBKGSTREAMSCACHE_H
