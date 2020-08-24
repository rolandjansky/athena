/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_SPACEPOINT_CONVERSION_TOOL_H__
#define __ITRIG_SPACEPOINT_CONVERSION_TOOL_H__

#include <vector>
#include "GaudiKernel/IAlgTool.h"

#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"

/** @class ITrigSpacePointConversionTool    
provides the abstract interface for offline SP conversion tool
@author D.Emeliyanov <http://consult.cern.ch/xwho>
*/

static const InterfaceID IID_ITrigSpacePointConversionTool("ITrigSpacePointConversionTool",1,0);

class ITrigSpacePointConversionTool : virtual public IAlgTool {
 public:
  /** other standard AlgTool methods */
  
  static const InterfaceID& interfaceID () {  //!< the Tool's interface
    return IID_ITrigSpacePointConversionTool; 
  }  	
  
  virtual StatusCode getSpacePoints(const IRoiDescriptor&, std::vector<TrigSiSpacePointBase>&, int&, int&, const EventContext& ctx) const = 0;

};

#endif
