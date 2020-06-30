/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


// make correction to one cluster 
void CaloTopoEMphioff::makeTheCorrection(const Context& myctx,
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
  int iEtaBin = (int)(aeta/m_Granularity(myctx));
  // compute CaloSampling
  CaloSampling::CaloSample samp = (CaloSampling::CaloSample)elt->getSampling();

  if (eclus <= 0)
    return;

  ATH_MSG_DEBUG( " ... phi-off BEGIN" << endmsg);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endmsg);

  const CxxUtils::Array<1> EtaFrontier = m_EtaFrontier (myctx);

  // Compute the correction
  if (aeta < EtaFrontier[0]) 
    { 
      qphioff = m_EdepA(myctx)[iEtaBin]/sqrt(eclus) + m_EdepB(myctx)[iEtaBin];
    }
  else if (aeta < EtaFrontier[2])
    {
      qphioff = m_EdepA(myctx)[iEtaBin]/eclus + m_EdepB(myctx)[iEtaBin];
      qphioff = -qphioff;
      if (aeta > EtaFrontier[1]) qphioff -= m_EndcapOffset(myctx);
    }
  else if (aeta < EtaFrontier[3]) 
    {
      qphioff = m_EdepA(myctx)[iEtaBin]*eclus + m_EdepB(myctx)[iEtaBin];
      qphioff -= m_EndcapOffset(myctx);
    }
  else // wrong eta value
    {
      return;
    }
  
  // Flip the sign, if needed.
  if (m_FlipPhi(myctx) && elt->eta_raw() < 0)
    qphioff = -qphioff;

  // Print out the function for debugging
  ATH_MSG_DEBUG( " ... Phi off " << qphioff << " " << adj_eta << " " << eclus << " " << iEtaBin << endmsg);

  // Apply the correction
  phi = CaloPhiRange::fix(phi + qphioff);
  cluster->setPhi(samp,phi);

  ATH_MSG_DEBUG( " ... phi-off END" << endmsg);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endmsg);

  // Done
  return ; 
}
