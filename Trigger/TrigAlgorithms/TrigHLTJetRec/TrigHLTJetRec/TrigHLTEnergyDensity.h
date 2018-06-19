/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTENERGYDENSITY_H
#define TRIGHLTJETREC_TRIGHLTENERGYDENSITY_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
// #include "JetEDM/LabelIndex.h"
// #include "JetEDM/PseudoJetVector.h"

class IEventShapeTool;
class ITriggerPseudoJetGetter2;

class TrigHLTEnergyDensity: public HLT::FexAlgo {

 public:
  TrigHLTEnergyDensity(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTEnergyDensity();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();
 
private:

  // IEventShapeTool - modifies EventShape to add energyDensity etc
  ToolHandle<IEventShapeTool> m_energyDensityTool;

  /* A PseudojetGetter shared by this algorithm and the IEventShapeTool
     The algorithm loads the psg, the iest processes them. */
  ToolHandle<ITriggerPseudoJetGetter2>  m_pseudoJetGetter;

  // allow fetching and monitoring of the energy density calculated
  // by the tool.
  std::string m_eventShapeSGKey;

  // allow different calo cluster sources
  std::string m_caloClusterContainerSGKey;  

// LC or EM - used to set the cluster lacal calibration
  std::string m_clusterCalib;

  float m_energyDensity;  // allow monitoring.

  // HLT::ErrorCode getPseudoJets(const xAOD::CaloClusterContainer*,
  //                              jet::LabelIndex*,
  //                              jet::PseudoJetVector&);

}; 
#endif
