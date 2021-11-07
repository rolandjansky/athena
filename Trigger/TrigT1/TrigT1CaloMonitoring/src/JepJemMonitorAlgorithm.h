/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_JEPJEMMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_JEPJEMMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

class JepJemMonitorAlgorithm : public AthMonitorAlgorithm {
public:JepJemMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JepJemMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

  // Phi scale for jet element eta/phi plots
  double m_phiScaleJE{};

  StringProperty m_packageName{this,"PackageName","JepJemMonitor","group name for histograming"};

  // The following enums are set in the python in order to get consistent histogram bins
  Gaudi::Property<int> m_maxSlices {this, "s_maxSlices",  5, "Maximum number of slices"};
  Gaudi::Property<int> m_crates    {this, "s_crates",     2, "Number of JEM crates"};
  Gaudi::Property<int> m_modules   {this, "s_modules",   16, "Number of modules per crate"};
  Gaudi::Property<int> m_tobsPerJEM{this, "s_tobsPerJEM", 4, "Maximum number of TOBs per JEM sent to CMX"};

  // container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::JetElementContainer> m_JetElementLocation{this, "JetElementLocation", LVL1::TrigT1CaloDefs::JetElementLocation, "Jet Element Container"};
  SG::ReadHandleKey<xAOD::JEMEtSumsContainer> m_JEMEtSumsLocation{this, "JemEtSumsLocation", LVL1::TrigT1CaloDefs::JEMEtSumsLocation, "JEM Et Sums Container"};
  SG::ReadHandleKey<xAOD::JEMTobRoIContainer> m_JEMRoILocation{this, "JEMTobRoILocation", LVL1::TrigT1CaloDefs::JEMTobRoILocation, "JEM RoI Container"};

  // Error summary bin labels
  enum SummaryErrors { EMParity, HadParity, EMLink, HadLink, JEMStatus };

  // JEM fill functions
  void fillJEMPhiMon(double eta, double phi,
                     Monitored::Scalar<double> &phiMonitoredScalar
                     ) const;

  void fillJEMEtaVsPhiMon(double eta, double phi,
                          Monitored::Scalar<double> &etaMonitoredScalar,
                          Monitored::Scalar<double> &phiMonitoredScalar,
                          Monitored::Scalar<int> &weight
                          ) const;

  void fillJEMEtaVsPhiMon(double eta, double phi,
                          Monitored::Scalar<double> &etaMonitoredScalar,
                          Monitored::Scalar<double> &phiMonitoredScalar
                          ) const;

  void fillJEMRoIEtaVsPhiMon(double eta, double phi,
                             Monitored::Scalar<double> &etaMonitoredScalar,
                             Monitored::Scalar<double> &phiMonitoredScalar,
                             Monitored::Scalar<int> &weight
                             ) const;

  void fillJEMRoIEtaVsPhiMon(double eta, double phi,
                             Monitored::Scalar<double> &etaMonitoredScalar,
                             Monitored::Scalar<double> &phiMonitoredScalar
                             ) const;
};

#endif
