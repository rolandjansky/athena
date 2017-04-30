/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MaxCellDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkCalo/MaxCellDecorator.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "LArCabling/LArCablingService.h"

#include <vector>
#include <string>

// Constructor
DerivationFramework::MaxCellDecorator::MaxCellDecorator(const std::string& t,
							const std::string& n,
							const IInterface* p) :
  AthAlgTool(t,n,p),
  m_SGKey_photons(""),
  m_SGKey_electrons("")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("LArCablingService", m_larCablingSvc );
  declareProperty("SGKey_photons", m_SGKey_photons);
  declareProperty("SGKey_electrons", m_SGKey_electrons);
}

// Destructor
DerivationFramework::MaxCellDecorator::~MaxCellDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::MaxCellDecorator::initialize(){
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");
  
  CHECK( m_larCablingSvc.retrieve() );
  ATH_MSG_INFO("Retrieved tool " << m_larCablingSvc); 
  
  if(m_SGKey_photons == "" && m_SGKey_electrons == "" ){
    ATH_MSG_FATAL("No e-gamma collection provided. At least one egamma collection (photon/electrons) must be provided!");
    return StatusCode::FAILURE;
  }

  if (m_SGKey_electrons!="") {
    ATH_MSG_INFO("Using "<< m_SGKey_electrons <<" for electrons");
  }

  if (m_SGKey_photons!="") {
    ATH_MSG_INFO("Using "<< m_SGKey_photons <<" for photons");
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MaxCellDecorator::finalize(){
  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::MaxCellDecorator::addBranches() const{
  // Retrieve photon container
  const xAOD::EgammaContainer* importedPhotons(0);
  if(m_SGKey_photons != ""){
    if (evtStore()->retrieve(importedPhotons,m_SGKey_photons).isFailure()) {
      ATH_MSG_ERROR("No e-gamma collection with name " << m_SGKey_photons << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
    for (auto photon : *importedPhotons) {
      if(photon->author() != 128) {
	decorateObject(photon);
      }
    }
  }

  // Retrieve electron container
  const xAOD::EgammaContainer* importedElectrons(0);
  if(m_SGKey_electrons != ""){
    if (evtStore()->retrieve(importedElectrons,m_SGKey_electrons).isFailure()) {
      ATH_MSG_ERROR("No e-gamma collection with name " << m_SGKey_electrons << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
    for (auto electron : *importedElectrons) {
      decorateObject(electron);
    }
  }

  return StatusCode::SUCCESS;
}

void DerivationFramework::MaxCellDecorator::decorateObject(const xAOD::Egamma*& egamma) const{

  float    maxEcell_time   = -9999.9;
  float    maxEcell_energy = -9999.9;
  int      maxEcell_gain   = -1;
  uint64_t maxEcell_onlId  = 0;
  float    maxEcell_x      = -9999.9;
  float    maxEcell_y      = -9999.9;
  float    maxEcell_z      = -9999.9;


  const xAOD::CaloCluster *cluster = egamma->caloCluster();
        
  if( cluster ){
    if( !cluster->getCellLinks() ){
      ATH_MSG_WARNING("CellLinks not found");
    }
   
    float emax = -9999.;

    const CaloCell* cell_maxE = 0;
    for( const CaloCell* cell : *cluster ){
      int sampling = cell->caloDDE()->getSampling();
      if( sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	if( (cell->provenance() & 0x2000) ){
	  if( cell->energy() > emax ) {
	    emax = cell->energy();
	    cell_maxE = cell;
	  }
	}
      }
    }
                
    if( cell_maxE ){
      const CaloDetDescrElement* caloDDEl = cell_maxE->caloDDE();
      maxEcell_time    = cell_maxE->time();
      maxEcell_energy = cell_maxE->energy();
      maxEcell_gain   = (int)cell_maxE->gain();
      maxEcell_onlId  = (uint64_t)(m_larCablingSvc->createSignalChannelID(caloDDEl->identify())).get_compact();
      maxEcell_x      = caloDDEl->x();
      maxEcell_y      = caloDDEl->y();
      maxEcell_z      = caloDDEl->z();
    }
  }
  
  SG::AuxElement::Decorator< float > decoration0( "maxEcell_time" );
  decoration0(*egamma) =  maxEcell_time;
  
  SG::AuxElement::Decorator< float > decoration1( "maxEcell_energy" );
  decoration1(*egamma) =  maxEcell_energy;

  SG::AuxElement::Decorator< int > decoration2( "maxEcell_gain" );
  decoration2(*egamma) =  maxEcell_gain;

  SG::AuxElement::Decorator< uint64_t > decoration3( "maxEcell_onlId" );
  decoration3(*egamma) =  maxEcell_onlId;

  SG::AuxElement::Decorator< float > decoration4( "maxEcell_x" );
  decoration4(*egamma) =  maxEcell_x;
  
  SG::AuxElement::Decorator< float > decoration5( "maxEcell_y" );
  decoration5(*egamma) =  maxEcell_y;
  
  SG::AuxElement::Decorator< float > decoration6( "maxEcell_z" );
  decoration6(*egamma) =  maxEcell_z;

}
