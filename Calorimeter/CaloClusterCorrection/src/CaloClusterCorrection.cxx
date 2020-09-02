/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterCorrection.cxx 
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec  1999

PURPOSE:  see CaloClusterCorrection.h

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 3, 2001    (HM)
          QA. 
Updated:  May 2001, (SR) Move to StoreGate

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Call base class's initialize().

Updated:  January, 2005    (MB)
          Move to Calorimeter/CaloClusterCorrection.

Updated:  February, 2006 (DLelas)
          New method added:
          "makeCorrectionOnClusCollection(CaloClusterContainer *clusColl)"
          Which enables corrections to be applied to a whole
          CaloCluster collection. It is needed by sliding window cell weights
          calculation.

********************************************************************/
// INCLUDE HEADER FILES:
#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"

using xAOD::CaloCluster;

StatusCode CaloClusterCorrection::execute (const EventContext& ctx,
                                           CaloCluster *cluster) const
{
  this->makeCorrection (context(ctx), cluster);

#if 0
  ATH_MSG_DEBUG( " ...... e, et " << cluster->e() << " " << cluster->et() << endmsg);
  ATH_MSG_DEBUG( " ...... eta, etaBE, etaSmp " << cluster->eta() << " " << cluster->etaBE(2) 
      << " " << cluster->etaSample(CaloSampling::EMB1) 
      << " " << cluster->etaSample(CaloSampling::EMB2) 
      << " " << cluster->etaSample(CaloSampling::EMB3) << endmsg);
  ATH_MSG_DEBUG( " ...... phi, phiBE, phiSmp " << cluster->phi() << " " << cluster->phiBE(2) 
      << " " << cluster->phiSample(CaloSampling::EMB1) 
      << " " << cluster->phiSample(CaloSampling::EMB2) 
      << " " << cluster->phiSample(CaloSampling::EMB3) << endmsg);
#endif
  
  return StatusCode::SUCCESS;
}

// set property of cluster in one sampling
void CaloClusterCorrection::setsample(CaloCluster* cluster, 
				      CaloCluster::CaloSample sampling,
				      float em, float etam, float phim, 
				      float emax, float etamax, float phimax, 
				      float etas, float phis) const
{
  cluster->setEnergy(sampling, em);
  cluster->setEta(sampling, etam);
  cluster->setPhi(sampling, phim);

  cluster->setEmax(sampling,emax);
  cluster->setEtamax(sampling,etamax);
  cluster->setPhimax(sampling,phimax);

  cluster->setEtasize(sampling, etas);
  cluster->setPhisize(sampling, phis);
}

// set energy, then 
// rescale the energy in each sampling also.
void CaloClusterCorrection::setenergy(CaloCluster* cluster, float energy ) const
{
  if (cluster->e() == 0) {
    if (energy != 0)
      REPORT_MESSAGE (MSG::WARNING)
        << "Attempt to rescale zero-energy cluster to energy " << energy
        << " ignored.";
    return;
  }

  float correction = energy/cluster->e();
  cluster->setE(energy);

  // also correct individual sampling energies:

  for (int iSample=CaloSampling::PreSamplerB; 
        iSample < CaloSampling::Unknown; 
       ++iSample) 
    {
      CaloSampling::CaloSample sampling=static_cast<CaloSampling::CaloSample>(iSample);
      if (cluster->hasSampling (sampling)) {
        double e = cluster->eSample(sampling);
        cluster->setEnergy(sampling,e*correction) ;
      }
  }
}

