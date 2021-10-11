/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOALG_H 1

#include <string>
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "TrigEgammaFastPhotonHypoTool.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/HypoBase.h"


/**
 * @class TrigEgammaFastPhotonHypoAlg
 * @brief Implements Hypo selection on L2 photons
 **/

class TrigEgammaFastPhotonHypoAlg  :  public ::HypoBase 
{ 
  public: 

    TrigEgammaFastPhotonHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute(const EventContext& context) const override;

  private: 
    ToolHandleArray< TrigEgammaFastPhotonHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
    Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };
    // internally used to getch from views
    SG::ReadHandleKey< xAOD::TrigPhotonContainer > m_photonsKey {this, "Photons", "L2PhotonContainer", "Input"};
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOALG_H                                                                                                                                                          
