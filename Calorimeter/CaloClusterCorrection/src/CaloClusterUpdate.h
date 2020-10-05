/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOCLUSTER_UPDATE_H
#define CALOCLUSTERCORRECTION_CALOCLUSTER_UPDATE_H
/********************************************************************

NAME:     CaloClusterUpdate.h
PACKAGE:  offline/LArCalorimeter/CaloclusterRec

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Nov, 2000

PURPOSE:  Recalculate the total energy, eta,phi of a cluster.
	  This should be called after all corrections to individual 
	  samplings are done. 

	  energy = Sum of energy in all sampling
	  eta = average of eta1 and eta2, weighted by energy and
                relative resolution.  
		This needs to be tuned. 
	  phi    = phi of second sampling. 

Base class: CaloClusterCorrection (tool)

Atrecon Orig: emreco/qetamod.F

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  March, 2005    (Maarten Boonekamp)
	  moved to CaloClusterCorrection


********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrection.h"

class CaloClusterUpdate : public CaloClusterCorrection
{

 public:
  // constructor
  using CaloClusterCorrection::CaloClusterCorrection;

  // virtual method in CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:

  // If true, the total cluster energy is set to the sum of all layer energies.
  Constant<bool> m_update_energy
  { this, "update_energy", "Should cluster total energy be updated?" };
};


#endif
