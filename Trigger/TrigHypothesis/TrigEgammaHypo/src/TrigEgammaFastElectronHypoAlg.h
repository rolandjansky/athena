/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "AthViews/View.h"
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "ITrigEgammaFastElectronHypoTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"



/**
 * @class TrigEgammaFastElectronHypoAlg
 * @brief Implements Hypo selection on L2 electrons
 **/

class TrigEgammaFastElectronHypoAlg  :  public ::HypoBase 
{ 
  public: 

    TrigEgammaFastElectronHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute(const EventContext& context) const override;

  private: 
  
    ToolHandle<ILumiBlockMuTool>                      m_lumiBlockMuTool;

    SG::ReadHandleKey< xAOD::TrigElectronContainer >  m_electronsKey {this, "Electrons", "L2ElectronContainer", "Input"};
    ToolHandleArray<ITrigEgammaFastElectronHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
    ToolHandleArray<Ringer::IAsgRingerSelectorTool>   m_ringerNNTools{this, "RingerNNSelectorTools", {}, "Ringer Neural Network tools." };
    ToolHandle< GenericMonitoringTool >               m_monTool{ this, "MonTool", "", "Monitoring tool" };
    Gaudi::Property<std::vector<std::string>>         m_pidNames{this, "PidNames", {}, "Ringer pid names"};
    Gaudi::Property< bool >                           m_runInView { this, "RunInView", false , "Set input DH for running in views" };
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOALG_H
