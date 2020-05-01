/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOALGMT_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOALGMT_PRECISION_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigEgammaPrecisionPhotonHypoTool.h"

/**
 * @class TrigEgammaPrecisionPhotonHypoAlgMT
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaPrecisionPhotonHypoAlgMT : public ::HypoBase {
 public: 

  TrigEgammaPrecisionPhotonHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigEgammaPrecisionPhotonHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigEgammaPrecisionPhotonHypoAlgMT();
  ToolHandleArray< ITrigEgammaPrecisionPhotonHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::PhotonContainer > m_photonsKey { this, "Photons", "Photons", "Photons in roi" };  
  

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONPHOTONHYPOALG_H
