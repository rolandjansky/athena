/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMiddleShape.h"
#include "egammaInterfaces/Iegammaqweta2c.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

#include "CaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <math.h>

// Constructor 
egammaMiddleShape::egammaMiddleShape(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cluster(0),
    m_cellContainer(0),
    m_eallsamples(0),
    m_width(0),
    m_poscs2(0),
    m_eta(0),
    m_phi(),
    m_deta(0),
    m_dphi(0), 
    m_etacell(0),
    m_calo_dd(0), 
    m_sam(CaloSampling::EMB2),
    m_subcalo(CaloCell_ID::LAREM), 
    m_barrel(0),
    m_sampling_or_module(0)
{ 
  // declare Interface
  declareInterface<IegammaMiddleShape>(this);

  InitVariables();
}

// ====================================================================
// DESTRUCTOR:
egammaMiddleShape::~egammaMiddleShape()
{ 
}

// ========================================================================
// INITIALIZE METHOD:
StatusCode egammaMiddleShape::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaMiddleShape");

  // retrieve all helpers from det store
  m_calo_dd = CaloDetDescrManager::instance();

  // Create egammaqweta2c Tool
  if(m_egammaqweta2c.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaqweta2c);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaqweta2c << " retrieved"); 

  // Create egammaEnergyPositionAllSamples Tool
  if(m_egammaEnergyPositionAllSamples.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaEnergyPositionAllSamples);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaEnergyPositionAllSamples << " retrieved"); 

  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaMiddleShape::finalize()
{
  return StatusCode::SUCCESS;
}

// =====================================================================
// ATHENA METHOD:
StatusCode egammaMiddleShape::execute(const xAOD::CaloCluster *cluster, 
				      const CaloCellContainer *cell_container) 
{
  //
  // Estimate shower shapes from 2nd compartment
  // based on hottest cell and deta,dphi
  // with eta = m_cluster->eta(sam)
  //      phi = m_cluster->phi(sam)
  // and search for hottest cell in a 7X7 window 
  // in the second sampling 
  //

  ATH_MSG_DEBUG(" egammaMiddleShape: execute");

  // check if cluster is available
  if(!cluster) { 
    ATH_MSG_DEBUG(" egammaMiddleShape: Invalid pointer to cluster");
    return StatusCode::SUCCESS;
  }

  // check if cell container is available
  if(!cell_container) { 
    ATH_MSG_DEBUG(" egammaMiddleShape: Invalid pointer to cell_container");
    return StatusCode::SUCCESS;
  }

  // check if cluster is in barrel or in the end-cap
  if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
    ATH_MSG_DEBUG(" egammaMiddleShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  
  m_cluster = cluster;
  m_cellContainer = cell_container;

  // initialisation of variables
  InitVariables();
  // retrieve energy in all samplings
  StatusCode sc = m_egammaEnergyPositionAllSamples->execute(m_cluster);
  if (sc) {};
  m_eallsamples = m_egammaEnergyPositionAllSamples->e();

  // check if cluster is in barrel or end-cap
  bool in_barrel = m_egammaEnergyPositionAllSamples->inBarrel();
  // define accordingly position of xAOD::CaloCluster
  if (in_barrel) {
    m_sam    = CaloSampling::EMB2; 
  } else {
    m_sam    = CaloSampling::EME2; 
  }

  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  if (!FindPosition()) return StatusCode::SUCCESS;

  // search the hottest cell around the (eta,phi)
  // (eta,phi) are defined as etaSample() and phiSample
  // around this position a hot cell is searched for in a window
  // (m_neta*m_deta,m_nphi*m_dphi), by default (m_neta,m_nphi)=(7,7)
  //
  // FIX-ME : is window 7*7 not too large wrt cluster size ?
  //
  CaloLayerCalculator calc;
  sc = calc.fill(m_cellContainer,m_cluster->etaSample(m_sam),m_cluster->phiSample(m_sam),
		 m_neta*m_deta,m_nphi*m_dphi, (CaloSampling::CaloSample) m_sam);
  if ( sc.isFailure() ) {
   ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }

  double etamax = calc.etarmax();
  double phimax = calc.phirmax();

  // estimate the relevant quantities around the hottest cell
  // in the following eta X phi windows

  // 3X3
  sc = calc.fill(m_cellContainer,etamax,phimax,3.*m_deta,3.*m_dphi, (CaloSampling::CaloSample) m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e233 = calc.em(); 

  // 3X5
  sc = calc.fill(m_cellContainer,etamax,phimax,3.*m_deta,5.*m_dphi, (CaloSampling::CaloSample) m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e235 = calc.em(); 
  double etaw = calc.etas();
  m_phiw = calc.phis();

  //egammaqweta2c qweta2; 
  m_etaw = m_egammaqweta2c->Correct(m_eta,m_etacell, etaw); 
  m_width = etaw;
  m_poscs2 = m_egammaqweta2c->RelPosition(m_eta,m_etacell);

  // 5X5
  if (m_ExecOtherVariables) {
    sc = calc.fill(m_cellContainer,etamax,phimax,5.*m_deta,5.*m_dphi, (CaloSampling::CaloSample) m_sam);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
    }
    m_e255 = calc.em(); 
  }

  // 3X7
  sc = calc.fill(m_cellContainer,etamax,phimax,3.*m_deta,7.*m_dphi, (CaloSampling::CaloSample) m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e237 = calc.em(); 

  // 7x7
  sc = calc.fill(m_cellContainer,etamax,phimax,7.*m_deta,7.*m_dphi, (CaloSampling::CaloSample) m_sam);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("CaloLayerCalculator failed fill ");
  }
  m_e277 = calc.em(); 


  return StatusCode::SUCCESS;
}

// ====================================================================
void egammaMiddleShape::InitVariables()
{
  //
  // initialisation
  //

  double x=0.;

  m_e233 = x;
  m_e235 = x; 
  m_e237 = x; 
  m_e255 = x;
  m_e277 = x; 
  m_etaw = x;
  m_phiw = x;  
}

// =====================================================================
bool egammaMiddleShape::FindPosition()
{
  //
  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  // For this we use the tool egammaEnergyAllSamples
  // which uses the xAOD::CaloCluster method inBarrel() and inEndcap()
  // but also, in case close to the crack region where both 
  // boolean can be true, the energy reconstructed in the sampling
  //

  // eta max and averaged eta 
  m_etacell  = m_cluster->etamax(m_sam);
  m_eta = m_cluster->etaSample(m_sam);
  m_phi = m_cluster->phiSample(m_sam);
  if ((m_eta==0. && m_phi==0.) || fabs(m_eta)>100) return false;

  // granularity in (eta,phi) in the pre sampler
  // CaloCellList needs both enums: subCalo and CaloSample
  m_calo_dd->decode_sample(m_subcalo, m_barrel, m_sampling_or_module, 
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


  return true;
}

