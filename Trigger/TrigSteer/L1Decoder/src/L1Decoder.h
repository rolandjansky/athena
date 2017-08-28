/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Decoder_L1Decoder_h
#define L1Decoder_L1Decoder_h


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "ICTPUnpackingTool.h"
#include "IRoIsUnpackingTool.h"


/*
  @brief an algorithm used to unpack the RoIB result and provide CTP bits, active chains and RoIs

  All the unpacking is outsourced to tools. However the menu mapping, this is from CTP items to chains 
  and threshods to chains is maintained in this algorithm and provided to unpacking tools.
 */

class L1Decoder : public AthReentrantAlgorithm {
public:
  L1Decoder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode beginRun() override;
  virtual StatusCode execute_r (const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

protected: // protected to support unit testing
  //  StatusCode flagPassingRoIs(TrigCompositeUtils::DecisionContainer* rois,
  //			     const xAOD::TrigCompositeUtils* chains) const;
  virtual StatusCode readConfiguration(); 
  StatusCode prescaleChains(const HLT::IDVec& active,
			    HLT::IDVec& prescaled) const;

  StatusCode saveChainsInfo(const HLT::IDVec& chains,
			    xAOD::TrigCompositeContainer* storage,
			    const std::string& type) const;
  
private:
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey;
  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_chainsKey;

  ToolHandle<ICTPUnpackingTool> m_ctpUnpacker; // = ToolHandle<CTPUnpackingTool>("CTPUnpackingTool/CTPUnpackingTool", this); // last arg makes it private tool    
  //  ToolHandle<PrescalingTool> m_prescaler = ToolHandle<PrescalingTool>("PrescalingTool/PrescalingTool", this); 
  ToolHandleArray<IRoIsUnpackingTool> m_roiUnpackers;  // = ToolHandleArray<IRoIsUnpackingTool>(this);    
  std::map<HLT::Identifier, float> m_prescalingInfo;  
};
#endif
