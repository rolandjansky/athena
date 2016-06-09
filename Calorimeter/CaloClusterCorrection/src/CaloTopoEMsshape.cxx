/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTopoEsshape.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  M.Boonekamp & N.Kerschen
CREATED:  March 2005

PURPOSE:  correction for the phi offset due to accordion structure
          base class: CaloClusterCorrection (Algorithm)

Updated:  March 12, 2005   (MB)
          corrections for the TopoCluster 
********************************************************************/

#include "CaloTopoEMsshape.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "GaudiKernel/MsgStream.h"
#include <math.h> 

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloTopoEMsshape::CaloTopoEMsshape(const std::string& type, const std::string& name, const IInterface* parent) 
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant("P0",                      m_P0);
  declareConstant("P1",                      m_P1);
  declareConstant("P2",                      m_P2);
  declareConstant("P3",                      m_P3);
  declareConstant("P4",                      m_P4);
  declareConstant("Granularity",             m_Granularity);
  declareConstant("EtaFrontier",             m_EtaFrontier);
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloTopoEMsshape::~CaloTopoEMsshape()
{ }

// Initialization
/*StatusCode CaloTopoEMsshape::initialize()
{
  ATH_MSG_DEBUG( " S-shape parameters : " << endreq);
  ATH_MSG_DEBUG( "   P0 =             " << m_P0 << endreq);
  ATH_MSG_DEBUG( "   P1 =             " << m_P1 << endreq);
  ATH_MSG_DEBUG( "   P2 =             " << m_P2 << endreq);
  ATH_MSG_DEBUG( "   P3 =             " << m_P3 << endreq);
  ATH_MSG_DEBUG( "   P4 =             " << m_P4 << endreq);
  ATH_MSG_DEBUG( "   Granularity =    " << m_Granularity << endreq);
  ATH_MSG_DEBUG( "   Eta frontiers =  " << m_EtaFrontier << endreq);
  return StatusCode::SUCCESS;
}*/

// make correction to one cluster 
void CaloTopoEMsshape::makeTheCorrection(const EventContext& /*ctx*/,
                                         xAOD::CaloCluster* cluster,
					 const CaloDetDescrElement* elt,
					 float eta,
					 float adj_eta,
					 float /*phi*/,
                                         float /*adj_phi*/,
					 CaloSampling::CaloSample /*samp*/) const
{
  float qsshape = 0.;
  float aeta = fabs(adj_eta);
  int iEtaBin = (int)(aeta/m_Granularity);
  // u is the normalized coordinate along eta (within a cell)
  // -0.5 < u < 0.5
  float u = (eta - elt->eta()) / elt->deta();
  if (elt->eta_raw() < 0) u = -u;
  if (u > 0.5) u = 0.5;
  else if (u < -0.5) u = -0.5;
  // compute CaloSampling
  CaloSampling::CaloSample samp = (CaloSampling::CaloSample)elt->getSampling();

  ATH_MSG_DEBUG( " ... s-shapes BEGIN ; u = " << u << " " << eta << " " << adj_eta << " " << elt->eta() << " " << elt->deta() << endreq);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endreq);

  // Compute the correction
  if (aeta < m_EtaFrontier[0] || (aeta > m_EtaFrontier[1] && aeta < m_EtaFrontier[2])) 
    { 
      if (samp == CaloSampling::EMB2 || samp == CaloSampling::EME2) 
	{
	  qsshape = m_P0[iEtaBin]*0.01*std::atan(m_P1[iEtaBin]*u)
	    + m_P2[iEtaBin]*0.01*u + m_P3[iEtaBin]*1e-3*fabs(u) + m_P4[iEtaBin]*1e-3;
	}
      else // wrong samp value
	{
	  return;
	}
    }
  else // wrong eta value
    {
      return;
    }
  
  // Print out the function for debugging
  ATH_MSG_DEBUG( " ... S shape " << qsshape << " " << u << " " << eta << " " << elt->eta() 
      << " " << adj_eta << " " << aeta << " " << iEtaBin << " " << samp << endreq);

  // Apply the correction
  // ... there was a sign mistake here
  /// ... if (eta < 0) qsshape = -qsshape;
  if (eta > 0) qsshape = -qsshape;
  cluster->setEta(samp, eta + qsshape);

  ATH_MSG_DEBUG( " ... s-shapes END" << endreq);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endreq);

  // Done
  return ; 
}
