/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Decoder_L1Decoder_h
#define L1Decoder_L1Decoder_h


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigTimeAlgs/TrigTimeStamp.h"
#include "L1Decoder/ICTPUnpackingTool.h"
#include "IRoIsUnpackingTool.h"
#include "IPrescalingTool.h"
#include "TrigCostMonitorMT/ITrigCostMTSvc.h"

/*
  @brief an algorithm used to unpack the RoIB result and provide CTP bits, active chains and RoIs

  All the unpacking is outsourced to tools. However the menu mapping, this is from CTP items to chains 
  and threshods to chains is maintained in this algorithm and provided to unpacking tools.
 */

class L1Decoder : public AthReentrantAlgorithm, public IIncidentListener {
public:
  L1Decoder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual void handle(const Incident& incident) override;
  virtual StatusCode execute (const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

protected: // protected to support unit testing
  //  StatusCode flagPassingRoIs(TrigCompositeUtils::DecisionContainer* rois,
  //			     const xAOD::TrigCompositeUtils* chains) const;
  virtual StatusCode readConfiguration();

  StatusCode saveChainsInfo(const HLT::IDVec& chains,
                            xAOD::TrigCompositeContainer* storage,
                            const std::string& type) const;

private:

  ///@{ @name Properties

  /// Level-1 result with RoIs from Run-2 hardware systems
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey{this, "RoIBResult", "RoIBResult", 
      "Name of RoIBResult"};

  /// Level-1 result with RoIs from Run-3 hardware systems
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_l1TriggerResultKey{this, "L1TriggerResult", "L1TriggerResult",
      "Name of the L1 Trigger Result"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey{this, "L1DecoderSummaryKey", "L1DecoderSummary", 
      "Chains status after L1 and prescaling"}; // Note: was previously property 'Chains' with default value 'HLTChains'

  SG::WriteHandleKey<TrigTimeStamp> m_startStampKey{ this, "StartStampKey", "L1DecoderStart", 
      "Object with the time stamp when decoding started" };

  Gaudi::Property<bool> m_doCostMonitoring{this, "DoCostMonitoring", false, 
    "Enables start-of-event cost monitoring behavior."};

  Gaudi::Property<std::string> m_costMonitoringChain{this, "CostMonitoringChain", "HLT_costmonitor_CostMonDS_L1All", 
    "Name of the chain which should enable HLT cost montoring."};

  ServiceHandle<ITrigCostMTSvc> m_trigCostSvcHandle { this, "TrigCostMTSvc", "TrigCostMTSvc", 
      "The trigger cost service" };

  ToolHandle<ICTPUnpackingTool> m_ctpUnpacker{this, "ctpUnpacker", "CTPUnpackingTool/CTPUnpackingTool",
      "Tool used to unpack the CTP info"};

  ToolHandleArray<IRoIsUnpackingTool> m_roiUnpackers{this, "roiUnpackers", {},
      "Tools unpacking RoIs"};

  ToolHandle<IPrescalingTool> m_prescaler{this, "prescaler", "PrescalingTool/PrescalingTool", 
      "Prescaling tool"};

  ToolHandleArray<IRoIsUnpackingTool> m_rerunRoiUnpackers{this, "rerunRoiUnpackers", {}, 
      "Unpackers that unpack RoIs into separate collections"};

};

#endif
