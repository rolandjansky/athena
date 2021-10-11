/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef __ASGELECTRONEFFICIENCYCORRECTIONTOOL__
#define __ASGELECTRONEFFICIENCYCORRECTIONTOOL__

/**
  @class AthElectronEfficiencyCorrectionTool
  @brief Calculate the egamma scale factors in Athena

  @author Kristin Lohwasser , Rob Roy Fletcher, Karsten Koeneke
  @date   May 2014
  */

// Fwd Declarations
class TH2F;
namespace Root {
class TElectronEfficiencyCorrectionTool;
}
#include "xAODEgamma/ElectronFwd.h"
// Interface
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
// STL includes
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
// PAT includes
#include "AsgTools/AsgMetadataTool.h"
#include "PATCore/PATCoreEnums.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicRegistry.h"

class AsgElectronEfficiencyCorrectionTool
  : virtual public IAsgElectronEfficiencyCorrectionTool
  , public asg::AsgMetadataTool
{
  ASG_TOOL_CLASS(AsgElectronEfficiencyCorrectionTool,
                 IAsgElectronEfficiencyCorrectionTool)

public:
  /// Standard constructor
  AsgElectronEfficiencyCorrectionTool(const std::string& myname);

  /// Standard destructor
  virtual ~AsgElectronEfficiencyCorrectionTool();

  /// Gaudi Service Interface method implementations
  virtual StatusCode initialize();

  /// Metadata methods
  virtual StatusCode beginInputFile();
  virtual StatusCode beginEvent();

  int getNumberOfToys() { return m_number_of_toys; };

  CP::CorrectionCode getEfficiencyScaleFactor(
    const xAOD::Electron& inputObject,
    double& efficiencyScaleFactor) const;
  CP::CorrectionCode applyEfficiencyScaleFactor(
    const xAOD::Electron& inputObject) const;

  /// print available/implemented correlation models
  void printCorrelationModels()
  {
    ATH_MSG_INFO(
      " Available Correlation Models for the ElectronEfficiencyCorrectionTool");
    ATH_MSG_INFO("FULL");
    ATH_MSG_INFO("SIMPLIFIED");
    ATH_MSG_INFO("MCTOYS");
    ATH_MSG_INFO("SYST");
    ATH_MSG_INFO("COMBMCTOYS");
    ATH_MSG_INFO("TOTAL");
  };

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic(
    const CP::SystematicVariation& systematic) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  /// returns: the currently applied systematics
  const CP::SystematicSet& appliedSystematics() const
  {
    return *m_appliedSystematics;
  }

  /// Configure this tool for the given systematics
  virtual StatusCode applySystematicVariation(
    const CP::SystematicSet& systConfig);

  StatusCode registerSystematics();

  int systUncorrVariationIndex(const xAOD::Electron& inputObject) const;

  // Private member variables
private:
  // To check if the metadat can be retrieved
  bool m_metadata_retrieved = false;

  // Get the simulation type from metadata
  StatusCode get_simType_from_metadata(
    PATCore::ParticleDataType::DataType& result) const;

  int currentSimplifiedUncorrSystRegion(const double cluster_eta,
                                        const double et) const;
  int currentUncorrSystRegion(const double cluster_eta, const double et) const;

  /// The main calculate method: the actual correction factors are determined
  /// here
  StatusCode InitSystematics();

  // struct for toys
  struct SystConf
  {
    float m_total;
    unsigned m_toy_index;
    float m_toy_scale;
  };

  // Gets the correction filename from map
  virtual StatusCode getFile(const std::string& recokey,
                             const std::string& idkey,
                             const std::string& isokey,
                             const std::string& trigkey);

  // Convert reco, ID, iso and trigger key values into a
  // single key according to the map file key format
  std::string convertToOneKey(const std::string& recokey,
                              const std::string& idkey,
                              const std::string& isokey,
                              const std::string& trigkey) const;

  // Retrieves the value from the provided map file as
  // associated with the provided key
  std::string getValueByKey(const std::string& mapFile, const std::string& key);

  // Reads the provided map file
  // and construct the map
  StatusCode read(const std::string& strFile);

  // Retrieves the value from the provided map file if
  // even the provided key is found. If the key has an
  // association then, the actual retrieved value would
  // be assigned to the 2nd argument of this method
  std::string getValue(const std::string& strKey, std::string& strValue);

  /// Pointer to the underlying ROOT based tool
  Root::TElectronEfficiencyCorrectionTool* m_rootTool;

  /// Systematics filter map
  std::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;

  /// Affected systematic, should be done only once
  CP::SystematicSet m_affectedSys;

  /// Currently applied systematics
  CP::SystematicSet* m_appliedSystematics;

  // Correlation Model
  std::string m_correlation_model_name;
  int m_correlation_model;

  // Map-key properties
  std::map<std::string, std::string> m_map;
  std::string m_mapFile;

  // Four SF key Properties
  std::string m_recoKey;
  std::string m_idKey;
  std::string m_isoKey;
  std::string m_trigKey;

  /// The list of file names
  std::vector<std::string> m_corrFileNameList;

  /// The prefix string for the result
  std::string m_resultPrefix;

  /// The string for the result
  std::string m_resultName;

  // The prefix for the systematic name
  std::string m_sysSubstring;

  /// The Event info collection name
  std::string m_eventInfoCollectionName;

  // use RandomRun Number
  bool m_useRandomRunNumber;
  int m_defaultRandomRunNumber;

  // The data type
  PATCore::ParticleDataType::DataType m_dataType;

  /// Force the data type to a given value
  int m_dataTypeOverwrite;

  // Number of toys:
  int m_number_of_toys;

  // seed for toys:
  int m_seed_toys;

  // scale for toy uncertainty:
  double m_scale_toys;

  // number if systematics uncertainty
  int m_nCorrSyst;
  int m_nUncorrSyst;
  std::map<float, std::vector<float>> m_pteta_bins;

  // custom binning for SIMPLIFIED model
  std::vector<float> m_uncorrSimplfEtaBinsUser;
  std::vector<float> m_uncorrSimplfEtBinsUser;

  // simplified uncorrelation regions
  TH2F* m_UncorrRegions;
  int m_nSimpleUncorrSyst;

}; // End: class definition

#endif
