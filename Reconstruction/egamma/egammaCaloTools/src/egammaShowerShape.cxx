/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaShowerShape.h"
#include "egammaInterfaces/IegammaPreSamplerShape.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "egammaInterfaces/IegammaBackShape.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "SGTools/DataProxy.h" 

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

// Constructor 
egammaShowerShape::egammaShowerShape(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cluster(0), 
    m_cellContainer(0),
    m_enecell(0), 
    m_etacell(0),
    m_gracell(0),
    m_ncell(0)
{ 
  // declare Interface
  declareInterface<IegammaShowerShape>(this);

  InitVariables();
}

// ====================================================================
// DESTRUCTOR:
egammaShowerShape::~egammaShowerShape()
{ 
}

// ========================================================================
// INITIALIZE METHOD:
StatusCode egammaShowerShape::initialize(){
  ATH_MSG_DEBUG(" Initializing egammaShowerShape");

  if(m_egammaPreSamplerShape.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaPreSamplerShape);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaPreSamplerShape << " retrieved"); 

  if(m_egammaStripsShape.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaStripsShape);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaStripsShape << " retrieved"); 

  if(m_egammaMiddleShape.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaMiddleShape);
    return StatusCode::FAILURE;
  } 

  if(m_egammaBackShape.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaBackShape);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaBackShape << " retrieved"); 

  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaShowerShape::finalize(){
  return StatusCode::SUCCESS;
}

// =====================================================================
// ATHENA METHOD:
StatusCode egammaShowerShape::execute(const xAOD::CaloCluster *cluster, 
				      const CaloCellContainer *cell_container) 
{
  // check if cluster is available
  if(!cluster) { 
    ATH_MSG_DEBUG(" egammaShowerShape: Invalid pointer to cluster");
    return StatusCode::SUCCESS;
  }

  // check if cell container is available
  if(!cell_container) { 
    ATH_MSG_DEBUG(" egammaShowerShape: Invalid pointer to cell_container");
    return StatusCode::SUCCESS;
  }

  // check if cluster is in barrel or in the end-cap
  if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
    ATH_MSG_DEBUG(" egammaShowerShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  
  m_cluster = cluster;
  m_cellContainer = cell_container;


  // initialisation of variables
  InitVariables();

  // shower shapes in presampler
  if ( m_ExecAllVariables && m_ExecOtherVariables && m_ExecPreSampler ) {
    // call execute method
    StatusCode sc = m_egammaPreSamplerShape->execute(cluster,cell_container);
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Presampler shape returned failure ");
    }
  }

  // shower shapes in 1st compartment
  if ( m_ExecEMFirst ) {
    // call the execute method
    StatusCode sc = m_egammaStripsShape->execute(cluster,cell_container);
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Strip shape returned failure ");
    }
  }

  // shower shapes in 2nd compartment
  if ( m_ExecEMSecond ) {
    // call the execute method
    StatusCode sc = m_egammaMiddleShape->execute(cluster,cell_container);
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Middle shape returned failure ");
    }
  }

  // shower shapes in 3rd compartment
  if ( m_ExecAllVariables && m_ExecEMThird ) {
    // call execute method
    StatusCode sc = m_egammaBackShape->execute(cluster,cell_container);
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Back shape returned failure ");
    }
  }
  
  // Fill shower object with previously filled values
  FillShower();

  // shower shapes combined in different samplings
  if ( m_ExecAllVariables && m_ExecEMCombined ) {
    StatusCode sc = CombinedShape();
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Combined shape returned failure ");
    }
  }

  return StatusCode::SUCCESS;
} 

// ====================================================================
void egammaShowerShape::InitVariables(){
  //
  // initialisation
  //

  double x=0.;
  double x1=-999.;

  // pre sampler
  m_e011 = x;
  m_e033 = x; 

  // first sampling
  m_etaseed              = x;
  m_phiseed              = x;
  m_etamax               = x;
  m_phimax               = x;
  m_ncetamax             = -1;
  m_ncetaseed            = -1;
  m_wstot                = x1;
  m_etot                 = x;
  m_etas3                = x;  
  m_deltaEtaTrackShower  = x1; 
  m_deltaEtaTrackShower7 = x1;
  m_e132                 = x; 
  m_e1152                = x; 
  m_ws3                  = x1; 
  m_ws3c                 = x1; 
  m_poscs1               = x1; 
  m_etaincell            = x1; 
  m_asymmetrys3          = x1; 
  m_f1                   = x;  
  m_f1core               = x;  
  m_f2                   = x; 
  m_widths5              = x1; 
  m_esec                 = x; 
  m_val                  = x;  
  m_fside                = x;
  m_emaxs1               = x; 
  m_emins1               = x; 
  m_esec1                = x;
  m_success              = false ;

  // second sampling
  m_e233 = x;
  m_e235 = x ; 
  m_e237 = x; 
  m_e255 = x;
  m_e277 = x; 
  m_etaw = x;
  m_phiw = x;
  m_width= x;
  m_poscs2=x;
  m_etagranularity = x;
  m_phigranularity = x;

  // third sampling
  m_f3     = x;
  m_f3core = x;
  m_e333   = x;
  m_e335   = x; 
  m_e355   = x; 
  m_e337   = x; 
  m_e377   = x; 

  // combination of samplings
  m_ecore            = x;
  m_reta3337_allcalo = x;
  
}

