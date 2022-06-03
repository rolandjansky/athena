/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @brief  Header file for interface of ToFVtxParamDBTool used to read vertex ToF parameters from database
#ifndef AFP_DBTOOLS_ITOFVTXPARAMDBTOOL_H
#define AFP_DBTOOLS_ITOFVTXPARAMDBTOOL_H


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

// forward declaration of nlohmann::json
#include "nlohmann/json_fwd.hpp"

namespace AFP
{
  // forward declarations
  class ToFVtxParamData;
  
  /// Interface to tool providing vertex parameters of ToF detectors from the conditions database.
  class IToFVtxParamDBTool : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID(IToFVtxParamDBTool, 1, 0);

    /// Provide ToF parameters for a given station. Returns zeros if no data available.    
    virtual nlohmann::json parametersData(const EventContext& ctx) const = 0;
    virtual const ToFVtxParamData parameters(const nlohmann::json& jsondata, const int stationID) const = 0;
  };

}      // namespace AFP

#endif // > ! AFP_DBTOOLS_ITOFVTXPARAMDBTOOL_H
