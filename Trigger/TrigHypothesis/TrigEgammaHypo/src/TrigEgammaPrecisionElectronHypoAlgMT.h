/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOALGMT_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONHYPOALGMT_PRECISION_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "AthViews/View.h"

#include "ITrigEgammaPrecisionElectronHypoTool.h"

/**
 * @class TrigEgammaPrecisionElectronHypoAlgMT
 * @brief Implements precision electron selection for the new HLT framework
 **/
class TrigEgammaPrecisionElectronHypoAlgMT : public ::HypoBase {
 public: 

  TrigEgammaPrecisionElectronHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigEgammaPrecisionElectronHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigEgammaPrecisionElectronHypoAlgMT();
  ToolHandleArray< ITrigEgammaPrecisionElectronHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
  Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };     
  SG::ReadHandleKey< xAOD::ElectronContainer > m_electronsKey { this, "Electrons", "Electrons", "Electrons in roi" };  
  

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONELECTRONHYPOALG_H
