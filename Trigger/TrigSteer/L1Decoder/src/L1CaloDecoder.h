/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Decoder_L1CaloDecoder_h
#define L1Decoder_L1CaloDecoder_h

#include "AthenaBaseComps/AthAlgorithm.h"


#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"


#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigT1Result/RoIBResult.h"
//#include "AthViews/View.h"

class L1CaloDecoder : public AthAlgorithm {
public:
  L1CaloDecoder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
private:
  std::vector<TrigConf::TriggerThreshold*> m_emtauThresholds;

  ///@{ @name Properties
  SG::WriteHandle< TrigRoiDescriptorCollection > m_trigRoIs;
  SG::WriteHandle< DataVector<LVL1::RecEmTauRoI> > m_recEMTauRoIs;  
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_decisions;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_decisionsAux;
  //  SG::WriteHandle< std::vector<SG::View*> > m_view;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc; //!< access to trigger menu
  ///@}
  
  SG::ReadHandle<ROIB::RoIBResult> m_RoIBResult;
  // can demand objects 
};



#endif
