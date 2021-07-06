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

// database access
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
// general includes
#include <cassert>
#include <string>
#include <vector>
#include <memory>

namespace AFP
{
  /// Tool providing local alignment of silicon detectors from the conditions database.
  class SiGlobAlignDBTool : virtual public AFP::ISiGlobAlignDBTool, 
			   public AthAlgTool
  {
  public:
    SiGlobAlignDBTool(const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

    /// Does nothing
    virtual ~SiGlobAlignDBTool() override {}

    /// Register method SiGlobAlignDBTool::update() to be called when conditions change
    virtual StatusCode initialize() override;

    /// Does nothing
    virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

    /// Provide alignment parameters for a given station. Returns nullptr if no data available.
    const SiGlobAlignData* alignment (const int stationID) const override;
    

  private:
    /// @brief Method called when new conditions are loaded
    ///
    /// The method copies information from #m_conditionsData to 
    StatusCode update (IOVSVC_CALLBACK_ARGS) override;

    /// Attributes list storing bare information from the database
    const DataHandle<AthenaAttributeList> m_conditionsData;

    /// @brief Information about alignments represented with SiGlobAlignData objects
    ///
    /// Main variable storing information about alignment. The index
    /// of the first vector represents stationID number. The index of
    /// the second vector represents plane number in the station. If
    /// there is no information about plane a nullptr is stored.
    std::vector<std::unique_ptr<const SiGlobAlignData> > m_alignments;

    /// Name of the database folder with alignment information
    std::string m_folderName;

    /// Number of AFP stations for which the tool should work
    static const int s_numberOfStations;
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_SIGLOBALIGNDBTOOL_H

