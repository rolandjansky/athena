/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaPreSamplerShape.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "SGTools/DataProxy.h" 

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

// Constructor 
egammaPreSamplerShape::egammaPreSamplerShape(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cluster(0), 
    m_cellContainer(0), 
    m_e011(0),
    m_e033(0),
    m_eta(0),
    m_phi(0),
    m_deta(0),
    m_dphi(0),
    m_calo_dd(0),
    m_sam(CaloSampling::PreSamplerB),
    m_sam2(CaloSampling::EMB2),
    m_subcalo(CaloCell_ID::LAREM),
    m_barrel(0),
    m_sampling_or_module(0)
{ 
  // declare Interface
  declareInterface<IegammaPreSamplerShape>(this);
}

// ====================================================================
// DESTRUCTOR:
egammaPreSamplerShape::~egammaPreSamplerShape()
{ 
}

// ========================================================================
// INITIALIZE METHOD:
StatusCode egammaPreSamplerShape::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaPreSamplerShape");

  // retrieve all helpers from det store
  m_calo_dd = CaloDetDescrManager::instance();

  // Create egammaEnergyAllSamples Tool
  if(m_egammaEnergyPositionAllSamples.retrieve().isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve "<<m_egammaEnergyPositionAllSamples);
    return StatusCode::SUCCESS;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaEnergyPositionAllSamples << " retrieved"); 

  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaPreSamplerShape::finalize()
{
  return StatusCode::SUCCESS;
}

// =====================================================================
// ATHENA METHOD:
StatusCode egammaPreSamplerShape::execute(const xAOD::CaloCluster *cluster, 
					  const CaloCellContainer *cell_container) 
{
  //
  // Estimate shower shapes in pre sampler
  // based on hottest cell and deta,dphi windows 
  // defined from the pre sampler granularity
  // with eta  = m_cluster->etaSample(sam)
  //      phi  = m_cluster->phiSample(sam)
  //      deta = 0.025 
  //      dphi = 10.*0.1*(2.*M_PI/64) 
  //

  ATH_MSG_DEBUG(" egammaPreSamplerShape: execute");

  // check if cluster is available
  if(!cluster) { 
    ATH_MSG_DEBUG(" egammaPreSamplerShape: Invalid pointer to cluster");
    return StatusCode::SUCCESS;
  }

  // check if cell container is available
  if(!cell_container) { 
    ATH_MSG_DEBUG(" egammaPreSamplerShape: Invalid pointer to cell_container");
    return StatusCode::SUCCESS;
  }

  // check if cluster is in barrel or in the end-cap
  if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
    ATH_MSG_DEBUG(" egammaPreSamplerShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  
  m_cluster = cluster;
  m_cellContainer = cell_container;

  // initialisation of variables
  m_e011 = 0.;
  m_e033 = 0.; 

  // retrieve energy in all samplings
  StatusCode sc = m_egammaEnergyPositionAllSamples->execute(m_cluster,0);
  if (sc) {};

  // check if cluster is in barrel or end-cap
  bool in_barrel = m_egammaEnergyPositionAllSamples->inBarrel();
  // define accordingly position of CaloSampling
  if (in_barrel) {
    m_sam  = CaloSampling::PreSamplerB; 
    m_sam2 = CaloSampling::EMB2; 
  } else {
    m_sam  = CaloSampling::PreSamplerE; 
    m_sam2 = CaloSampling::EME2; 
  }

  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  if (!FindPosition()) return StatusCode::SUCCESS;

  // estimate the relevant quantities around the hottest cell
  // in the following eta X phi windows
  CaloLayerCalculator calc;

  // 1X1
  sc = calc.fill (m_cellContainer, m_eta, m_phi, m_deta, m_dphi, m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e011 = calc.em(); 

  // 3X3
  sc = calc.fill (m_cellContainer, m_eta, m_phi, 3.*m_deta, 3.*m_dphi, m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e033 = calc.em(); 
  
  //std::cout << " E011 = " << m_e011 << " " << m_e033 << std::endl;

  return StatusCode::SUCCESS;
}

// =====================================================================
bool egammaPreSamplerShape::FindPosition()
{
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
  m_eta  = m_cluster->etamax(m_sam2);
  m_phi  = m_cluster->phimax(m_sam2);
  // bad patch to avoid crash with dde
  if ( (m_eta==0. && m_phi==0.) || fabs(m_eta)>100 ) return false;

  // granularity in (eta,phi) in the pre sampler
  // CaloCellList needs both enums: subCalo and CaloSample
  m_calo_dd->decode_sample(m_subcalo,m_barrel,m_sampling_or_module, 
			   (CaloCell_ID::CaloSample) m_sam);

  // Get the corresponding grannularities : needs to know where you are
  //                  the easiest is to look for the CaloDetDescrElement

  const CaloDetDescrElement* dde =
    m_calo_dd->get_element(m_subcalo,m_sampling_or_module,m_barrel,m_eta,m_phi);
  // if object does not exist then return
  if (!dde) return false;

  // local granularity
  m_deta = dde->deta();
  m_dphi = dde->dphi();
  
  // change eta,phi values
  m_eta = dde->eta_raw();
  m_phi = dde->phi_raw();
  
  return true;
}

