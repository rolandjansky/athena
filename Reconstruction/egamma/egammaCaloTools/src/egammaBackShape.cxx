/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaBackShape.h"

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
egammaBackShape::egammaBackShape(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cluster(0),
    m_cellContainer(0),
    m_eallsamples(0),
    m_e3(0),
    m_eta(0),
    m_phi(),
    m_deta(0),
    m_dphi(0), 
    m_calo_dd(0), 
    m_sam(CaloSampling::EMB3),
    m_sam2(CaloSampling::EMB2),
    m_subcalo(CaloCell_ID::LAREM), 
    m_barrel(0),
    m_sampling_or_module(0)
{ 
  // declare Interface
  declareInterface<IegammaBackShape>(this);
  
  // initialisation of variables
  InitVariables();
}

// ====================================================================
// DESTRUCTOR:
egammaBackShape::~egammaBackShape()
{ 
}

// ========================================================================
// INITIALIZE METHOD:
StatusCode egammaBackShape::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaBackShape");

  // retrieve all helpers from det store
  m_calo_dd = CaloDetDescrManager::instance();

  // Create egammaEnergyAllSamples Tool
  if(m_egammaEnergyPositionAllSamples.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaEnergyPositionAllSamples);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaEnergyPositionAllSamples << " retrieved"); 

  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaBackShape::finalize()
{
  return StatusCode::SUCCESS;
}

// =====================================================================
// ATHENA METHOD:
StatusCode egammaBackShape::execute(const xAOD::CaloCluster *cluster, 
				    const CaloCellContainer *cell_container) 
{
  //
  // Estimate shower shapes from third compartment
  // based on hottest cell and deta,dphi
  // with eta = m_cluster->eta(sam)
  //      phi = m_cluster->phi(sam)
  //

  ATH_MSG_DEBUG(" egammaBackShape: execute");

  // check if cluster is available
  if(!cluster) { 
    ATH_MSG_DEBUG(" egammaBackShape: Invalid pointer to cluster");
    return StatusCode::SUCCESS;
  }

  // check if cell container is available
  if(!cell_container) { 
    ATH_MSG_DEBUG(" egammaBackShape: Invalid pointer to cell_container");
    return StatusCode::SUCCESS;
  }

  // check if cluster is in barrel or in the end-cap
  if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
    ATH_MSG_DEBUG(" egammaBackShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  
  m_cluster = cluster;
  m_cellContainer = cell_container;

  // initialisation of variables
  InitVariables();

  // retrieve energy in all samplings
  StatusCode sc = m_egammaEnergyPositionAllSamples->execute(m_cluster,3);
  if (sc) {};
  m_eallsamples = m_egammaEnergyPositionAllSamples->e();
  m_e3 = m_egammaEnergyPositionAllSamples->e3();

  // check if cluster is in barrel or end-cap
  bool in_barrel = m_egammaEnergyPositionAllSamples->inBarrel();
  // define accordingly position of CaloSampling
  if (in_barrel) {
    m_sam  = CaloSampling::EMB3; 
    m_sam2 = CaloSampling::EMB2; 
  } else {
    m_sam  = CaloSampling::EME3; 
    m_sam2 = CaloSampling::EME2; 
  }

  // fraction of energy deposited in third sampling
  m_f3 = fabs(m_eallsamples)>0. ? m_e3/m_eallsamples : 0.; 
  
  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  if (!FindPosition()) return StatusCode::SUCCESS;
  
  // estimate the relevant quantities around the hottest cell
  // in the following eta X phi windows
  CaloLayerCalculator calc;
  
  // 3X3
  sc = calc.fill(m_cellContainer,m_eta,m_phi,3.*m_deta,3.*m_dphi,m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e333 = calc.em(); 
  
  if (m_ExecOtherVariables) {
    // 3X5
    sc = calc.fill(m_cellContainer,m_eta,m_phi,3.*m_deta,5.*m_dphi,m_sam);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
    }
    m_e335 = calc.em(); 
    
    // 5X5
    sc = calc.fill(m_cellContainer,m_eta,m_phi,5.*m_deta,5.*m_dphi,m_sam);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
    }
    m_e355 = calc.em(); 
    
    // 3X7
    sc = calc.fill(m_cellContainer,m_eta,m_phi,3.*m_deta,7.*m_dphi,m_sam);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
    }
    m_e337 = calc.em(); 
    
    // 7x7
    sc = calc.fill(m_cellContainer,m_eta,m_phi,7.*m_deta,7.*m_dphi,m_sam);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
    }
    m_e377 = calc.em(); 
  }

  // f3core
  if ( m_eallsamples > 0. && m_e333 >-999. )
    m_f3core = m_e333/m_eallsamples;

  // std::cout << " E333 = " << m_e333 << " " << m_e337 << " " << m_e377 << std::endl;

  return StatusCode::SUCCESS;
}


// ====================================================================
void egammaBackShape::InitVariables()
{
  //
  // initialisation
  //
  double x = 0.;
  m_f3     = x;
  m_f3core = x;
  m_e333   = x;
  m_e335   = x; 
  m_e355   = x; 
  m_e337   = x; 
  m_e377   = x; 
}

// =====================================================================
bool egammaBackShape::FindPosition()
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

  if ((m_eta==0. && m_phi==0.) || fabs(m_eta)>100) return false;

  // granularity in (eta,phi) in the pre sampler
  // CaloCellList needs both enums: subCalo and CaloSample
  m_calo_dd->decode_sample(m_subcalo,m_barrel,m_sampling_or_module, 
			   (CaloCell_ID::CaloSample)m_sam);
  // Get the corresponding grannularities : needs to know where you are
  //                  the easiest is to look for the CaloDetDescrElement
  const CaloDetDescrElement* dde =
    m_calo_dd->get_element(m_subcalo,m_sampling_or_module,m_barrel,m_eta,m_phi);
  // if object does not exist then return
  if (!dde) return false;

  // local granularity
  m_deta = dde->deta();
  m_dphi = dde->dphi();

  // change values of m_eta,m_phi
  m_eta = dde->eta_raw(); 
  m_phi = dde->phi_raw(); 


  return true;
}

