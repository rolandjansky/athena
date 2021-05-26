/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOALG_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOALG_PRECISION_H 1

#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "ITrigEgammaPrecisionPhotonHypoTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"


/**
 * @class TrigEgammaPrecisionPhotonHypoAlg
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaPrecisionPhotonHypoAlg : public ::HypoBase {
  public: 

    TrigEgammaPrecisionPhotonHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private: 

    ToolHandleArray< ITrigEgammaPrecisionPhotonHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" }; 
    SG::ReadHandleKey< xAOD::PhotonContainer > m_photonsKey { this, "Photons", "Photons", "Photons in roi" };  
  
    Gaudi::Property<std::vector<std::string>> m_isemNames {this, "IsEMNames", {}, "IsEM pid names."};
    ToolHandleArray< IAsgPhotonIsEMSelector > m_egammaPhotonCutIDTools { this, "PhotonIsEMSelectorTools", {},"" };

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONPHOTONHYPOALG_H
