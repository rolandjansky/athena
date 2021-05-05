/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOALGMT_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOALGMT_PRECISION_H 1

#include <string>

#include "DecisionHandling/HypoBase.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionElectronHypoTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
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

    Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };     
    SG::ReadHandleKey< xAOD::ElectronContainer > m_electronsKey { this, "Electrons", "Electrons", "Electrons in roi" };  

    ToolHandleArray< ITrigEgammaPrecisionElectronHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
    ToolHandleArray<IAsgElectronIsEMSelector> m_egammaElectronCBTools{ this, "ElectronCBSelectorTools", {},"Cut-based tools" };
    ToolHandleArray<IAsgElectronLikelihoodTool> m_egammaElectronLHTools{ this, "ElectronLHSelectorTools", {},"Likelihood tools" };
    //ToolHandleArray<?> m_egammaElectronLHTool{ this, "ElectronDNNSelectorTools", {},"DNN tools" };

    Gaudi::Property<std::vector<std::string>> m_cbNames {this, "CBNames", {}, "CB pid names."};
    Gaudi::Property<std::vector<std::string>> m_lhNames {this, "LHNames", {}, "LH pid names."};
    //Gaudi::Property<std::vector<std::string>> m_dnnNames {this, "DNNNames", {}, "DNN pid names."};

    /*Luminosity info*/
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_avgMuKey { this, "averageInteractionsPerCrossingKey", "EventInfo.averageInteractionsPerCrossing", "Decoration for Average Interaction Per Crossing" };

    ToolHandle< GenericMonitoringTool >   m_monTool { this, "MonTool", "", "Monitoring tool" };


}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONELECTRONHYPOALG_H
