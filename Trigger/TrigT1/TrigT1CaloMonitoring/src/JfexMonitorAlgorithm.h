/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_JFEXMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_JFEXMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoI.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoI.h"
#include "xAODTrigger/jFexTauRoIContainer.h" 
#include "xAODTrigger/jFexTauRoI.h"
#include "xAODTrigger/jFexFwdElRoIContainer.h"
#include "xAODTrigger/jFexFwdElRoI.h"
#include "xAODTrigger/jFexMETRoIContainer.h"
#include "xAODTrigger/jFexMETRoI.h"
#include "xAODTrigger/jFexSumETRoIContainer.h"
#include "xAODTrigger/jFexSumETRoI.h"



class JfexMonitorAlgorithm : public AthMonitorAlgorithm {
public:JfexMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JfexMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

  StringProperty m_packageName{this,"PackageName","JfexMonitor","group name for histograming"};

  // container keys including this, steering parameter, default value and help description
  SG::ReadHandleKey< xAOD::jFexSRJetRoIContainer > m_jFexSRJetContainerKey {this,"jFexSRJetRoIContainer","L1_jFexSRJetRoI","SG key of the input jFex SR Jet Roi container"};
  SG::ReadHandleKey< xAOD::jFexLRJetRoIContainer > m_jFexLRJetContainerKey {this,"jFexLRJetRoIContainer","L1_jFexLRJetRoI","SG key of the input jFex LR Jet Roi container"};
  SG::ReadHandleKey< xAOD::jFexTauRoIContainer   > m_jFexTauContainerKey   {this,"jFexTauRoIContainer"  ,"L1_jFexTauRoI"  ,"SG key of the input jFex Tau Roi container"};
  SG::ReadHandleKey< xAOD::jFexFwdElRoIContainer > m_jFexFwdElContainerKey {this,"jFexFwdElRoIContainer","L1_jFexFwdElRoI","SG key of the input jFex EM Roi container"};
  SG::ReadHandleKey< xAOD::jFexMETRoIContainer   > m_jFexMETContainerKey   {this,"jFexMETRoIContainer"  ,"L1_jFexMETRoI"  ,"SG key of the input jFex MET Roi container"};
  SG::ReadHandleKey< xAOD::jFexSumETRoIContainer > m_jFexSumEtContainerKey {this,"jFexSumETRoIContainer","L1_jFexSumETRoI","SG key of the input jFex SumEt Roi container"};    
      
      
    


};
#endif
