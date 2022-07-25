/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @file   ToFVtxParamDBTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date   2022-06-03
/// 
/// @brief  Header file for ToFVtxParamDBTool used to read vertex ToF parameters from database
#ifndef AFP_DBTOOLS_TOFVTXPARAMDBTOOL_H
#define AFP_DBTOOLS_TOFVTXPARAMDBTOOL_H

// Vertex includes
#include "AFP_DBTools/IToFVtxParamDBTool.h"
#include "AFP_DBTools/ToFVtxParamData.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandle.h"

// database access
#include "GaudiKernel/EventContext.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "nlohmann/json.hpp"
#include <string>
#include <utility>

namespace AFP
{
  /// Tool providing vertex parameters of ToF detectors from the conditions database.
  class ToFVtxParamDBTool : public extends<AthAlgTool, IToFVtxParamDBTool>
  {
  public:
    ToFVtxParamDBTool(const std::string& type, const std::string& name, const IInterface* parent);

    /// Does nothing
    virtual ~ToFVtxParamDBTool() override {}

    /// Does nothing
    virtual StatusCode initialize() override;

    /// Does nothing
    virtual StatusCode finalize() override;

    /// Provide ToF parameters for a given station. Returns zeros if no data available.
    nlohmann::json parametersData(const EventContext& ctx) const override;
    const ToFVtxParamData parameters(const nlohmann::json& jsondata, const int stationID) const override;
    
  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_rch_vtx {this, "vtx_param_key", "/FWD/Onl/AFP/ToFParameters/Vertex", "read condition handle for vertex ToF parameters"};
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_TOFVTXPARAMDBTOOL_H
