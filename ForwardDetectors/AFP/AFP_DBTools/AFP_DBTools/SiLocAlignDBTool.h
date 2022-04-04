/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file   SiLocAlignDBTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-12-03
/// 
/// @brief  Header file for SiLocAlignDBTool used to read local alignment for database
#ifndef AFP_DBTOOLS_SILOCALIGNDBTOOL_H
#define AFP_DBTOOLS_SILOCALIGNDBTOOL_H

// Local includes
#include "AFP_DBTools/ISiLocAlignDBTool.h"
#include "AFP_DBTools/SiLocAlignData.h"

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
  class SiLocAlignDBTool : public extends<AthAlgTool, ISiLocAlignDBTool>
  {
  public:
    SiLocAlignDBTool(const std::string& type, const std::string& name, const IInterface* parent);

    /// Does nothing
    virtual ~SiLocAlignDBTool() override {}

    /// Does nothing
    virtual StatusCode initialize() override;

    /// Does nothing
    virtual StatusCode finalize() override;

    /// Provide alignment parameters for a given plane. Returns zeros if no data available.
    const SiLocAlignData alignment (const EventContext& ctx, const int stationID, const int planeID) const override;

  private:
    SG::ReadCondHandleKey<CondAttrListCollection> m_rch_loc {this, "loc_align_key", "/FWD/Onl/AFP/Align/Local", "read condition handle for local alignement"};
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_SILOCALIGNDBTOOL_H
