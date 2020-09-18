/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetECPSFractionTool.cxx

#include "AsgDataHandles/WriteDecorHandle.h"
#include "JetMomentTools/JetECPSFractionTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"
#include "CaloGeoHelpers/CaloSampling.h"

using xAOD::IParticle;
using xAOD::CaloCluster;
using xAOD::PFO;
using xAOD::JetConstituent;
using xAOD::JetFourMom_t;

#undef  JetECPSFractionTool_DEBUG

//**********************************************************************

JetECPSFractionTool::JetECPSFractionTool(std::string myname)
: asg::AsgTool(myname) {
}

//**********************************************************************

StatusCode JetECPSFractionTool::initialize(){

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetECPSFractionTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  // Prepend jet collection name
  m_fracKey = m_jetContainerName + "." + m_fracKey.key();

  ATH_CHECK(m_fracKey.initialize());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetECPSFractionTool::decorate(const xAOD::JetContainer& jets) const {

  SG::WriteDecorHandle<xAOD::JetContainer, float> fracHandle(m_fracKey);

  for(const xAOD::Jet* jet : jets) fracHandle(*jet) = energyFraction(*jet);
  return StatusCode::SUCCESS;
}

//**********************************************************************

double JetECPSFractionTool::energyFraction(const xAOD::Jet& jet) const {
  // Loop over jet constituents.
  double jetECPSEnergy = 0.0;
  double jetConstitEnergy = 0.0;
  for ( const JetConstituent* pcon : jet.getConstituents() ) {
    if ( pcon == nullptr ) { 
      ATH_MSG_WARNING("Constituent is missing."); 
      continue;
    }
    const IParticle* ppar = pcon->rawConstituent();
    double conEnergy = pcon->e();
    // If constituent is pflow, then look to see if it is cluster-based.
    const PFO* ppfl = dynamic_cast<const PFO*>(ppar);
    const CaloCluster* pclu = nullptr;
    if ( ppfl != nullptr ) {
      if ( !ppfl->isCharged() ) {
        pclu = ppfl->cluster(0);  // Assume PFO has exactly one cluster.
        if ( pclu != nullptr ) ATH_MSG_VERBOSE("  Constituent is a PFO pointing to a cluster");
        else ATH_MSG_WARNING("  Constituent is cluster-based PFO but the cluster is not found.");
      } else {
        ATH_MSG_VERBOSE("  Constituent is a PFO pointing to a track");
      }
    // Otherwise check if constituent is cluster.
    } else {
      pclu = dynamic_cast<const CaloCluster*>(ppar);
      if ( pclu != nullptr ) ATH_MSG_VERBOSE("  Constituent is a cluster");
    }
    if ( pclu == nullptr ) {
      ATH_MSG_VERBOSE("  Skipping non-cluster constituent.");
      continue;
    }
    ATH_MSG_VERBOSE("  Processing constituent cluster.");
    // Extract the energy in the ECPS layer.
    double ecpsEnergy = pclu->eSample(CaloSampling::PreSamplerE);
    // Extract the energy assigned to the cluster.
    // Calculate the cluster energy sum from its layer energies.
    // This will not be the same as the cluster energy for LC clusters.
    double layerEnergy = 0.0;
    for ( size_t isam=CaloSampling::PreSamplerB; isam<CaloSampling::Unknown; ++isam ) {
      layerEnergy += pclu->eSample((xAOD::CaloCluster::CaloSample) isam);
    }
    ATH_MSG_DEBUG("Constit ecps/all/cluster = " << ecpsEnergy << "/" << layerEnergy << "/" << conEnergy);
    // If the ECPS energy fraction is above threshold, add this cluster's
    // energy to the ECPS energy sum.
    if ( ecpsEnergy > m_fraclimit*layerEnergy ) jetECPSEnergy += conEnergy;
    jetConstitEnergy += conEnergy;
  }
  if ( jetConstitEnergy <= 0.0 ) {
    return 0.0;
  }
#ifdef JetECPSFractionTool_DEBUG
  double jetEnergy = jet.jetP4("JetConstitScaleMomentum").e();
  ATH_MSG_INFO("Jet ecps/all/jetE = " << jetECPSEnergy << "/" << jetConstitEnergy << "/" << jetEnergy);
#endif
  double frac = jetECPSEnergy/jetConstitEnergy;
  ATH_MSG_DEBUG("Jet ecps/all = " << jetECPSEnergy << "/" << jetConstitEnergy << " = " << frac);
  return frac;
}

//**********************************************************************
