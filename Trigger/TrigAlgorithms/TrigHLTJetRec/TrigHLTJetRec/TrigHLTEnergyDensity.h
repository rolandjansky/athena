/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTENERGYDENSITY_H
#define TRIGHLTJETREC_TRIGHLTENERGYDENSITY_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

class IEventShapeTool;
class ITriggerPseudoJetGetter;

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
  ToolHandle<ITriggerPseudoJetGetter>  m_pseudoJetGetter;

  std::string m_eventShapeSGKey;  // allow fetching and monitoring.
  std::string m_caloClusterContainerSGKey;  // allow different calo cluster sources
  float m_energyDensity;  // allow monitoring.
}; 
#endif
