/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_JEPJEMMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_JEPJEMMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

// maybe??
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

class JepJemMonitorAlgorithm : public AthMonitorAlgorithm {
public:JepJemMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JepJemMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  struct MonitorJepJemJE {
    const xAOD::JetElement* jelement;

    // some modified/derived information
    double eta;
    double phi;
    // errors
    // bool emParityError;
    // bool emLinkDownError;
    // bool emGLinkParityError[8];
    // bool hadParityError;
    // bool hadLinkDownError;
  };


private:

  StringProperty m_packageName{this,"PackageName","JepJemMonitor","group name for histograming"};

  // The following enums are set in the python in order to get consistent histogram bins
  // only add here if they are used in the .cxx
  Gaudi::Property<int> m_maxSlices{this,"s_maxSlices", 5,  "Maximum number of slices"};

  // container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_xAODTriggerTowerContainerName{this, "BS_xAODTriggerTowerContainer",LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation,"Trigger Tower Container"};
  SG::ReadHandleKey<xAOD::JetElementContainer> m_JetElementLocation{this, "JetElementLocation" ,LVL1::TrigT1CaloDefs::JetElementLocation,"Jet Element Container"};
  
  StatusCode fillJepJemJEVectors(SG::ReadHandle<xAOD::JetElementContainer> &jetElements,
						    std::vector<MonitorJepJemJE> &monJepJemJEs_em, std::vector<MonitorJepJemJE> &monJepJemJEs_had
						    ) const;

};
#endif
