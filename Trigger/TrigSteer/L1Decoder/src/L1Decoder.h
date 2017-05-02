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
#include "CTPUnpackingTool.h"


/*
  @brief an algorithm used to unpack the RoIB result and provide CTP bits, active chains and RoIs

  All the unpacking is outsourced to tools. However the menu mapping, this is from CTP items to chains 
  and threshods to chains is maintained in this algorithm and provided to unpacking tools.
 */

class L1Decoder : public AthReentrantAlgorithm {
public:
  L1Decoder(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute_r (const EventContext& ctx) const override;
  StatusCode finalize();

protected: // protected to support unit testing
  //  StatusCode flagPassingRoIs(TrigCompositeUtils::DecisionContainer* rois,
  //			     const xAOD::TrigCompositeUtils* chains) const;
  virtual StatusCode readConfiguration(); 
  StatusCode prescaleChains(const std::vector<HLT::Identifier>& active,
			    std::vector<HLT::Identifier>& prescaled) const;
  
private:
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey;
  SG::WriteHandleKey< xAOD::TrigCompositeContainer > m_chainsKey;

  ToolHandle<CTPUnpackingTool> m_ctpUnpacker;  
  //  ToolHandle<PrescalingTool> m_prescaler;
  //  ToolHandleArray<RoIUnpackingTool> m_roiUnpackers;
  
  CTPUnpackingTool::IndexToIdentifiers m_ctpIDToChain;
  HLT::IDtoVIDMap m_thresholdToChain;
  std::map<HLT::Identifier, float> m_prescalingInfo;


  
};



#endif
