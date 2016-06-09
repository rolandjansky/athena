/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTopoEMphimod.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  M.Boonekamp & N.Kerschen
CREATED:  March 2005

PURPOSE:  correction for the phi offset due to accordion structure
          base class: CaloClusterCorrection (Algorithm)

Updated:  March 12, 2005   (MB)
          corrections for the TopoCluster 
********************************************************************/

#include "CaloTopoEMphimod.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h> 

using CLHEP::pi;

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloTopoEMphimod::CaloTopoEMphimod(const std::string& type, const std::string& name, const IInterface* parent) 
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant("P1b",                     m_P1b);
  declareConstant("P2b",                     m_P2b);
  declareConstant("P3b",                     m_P3b);
  declareConstant("P4b",                     m_P4b);
  declareConstant("P1e",                     m_P1e);
  declareConstant("P2e",                     m_P2e);
  declareConstant("P3e",                     m_P3e);
  declareConstant("P4e",                     m_P4e);
  declareConstant("BarrelGranularity",       m_BarrelGranularity);
  declareConstant("EndcapGranularity",       m_EndcapGranularity);
  declareConstant("EtaFrontier",             m_EtaFrontier);
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloTopoEMphimod::~CaloTopoEMphimod()
{ }

// Initialization
/*StatusCode CaloTopoEMphimod::initialize()
{
  ATH_MSG_DEBUG( " Phi modulation parameters : " << endreq);
  ATH_MSG_DEBUG( "   P1b =          " << m_P1b << endreq);
  ATH_MSG_DEBUG( "   P2b =          " << m_P2b << endreq);
  ATH_MSG_DEBUG( "   P3b =          " << m_P3b << endreq);
  ATH_MSG_DEBUG( "   P4b =          " << m_P4b << endreq);
  ATH_MSG_DEBUG( "   P1e =          " << m_P1e << endreq);
  ATH_MSG_DEBUG( "   P2e =          " << m_P2e << endreq);
  ATH_MSG_DEBUG( "   P3e =          " << m_P3e << endreq);
  ATH_MSG_DEBUG( "   P4e =          " << m_P4e << endreq);
  ATH_MSG_DEBUG( "   Granularity =  " << m_BarrelGranularity << " / " << m_EndcapGranularity << endreq);
  ATH_MSG_DEBUG( "   EtaFrontier =  " << m_EtaFrontier << endreq);
  return StatusCode::SUCCESS;
}*/

// make correction to one cluster 
void CaloTopoEMphimod::makeTheCorrection(const EventContext& /*ctx*/,
                                         xAOD::CaloCluster* cluster,
					 const CaloDetDescrElement* elt,
					 float /*eta*/,
					 float adj_eta,
					 float phi,
                                         float /*adj_phi*/,
					 CaloSampling::CaloSample /*samp*/) const
{
  float qphimod = 0.;
  float aeta = fabs(adj_eta);
  int iEtaBin;
  // u is the normalized coordinate along phi (within a cell)
  // 0 < u < 1
  float u = (phi - elt->phi()) / elt->dphi() + 0.5;

  ATH_MSG_DEBUG( " ... phi-mod BEGIN" << endreq);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << endreq);

  // Compute the correction
  if (aeta < m_EtaFrontier[0]) 
    { 
      iEtaBin = (int)(aeta / m_BarrelGranularity);
      qphimod = 1 - m_P1b[iEtaBin]*std::cos(8*pi*u) - m_P2b[iEtaBin]*std::cos(16*pi*u) 
	          - m_P3b[iEtaBin]*std::sin(8*pi*u) - m_P4b[iEtaBin]*std::sin(16*pi*u);
    }
  else if (aeta > m_EtaFrontier[1] && aeta < m_EtaFrontier[2])
    {
      iEtaBin = (int)((aeta - m_EtaFrontier[1]) / m_EndcapGranularity);
      qphimod = 1 - m_P1e[iEtaBin]*std::cos(6*pi*u) - m_P2e[iEtaBin]*std::cos(12*pi*u) 
	          - m_P3e[iEtaBin]*std::sin(6*pi*u) - m_P4e[iEtaBin]*std::sin(12*pi*u);
    }
  else // wrong eta value
    {
      return;
    }
  
  // Print out the function for debugging
  ATH_MSG_DEBUG( " ... Phi mod " << qphimod << " " << u << " " << phi << " " << elt->phi() 
      << " " << adj_eta << " " << aeta << " " << iEtaBin << endreq);

  // Apply the correction
  setenergy (cluster, cluster->e() * qphimod);

  ATH_MSG_DEBUG( " ... phi-mod END" << endreq);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << endreq);

  // Done
  return ; 
}
