/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <cmath> 


// make correction to one cluster 
void CaloTopoEMsshape::makeTheCorrection(const Context& myctx,
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
  int iEtaBin = (int)(aeta/m_Granularity(myctx));
  // u is the normalized coordinate along eta (within a cell)
  // -0.5 < u < 0.5
  float u = (eta - elt->eta()) / elt->deta();
  if (elt->eta_raw() < 0) u = -u;
  if (u > 0.5) u = 0.5;
  else if (u < -0.5) u = -0.5;
  // compute CaloSampling
  CaloSampling::CaloSample samp = (CaloSampling::CaloSample)elt->getSampling();

  ATH_MSG_DEBUG( " ... s-shapes BEGIN ; u = " << u << " " << eta << " " << adj_eta << " " << elt->eta() << " " << elt->deta() << endmsg);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endmsg);

  const CxxUtils::Array<1> EtaFrontier = m_EtaFrontier (myctx);

  // Compute the correction
  if (aeta < EtaFrontier[0] || (aeta > EtaFrontier[1] && aeta < EtaFrontier[2])) 
    { 
      if (samp == CaloSampling::EMB2 || samp == CaloSampling::EME2) 
	{
	  qsshape = m_P0(myctx)[iEtaBin]*0.01*std::atan(m_P1(myctx)[iEtaBin]*u)
	    + m_P2(myctx)[iEtaBin]*0.01*u + m_P3(myctx)[iEtaBin]*1e-3*fabs(u)
            + m_P4(myctx)[iEtaBin]*1e-3;
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
      << " " << adj_eta << " " << aeta << " " << iEtaBin << " " << samp << endmsg);

  // Apply the correction
  // ... there was a sign mistake here
  /// ... if (eta < 0) qsshape = -qsshape;
  if (eta > 0) qsshape = -qsshape;
  cluster->setEta(samp, eta + qsshape);

  ATH_MSG_DEBUG( " ... s-shapes END" << endmsg);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << samp << endmsg);

  // Done
  return ; 
}
