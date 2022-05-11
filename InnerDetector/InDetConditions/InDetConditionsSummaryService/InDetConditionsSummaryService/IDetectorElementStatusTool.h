/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDetectorElementStatusTool_h
#define IDetectorElementStatusTool_h

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
    */
   virtual std::tuple<std::unique_ptr<InDet::SiDetectorElementStatus>, EventIDRange> getDetectorElementStatus(const EventContext& ctx) const = 0;

   static EventIDBase getUndefinedEventId() { return EventIDBase (0, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM, 0, 0); }
   static EventIDRange getInvalidRange()    { return EventIDRange (getUndefinedEventId(),getUndefinedEventId()); }

};


#endif
