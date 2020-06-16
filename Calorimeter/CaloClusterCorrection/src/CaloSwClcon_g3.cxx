/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwClcon_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the finite containment for a given cluster size
          (Tuned using 50 GeV Et photons)
	  Base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emrecon/qclcon.F
          
Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  Feb 2003 (LC)
          New corrections for DC1 geometry

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/


#include "CaloSwClcon_g3.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cmath>


using xAOD::CaloCluster;

// apply correction to one cluster.
void CaloSwClcon_g3::makeCorrection (const Context& myctx,
                                     CaloCluster* cluster) const
{
  const float etamin = m_etamin (myctx);
  const float etamax = m_etamax (myctx);

  float eta = cluster->eta();
  float aeta = fabs(eta);
  if (aeta < etamin || aeta >= etamax) return;

  const CxxUtils::Array<1> correction = m_correction (myctx);
  if (correction.size() == 0) return;
  
  // Determine the correction
  double granularity = (etamax - etamin) / correction.size();
  int i = static_cast<int> ((aeta - etamin)/granularity);
  float qclcon = correction[i];
 
  // Do the Correction
  setenergy(cluster, qclcon * cluster->e());
}


/// Currently at version==1.
int CaloSwClcon_g3::version() const
{
  return 1;
}
