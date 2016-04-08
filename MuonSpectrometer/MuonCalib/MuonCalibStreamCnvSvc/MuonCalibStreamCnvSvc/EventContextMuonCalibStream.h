/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTITERATORMUONCALIBSTREAM_H
#define EVENTITERATORMUONCALIBSTREAM_H

/** @file EventContextMuonCalibStream.h
 * @brief This file contains the class definition for the
EventContextMuonCalibStream class.
 *  @author Hong Ma <hma@bnl.gov>
 *  @author Domizia Orestano <domizia.orestano@cern.ch> 
**/

#include "GaudiKernel/IEvtSelector.h"

class EventSelectorMuonCalibStream;
class IOpaqueAddress;

/** @class EventContextMuonCalibStream
 * @brief This class provides the Context for EventSelectorMuonCalibStream
 **/

class EventContextMuonCalibStream : virtual public IEvtSelector::Context {

 public:
  /// Constructor
  EventContextMuonCalibStream(const IEvtSelector* selector);
  /// Copy constructor
  EventContextMuonCalibStream(const EventContextMuonCalibStream& ctxt);
  /// Destructor
  virtual ~EventContextMuonCalibStream();

  /// Inequality operator.
  virtual void* identifier() const;

 private:
  const IEvtSelector* m_evtSelector;
};
#endif
