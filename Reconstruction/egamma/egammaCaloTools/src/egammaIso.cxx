/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaIso.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "FourMomUtils/P4Helpers.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath> 

using CLHEP::MeV;


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
egammaIso::egammaIso(const std::string& type,
		     const std::string& name,
		     const IInterface* parent)
  : AthAlgTool(type, name, parent){

  // declare Interface
  declareInterface<IegammaIso>(this);

}

// ====================================================================
// DESTRUCTOR:
egammaIso::~egammaIso()
{ 
}

// ====================================================================
// INITIALIZE METHOD:
StatusCode egammaIso::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaIso");

  
  return StatusCode::SUCCESS;
}

// ====================================================================
// FINALIZE METHOD:
StatusCode egammaIso::finalize()
{
  return StatusCode::SUCCESS;
}


// NOTE:: First cone has to be the largest

//C.A
//Method to calculate just the hadronic leakage
//should be a copy of the relevant lines from the method above
// ===============================================================
StatusCode egammaIso::execute(const xAOD::CaloCluster *cluster, CaloCellList* had)
{
  m_ethad  = 0;
  m_ethad1 = 0;
  m_ehad1  = 0;

  m_cluster = cluster;
  if(!m_cluster) { 
    ATH_MSG_WARNING(" egammaIso: Invalid pointer to cluster"); 
    return StatusCode::RECOVERABLE;
  }
  m_HADCellList = had;
  
  const double eta  = m_cluster->eta(); 
  const double phi  = m_cluster->phi(); 
  double egap=0.;
  double ehad=0.;
  const double size = 0.12;
  
  // increase window size a bit to avoid problems due to 4cm shift.
  // considering that in the tile the granularity is 0.1, the value
  // choosen below is safe.
  // all hadron in 0.2X0.2
  if (m_HADCellList) {
    m_HADCellList->select(eta,phi,size,size);       
    ehad +=m_HADCellList->energy(); 
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileGap3);       
    egap +=m_HADCellList->energy(); 
    
    m_HADCellList->select(eta,phi,size,size,CaloSampling::HEC0);       
    m_ehad1 +=m_HADCellList->energy(); 
    
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileBar0);       
    m_ehad1 +=m_HADCellList->energy(); 
    
    // Fix had leakage in crack (TileGap1 and TileGap2 missing before 14.2)
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileGap1);
    m_ehad1 +=m_HADCellList->energy();
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileGap2);       
    m_ehad1 +=m_HADCellList->energy(); 
    
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileExt0);       
    m_ehad1 +=m_HADCellList->energy(); 
  }

  const double eta2 = m_cluster->etaBE(2);
  if (eta2==-999.) {
    m_ethad1 = -999;
    m_ethad = -999;
  } else {
    const double cheta2 = cosh(eta2);
    m_ethad1 = cheta2!=0. ? m_ehad1/cheta2 : 0.; 
    // substract scintillator energy from ehad! 
    m_ethad = cheta2!=0. ? (ehad-egap)/cheta2 : 0.;
  }

  return StatusCode::SUCCESS;
}


