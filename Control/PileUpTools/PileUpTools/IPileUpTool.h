/* -*- C++ -*- */

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_IPILEUPTOOL_H
#define PILEUPTOOLS_IPILEUPTOOL_H
/** @file IPileUpTool.h
 * @brief a call-back interface for tools that merge pileup events information
 * An IPileUpTool is called back for each bunch crossing that it is interested
 * in processing, and again at the end of the loop over input events.
 * IPileUpTools should be implemented as private tools of PileUpMergeSvc
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */
#include "GaudiKernel/IAlgTool.h"

#include "xAODEventInfo/EventInfo.h"

#include <vector>
typedef std::vector<xAOD::EventInfo::SubEvent>::const_iterator SubEventIterator;

class IPileUpTool : virtual public IAlgTool{
public:
  ///called before the bunchXing loop
  virtual StatusCode prepareEvent(unsigned int /*nInputEvents*/) { return StatusCode::SUCCESS; }
  ///called for each active bunch-crossing (time in ns)
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) = 0;
  ///flags whether this tool is "live" for bunchXing (time in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  virtual bool toProcess(int bunchXing) const =0;
  ///called at the end of the bunchXing loop
  virtual StatusCode mergeEvent() { return StatusCode::SUCCESS; }
  ///alternative interface which uses the PileUpMergeSvc to obtain all
  ///the required SubEvents.
  virtual StatusCode processAllSubEvents() = 0;
  ///flags whether the event should be removed or not
  virtual bool filterPassed() const =0;
  ///reset the filter
  virtual void resetFilter() =0;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IPileUpTool, 1, 0 );
};
#endif // PILEUPTOOLS_IPILEUPTOOL_H
