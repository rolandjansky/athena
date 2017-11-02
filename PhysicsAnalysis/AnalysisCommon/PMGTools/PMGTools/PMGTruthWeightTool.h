/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_PMGTRUTHWEIGHTTOOL_H
#define PMGTOOLS_PMGTRUTHWEIGHTTOOL_H

// EDM include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthMetaData.h"
#include "xAODTruth/TruthMetaDataContainer.h"

// Interface include(s):
#include "PMGAnalysisInterfaces/IPMGTruthWeightTool.h"


namespace PMGTools
{
  /// Implementation for the xAOD truth meta data weight tool
  ///
  /// @author James Robinson <james.robinson@cern.ch>
  ///
  class PMGTruthWeightTool: public virtual IPMGTruthWeightTool, public asg::AsgMetadataTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(PMGTruthWeightTool, PMGTools::IPMGTruthWeightTool)

  public:
    /// Create a constructor for standalone usage
    PMGTruthWeightTool(const std::string& name);

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{

    /// Function initialising the tool
    virtual StatusCode initialize();

    /// @}

    /// @name Function(s) implementing the IPMGTruthWeightTool interface
    /// @{

    /// Implements interface from IPMGTruthWeightTool
    const std::vector<std::string>& getWeightNames() const;

    /// Implements interface from IPMGTruthWeightTool
    float getWeight(const std::string& weightName) const;

    /// Implements interface from IPMGTruthWeightTool
    bool hasWeight(const std::string& weightName) const;

    /// @}

  protected:
    /// @name Callback function(s) from AsgMetadataTool
    /// @{

    /// Function called when a new input file is opened
    virtual StatusCode beginInputFile();

    /// Function called when a new event is loaded
    virtual StatusCode beginEvent();

    /// @}

    /// Loads weight information from xAOD::TruthMetaDataContainer
    virtual StatusCode loadMetaData();

    /// Loads weight information from HepMCWeightNames
    virtual StatusCode loadMetaData(const std::map<std::string, int>& hepMCWeightNamesMap);

    /// Validate weight caches
    virtual StatusCode validateWeightCaches() const;

    /// Clear caches
    virtual void clearWeightCaches();

    /// Stores the meta data record name
    std::string m_metaName;

    /// Current MC channel number
    uint32_t m_mcChannelNumber;

    /// Ptr to the meta data container for this file
    const xAOD::TruthMetaDataContainer* m_metaDataContainer;

    /// Ptr to the event info for the current event
    const xAOD::EventInfo* m_evtInfo;

    /// Flag to indicate whether the xAOD::TruthMetaData objects have incorrect McChannelNumber
    mutable bool m_hasInvalidMcChannel;

    /// List of available weight names
    std::vector<std::string> m_weightNames;

    /// List of indices for available weights
    std::unordered_map<std::string, int> m_weightIndices;
  };
} // namespace PMGTools

#endif // PMGTOOLS_PMGTRUTHWEIGHTTOOL_H
