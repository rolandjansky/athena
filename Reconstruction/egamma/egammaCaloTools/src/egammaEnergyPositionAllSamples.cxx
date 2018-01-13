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
  : AthAlgTool(type, name, parent)
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

double egammaEnergyPositionAllSamples::e(const xAOD::CaloCluster& cluster)  const { return 
  return (e0(cluster) +e1(cluster) +e2(cluster) +e3(cluster));
}

double egammaEnergyPositionAllSamples::e0(const xAOD::CaloCluster& cluster) const { 
     return (cluster->eSample(CaloSampling::PreSamplerB)+
             cluster->eSample(CaloSampling::PreSamplerE));
}

double egammaEnergyPositionAllSamples::e1(const xAOD::CaloCluster& cluster) const { 
    
     return (cluster->eSample(CaloSampling::EMB1)+
             cluster->eSample(CaloSampling::EME1));
}

double egammaEnergyPositionAllSamples::e2(const xAOD::CaloCluster& cluster) const { 
         return (cluster->eSample(CaloSampling::EMB2)+
             cluster->eSample(CaloSampling::EME2));
}


double egammaEnergyPositionAllSamples::e3(const xAOD::CaloCluster& cluster) const { 

  return (cluster->eSample(CaloSampling::EMB3)+
          cluster->eSample(CaloSampling::EME3));
}


bool   egammaEnergyPositionAllSamples::inBarrel(const xAOD::CaloCluster& cluster,
                                                const int is /*=2*/) const { 
  
    // from cluster position and energy define 
  // if we are in barrel or end-cap
  // In particular it uses energy deposit in 2nd sampling
  // to check close to the crack region
  // is = sampling (0=presampler,1=strips,2=middle,3=back)
  //
  bool in_barrel=true;
  if (!cluster) return false;

  if ( cluster->inBarrel() && !cluster->inEndcap() ){
    in_barrel = true;
  }
  else if ( !cluster->inBarrel() && cluster->inEndcap() ){
    in_barrel = false;
  }
  // if both in barrel and end-cap then have to
  // rely on energy deposition
  // be careful to test 0 precisely either 
  // 0 (no deposit) > noise (which is negative) !!!!
  else if ( cluster->inBarrel() && cluster->inEndcap() ) {
    if (
	// case for PreSampler
	((is==0 && (( cluster->eSample(CaloSampling::PreSamplerB) != 0. && 
		      cluster->eSample(CaloSampling::PreSamplerE) != 0. )&&
		    (cluster->eSample(CaloSampling::PreSamplerB) >=
		     cluster->eSample(CaloSampling::PreSamplerE)))) ||
	 ( cluster->eSample(CaloSampling::PreSamplerE) == 0. ) ) ||
	// case for 1st sampling
	((is==1 && (( cluster->eSample(CaloSampling::EMB1) != 0. && 
                      cluster->eSample(CaloSampling::EME1) != 0. ) &&
                    (cluster->eSample(CaloSampling::EMB1) >=
                     cluster->eSample(CaloSampling::EME1)))) ||
	 ( cluster->eSample(CaloSampling::EME1) == 0. ) ) ||
	// case for middle sampling
	((is==2 && (( cluster->eSample(CaloSampling::EMB2) != 0. && 
                      cluster->eSample(CaloSampling::EME2) != 0. ) &&
                    (cluster->eSample(CaloSampling::EMB2) >=
                     cluster->eSample(CaloSampling::EME2)))) ||
	 ( cluster->eSample(CaloSampling::EME2) == 0. ) ) ||
	// case for back sampling
	((is==3 && (( cluster->eSample(CaloSampling::EMB3) != 0. && 
                      cluster->eSample(CaloSampling::EME3) != 0. ) &&
                    (cluster->eSample(CaloSampling::EMB3) >=
                     cluster->eSample(CaloSampling::EME3)))) ||
	 ( cluster->eSample(CaloSampling::EME3) == 0. ) ) )
      
      {
	in_barrel = true;
      }
    else {
      in_barrel = false;
    }
  }   
  return in_barrel;
}

