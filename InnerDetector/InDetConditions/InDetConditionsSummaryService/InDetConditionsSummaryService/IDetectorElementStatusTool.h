/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDetectorElementStatusTool_h
#define IDetectorElementStatusTool_h

#include "StoreGate/WriteCondHandle.h"
#include "GaudiKernel/EventIDRange.h"
#include <vector>
#include <tuple>

namespace InDet {
   class SiDetectorElementStatus;
}

class IDetectorElementStatusTool : virtual public IInterface, virtual public IAlgTool {
public:

   /** get the module and chip status of Si detector elements
    * @param ctx the current event contex
    * @param whandle Pointer to the write handle for the conditions object
    *                being built, for propagation of IOV range and
    *                dependency information.
    *                Pass nullptr if we're building an object for event data,
    *                rather than conditions data.
    */
   virtual std::unique_ptr<InDet::SiDetectorElementStatus>
   getDetectorElementStatus(const EventContext& ctx,
                            SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const = 0;

   static EventIDBase getUndefinedEventId() { return EventIDBase (0, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM, 0, 0); }
   static EventIDRange getInvalidRange()    { return EventIDRange (getUndefinedEventId(),getUndefinedEventId()); }

};


#endif
