/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVGENPRODTOOLS_HEPMCWEIGHTSVC_H
#define EVGENPRODTOOLS_HEPMCWEIGHTSVC_H

#include "AthenaBaseComps/AthService.h"
#include "xAODEventInfo/EventInfo.h"
#include "GenInterfaces/IHepMCWeightSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include <string>

#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <mutex>

/// @brief Service to read/write HepMC's WeightContainer key names from/to IOVMetaDataContainers
///  author: will buttinger , NLAA
/// 
///
class HepMCWeightSvc : public extends<AthService, IHepMCWeightSvc>
{
public:
  using base_class::base_class;


  /**
   * @brief Standard Gaudi initialize.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Record weight names to metadata if none have yet been set.
   * @param weightNames Map of names to record.
   * @param ctx Current event context.
   *
   * Does nothing if weight names have already been set or read.
   */
  virtual StatusCode
  setWeightNames (const WeightMap& weightNames,
                  const EventContext& ctx = Gaudi::Hive::currentContext()) override;


  /**
   * @brief Return the current weight names.
   */
  virtual WeightMap
  weightNames (const EventContext& ctx = Gaudi::Hive::currentContext()) override;


  /**
   * @brief Return the current weight names.
   */
  virtual std::vector<std::string>
  weightNameVec (const EventContext& ctx = Gaudi::Hive::currentContext()) override;


private:
  /**
   * @brief Return the `channel number' for the current event.
   * @param ctx Current event context.
   *
   * Either the MC channel number or the run number if that hasn't been set.
   */
  unsigned long getChanNum (const EventContext& ctx) const;


  /**
   * @brief Try to load weight names from metadata.
   * @param chanNum Channel number for which to load weights.
   *
   * Returns the index in m_weights of the loaded weights.
   */
  unsigned int loadWeights (unsigned long chanNum);

  
  /**
   * @brief Return the index in m_weights for the current event.
   * @param ctx The current event context.
   *
   * Returns NWEIGHTS on failure.
   */
  unsigned int getWeightIndex (const EventContext& ctx);


  /// Handle to metadata tool.
  PublicToolHandle<IIOVDbMetaDataTool> m_metaDataTool
    { this, "IOVDbMetaDataTool", "IOVDbMetaDataTool" };


  // Property: Allow disabling reading from metadata.
  Gaudi::Property<bool> m_enabled
    { this, "Enable", true, "If false, will return failure on loadWeights" };


  /// Cached sets of weights.
  /// We retain NWEIGHTS sets of weights.
  /// They are overwritten in a round-robin fashion if needed.
  struct WeightInfo
  {
    /// Channel number for this set of weights.
    unsigned long m_chanNum = 0;

    /// Map of weight names.
    WeightMap m_weightNames;

    /// Sorted vector of weight names.
    std::vector<std::string> m_weightNameVec;

    /// Initialize sorted vector from map.
    void fillVec();
  };

  /// Array of weights.
  static const unsigned int NWEIGHTS = 8;
  WeightInfo m_weights[NWEIGHTS];

  /// Index of next set of weights to overwrite.
  size_t m_nextWeight = 0;

  /// Used to get MC channel for the current event.
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
    { this, "EventInfoKey", "EventInfo", "SG key for EventInfo object." };

  /// Serialize access to this service.
  std::mutex m_mutex;
};

#endif
