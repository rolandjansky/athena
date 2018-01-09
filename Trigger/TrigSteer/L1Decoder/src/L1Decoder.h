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
  virtual StatusCode start() override;
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

  ///@{ @name Properties
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey{this, "RoIBResult", "RoIBResult", 
      "Name of RoIBResult"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_chainsKey{this, "Chains", "HLTChains", 
      "Chains status after L1 and prescaling"};

  ToolHandle<ICTPUnpackingTool> m_ctpUnpacker{this, "ctpUnpacker", "CTPUnpackingTool/CTPUnpackingTool",
      "Tool used to unpack the CTP info"};

  ToolHandleArray<IRoIsUnpackingTool> m_roiUnpackers{this, "roiUnpackers", {},
      "Tools unpacking RoIs"};
  ///@}

  std::map<HLT::Identifier, float> m_prescalingInfo;  
};

#endif
