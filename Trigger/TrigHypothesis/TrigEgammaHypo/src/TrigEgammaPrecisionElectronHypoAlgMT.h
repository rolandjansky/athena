/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOALGMT_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOALGMT_PRECISION_H 1

#include <string>

#include "DecisionHandling/HypoBase.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionElectronHypoTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "StoreGate/ReadDecorHandle.h"
#include "xAODEventInfo/EventInfo.h"


/**
 * @class TrigEgammaPrecisionElectronHypoAlgMT
 * @brief Implements precision electron selection for the new HLT framework
 **/
class TrigEgammaPrecisionElectronHypoAlgMT : public ::HypoBase {
 public: 

  TrigEgammaPrecisionElectronHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
 
 private: 
  TrigEgammaPrecisionElectronHypoAlgMT();
  ToolHandleArray< ITrigEgammaPrecisionElectronHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };     
  SG::ReadHandleKey< xAOD::ElectronContainer > m_electronsKey { this, "Electrons", "Electrons", "Electrons in roi" };  
  
  ToolHandle<IAsgElectronLikelihoodTool> m_egammaElectronLHTool_vloose { this, "ElectronLHSelector_vLoose", "AsgElectronLHVLooseSelector","Likelihood tool vloose tune" };
  ToolHandle<IAsgElectronLikelihoodTool> m_egammaElectronLHTool_loose { this, "ElectronLHSelector_Loose", "AsgElectronLHLooseSelector", "Likelihood tool loose tune" };  
  ToolHandle<IAsgElectronLikelihoodTool> m_egammaElectronLHTool_medium { this, "ElectronLHSelector_Medium", "AsgElectronLHMediumSelector", "Likelihood tool medium tune" };
  ToolHandle<IAsgElectronLikelihoodTool> m_egammaElectronLHTool_tight { this, "ElectronLHSelector_Tight", "AsgElectronLHTightSelector", "Likelihood tool tight tune" };

  /*Luminosity info*/
  SG::ReadDecorHandleKey<xAOD::EventInfo> m_avgMuKey { this, "averageInteractionsPerCrossingKey", "EventInfo.averageInteractionsPerCrossing", "Decoration for Average Interaction Per Crossing" };


}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONELECTRONHYPOALG_H
