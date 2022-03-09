/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "OverviewMonitorAlgorithm.h"

OverviewMonitorAlgorithm::OverviewMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode OverviewMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("OverviewMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode OverviewMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  (void)ctx; // ctx not used in the overview algorithm 

  ATH_MSG_DEBUG("OverviewMonitorAlgorithm::fillHistograms");

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;


  // Update Global overview plot

  const int ppmCrates = 8;
  const int cpmCrates = 4;

  Monitored::Scalar<int> globalOverviewX = Monitored::Scalar<int>("globalOverviewX", 0);
  Monitored::Scalar<int> globalOverviewY = Monitored::Scalar<int>("globalOverviewY", 0);

  int err_per_LB=0;

  StatusCode sc;
  const ErrorVector *errTES = 0;

  // CPM and CPM CMX Error data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_cpmErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_cpmErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(cpmCrates)) {
    ATH_MSG_INFO("No CPM error vector of expected size");
  } else {
    for (int crate = 0; crate < cpmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates;

      if ((err >> CPMStatus) & 0x1){
	globalOverviewX=SubStatus;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if (((err >> CPMEMParity) & 0x1) || ((err >> CPMHadParity) & 0x1)) {
	globalOverviewX=Parity;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if (((err >> CPMEMLink) & 0x1) || ((err >> CPMHadLink) & 0x1)) {
	globalOverviewX=LinkDown;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if ((err >> CMXCPTobParity) & 0x1) {
	globalOverviewX=GbCMXParity;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if ((err >> CMXCPSumParity) & 0x1) {
	globalOverviewX=GbCMXParity;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if ((err >> CMXCPStatus) & 0x1) {
	globalOverviewX=CMXSubStatus;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
    }
  }

  // CPM Mismatch data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_cpmMismatchLocation)) {
    sc = evtStore()->retrieve(errTES, m_cpmMismatchLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(cpmCrates)) {
    ATH_MSG_INFO("No CPM mismatch vector of expected size");
  } else {
    for (int crate = 0; crate < cpmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates;
      if (((err >> EMTowerMismatch) & 0x1) || ((err >> HadTowerMismatch) & 0x1)) {
	globalOverviewX=Transmission;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if (((err >> EMRoIMismatch) & 0x1) || ((err >> TauRoIMismatch) & 0x1)) {
	globalOverviewX=Simulation;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if (((err >> LeftCMXTobMismatch) & 0x1) ||
          ((err >> RightCMXTobMismatch) & 0x1) ||
          ((err >> RemoteSumMismatch) & 0x1)) {
	globalOverviewX=CMXTransmission;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
      if (((err >> LocalSumMismatch) & 0x1) ||
          ((err >> TotalSumMismatch) &
           0x1)) {
	globalOverviewX=CMXSimulation;
	globalOverviewY=cr;
	fill(m_packageName,globalOverviewX,globalOverviewY);
	err_per_LB+=1;
      }
    }
  }

  // errors per lumiblock
  auto lb_errors  = Monitored::Scalar<int>("lb_errors", err_per_LB);
  
  fill(m_packageName,variables);
  variables.clear();
  return StatusCode::SUCCESS;
}


