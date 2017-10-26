/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaCheckEnergyDepositTool
PACKAGE:  offline/Reconstruction/egamma/egammaTools

AUTHORS:  F. Derue 
CREATED:  July 3, 2009

PURPOSE:     Check in each sampling that fraction of energy 
   reconstructed is not greater thana a certain threshold

UPDATED :
           Dec. 29, 2009 (FD) protection against bad pointers
********************************************************************/

#include "egammaCheckEnergyDepositTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"


// =================================================================
egammaCheckEnergyDepositTool::egammaCheckEnergyDepositTool(const std::string& type, 
							   const std::string& name, 
							   const IInterface* parent) :
  AthAlgTool(type, name, parent)
{
  declareInterface<IegammaCheckEnergyDepositTool>(this);

}

// ===============================================================
egammaCheckEnergyDepositTool::~egammaCheckEnergyDepositTool() 
{
}

// =============================================================
StatusCode egammaCheckEnergyDepositTool::initialize() 
{

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaCheckEnergyDepositTool::finalize() 
{
  return StatusCode::SUCCESS;
}

// =========================================================================
bool egammaCheckEnergyDepositTool::checkFractioninSampling(const xAOD::Egamma* eg) 
{

  // protection against bad pointers
  if (eg==0) return false;

  // retrieve associated cluster
  const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 

  return checkFractioninSamplingCluster(cluster);
}

bool egammaCheckEnergyDepositTool::checkFractioninSamplingCluster(const xAOD::CaloCluster*   cluster) 
{

  // return true if no check to be done
  if(!m_useThrFmax) return (true);

  // retrieve associated cluster
  if (cluster==0) return false;

  // Retrieve energy in all samplings
  double e0 = 0.;
  double e1 = 0.;
  double e2 = 0.;
  double e3 = 0.;
  
  if ( cluster->inBarrel() && !cluster->inEndcap() ){
    e0 = cluster->eSample(CaloSampling::PreSamplerB);
    e1 = cluster->eSample(CaloSampling::EMB1);
    e2 = cluster->eSample(CaloSampling::EMB2);
    e3 = cluster->eSample(CaloSampling::EMB3);
  }
  else if ( !cluster->inBarrel() && cluster->inEndcap() ){
    e0 = cluster->eSample(CaloSampling::PreSamplerE);
    e1 = cluster->eSample(CaloSampling::EME1);
    e2 = cluster->eSample(CaloSampling::EME2);
    e3 = cluster->eSample(CaloSampling::EME3);
  }
  // if both in barrel and end-cap then have to
  // rely on energy deposition
  // be careful to test 0 precisely either 
  // 0 (no deposit) > noise (which is negative) !!!!
  else if ( cluster->inBarrel() && cluster->inEndcap() ) {
    if ( ( ( cluster->eSample(CaloSampling::EMB2) != 0. && 
	     cluster->eSample(CaloSampling::EME2) != 0. ) && 
	   ( cluster->eSample(CaloSampling::EMB2) >=
	     cluster->eSample(CaloSampling::EME2) ) ) || 
	 cluster->eSample(CaloSampling::EME2) == 0. ) {
      e0 = cluster->eSample(CaloSampling::PreSamplerB);
      e1 = cluster->eSample(CaloSampling::EMB1);
      e2 = cluster->eSample(CaloSampling::EMB2);
      e3 = cluster->eSample(CaloSampling::EMB3);
    }
    else {
      e0 = cluster->eSample(CaloSampling::PreSamplerE);
      e1 = cluster->eSample(CaloSampling::EME1);
      e2 = cluster->eSample(CaloSampling::EME2);
      e3 = cluster->eSample(CaloSampling::EME3);
    }
  }   
  
  // sum of energy in all samplings
  const double eallsamples = e0+e1+e2+e3;

  if (eallsamples!=0.) {
    const double inv_eallsamples = 1. / eallsamples;

    // check fraction of energy reconstructed in presampler
    double f0 = e0 * inv_eallsamples;
    if (f0 > m_thrF0max) return false;

    // check fraction of energy reconstructed in first sampling
    double f1 = e1 * inv_eallsamples;
    if (f1 > m_thrF1max) return false;

    // check fraction of energy reconstructed in second sampling
    double f2 = e2 * inv_eallsamples;
    if (f2 > m_thrF2max) return false;

    // check fraction of energy reconstructed in third sampling
    double f3 = e3 * inv_eallsamples;
    if (f3 > m_thrF3max) return false;
  }

  return true;
}
