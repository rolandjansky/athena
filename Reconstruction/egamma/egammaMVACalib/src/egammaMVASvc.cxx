/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>

#include "egammaMVASvc.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "PathResolver/PathResolver.h"

egammaMVASvc::egammaMVASvc(const std::string& name, ISvcLocator* svc) :
  base_class( name, svc )
{
}

StatusCode egammaMVASvc::initialize() 
{
  ATH_MSG_DEBUG("In initialize of " << name() << "..." );
  
  if (m_mvaElectron.isEnabled()) {
    ATH_CHECK(m_mvaElectron.retrieve());
  } else {
    m_mvaElectron.disable();
  }

  if (m_mvaUnconvertedPhoton.isEnabled()) {
    ATH_CHECK(m_mvaUnconvertedPhoton.retrieve());
  } else {
    m_mvaUnconvertedPhoton.disable();
  }

  if (m_mvaConvertedPhoton.isEnabled()) {
    ATH_CHECK(m_mvaConvertedPhoton.retrieve());
  } else {
    m_mvaConvertedPhoton.disable();
  }

  return StatusCode::SUCCESS;
}


StatusCode egammaMVASvc::finalize(){
  ATH_MSG_DEBUG( "in finalize" );
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::execute(xAOD::CaloCluster* cluster,
				 const xAOD::Egamma* eg) const
{
  if (!eg || !cluster) {
    ATH_MSG_FATAL("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }

  float mvaE = 0.0;

  if (xAOD::EgammaHelpers::isElectron(eg)) {
    if (m_mvaElectron.isEnabled()) {
      mvaE = m_mvaElectron->getEnergy(eg, cluster);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an electron, but disabled");
      return StatusCode::FAILURE;
    }
  } else if (xAOD::EgammaHelpers::isConvertedPhoton(eg)) {
    if (m_mvaConvertedPhoton.isEnabled()) {
      mvaE = m_mvaConvertedPhoton->getEnergy(eg, cluster);
    } else {
      ATH_MSG_FATAL("Trying to calibrate a converted photon, but disabled");
      return StatusCode::FAILURE;
    }
  } else if (xAOD::EgammaHelpers::isPhoton(eg)) {
    if (m_mvaUnconvertedPhoton.isEnabled()) {
      mvaE = m_mvaUnconvertedPhoton->getEnergy(eg, cluster);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an unconverted photon, but disabled");
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_FATAL("Egamma object is of unsupported type");
    return StatusCode::FAILURE;
  }
      
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > eg->m()) {
    cluster->setCalE(mvaE);
  }
  else {
    ATH_MSG_DEBUG("MVA energy (" << mvaE << ") < particle mass ("
                  << eg->m() << "), setting e = cluster energy (" << cluster->e() << ")");
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::execute(xAOD::CaloCluster* cluster,
				 const xAOD::EgammaParameters::EgammaType egType) const
{
  if (!cluster) {
    ATH_MSG_ERROR("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }
  float mvaE = 0.0;
  switch (egType) {
  case xAOD::EgammaParameters::electron:
    if (m_mvaElectron.isEnabled()) {
      mvaE = m_mvaElectron->getEnergy(nullptr, cluster);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an electron, but disabled");
      return StatusCode::FAILURE;
    }
    break;
  case xAOD::EgammaParameters::convertedPhoton:
    if (m_mvaConvertedPhoton.isEnabled()) {
      mvaE = m_mvaConvertedPhoton->getEnergy(nullptr, cluster);
    } else {
      ATH_MSG_FATAL("Trying to calibrate a converted photon, but disabled");
      return StatusCode::FAILURE;
    }
    break;
  case xAOD::EgammaParameters::unconvertedPhoton:
    if (m_mvaUnconvertedPhoton.isEnabled()) {
      mvaE = m_mvaUnconvertedPhoton->getEnergy(nullptr, cluster);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an unconverted photon, but disabled");
      return StatusCode::FAILURE;
    }
    break;
  default:
    ATH_MSG_FATAL("Egamma object is of unsupported type");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > 0) {
    cluster->setCalE(mvaE);
  }
  else {
    ATH_MSG_DEBUG("MVA energy (" << mvaE << ") < 0, setting e = cluster energy (" 
		  << cluster->e() << ")");
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::hltexecute(xAOD::CaloCluster* cluster, const std::string& egType) const
{
  if (egType == "Electron") {
    return execute(cluster, xAOD::EgammaParameters::electron);
  } else if (egType == "Photon"){
    return execute(cluster, xAOD::EgammaParameters::unconvertedPhoton);
  } else {
    ATH_MSG_FATAL("The egType string is not Electron or Photon: " << egType);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
