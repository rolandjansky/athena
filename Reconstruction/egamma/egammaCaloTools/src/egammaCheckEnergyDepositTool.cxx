/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaCheckEnergyDepositTool
PACKAGE:  offline/Reconstruction/egamma/egammaTools

AUTHORS:  F. Derue 
CREATED:  July 3, 2009

PURPOSE:     Check in each sampling that the fraction of energy 
              reconstructed is not greater than a certain threshold.
              Also make sure there is some minimal energy in the 2nd
              sampling.  

UPDATED :
           Dec. 29, 2009 (FD) protection against bad pointers
********************************************************************/

#include "egammaCheckEnergyDepositTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"


egammaCheckEnergyDepositTool::egammaCheckEnergyDepositTool(const std::string& type, 
							   const std::string& name, 
							   const IInterface* parent) :
  AthAlgTool(type, name, parent)
{
  declareInterface<IegammaCheckEnergyDepositTool>(this);

}

egammaCheckEnergyDepositTool::~egammaCheckEnergyDepositTool() 
= default;

StatusCode egammaCheckEnergyDepositTool::initialize() 
{

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  return StatusCode::SUCCESS;
}

StatusCode egammaCheckEnergyDepositTool::finalize() 
{
  return StatusCode::SUCCESS;
}

bool egammaCheckEnergyDepositTool::checkFractioninSamplingCluster(const xAOD::CaloCluster*   cluster) const  
{

  // retrieve associated cluster
  if (cluster==nullptr) return false;

  // Retrieve energy in all samplings
  double e0 = cluster->energyBE(0);
  double e1 = cluster->energyBE(1);
  double e2 = cluster->energyBE(2);
  double e3 = cluster->energyBE(3);

  if (e2<m_thrE2min){
     return false;
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