// =================================================================  
void egammaShowerShape::FillShower(){
  //
  // Fill egammaShowerShape object
  // with values from other tools
  // 

  // presampler
  m_e011 = m_egammaPreSamplerShape->e011();
  m_e033 = m_egammaPreSamplerShape->e033();

  // strips
  m_etot                 = m_egammaStripsShape->etot();
  m_e1152                = m_egammaStripsShape->e1152();
  m_e132                 = m_egammaStripsShape->e132();
  m_emaxs1               = m_egammaStripsShape->emaxs1();
  m_emins1               = m_egammaStripsShape->emins1();
  m_esec                 = m_egammaStripsShape->esec();
  m_esec1                = m_egammaStripsShape->esec1();
  m_f1                   = m_egammaStripsShape->f1();
  m_f1core               = m_egammaStripsShape->f1core();
  m_f2                   = m_egammaStripsShape->f2();
  m_ncetamax             = m_egammaStripsShape->ncetamax();
  m_ncetaseed            = m_egammaStripsShape->ncetaseed();
  m_etas3                = m_egammaStripsShape->etas3();
  m_poscs1               = m_egammaStripsShape->poscs1();
  m_deltaEtaTrackShower  = m_egammaStripsShape->deltaEtaTrackShower();
  m_deltaEtaTrackShower7 = m_egammaStripsShape->deltaEtaTrackShower7();
  m_wstot                = m_egammaStripsShape->wstot();
  m_ws3                  = m_egammaStripsShape->ws3();
  m_ws3c                 = m_egammaStripsShape->ws3c();
  m_widths5              = m_egammaStripsShape->widths5();
  m_asymmetrys3          = m_egammaStripsShape->asymmetrys3();
  m_val                  = m_egammaStripsShape->val();
  m_fside                = m_egammaStripsShape->fside();
  m_success              = m_egammaStripsShape->success();

  m_etacell = m_egammaStripsShape->etacell_arr();
  m_enecell = m_egammaStripsShape->enecell_arr();
  m_gracell = m_egammaStripsShape->gracell_arr();
  m_ncell   = m_egammaStripsShape->ncell_arr();

  // middle
  m_e233   = m_egammaMiddleShape->e233();
  m_e235   = m_egammaMiddleShape->e235();
  m_e255   = m_egammaMiddleShape->e255();
  m_e237   = m_egammaMiddleShape->e237();
  m_e277   = m_egammaMiddleShape->e277();
  m_etaw   = m_egammaMiddleShape->etaw();
  m_phiw   = m_egammaMiddleShape->phiw();
  m_poscs2 = m_egammaMiddleShape->poscs2();

  // back
  m_e333   = m_egammaBackShape->e333();
  m_e335   = m_egammaBackShape->e335();
  m_e355   = m_egammaBackShape->e355();
  m_e337   = m_egammaBackShape->e337();
  m_e377   = m_egammaBackShape->e377();
  m_f3     = m_egammaBackShape->f3();
  m_f3core = m_egammaBackShape->f3core();
}

// =================================================================  
StatusCode egammaShowerShape::CombinedShape(){ 

  // Shower shapes combining different samplings
  //
  // all energy values have to be initialised to zero 
  // which is equivalent to have nothing (if it does not exists)
  // or 0 if there is no deposit
  double x=-999.;

  // energy in 3 strips in the 1st sampling
  double e132  = (m_e132>x) ? m_e132 : 0.;

  if (m_ExecOtherVariables) {
    // energy in 1X1 in the presampler
    double e011  = (m_e011>x) ? m_e011 : 0.; 
    // energy in 3X3 in the presampler
    double e033  = (m_e033>x) ? m_e033 : 0.;
    // energy in 15 strips in the 1st sampling
    double e1152  = (m_e1152>x) ? m_e1152 : 0.;
    // energy in 3X3 in the 2nd sampling
    double e233  = (m_e233>x) ? m_e233 : 0.;
    // energy in 5X5 in the 2nd sampling
    double e255  = (m_e255>x) ? m_e255 : 0.;
    // energy in 3X7 in the 2nd sampling
    double e237  = (m_e237>x) ? m_e237 : 0.;
    // energy in 3X3 in the 3rd sampling
    double e333  = (m_e333>x) ? m_e333 : 0.;
    // energy in 3X5 in the 3rd sampling
    double e335  = (m_e335>x) ? m_e335 : 0.;
    // energy in 3X7 in the 3rd sampling
    double e337  = (m_e337>x) ? m_e337 : 0.;

    // energy in core of the shower
    // 3X3 in the presampler (m_e033)
    // 15X2 in the strips     (m_e1152)
    // 5X5 in the middle     (m_e255)
    // 3X5 in the back       (m_e335)
    m_ecore = e033 + e1152 + e255 + e335;

    // e 3X3 / e 3X7, the ratio of energies deposited 
    // in a 3X3 and 3X7 clusters. More precisely, 
    // for the 3X3 cluster, sampling by sampling, the cell energies
    // are summed on the following eta by phi windows
    // 1X1 in the presampler (m_e011)
    // 3X2 in the strips     (m_e132)
    // 3X3 in the middle     (m_e233)
    // 3X3 in the back       (m_e333)
    // for the 7X3 cluster, sampling by sampling, the cell energies
    // are summed on the following eta by phi windows
    // 3X3 in the presampler (m_e033)
    // 15X2 in the strips     (m_e1152) 
    // 3X7 in the middle     (m_e237)
    // 3X7 in the back       (m_e337)
    double e33 = e011 + e132 + e233 + e333;
    double e37 = e033 + e1152 + e237 + e337;
    m_reta3337_allcalo = fabs(e37) > 0. ? 1.-e33/e37 : 1.;
  }

  return StatusCode::SUCCESS;  
}

