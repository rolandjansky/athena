/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  // set up the sampling windows:
  float f_neta = (float)m_neta;
  float f_nphi = (float)m_nphi;

  m_deta0 = m_detas2*f_neta;
  m_dphi0 = m_dphis2*f_nphi;

  m_deta1 = m_detas2*f_neta;
  m_dphi1 = m_dphis2*f_nphi;

  m_deta2 = m_detas2*f_neta;
  m_dphi2 = m_dphis2*f_nphi;

  m_deta3 = m_detas2*f_neta;
  m_dphi3 = m_dphis2*f_nphi;

  return StatusCode::SUCCESS;
}



void CaloTopoEMlayers::get_seed (const xAOD::CaloCluster* cluster,
                                 const CaloCell* /*max_et_cell*/,
                                 double& eta,
                                 double& phi) const
{
  eta = cluster->eta();
  phi = cluster->phi();
}
