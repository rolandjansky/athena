/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IExtendedInDetConditionsTool_h
#define IExtendedInDetConditionsTool_h

#include "IInDetConditionsTool.h"
#include <vector>

namespace InDet {
   class SiDetectorElementStatus;
}

class IExtendedInDetConditionsTool : public IInDetConditionsTool{
public:

   /** get the module and chip status of Si detector elements
    * @param ctx the current event contex
    * @param active_only if true instead of the full status, the status only reflects whether the a module or chip is active not necessarily provides good signals 
    */
  virtual std::unique_ptr<InDet::SiDetectorElementStatus> getDetectorElementStatus(const EventContext& ctx, bool active_only) const = 0;

};

#endif
