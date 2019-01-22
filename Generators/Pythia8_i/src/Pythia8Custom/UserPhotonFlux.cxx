/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "UserPhotonFlux.h"

  UserPhotonFlux::UserPhotonFlux(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent)
 {
    declareInterface<IPythia8Custom>(this);
    declareProperty("Process", m_process=1);
    declareProperty("NuclearCharge", m_flux_Z=82.);
    declareProperty("MinimumB", m_flux_min_b=13.24);
    declareProperty("MinimumX", m_flux_min_x=-1);//default applies no cut since sampled x's always >0.
  }
  
  
  StatusCode UserPhotonFlux::initialize() {
    ATH_MSG_INFO( "Initialisation of " << name() << " was successful" );
    return StatusCode::SUCCESS;
  }

  StatusCode UserPhotonFlux::InitializePythiaInfo(Pythia8::Pythia& pythia) const {

    ATH_MSG_INFO( "InitializePythiaInfo " << name() << " with process " << m_process);
    if (m_process == 1) {
      PDF* photonFlux = new Lepton2gamma2(-11);
      pythia.setPhotonFluxPtr(photonFlux, 0);
      return StatusCode::SUCCESS;
    }
    else if(m_process ==2 || m_process ==3) {    
      ATH_MSG_INFO( "InitializePythiaInfo " << name() << " using nuclear photon flux with Z = " << m_flux_Z << ", bmin = " << m_flux_min_b);
      Nucleus2gamma* photonFlux = new Nucleus2gamma(-11);
      photonFlux->setZ(m_flux_Z);
      photonFlux->setMinB(m_flux_min_b);
      photonFlux->setMinX(m_flux_min_x);
      Nucleus2gamma* photonFlux2 = nullptr;
      if(m_process==3) photonFlux2=photonFlux;
      pythia.setPhotonFluxPtr(photonFlux, photonFlux2);
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_ERROR("Unknown process: "<< m_process);
      return StatusCode::FAILURE;
    }

  }
  
  StatusCode UserPhotonFlux::finalize(){
    ATH_MSG_INFO( "Finalize" << name());
    return StatusCode::SUCCESS;
  }
  

StatusCode UserPhotonFlux::ModifyPythiaEvent(Pythia8::Pythia& /*pythia*/) const {
    return StatusCode::SUCCESS;
  }

  double UserPhotonFlux::CrossSectionScaleFactor() const {
    ATH_MSG_INFO( "CrossSectionScaleFactor" << name());
    return 1.0;
  }



