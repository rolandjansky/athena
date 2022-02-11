/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTSeeding_HLTSeeding_h
#define HLTSeeding_HLTSeeding_h

#include "IRoIsUnpackingTool.h"
#include "IPrescalingTool.h"
#include "L1DataConsistencyChecker.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "HLTSeeding/ICTPUnpackingTool.h"
#include "TrigCostMonitor/ITrigCostSvc.h"
#include "TrigConfxAOD/IKeyWriterTool.h"
#include "TrigTimeAlgs/TrigTimeStamp.h"
#include "xAODTrigger/TrigCompositeContainer.h"

/**
 * @class HLTSeeding
 * @brief Collects information from CTP bits and LVL1 RoIs, activates HLT chains and creates seeding RoIs
 *
 * All the unpacking is outsourced to tools. However the menu mapping, this is from CTP items to chains
 * and from thresholds to chains, is maintained in this algorithm and provided to unpacking tools.
 */
class HLTSeeding : public AthReentrantAlgorithm {
public:
  HLTSeeding(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;

private:
  static StatusCode saveChainsInfo(const HLT::IDVec& chains,
                                   xAOD::TrigCompositeContainer* storage,
                                   const std::string& type);

  /// @name Data dependencies
  /// @{
  /// Level-1 result with RoIs from Run-2 hardware systems
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey{
    this, "RoIBResult", "RoIBResult", "Name of RoIBResult"};

  /// Level-1 result with RoIs from Run-3 hardware systems
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_l1TriggerResultKey{
    this, "L1TriggerResult", "L1TriggerResult", "Name of the L1 Trigger Result"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey{
    this, "HLTSeedingSummaryKey", "HLTSeedingSummary", "Chains status after L1 and prescaling"};

  SG::WriteHandleKey<TrigTimeStamp> m_startStampKey{
    this, "StartStampKey", "HLTSeedingStart", "Object with the time stamp when decoding started" };
  /// @}

  /// @name Properties
  /// @{
  Gaudi::Property<bool> m_doCostMonitoring{
    this, "DoCostMonitoring", false, "Enables start-of-event cost monitoring behavior."};

  Gaudi::Property<std::string> m_costMonitoringChain{
    this, "CostMonitoringChain", "HLT_noalg_CostMonDS_L1All",
    "Name of the chain which should enable HLT cost monitoring."};
  /// @}

  /// @name Tools and Services
  /// @{
  ServiceHandle<ITrigCostSvc> m_trigCostSvcHandle {
    this, "TrigCostSvc", "TrigCostSvc", "The trigger cost service" };

  ToolHandle<ICTPUnpackingTool> m_ctpUnpacker{
    this, "ctpUnpacker", "CTPUnpackingTool/CTPUnpackingTool", "Tool used to unpack the CTP info"};

  ToolHandleArray<IRoIsUnpackingTool> m_roiUnpackers_roib{
    this, "RoIBRoIUnpackers", {}, "Tools unpacking Run-2 RoIs from RoIBResult"};

  ToolHandleArray<IRoIsUnpackingTool> m_roiUnpackers_xaod{
    this, "xAODRoIUnpackers", {}, "Tools unpacking xAOD RoIs from L1TriggerResult"};

  ToolHandle<IPrescalingTool> m_prescaler{
    this, "prescaler", "PrescalingTool/PrescalingTool", "Prescaling tool"};

  ToolHandle<TrigConf::IKeyWriterTool> m_keyWriterTool{
    this, "KeyWriterTool", "", "Writes the keys used when the trigger executes on an event"};

  ToolHandle<L1DataConsistencyChecker> m_consistencyChecker{
    this, "L1DataConsistencyChecker", "", "L1 data consistency check tool"};
  /// @}
};

#endif
