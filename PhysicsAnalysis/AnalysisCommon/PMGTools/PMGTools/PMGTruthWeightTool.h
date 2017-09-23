/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_PMGTRUTHWEIGHTTOOL_H
#define PMGTOOLS_PMGTRUTHWEIGHTTOOL_H

// STL includes
#include <memory>

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
  /// @author Tobias Bisanz  <tobias.bisanz@cern.ch>
  /// @author Dag Gillberg <dag.gillberg@cern.ch>, trivial modifications
  /// @author James Robinson <james.robinson@cern.ch>, rewrite for inclusion in PMGTools
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
    const std::vector<float>& getWeights() const;

    /// Implements interface from IPMGTruthWeightTool
    float getWeight(const std::string& weightName) const;

    /// Implements interface from IPMGTruthWeightTool
    size_t getWeightIndex(const std::string& weightName) const;

    /// Implements interface from IPMGTruthWeightTool
    bool hasWeight(const std::string& weightName) const;

    /// @}

  protected:
    /// @name Function(s) implementing the IPMGTruthWeightTool interface
    /// @{

    /// Implements interface from IPMGTruthWeightTool
    virtual std::shared_ptr<IPMGTruthWeightIndexRetriever> spawnTruthWeightIndexRetriever(std::string weightName) const;

    /// @}

    /// @name Callback function(s) from AsgMetadataTool
    /// @{

    /// Function called when a new input file is opened
    virtual StatusCode beginInputFile();

    /// Function called when a new event is loaded
    virtual StatusCode beginEvent();

    /// @}

    /// Function called when new meta data is available
    virtual void onNewMetaData();

    /// Stores the meta data record name
    std::string m_metaName;

    /// Ptr to the meta data container
    const xAOD::TruthMetaDataContainer* m_metaDataContainer;

    /// Ptr to the currently valid meta data object
    const xAOD::TruthMetaData* m_metaData;

    /// Map of all spawned PMGTruthWeightIndexRetriever, indexed by name
    mutable std::map<std::string, std::weak_ptr<IPMGTruthWeightIndexRetriever> > m_indexRetrievers;

    /// Previous MC channel number
    uint32_t m_mcChannelNumber;

    /// Flag to check if we actaully processed a previous event
    bool m_uninitialized;

    /// Event info
    const xAOD::EventInfo* m_evtInfo;

    /// TruthEvent
    const xAOD::TruthEventContainer* m_truthEvents;

    /// weight names from POOL metadata .. a fallback, available to athena only
    std::vector<std::string> m_poolWeightNames;
  };
} // namespace PMGTools

#endif // PMGTOOLS_PMGTRUTHWEIGHTTOOL_H
