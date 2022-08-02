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
  
  ATH_CHECK(m_cpmErrorLocation.initialize());
  ATH_CHECK(m_cpmMismatchLocation.initialize());
  ATH_CHECK(m_ppmSimBSMismatchLocation.initialize());


  return AthMonitorAlgorithm::initialize();
}

StatusCode OverviewMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("OverviewMonitorAlgorithm::fillHistograms");

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;


  // Update Global overview plot

  const int ppmCrates = 8;
  const int cpmCrates = 4;

  Monitored::Scalar<int> globalOverviewX = Monitored::Scalar<int>("globalOverviewX", 0);
  Monitored::Scalar<int> globalOverviewY = Monitored::Scalar<int>("globalOverviewY", 0);

  int err_per_LB=0;

  // CPM and CPM CMX Error data
  {
    const auto* errTES = SG::get(m_cpmErrorLocation, ctx);

    if (!errTES || errTES->size() != size_t(cpmCrates)) {
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
      } // crates
    }
  } 

  // CPM and CMX Simulation Mismatch data
  { 
	const auto* errTES = SG::get(m_cpmMismatchLocation, ctx);

    if (!errTES || errTES->size() != size_t(cpmCrates)) {
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
      } // crates
    }
  }


  // PPM Mismatch data
  { 
    const auto* errTES = SG::get(m_ppmSimBSMismatchLocation, ctx);
    if (!errTES || errTES->size() != size_t(ppmCrates)) {
      ATH_MSG_INFO("No PPM SimBS mismatch vector of expected size");
    } else {
      for (int crate = 0; crate < ppmCrates; ++crate) {
	const int err = (*errTES)[crate];
	if (err == 0)
	  continue;
	if (((err >> LUTMismatch) & 0x1)){
	  globalOverviewX=Simulation;
	  globalOverviewY=crate;
	  fill(m_packageName,globalOverviewX,globalOverviewY);
	  err_per_LB+=1;
	}
      }
    }
  }



  // errors per lumiblock and events processed
  auto lb = GetEventInfo(ctx)->lumiBlock();

  auto n_processed  = Monitored::Scalar("n_processed",0);
  auto lb_errors  = Monitored::Scalar("lb_errors",lb);
  auto n_lb_errors  = Monitored::Scalar("n_lb_errors",err_per_LB);

  variables.push_back(n_processed);
  if (err_per_LB>0) {
    variables.push_back(lb_errors);
    variables.push_back(n_lb_errors);
  }

  fill(m_packageName,variables);
  variables.clear();
  return StatusCode::SUCCESS;
}


