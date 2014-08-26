/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EMExtrapolCaloConversion.h"


#include "CaloDetDescr/CaloDepthTool.h"
#include "CLHEP/Units/SystemOfUnits.h"

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include  "CLHEP/Vector/ThreeVector.h"
#include  "CLHEP/Vector/TwoVector.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODEgamma/Egamma.h"

#include "CaloGeoHelpers/CaloSampling.h"



#include <iostream>
#include <iomanip>

#include <cmath>

const double UNKNOWN_BIS = -99999;

EMExtrapolCaloConversion::EMExtrapolCaloConversion(const std::string& type,
			   const std::string& name,
			   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IEMExtrapolCaloConversion>(this);

  declareProperty("CalorimeterDepth",m_CaloDepth="CaloDepthTool");
}

// ==============================================================
EMExtrapolCaloConversion::~EMExtrapolCaloConversion() 
{
}

//================================================================       
StatusCode EMExtrapolCaloConversion::initialize()
{
  ATH_MSG_DEBUG(" Initializing EMExtrapolCaloConversion");

  m_egammaqdepth = ToolHandle<CaloDepthTool>(m_CaloDepth);
  if(m_egammaqdepth.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_CaloDepth);
    return StatusCode::RECOVERABLE; // why success?
  } 
  else {
    ATH_MSG_DEBUG("Retrieved CaloDepthTool " << m_egammaqdepth);   
  }
 
  return StatusCode::SUCCESS;
}
//====================================================================
StatusCode EMExtrapolCaloConversion::finalize()
{
  return StatusCode::SUCCESS;
}

// ==================================================================
StatusCode EMExtrapolCaloConversion::execute(const xAOD::Egamma* eg,const xAOD::Vertex* vertex, double error_etaS1)
{
  m_eta=99999.;
  m_error_eta=99999.;

  // protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;
  // retrieve associated cluster
  const xAOD::CaloCluster* cluster  = eg->caloCluster(); 
  if (cluster==0) return StatusCode::SUCCESS;
  if (vertex==0) return StatusCode::SUCCESS;

  double R_photon_front=UNKNOWN_BIS;
  double Z_photon_front=UNKNOWN_BIS;

  const double eta_cluster_2nd_sampling = cluster->inBarrel() ? cluster->etaSample( CaloSampling::EMB2): cluster->etaSample( CaloSampling::EME2); 
  const double eta_cluster_1st_sampling = cluster->inBarrel() ? cluster->etaSample( CaloSampling::EMB1): cluster->etaSample( CaloSampling::EME1); 
  const double phi_cluster_2nd_sampling = cluster->inBarrel() ? cluster->phiSample( CaloSampling::EMB2): cluster->phiSample( CaloSampling::EME2); 
  
  if (fabs(eta_cluster_2nd_sampling) < 1.5) {      // barrel
    R_photon_front=m_egammaqdepth->radius(CaloCell_ID::EMB1,eta_cluster_1st_sampling,phi_cluster_2nd_sampling);
    Z_photon_front=R_photon_front*sinh(eta_cluster_1st_sampling);
  }
  else if (fabs(eta_cluster_1st_sampling) < 5) {  // endcap
    Z_photon_front=m_egammaqdepth->radius(CaloCell_ID::EME1,eta_cluster_1st_sampling,phi_cluster_2nd_sampling);
    R_photon_front=Z_photon_front/sinh(eta_cluster_1st_sampling);
  }
  else
    return StatusCode::SUCCESS;

  CLHEP::Hep3Vector v_cluster;
  v_cluster.setRhoPhiZ(R_photon_front, phi_cluster_2nd_sampling, Z_photon_front);

  const CLHEP::Hep3Vector v_conversion(vertex->x(), 
                                       vertex->y(), 
                                       vertex->z());
 
  const CLHEP::Hep3Vector v_diff = v_cluster - v_conversion;
  m_eta=v_diff.eta();
   
  // Error propagation was done in mathematica starting with:
  // etan = ArcSinh[(z - za) / |p - pa|]
  // where the "a" values are the values of the conversion vertex
  // while the plain values are the values in the calo. p is the 2D
  // vector in the x-y plane. For the barrel, I sub in z = rho Sinh[eta]
  // and do the partial derivatives assuming that etan is only a 
  // function of eta (the detector eta based on 0,0,0).
  // For the endcap, I chose the x-y coordinates such that p lies along
  // the x axis, and substituted in p = z / Sinh [eta].
  // The resulting formulas are exactly the same as what Marc got, even
  // if their form is slightly different, if you assume that there is no
  // vertex displacement. (In the barrel case, it is exactly the same.)
  // The errors, however, still need work, because the pulls are not good.

  if ( (cluster->inBarrel() && !cluster->inEndcap()) || 
       (cluster->inBarrel() && cluster->inEndcap() && 
  	cluster->eSample(CaloSampling::EMB1) > cluster->eSample(CaloSampling::EME1) ) ) { 

    //Barrel case
    const double mag = v_diff.mag();
    if (mag) m_error_eta = error_etaS1 * v_cluster.mag() / mag;

  }
  else if( ( !cluster->inBarrel() && cluster->inEndcap() ) || 
  	   (  cluster->inBarrel() && cluster->inEndcap() && 
  	      cluster->eSample(CaloSampling::EME1) > cluster->eSample(CaloSampling::EMB1) ) ) { 

    //EndCap case

    const CLHEP::Hep2Vector rho_cluster(v_cluster.x(), v_cluster.y());
    const CLHEP::Hep2Vector rho_conversion(v_conversion.x(), v_conversion.y());

    const double denom = v_diff.mag() * v_diff.perp2() * v_cluster.z();

    if (denom) {
      m_error_eta = error_etaS1 * 
	v_cluster.mag() * v_diff.z() * (R_photon_front * R_photon_front - rho_cluster * rho_conversion) / denom;
    }
  }
  
  ATH_MSG_DEBUG("Result is Eta=" << m_eta);
  ATH_MSG_DEBUG("Result is ErrorEta=" << m_error_eta);

  return StatusCode::SUCCESS; 
}
