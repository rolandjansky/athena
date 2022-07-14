/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_GFEXMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_GFEXMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoI.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"



class GfexMonitorAlgorithm : public AthMonitorAlgorithm {
public:GfexMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GfexMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:
  StringProperty m_packageName{this,"PackageName","gfexMonitor","group name for histograming"};
  // container keys including this, steering parameter, default value and help description
  SG::ReadHandleKey< xAOD::gFexJetRoIContainer > m_gFexLRJetContainerKey {this,"mygFexLRJetRoIContainer","L1_gFexLRJetRoI","SG key of the input gFex LR Jet Roi container"};
  SG::ReadHandleKey< xAOD::gFexJetRoIContainer > m_gFexRhoContainerKey {this,"mygFexRhoContainer","L1_gFexRhoRoI","SG key of the input gFex Rho container"};
  SG::ReadHandleKey< xAOD::gFexJetRoIContainer > m_gFexSRJetContainerKey {this,"mygFexSRJetOutputContainer","L1_gFexSRJetRoI","SG key of the input gFex SR Jet Roi container"};

  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarEJwojContainerKey {this,"mygScalarEJwojContainer","L1_gScalarEJwoj","SG key of the inputScalar MET and SumET (from Jets without Jets algo) container"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsJwojContainerKey {this,"mygMETComponentsJwojOutputContainer","L1_gMETComponentsJwoj","SG key of the input total MET components (from Jets without Jets algo) container"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMHTComponentsJwojContainerKey {this,"mygMHTComponentsJwojOutputContainer","L1_gMHTComponentsJwoj","SG key of the inputhard MET components (from Jets without Jets algo) container"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMSTComponentsJwojContainerKey {this,"mygMSTComponentsJwojOutputContainer","L1_gMSTComponentsJwoj","SG key of the input soft MET components (from Jets without Jets algo) container"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsNoiseCutContainerKey {this,"my_gMETComponentsNoiseCutOutputContainer","L1_gMETComponentsNoiseCut","SG key for MET_x and MET_y components obtained with the Noise Cut algorithm"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsRmsContainerKey {this,"my_gMETComponentsRmsOutputContainer","L1_gMETComponentsRms","SG key for MET_x and MET_y components obtained with the Components Rms algorithm"};

  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarENoiseCutContainerKey {this,"mygScalarENoiseCutContainer","L1_gScalarENoiseCut","SG key of the inputScalar MET and SumET contianer obtained with the Noise Cut algorithm"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarERmsContainerKey {this,"mygScalarERmsContainer","L1_gScalarERms","SG key of the inputScalar MET and SumET contianer obtained with the Components Rms algorithm"};
};
#endif
