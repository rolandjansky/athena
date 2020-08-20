/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaPreSamplerShape.h"
#include "egammaUtils/egammaEnergyPositionAllSamples.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "SGTools/DataProxy.h" 

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ListItem.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

egammaPreSamplerShape::egammaPreSamplerShape(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : AthAlgTool(type, name, parent){

        // declare Interface
        declareInterface<IegammaPreSamplerShape>(this);
    }

egammaPreSamplerShape::~egammaPreSamplerShape(){ 
}

StatusCode egammaPreSamplerShape::initialize(){
    ATH_MSG_DEBUG(" Initializing egammaPreSamplerShape");

    return StatusCode::SUCCESS;
}

StatusCode egammaPreSamplerShape::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode
egammaPreSamplerShape::execute(const xAOD::CaloCluster& cluster,
                               const CaloDetDescrManager& cmgr,
                               const CaloCellContainer& cell_container,
                               Info& info) const {
  //
  // Estimate shower shapes in pre sampler
  // based on hottest cell and deta,dphi windows
  // defined from the pre sampler granularity
  // with eta  = cluster->etaSample(sam)
  //      phi  = cluster->phiSample(sam)
  //      deta = 0.025
  //      dphi = 10.*0.1*(2.*M_PI/64)
  //

  ATH_MSG_DEBUG(" egammaPreSamplerShape: execute");

  // check if cluster is in barrel or in the end-cap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    ATH_MSG_DEBUG(" egammaPreSamplerShape: Cluster is neither in Barrel nor in "
                  "Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  CaloSampling::CaloSample sam = CaloSampling::PreSamplerB;
  CaloSampling::CaloSample sam2 = CaloSampling::EMB2;
  // check if cluster is in barrel or end-cap
  bool in_barrel = egammaEnergyPositionAllSamples::inBarrel(cluster, 0);
  // define accordingly the position of CaloSampling
  if (in_barrel) {
    sam = CaloSampling::PreSamplerB;
    sam2 = CaloSampling::EMB2;
  } else {
    sam = CaloSampling::PreSamplerE;
    sam2 = CaloSampling::EME2;
  }
  double eta = 0;
  double phi = 0;
  double deta = 0;
  double dphi = 0;
  //
  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  // For this we use the tool egammaEnergyAllSamples
  // which uses the CaloCluster method inBarrel() and inEndcap()
  // but also, in case close to the crack region where both
  // boolean can be true, the energy reconstructed in the sampling
  //

  // Fetch eta and phi of the sampling
  // Note that we use m_sam2 in the 2nd sampling, not in presampler
  eta = cluster.etamax(sam2);
  phi = cluster.phimax(sam2);

  // bad patch to avoid crash with dde
  if ((eta == 0. && phi == 0.) || fabs(eta) > 100) {
    return StatusCode::SUCCESS;
  }
  // granularity in (eta,phi) in the pre sampler
  CaloCell_ID::SUBCALO subcalo = CaloCell_ID::LAREM;
  bool barrel = false;
  int sampling_or_module = 0;
  // CaloCellList needs both enums: subCalo and CaloSample
  CaloDetDescrManager::decode_sample(subcalo, barrel, sampling_or_module,
                     (CaloCell_ID::CaloSample)sam);

  // Get the corresponding grannularities : needs to know where you are
  //                  the easiest is to look for the CaloDetDescrElement
  const CaloDetDescrElement* dde =
    cmgr.get_element(subcalo, sampling_or_module, barrel, eta, phi);
  // if object does not exist then return
  if (!dde) {
    return StatusCode::SUCCESS;
  }
  // local granularity
  deta = dde->deta();
  dphi = dde->dphi();
  // change eta,phi values
  eta = dde->eta_raw();
  phi = dde->phi_raw();
  // estimate the relevant quantities around the hottest cell
  // in the following eta X phi windows
  CaloLayerCalculator calc;
  // 1X1
  StatusCode sc = calc.fill(cmgr,&cell_container, eta, phi, deta, dphi, sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  info.e011 = calc.em();
  // 3X3
  sc = calc.fill(cmgr,&cell_container, eta, phi, 3. * deta, 3. * dphi, sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  info.e033 = calc.em();
  return StatusCode::SUCCESS;
}

