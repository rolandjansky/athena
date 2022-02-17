/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file   SiGlobAlignDBTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date   2021-02-23
/// 
/// @brief  Header file for SiGlobAlignDBTool used to read local alignment for database
#ifndef AFP_DBTOOLS_SIGLOBALIGNDBTOOL_H
#define AFP_DBTOOLS_SIGLOBALIGNDBTOOL_H

// Global includes
#include "AFP_DBTools/ISiGlobAlignDBTool.h"
#include "AFP_DBTools/SiGlobAlignData.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandle.h"

// database access
#include "GaudiKernel/EventContext.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "nlohmann/json.hpp"
#include <string>

namespace AFP
{
  /// Tool providing local alignment of silicon detectors from the conditions database.
  class SiGlobAlignDBTool : public extends<AthAlgTool, ISiGlobAlignDBTool>
  {
  public:
    SiGlobAlignDBTool(const std::string& type, const std::string& name, const IInterface* parent);

    /// Does nothing
    virtual ~SiGlobAlignDBTool() override {}

    /// Does nothing
    virtual StatusCode initialize() override;

    /// Does nothing
    virtual StatusCode finalize() override;

    /// Provide alignment parameters for a given station. Returns nullptr if no data available.
    const SiGlobAlignData alignment (const EventContext& ctx, const int stationID) const override;
    
  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_rch_glob {this, "glob_align_key", "/FWD/Onl/AFP/Align/Global", "read condition handle for global alignement"};
  };

}      // namespace AFP

#endif // > ! AFP_DBTOOLS_SIGLOBALIGNDBTOOL_H

