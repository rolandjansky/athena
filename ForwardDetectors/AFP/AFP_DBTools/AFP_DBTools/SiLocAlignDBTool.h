/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   SiLocAlignDBTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-12-03
/// 
/// @brief  Header file for SiLocAlignDBTool used to read local alignment for database
#ifndef AFP_DBTOOLS_SILOCALIGNDBTOOL_H
#define AFP_DBTOOLS_SILOCALIGNDBTOOL_H

// Local includes
#include "AFP_DBTools/AlignmentData.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/ServiceHandle.h"

// database access
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "AthenaPoolUtilities/AthenaAttributeList.h"

// general includes
#include <cassert>
#include <string>
#include <vector>

namespace AFP
{

  /// Tool providing local alignment of silicon detectors from the conditions database.
  class SiLocAlignDBTool : public ::AthAlgTool
  {
  public:
    SiLocAlignDBTool(const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

    /// Deletes all objects stored in #m_alignments
    virtual ~SiLocAlignDBTool() override {clearAlignments();}

    /// Register method SiLocAlignDBTool::update() to be called when conditions change
    virtual StatusCode initialize() override;

    /// Does nothing
    virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

    /// Provide alignment parameters for a given plane. Returns nullptr if no data available.
    const AlignmentData* alignment (const int stationID, const int planeID) const;

    /// Returns reference to a vector of alignments for a given station.
    ///
    /// It is enough to get this reference only once, because the
    /// vector is a member of the class. Whenever new conditions are
    /// loaded the values in the vector will be updated.
    ///
    /// @warning if in database there was no data about the given
    /// layer a nullptr will be stored in the vector.
    const std::vector<const AlignmentData*>& alignment (const int stationID) const
    {assert (stationID < s_numberOfStations); return m_alignments.at(stationID);}


  private:
    /// @brief Method called when new conditions are loaded
    ///
    /// The method copies information from #m_conditionsData to 
    StatusCode update (IOVSVC_CALLBACK_ARGS);

    /// Deletes all objects from #m_alignments and sets pointers to nullptr
    void clearAlignments ();

    /// Attributes list storing bare information from the database
    const DataHandle<CondAttrListCollection> m_conditionsData;

    /// @brief Information about alignments represented with AlignmentData objects
    ///
    /// Main variable storing information about alignment. The index
    /// of the first vector represents stationID number. The index of
    /// the second vector represents plane number in the station. If
    /// there is no information about plane a nullptr is stored.
    std::vector<std::vector<const AlignmentData*> > m_alignments;

    /// Name of the database folder with alignment information
    std::string m_folderName;

    /// Number of AFP stations for which the tool should work
    static const int s_numberOfStations;
  };


}      // namespace AFP

#endif // > ! AFP_DBTOOLS_SILOCALIGNDBTOOL_H
