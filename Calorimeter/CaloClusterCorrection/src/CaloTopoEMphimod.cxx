/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "CLHEP/Units/PhysicalConstants.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include <cmath> 

using CLHEP::pi;


// make correction to one cluster 
void CaloTopoEMphimod::makeTheCorrection(const Context& myctx,
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

  ATH_MSG_DEBUG( " ... phi-mod BEGIN" << endmsg);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << endmsg);

  const CxxUtils::Array<1> EtaFrontier = m_EtaFrontier (myctx);

  // Compute the correction
  if (aeta < EtaFrontier[0]) 
    { 
      iEtaBin = (int)(aeta / m_BarrelGranularity(myctx));
      qphimod = 1 - m_P1b(myctx)[iEtaBin]*std::cos(8*pi*u)
                  - m_P2b(myctx)[iEtaBin]*std::cos(16*pi*u) 
                  - m_P3b(myctx)[iEtaBin]*std::sin(8*pi*u)
                  - m_P4b(myctx)[iEtaBin]*std::sin(16*pi*u);
    }
  else if (aeta > EtaFrontier[1] && aeta < EtaFrontier[2])
    {
      iEtaBin = (int)((aeta - EtaFrontier[1]) / m_EndcapGranularity(myctx));
      qphimod = 1 - m_P1e(myctx)[iEtaBin]*std::cos(6*pi*u)
                  - m_P2e(myctx)[iEtaBin]*std::cos(12*pi*u) 
                  - m_P3e(myctx)[iEtaBin]*std::sin(6*pi*u)
                  - m_P4e(myctx)[iEtaBin]*std::sin(12*pi*u);
    }
  else // wrong eta value
    {
      return;
    }
  
  // Print out the function for debugging
  ATH_MSG_DEBUG( " ... Phi mod " << qphimod << " " << u << " " << phi << " " << elt->phi() 
      << " " << adj_eta << " " << aeta << " " << iEtaBin << endmsg);

  // Apply the correction
  setenergy (cluster, cluster->e() * qphimod);

  ATH_MSG_DEBUG( " ... phi-mod END" << endmsg);
  ATH_MSG_DEBUG( " ... e, eta, phi " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() << " " << endmsg);

  // Done
  return ; 
}
