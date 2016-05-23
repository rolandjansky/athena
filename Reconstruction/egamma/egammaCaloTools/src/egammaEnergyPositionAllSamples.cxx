/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaEnergyPositionAllSamples.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "SGTools/DataProxy.h" 

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>

// Constructor 
egammaEnergyPositionAllSamples::egammaEnergyPositionAllSamples(const std::string& type,
							       const std::string& name,
							       const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cluster(0),
    m_eallsamples(0),
    m_e0(0),
    m_e1(0),
    m_e2(0),
    m_e3(0),
    m_inbarrel(0)
{ 
  // declare Interface
  declareInterface<IegammaEnergyPositionAllSamples>(this);
}

// ====================================================================
// Destructor:
egammaEnergyPositionAllSamples::~egammaEnergyPositionAllSamples()
{ 
}

// ========================================================================
// INITIALIZE METHOD:
StatusCode egammaEnergyPositionAllSamples::initialize(){
  ATH_MSG_DEBUG(" Initializing egammaEnergyPositionAllSamples");
  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaEnergyPositionAllSamples::finalize(){
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode egammaEnergyPositionAllSamples::execute(const xAOD::CaloCluster *cluster) {
  //
  // default execute method
  //
  // protection against bad clusters
  if (!cluster) return StatusCode::SUCCESS;
  return execute(cluster,2);
}

// =====================================================================
StatusCode egammaEnergyPositionAllSamples::execute(const xAOD::CaloCluster *cluster, 
						   const int is) {
  //
  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  // Once this location is found it returns uncorrected energy in each sampling
  // as well as sum of these energies
  //
  ATH_MSG_DEBUG(" egammaEnergyPositionAllSamples: execute");
  // check if cluster is available
  if(!cluster) { 
    ATH_MSG_DEBUG(" egammaEnergyPositionAllSamples: Invalid pointer to cluster");
    return StatusCode::SUCCESS;
  }

  // check if cluster is in barrel or in the end-cap
  if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
    ATH_MSG_DEBUG(" egammaEnergyPositionAllSamples: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  m_cluster = cluster;
  // check energy in each sampling
  CHECK(energy());
  // check if cluster is in barrel or end-cap in the sampling
  m_inbarrel = isClusterinBarrel(is);
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode egammaEnergyPositionAllSamples::energy() 
{
  //
  // fill energy in each sampling
  //
  if (!m_cluster)   return StatusCode::SUCCESS;
  m_e0 = m_cluster->eSample(CaloSampling::PreSamplerB) + 
    m_cluster->eSample(CaloSampling::PreSamplerE);
  m_e1 = m_cluster->eSample(CaloSampling::EMB1) + 
    m_cluster->eSample(CaloSampling::EME1);
  m_e2 = m_cluster->eSample(CaloSampling::EMB2) + 
    m_cluster->eSample(CaloSampling::EME2);
  m_e3 = m_cluster->eSample(CaloSampling::EMB3) + 
    m_cluster->eSample(CaloSampling::EME3);
  // sum of energy in all samplings
  m_eallsamples = m_e0+m_e1+m_e2+m_e3;
  //std::cout << " eall = " << m_eallsamples << std::endl;
  return StatusCode::SUCCESS;
}

// =====================================================================
bool egammaEnergyPositionAllSamples::isClusterinBarrel(const int is /*=2*/){
  //
  // from cluster position and energy define 
  // if we are in barrel or end-cap
  // In particular it uses energy deposit in 2nd sampling
  // to check close to the crack region
  // is = sampling (0=presampler,1=strips,2=middle,3=back)
  //
  bool in_barrel=true;
  if (!m_cluster) return false;

  if ( m_cluster->inBarrel() && !m_cluster->inEndcap() ){
    in_barrel = true;
  }
  else if ( !m_cluster->inBarrel() && m_cluster->inEndcap() ){
    in_barrel = false;
  }
  // if both in barrel and end-cap then have to
  // rely on energy deposition
  // be careful to test 0 precisely either 
  // 0 (no deposit) > noise (which is negative) !!!!
  else if ( m_cluster->inBarrel() && m_cluster->inEndcap() ) {
    if (
	// case for PreSampler
	((is==0 && (( m_cluster->eSample(CaloSampling::PreSamplerB) != 0. && 
		      m_cluster->eSample(CaloSampling::PreSamplerE) != 0. )&&
		    (m_cluster->eSample(CaloSampling::PreSamplerB) >=
		     m_cluster->eSample(CaloSampling::PreSamplerE)))) ||
	 ( m_cluster->eSample(CaloSampling::PreSamplerE) == 0. ) ) ||
	// case for 1st sampling
	((is==1 && (( m_cluster->eSample(CaloSampling::EMB1) != 0. && 
                      m_cluster->eSample(CaloSampling::EME1) != 0. ) &&
                    (m_cluster->eSample(CaloSampling::EMB1) >=
                     m_cluster->eSample(CaloSampling::EME1)))) ||
	 ( m_cluster->eSample(CaloSampling::EME1) == 0. ) ) ||
	// case for middle sampling
	((is==2 && (( m_cluster->eSample(CaloSampling::EMB2) != 0. && 
                      m_cluster->eSample(CaloSampling::EME2) != 0. ) &&
                    (m_cluster->eSample(CaloSampling::EMB2) >=
                     m_cluster->eSample(CaloSampling::EME2)))) ||
	 ( m_cluster->eSample(CaloSampling::EME2) == 0. ) ) ||
	// case for back sampling
	((is==3 && (( m_cluster->eSample(CaloSampling::EMB3) != 0. && 
                      m_cluster->eSample(CaloSampling::EME3) != 0. ) &&
                    (m_cluster->eSample(CaloSampling::EMB3) >=
                     m_cluster->eSample(CaloSampling::EME3)))) ||
	 ( m_cluster->eSample(CaloSampling::EME3) == 0. ) ) )
      
      {
	in_barrel = true;
      }
    else {
      in_barrel = false;
    }
  }   
  return in_barrel;
}

