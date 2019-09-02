/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTopoEMLayers.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  Computer eta, phi, energy of each layers of a cluster
	  Base class: CaloClusterCorrection (Algorithm)
          

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 
Updated:  Apr 14, 2003    (MW)
          small bug fix for barrel/endcap region

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  January, 2005    (MB)
          Move to Calorimeter/CaloClusterCorrection.

Updated:  March, 2005    (MB)
          Temporary : this should not be needed when TopoCluster fills
          information as sliding window (in particular eta0, phi0 of 
          clusters)

*********************************************************************/

// include header files
#include "CaloTopoEMlayers.h"
#include "CaloEvent/CaloCluster.h"


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloTopoEMlayers::CaloTopoEMlayers(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
  : CaloFillRectangularCluster(type, name, parent)
{ 
  m_fill_cluster = false;
}


// Algorithm initialization. 
StatusCode CaloTopoEMlayers::initialize()
{
  CHECK(CaloFillRectangularCluster::initialize() );

  return StatusCode::SUCCESS;
}


void CaloTopoEMlayers::get_seed (CaloClusterCorr::SamplingHelper& /*helper*/,
                                 const xAOD::CaloCluster* cluster,
                                 double& eta,
                                 double& phi) const
{
  eta = cluster->eta();
  phi = cluster->phi();
}


/** 
 * @brief Set up layer-by-layer cluster window sizes.
 * @param neta Cluster eta size.
 * @param nphi Cluster phi size.
 * @param detas2 Middle layer cell eta size.
 * @param detas2 Middle layer cell phi size.
 *
 * Returns per-layer array of deta,dphi pairs.
 */
CaloFillRectangularCluster::WindowArray_t
CaloTopoEMlayers::initWindows (const int neta,
                               const int nphi,
                               const double detas2,
                               const double dphis2) const
{
  CaloFillRectangularCluster::WindowArray_t w;

  float f_neta = (float)neta;
  float f_nphi = (float)nphi;

  w[0].first  = detas2*f_neta;
  w[1].first  = w[0].first;
  w[2].first  = w[0].first;
  w[3].first  = w[0].first;

  w[0].second = dphis2*f_nphi;
  w[1].second = w[0].second;
  w[2].second = w[0].second;
  w[3].second = w[0].second;

  return w;
}
