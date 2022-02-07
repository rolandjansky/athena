/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IExtendedInDetConditionsTool_h
#define IExtendedInDetConditionsTool_h

#include "IInDetConditionsTool.h"
#include "GaudiKernel/EventIDRange.h"
#include <vector>
#include <tuple>

namespace InDet {
   class SiDetectorElementStatus;
}

class IExtendedInDetConditionsTool : public IInDetConditionsTool{
public:

   /** get the module and chip status of Si detector elements
    * @param ctx the current event contex
    * @param active_only if true instead of the full status, the status only reflects whether the a module or chip is active not necessarily provides good signals 
    */
   virtual std::tuple<std::unique_ptr<InDet::SiDetectorElementStatus>, EventIDRange> getDetectorElementStatus(const EventContext& ctx, bool active_only) const = 0;

   static EventIDBase getUndefinedEventId() { return EventIDBase (0, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM, 0, 0); }
   static EventIDRange getInvalidRange()    { return EventIDRange (getUndefinedEventId(),getUndefinedEventId()); }

   
};


#endif
