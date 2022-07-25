/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @file   ToFLocParamDBTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date   2022-06-03
/// 
/// @brief  Header file for ToFLocParamDBTool used to read local ToF parameters from database
#ifndef AFP_DBTOOLS_TOFLOCPARAMDBTOOL_H
#define AFP_DBTOOLS_TOFLOCPARAMDBTOOL_H

// Local includes
#include "AFP_DBTools/IToFLocParamDBTool.h"
#include "AFP_DBTools/ToFLocParamData.h"

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
  /// Tool providing local parameters of ToF detectors from the conditions database.
  class ToFLocParamDBTool : public extends<AthAlgTool, IToFLocParamDBTool>
  {
  public:
    ToFLocParamDBTool(const std::string& type, const std::string& name, const IInterface* parent);

    /// Does nothing
    virtual ~ToFLocParamDBTool() override {}

    /// Does nothing
    virtual StatusCode initialize() override;

    /// Does nothing
    virtual StatusCode finalize() override;

    /// Provide ToF parameters for a given bar. Returns zeros if no data available.
    nlohmann::json parametersData(const EventContext& ctx) const override;
    const ToFLocParamData parameters(const nlohmann::json& jsondata, const int stationID, const int trainID, const int barID) const override;
    
  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_rch_loc {this, "loc_param_key", "/FWD/Onl/AFP/ToFParameters/Local", "read condition handle for local ToF parameters"};
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_TOFLOCPARAMDBTOOL_H
