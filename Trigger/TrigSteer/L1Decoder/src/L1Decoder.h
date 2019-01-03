/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Decoder_L1Decoder_h
#define L1Decoder_L1Decoder_h


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigTimeAlgs/TrigTimeStamp.h"
#include "ICTPUnpackingTool.h"
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
                            const std::string& type,
                            const EventContext& ctx) const;
  
private:

  ///@{ @name Properties
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey{this, "RoIBResult", "RoIBResult", 
      "Name of RoIBResult"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_summaryKey{this, "L1DecoderSummaryKey", "L1DecoderSummary", 
      "Chains status after L1 and prescaling"}; // Note: was previously property 'Chains' with default value 'HLTChains'

  SG::WriteHandleKey<TrigTimeStamp> m_startStampKey{ this, "StartStampKey", "L1DecoderStart", 
      "Object with the time stamp when decoding started" };

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigFSRoIKey{
    this, "OutputFSTrigRoI", "FSRoI", "Name of the RoIs object containing the single FS RoI tagged with all jet chains produced by the unpacker"};

  Gaudi::Property<bool> m_enableCostMonitoring{this, "EnableCostMonitoring", false, 
    "Enables start-of-event cost monitoring behavior."};

  Gaudi::Property<std::string> m_costMonitoringChain{this, "CostMonitoringChain", "HLT_costmonitor", 
    "Name of the chain which should enable HLT cost montoring."};

  Gaudi::Property<std::map<std::string, std::string>> m_chainToCTPProperty{
    this, "ChainToCTPMapping", {}, "Seeding in the form: HLT_chain : L1_item"};
  ///@}

  ServiceHandle<ITrigCostMTSvc> m_trigCostSvcHandle { this, "TrigCostMTSvc", "TrigCostMTSvc", 
      "The trigger cost service" };

  ToolHandle<ICTPUnpackingTool> m_ctpUnpacker{this, "ctpUnpacker", "CTPUnpackingTool/CTPUnpackingTool",
      "Tool used to unpack the CTP info"};

  ToolHandleArray<IRoIsUnpackingTool> m_roiUnpackers{this, "roiUnpackers", {},
      "Tools unpacking RoIs"};

  ToolHandle<IPrescalingTool> m_prescaler{this, "prescaler", "PrescalingEmulationTool/PrescalingEmulationTool", 
      "Prescaling tool"};

  ToolHandleArray<IRoIsUnpackingTool> m_rerunRoiUnpackers{this, "rerunRoiUnpackers", {}, 
      "Unpackers that unpack RoIs into separate collections"};

};

#endif
