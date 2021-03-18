/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVASvc.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaxAODHelpers.h"


egammaMVASvc::egammaMVASvc(const std::string& name, ISvcLocator* svc) :
  base_class( name, svc )
{
}

StatusCode egammaMVASvc::initialize()
{
  ATH_MSG_DEBUG("In initialize of " << name() << "..." );

  if (m_mvaElectron.isEnabled()) {
    ATH_MSG_DEBUG("Retrieving mvaElectron");
    ATH_CHECK(m_mvaElectron.retrieve());
  } else {
    ATH_MSG_DEBUG("Disabling mvaElectron");
    m_mvaElectron.disable();
  }

  if (m_mvaUnconvertedPhoton.isEnabled()) {
    ATH_MSG_DEBUG("Retrieving mvaUnconvertedPhoton");
    ATH_CHECK(m_mvaUnconvertedPhoton.retrieve());
  } else {
    ATH_MSG_DEBUG("Disabling mvaUnconvertedPhoton");
    m_mvaUnconvertedPhoton.disable();
  }

  if (m_mvaConvertedPhoton.isEnabled()) {
    ATH_MSG_DEBUG("Retrieving mvaConvertedPhoton");
    ATH_CHECK(m_mvaConvertedPhoton.retrieve());
  } else {
    ATH_MSG_DEBUG("Disabling mvaConvertedPhoton");
    m_mvaConvertedPhoton.disable();
  }

  return StatusCode::SUCCESS;
}


StatusCode egammaMVASvc::finalize(){
  ATH_MSG_DEBUG( "in finalize" );
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::execute(xAOD::CaloCluster& cluster,
                                 const xAOD::Egamma& eg) const
{

  ATH_MSG_DEBUG("calling execute with cluster and eg");

  float mvaE = 0.0;

  if (xAOD::EgammaHelpers::isElectron(&eg)) {
    if (m_mvaElectron.isEnabled()) {
      mvaE = m_mvaElectron->getEnergy(cluster, &eg);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an electron, but disabled");
      return StatusCode::FAILURE;
    }
  } else if (xAOD::EgammaHelpers::isConvertedPhoton(&eg) &&
             xAOD::EgammaHelpers::conversionRadius(static_cast<const xAOD::Photon*>(&eg)) < m_maxConvR) {
    if (m_mvaConvertedPhoton.isEnabled()) {
      mvaE = m_mvaConvertedPhoton->getEnergy(cluster, &eg);
    } else {
      ATH_MSG_FATAL("Trying to calibrate a converted photon, but disabled");
      return StatusCode::FAILURE;
    }
  } else if (xAOD::EgammaHelpers::isPhoton(&eg)) {
    if (m_mvaUnconvertedPhoton.isEnabled()) {
      mvaE = m_mvaUnconvertedPhoton->getEnergy(cluster, &eg);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an unconverted photon, but disabled");
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_FATAL("Egamma object is of unsupported type");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > eg.m()) {
    cluster.setCalE(mvaE);
  }
  else {
    ATH_MSG_DEBUG("MVA energy (" << mvaE << ") < particle mass ("
                  << eg.m() << "), setting e = cluster energy (" << cluster.e() << ")");
    cluster.setCalE(cluster.e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::execute(xAOD::CaloCluster& cluster,
                                 const xAOD::EgammaParameters::EgammaType egType) const
{

  ATH_MSG_DEBUG("calling execute with cluster and egType (" << egType <<")");

  float mvaE = 0.0;
  switch (egType) {
  case xAOD::EgammaParameters::electron:
    if (m_mvaElectron.isEnabled()) {
      mvaE = m_mvaElectron->getEnergy(cluster,nullptr);
    } else {
      ATH_MSG_FATAL("Trying to calibrate an electron, but disabled");
      return StatusCode::FAILURE;
    }
    break;
  case xAOD::EgammaParameters::convertedPhoton:
  case xAOD::EgammaParameters::unconvertedPhoton:
    // treat converted photons like unconverted photons since don't have access to vertex
    if (m_mvaUnconvertedPhoton.isEnabled()) {
      mvaE = m_mvaUnconvertedPhoton->getEnergy(cluster,nullptr);
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
    cluster.setCalE(mvaE);
  }
  else {
    ATH_MSG_DEBUG("MVA energy (" << mvaE << ") < 0, setting e = cluster energy ("
		  << cluster.e() << ")");
    cluster.setCalE(cluster.e());
  }
  return StatusCode::SUCCESS;
}
