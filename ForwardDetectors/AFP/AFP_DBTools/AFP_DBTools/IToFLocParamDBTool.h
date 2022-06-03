/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @brief  Header file for interface of ToFLocParamDBTool used to read local ToF parameters from database
#ifndef AFP_DBTOOLS_ITOFLOCPARAMDBTOOL_H
#define AFP_DBTOOLS_ITOFLOCPARAMDBTOOL_H


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

// forward declaration of nlohmann::json
#include "nlohmann/json_fwd.hpp"

namespace AFP
{
  // forward declarations
  class ToFLocParamData;
  
  /// Interface to tool providing local parameters of ToF detectors from the conditions database.
  class IToFLocParamDBTool : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID(IToFLocParamDBTool, 1, 0);

    /// Provide ToF parameters for a given bar. Returns zeros if no data available.    
    virtual nlohmann::json parametersData(const EventContext& ctx) const = 0;
    virtual const ToFLocParamData parameters(const nlohmann::json& jsondata, const int stationID, const int trainID, const int barID) const = 0;
  };

}      // namespace AFP

#endif // > ! AFP_DBTOOLS_ITOFLOCPARAMDBTOOL_H
