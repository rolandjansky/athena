/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaForwardPrecisionElectronHypoAlgMT_h
#define TrigEgammaForwardPrecisionElectronHypoAlgMT_h

#include <string>

#include "DecisionHandling/HypoBase.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaForwardPrecisionElectronHypoTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "StoreGate/ReadDecorHandle.h"
#include "xAODEventInfo/EventInfo.h"


/**
 * @class TrigEgammaForwardPrecisionElectronHypoAlgMT
 * @brief Implements precision electron selection for the new HLT framework
 **/
class TrigEgammaForwardPrecisionElectronHypoAlgMT : public ::HypoBase {

  public: 

    TrigEgammaForwardPrecisionElectronHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;
  
  private: 

    TrigEgammaForwardPrecisionElectronHypoAlgMT();

    Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };     
    SG::ReadHandleKey< xAOD::ElectronContainer > m_electronsKey { this, "Electrons", "Electrons", "Electrons in roi" };  

    ToolHandleArray< ITrigEgammaForwardPrecisionElectronHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
    ToolHandleArray<IAsgElectronLikelihoodTool> m_egammaElectronLHTools{ this, "FwdElectronLHSelectorTools", {},"Forward Likelihood tools" };

    Gaudi::Property<std::vector<std::string>> m_lhNames {this, "LHNames", {}, "LH pid names."};

    /*Luminosity info*/
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_avgMuKey { this, "averageInteractionsPerCrossingKey", "EventInfo.averageInteractionsPerCrossing", "Decoration for Average Interaction Per Crossing" };

    ToolHandle< GenericMonitoringTool >   m_monTool { this, "MonTool", "", "Monitoring tool" };


}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONELECTRONHYPOALG_H
