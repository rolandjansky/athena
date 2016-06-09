/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
using xAOD::CaloCluster;
CaloSwClcon_g3::CaloSwClcon_g3(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : CaloClusterCorrection(type, name, parent)
{ 
  declareConstant ("etamin",     m_etamin);
  declareConstant ("etamax",     m_etamax);
  declareConstant ("correction", m_correction);
}


// -------------------------------------------------------------
// Destructor
// -------------------------------------------------------------
CaloSwClcon_g3::~CaloSwClcon_g3()
{ }

// apply correction to one cluster.
void CaloSwClcon_g3::makeCorrection(const EventContext& /*ctx*/,
                                    CaloCluster* cluster) const
{
  float eta = cluster->eta();
  float aeta = fabs(eta);
  if (aeta < m_etamin || aeta >= m_etamax) return;
  if (m_correction.size() == 0) return;
  
  // Determine the correction
  double granularity = (m_etamax - m_etamin) / m_correction.size();
  int i = static_cast<int> ((aeta - m_etamin)/granularity);
  float qclcon = m_correction[i];
 
  // Do the Correction
  setenergy(cluster, qclcon * cluster->e());
}


/// Currently at version==1.
int CaloSwClcon_g3::version() const
{
  return 1;
}
