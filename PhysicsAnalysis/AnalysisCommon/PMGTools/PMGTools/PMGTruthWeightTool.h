/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_PMGTRUTHWEIGHTTOOL_H
#define PMGTOOLS_PMGTRUTHWEIGHTTOOL_H

// EDM include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthMetaData.h"
#include "xAODTruth/TruthMetaDataContainer.h"

// Interface include(s):
#include "PMGAnalysisInterfaces/IPMGTruthWeightTool.h"


namespace PMGTools
{
  /// \brief the systematics prefix for generator weights
  constexpr char generatorSystematicsPrefix[] {"GEN_"};

  /// Implementation for the xAOD truth meta data weight tool
  ///
  /// @author James Robinson <james.robinson@cern.ch>
  ///
  class PMGTruthWeightTool : public virtual IPMGTruthWeightTool, public asg::AsgMetadataTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(PMGTruthWeightTool, PMGTools::IPMGTruthWeightTool)

  public:
    /// Create a constructor for standalone usage
    PMGTruthWeightTool(const std::string& name);

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{

    /// Function initialising the tool
    virtual StatusCode initialize() override;

    /// @}

    /// @name Function(s) implementing the IPMGTruthWeightTool interface
    /// @{

    /// Implements interface from IPMGTruthWeightTool
    virtual const std::vector<std::string>& getWeightNames() const override;

    /// Implements interface from IPMGTruthWeightTool
    virtual float getWeight(const std::string& weightName) const override;

    /// Implements interface from IPMGTruthWeightTool
    virtual bool hasWeight(const std::string& weightName) const override;

    /// Implements interface from IPMGTruthWeightTool
    virtual float getSysWeight() const override;

    /// Implements interface from IPMGTruthWeightTool
    virtual size_t getSysWeightIndex() const override;

    /// @}

    /// @name Function(s) implementing the ISystematicsTool interface
    /// @{

    /// Implements interface from ISystematicsTool
    virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const override;

    /// Implements interface from ISystematicsTool
    virtual CP::SystematicSet affectingSystematics() const override;

    /// Implements interface from ISystematicsTool
    virtual CP::SystematicSet recommendedSystematics() const override;

    /// Implements interface from ISystematicsTool
    virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet& systConfig) override;

    /// @}

  protected:
    /// @name Callback function(s) from AsgMetadataTool
    /// @{

    /// Function called when a new input file is opened
    virtual StatusCode beginInputFile() override;

    /// Function called when a new event is loaded
    virtual StatusCode beginEvent() override;

    /// @}

    /// Loads weight information from xAOD::TruthMetaDataContainer
    StatusCode loadMetaData();

    /// Loads weight information from POOL using HepMCWeightNames
    StatusCode loadPOOLMetaData();

    /// Validate weight caches
    StatusCode validateWeightLocationCaches();

    /// Clear caches
    void clearWeightLocationCaches();

    /// Process the weight name to be used for systematics
    std::string weightNameToSys(const std::string &name) const;

    /// Stores the meta data record name
    std::string m_metaName;

    /// Current MC channel number
    uint32_t m_mcChannelNumber{};

    /// Systematics set of the weight systematics
    CP::SystematicSet m_systematicsSet;

    /// Ptr to the meta data container for this file
    const xAOD::TruthMetaDataContainer* m_metaDataContainer{nullptr};

    /// Flag to indicate whether the xAOD::TruthMetaData objects have incorrect McChannelNumber
    bool m_useChannelZeroInMetaData{false};

    /// Available weight names for this file
    std::vector<std::string> m_weightNames;

    /// Indices of available weights in this file
    std::unordered_map<std::string, size_t> m_weightIndices;

    /// Indices of available weights in this file
    std::unordered_map<std::string, size_t> m_weightIndicesSys;

    /// Values of weights in this event
    std::vector<float> m_weights;

    /// Weight data cache helper struct
    struct WeightData
    {
      float weight;
      std::size_t index;
    };

    /// Systematics cache of available weights in this file
    std::unordered_map<CP::SystematicSet, WeightData> m_weightData;

    /// Current systematics weight
    WeightData *m_currentWeightData {nullptr};
  };
} // namespace PMGTools

#endif // PMGTOOLS_PMGTRUTHWEIGHTTOOL_H
