/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTopoEMphioff.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  M.Boonekamp & N.Kerschen
CREATED:  March 2005

PURPOSE:  correction for the phi offset due to accordion structure
          base class: CaloClusterCorrection (Algorithm)

Updated:  March 12, 2005   (MB)
          corrections for the TopoCluster 
********************************************************************/

#include "CaloTopoEMphioff.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h> 

using CLHEP::GeV;

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloTopoEMphioff::CaloTopoEMphioff(const std::string& type, const std::string& name, const IInterface* parent) 
  : CaloClusterCorrectionCommon(type, name, parent)
{ 
  declareConstant("EdepA",                     m_EdepA);
  declareConstant("EdepB",                     m_EdepB);
  declareConstant("Granularity",               m_Granularity);
  declareConstant("EtaFrontier",               m_EtaFrontier);
  declareConstant("FlipPhi",                   m_FlipPhi);
  declareConstant("EndcapOffset",              m_EndcapOffset);
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloTopoEMphioff::~CaloTopoEMphioff()
{ }

// Initialization
/*StatusCode CaloTopoEMphioff::initialize()
{
  ATH_MSG_DEBUG( " Phi offset parameters : " << endreq);
  ATH_MSG_DEBUG( "   EdepA =          " << m_EdepA << endreq);
  ATH_MSG_DEBUG( "   EdepB =          " << m_EdepB << endreq);
  ATH_MSG_DEBUG( "   Granularity =    " << m_Granularity << endreq);
  ATH_MSG_DEBUG( "   EtaFrontier =    " << m_EtaFrontier << endreq);
  ATH_MSG_DEBUG( "   FlipPhi =        " << m_FlipPhi << endreq);
  ATH_MSG_DEBUG( "   EndcapOffset =   " << m_EndcapOffset << endreq);
  return StatusCode::SUCCESS;
}*/

// make correction to one cluster 
void CaloTopoEMphioff::makeTheCorrection(const EventContext& /*ctx*/,
                                         xAOD::CaloCluster* cluster,
					 const CaloDetDescrElement* elt,
					 float /*eta*/,
					 float adj_eta,
					 float phi,
                                         float /*adj_phi*/,
					 CaloSampling::CaloSample /*samp*/) const
{
  float qphioff = 0.;
  float aeta = fabs(adj_eta);
  float eclus = cluster->e() * (1./GeV);
  int iEtaBin = (int)(aeta/m_Granularity);
  // compute CaloSampling
  CaloSampling::CaloSample samp = (CaloSampling::CaloSample)elt->getSampling();

  if (eclus <= 0)
    return;

  ATH_MSG_DEBUG( " ... phi-off BEGIN" << endreq);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endreq);

  // Compute the correction
  if (aeta < m_EtaFrontier[0]) 
    { 
      qphioff = m_EdepA[iEtaBin]/sqrt(eclus) + m_EdepB[iEtaBin];
    }
  else if (aeta < m_EtaFrontier[2])
    {
      qphioff = m_EdepA[iEtaBin]/eclus + m_EdepB[iEtaBin];
      qphioff = -qphioff;
      if (aeta > m_EtaFrontier[1]) qphioff -= m_EndcapOffset;
    }
  else if (aeta < m_EtaFrontier[3]) 
    {
      qphioff = m_EdepA[iEtaBin]*eclus + m_EdepB[iEtaBin];
      qphioff -= m_EndcapOffset;
    }
  else // wrong eta value
    {
      return;
    }
  
  // Flip the sign, if needed.
  if (m_FlipPhi && elt->eta_raw() < 0)
    qphioff = -qphioff;

  // Print out the function for debugging
  ATH_MSG_DEBUG( " ... Phi off " << qphioff << " " << adj_eta << " " << eclus << " " << iEtaBin << endreq);

  // Apply the correction
  phi = CaloPhiRange::fix(phi + qphioff);
  cluster->setPhi(samp,phi);

  ATH_MSG_DEBUG( " ... phi-off END" << endreq);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endreq);

  // Done
  return ; 
}
