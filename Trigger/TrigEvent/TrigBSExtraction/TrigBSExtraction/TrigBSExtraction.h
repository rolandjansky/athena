/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_TRIGBSEXTRACTION_H
#define TRIGBSEXTRACTION_TRIGBSEXTRACTION_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandleKeyArray.h"

#include "TrigNavigation/Navigation.h"
#include "TrigBSExtraction/ITrigBStoxAODTool.h"
#include "TrigSteeringEvent/HLTResult.h"


/**
 * @brief Top algorithms which unpacks objects from BS and places them in SG.
 *
 * If the BStoxAOD tool is configured an AOD->xAOD conversion is done for the
 * containers in the Run-1 EF result (not for L2).
 */

class TrigBSExtraction : public AthAlgorithm {
public:
  TrigBSExtraction(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  /**
   @brief method which does loop over objects
   @param  navTool   navigation tool
   @param  key       does the job for EF or L2
   @param  equalize  flattens the EDM if true
   @param  xAODCnv   do xAOD conversion of Run-1 EF containers
   */
  StatusCode repackFeaturesToSG (HLT::Navigation& navTool,
                                 const SG::ReadHandleKey<HLT::HLTResult>& key,
                                 SG::WriteHandleKey<HLT::HLTResult>& keyOut,
                                 bool equalize, bool xAODConv);

  ToolHandle<HLT::Navigation> m_navToolL2{this, "NavigationForL2", "HLT::Navigation/NavigationForL2",
                                          "Navigation tool for Run-1 L2 result"};
  ToolHandle<HLT::Navigation> m_navTool{this, "Navigation", "HLT::Navigation/Navigation",
                                        "Navigation tool for EF/HLT result"};

  ToolHandle<ITrigBStoxAODTool> m_xAODTool{this, "BStoxAOD", "",
                                           "Optional xAOD converter tool for Run-1 EF AOD containers"};

  SG::ReadHandleKey<HLT::HLTResult> m_l2ResultKeyIn{
    this, "L2ResultKeyIn", "", "Input key for L2 result (Run-1)"};
  SG::WriteHandleKey<HLT::HLTResult> m_l2ResultKeyOut{
    this, "L2ResultKeyOut", "", "Output key for L2 result (Run-1)"};

  SG::ReadHandleKey<HLT::HLTResult> m_hltResultKeyIn{
    this, "HLTResultKeyIn", "HLTResult_HLT_BS", "Input key for EF/HLT result"};
  SG::WriteHandleKey<HLT::HLTResult> m_hltResultKeyOut{
    this, "HLTResultKeyOut", "HLTResult_HLT", "Output key for EF/HLT result"};

  SG::ReadHandleKeyArray<HLT::HLTResult> m_dataScoutingKeysIn{
    this, "DSResultKeysIn", {}, "Input keys for DataScouting HLT results"};
  SG::WriteHandleKeyArray<HLT::HLTResult> m_dataScoutingKeysOut{
    this, "DSResultKeysOut", {}, "Output keys for DataScouting HLT results"};
};


#endif // TRIGBSEXTRACTION_TRIGBSEXTRACTION_H
