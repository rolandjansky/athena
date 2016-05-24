/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTEXTBYTESTREAM_H
#define EVENTCONTEXTBYTESTREAM_H

/** @file EventContextByteStream.h
 *  @brief This file contains the class definition for the EventContextByteStream class.
 *  @author Hong Ma <hma@bnl.gov>
 **/

#include "GaudiKernel/IEvtSelector.h"

class EventSelectorByteStream;
class IOpaqueAddress;

/** @class EventContextByteStream
 *  @brief This class provides the Context for EventSelectorByteStream
 **/
class EventContextByteStream : virtual public IEvtSelector::Context {

public:
   /// Constructor
   EventContextByteStream(const IEvtSelector* selector);
   /// Copy constructor
   EventContextByteStream(const EventContextByteStream& ctxt);
   /// Destructor
   virtual ~EventContextByteStream();

   /// Inequality operator.
   virtual void* identifier() const;

private:
   const IEvtSelector* m_evtSelector;
};

#endif
