/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: Denis Oliveira Damazio                              +
// + Institute .....: BNL                                                 +
// + Creation date .: 17/06/2012                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArL1Sim/LArSCSimpleMaker.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "boost/foreach.hpp"
#include <cmath>
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include <random>


#include <iostream>

/**
 * @brief Standard Gaudi algorithm constructor.
 */
LArSCSimpleMaker::LArSCSimpleMaker(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_scidtool ("CaloSuperCellIDTool"),
    m_sem_mgr(0),
    m_calo_id_manager(0),
    m_noisetool("CaloNoiseToolDefault"),
    m_caloLumiBCIDTool(""), m_first(true)
{
  declareProperty ("SCIDTool", m_scidtool,
                   "Offline / supercell mapping tool.");
  declareProperty ("CellContainer", m_cellContainer = "AllCalo",
                   "SG key for the input calorimeter cell container.");
  declareProperty ("SCellContainer", m_sCellContainer = "SCellContainer",
                   "SG key for the output supercell LAr channel container.");
  declareProperty("CaloNoiseTool", m_noisetool, "Tool Handle for noise tool");
  declareProperty("LumiBCIDTool",m_caloLumiBCIDTool,"Tool for BCID pileup offset average correction");
  declareProperty("CompensateForNoise", m_compNoise=true,"Compensate for the noise with low noise");
  declareProperty("addBCID", m_addBCID=true,"Compensate for the BCID");

}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArSCSimpleMaker::initialize()
{

  if ( m_compNoise ) ATH_CHECK( m_noisetool.retrieve() );
  if ( m_addBCID ) ATH_CHECK( m_caloLumiBCIDTool.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 */
StatusCode LArSCSimpleMaker::execute()
{

  CHECK( m_scidtool.retrieve() );
  CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );
  CHECK( detStore()->retrieve (m_calo_id_manager, "CaloIdManager") );

  const xAOD::EventInfo* evt = nullptr;
  CHECK( evtStore()->retrieve(evt,"EventInfo") );
  long bunch_crossing(-1);
  bunch_crossing = evt->bcid();

  const CaloCell_SuperCell_ID * calo_sc_id   =  m_calo_id_manager->getCaloCell_SuperCell_ID(); 
  const CaloCell_ID * calo_cell_id =  m_calo_id_manager->getCaloCell_ID(); 

  const Tile_SuperCell_ID * tile_sc_id  =  m_calo_id_manager->getTile_SuperCell_ID(); 
  const TileID * tile_cell_id           =  m_calo_id_manager->getTileID(); 

  DataPool<CaloCell> dataPool;
  if (dataPool.allocated()==0)
    dataPool.reserve (calo_sc_id->calo_cell_hash_max());

  const DataHandle<CaloCellContainer> cells;
  CHECK( evtStore()->retrieve(cells, m_cellContainer) );
  ATH_MSG_DEBUG( "Got container Size : " << cells->size()  );
  
  int hash_max = calo_sc_id->calo_cell_hash_max(); 
  std::vector<float> energies (hash_max,0);
  std::vector<float> enTime (hash_max,0);
  std::vector<float> enForTime (hash_max,0);
  std::vector<bool> timeDef (hash_max,false);
  std::vector<uint16_t> gains (hash_max,0);
  std::vector<uint16_t> qualities (hash_max,0);
  std::vector<float> sigma_noise_per_scell(hash_max,0);
  int idx = 0;

  if ( m_first ) {
    m_noise_per_cell.reserve(cells->size());
    m_noise_per_cell.assign(cells->size(),0.0);
    if ( m_compNoise ) {
    BOOST_FOREACH (const CaloCell* cell, *cells) {
      const CaloDetDescrElement* cdde = cell->caloDDE();
      if ( cdde->is_tile () ) {idx++; continue;}
      std::vector<float> nn = m_noisetool->elecNoiseRMS3gains(cdde);
      float sigma_diff = nn[1]*nn[1] - nn[0]*nn[0] ;
      if ( sigma_diff < 0.0 ) { m_noise_per_cell[idx++]=0; continue;}
      m_noise_per_cell[idx++] = sqrt( sigma_diff );
    } // end of BOOST_FOREACH
    } // end of if m_compNoise
    m_first = false;
  } // end of if first

  idx=0;
  BOOST_FOREACH (const CaloCell* cell, *cells) {
    Identifier cell_id = cell->ID(); 
    Identifier sCellID  = m_scidtool->offlineToSuperCellID (cell_id);

    if (!sCellID.is_valid()) {
      // ATH_MSG_WARNING( " SC ID not valid  " << sCellID.get_identifier32().get_compact() << " offline id = " << cell->ID().get_identifier32().get_compact() );
      // calo_sc_id->print(cell->ID()); 
      continue;
    }
    IdentifierHash hash;
    hash = calo_sc_id->calo_cell_hash (sCellID);
    assert (hash < energies.size() );
    float pedestalshift = 0.0;
    if ( m_addBCID ) pedestalshift = m_caloLumiBCIDTool->average(cell,bunch_crossing);
    energies[hash] += cell->energy() + pedestalshift; 
    if ( cell->gain() == CaloGain::LARHIGHGAIN )
	sigma_noise_per_scell[hash]+=m_noise_per_cell[idx];
    idx++;
    uint16_t prov = cell->provenance();
    if ( ((prov & 0x2000) == 0x2000) && (cell->et()>50) ) {
	// time defined for one of the cells
	timeDef[hash] = timeDef[hash] || true;
        enForTime[hash] += cell->energy();
        enTime[hash] += cell->energy()*cell->time();
    }
    gains[hash] = std::max(gains[hash],(uint16_t)cell->gain());
    if ( qualities[hash] + (int) cell->quality() > 65535 ){
      qualities[hash] = 65535 ;
    }else
      {
	qualities[hash] += cell->quality();
      }

    if ( calo_cell_id->is_tile(cell_id) && tile_cell_id->sampling(cell_id)==TileID::SAMP_D){
      // Special case for SAMP_D in tile. Signal is split into two SCs 
      int section = tile_cell_id->section (cell_id);
      int side = tile_cell_id->side (cell_id);
      int module = tile_cell_id->module (cell_id);
      int tower = tile_cell_id->tower (cell_id);

      int section1 = section;
      int section2 = section;
      int side1 = side;
      int side2 = side; 
      int tower1= tower;
      int tower2= tower-1;

      if (tower ==0){
	side1 = -1;
	side2 =  1;
	tower1=0;
	tower2=0; 
      }
      if (tower==10){
	section2 = TileID::BARREL;
	section1 = TileID::EXTBAR;
      }


      Identifier sc_id1 = tile_sc_id->cell_id(section1,side1,module,tower1,0);
      Identifier sc_id2 = tile_sc_id->cell_id(section2,side2,module,tower2,0);


      int hash1 = calo_sc_id->calo_cell_hash (sc_id1);
      int hash2 = calo_sc_id->calo_cell_hash (sc_id2);

      energies[hash1] += cell->energy()*0.5; 
      energies[hash2] += cell->energy()*0.5; 
      //ignore the rest for now

      /*
      if (module==0){
	
	std::cout<<" cell, " <<tile_cell_id->print_to_string (cell_id)<< " " <<cell->eta()<< std::endl;
	std::cout<<" sc1, " <<tile_sc_id->print_to_string(sc_id1)<<std::endl;
	std::cout<<" sc2, " <<tile_sc_id->print_to_string(sc_id2)<<std::endl;

	//std::cout<<"  section, side, module, tower, sampling "<<section<<" " << side<< " "<< module<< " " << tower << " " << sampling<< " " << cell->eta() << std::endl; 
	std::cout<<" cell, " <<tile_cell_id->print_to_string (cell_id)<< " " <<cell->eta()<< std::endl;
	std::cout<<" sc, " <<tile_sc_id->print_to_string(sCellID)<<std::endl;
	std::vector<Identifier> ids_in_SC  = m_scidtool->superCellToOfflineID (sCellID);
	BOOST_FOREACH (Identifier id_in_SC, ids_in_SC)
	  {
	    std::cout<<" cells in sc " <<tile_cell_id->print_to_string(id_in_SC)<< std::endl;	    
	  }
      }
      */ 
    }


  } 

  CaloCellContainer* superCellContainer= new CaloCellContainer(SG::VIEW_ELEMENTS);
  superCellContainer->reserve(energies.size());
  if ( evtStore()->record(superCellContainer, m_sCellContainer).isFailure() ){
    REPORT_ERROR (StatusCode::FAILURE) << "Could not store container";
    delete superCellContainer;
    return StatusCode::FAILURE;
  }

  std::default_random_engine generator;
  for (unsigned int i=0; i < energies.size(); i++) {

    const CaloDetDescrElement* dde = m_sem_mgr->get_element (i);
    if (!dde) {
      // ATH_MSG_WARNING( " Not valid DDE, hash index =  "<< i  );
      continue;
    }

    // More noise
    float add_noise = 0.0;
    IdentifierHash hash = dde->calo_hash();
    if ( (!dde->is_tile()) && (sigma_noise_per_scell[hash] > 0.0) ){
	std::normal_distribution<double> distribution(0.0,sigma_noise_per_scell[hash] );
	add_noise = distribution(generator);
    }
    energies[i]+=add_noise;
      

    CaloCell* ss = dataPool.nextElementPtr();
    ss->setCaloDDE( m_sem_mgr->get_element (i));
    ss->setEnergy( energies[i] );
    uint16_t prov (0);
    if ( timeDef[i]  ){
        float time = enTime[i] / enForTime[i]; // if timeDef is true, enForTime is > 10
        float et = ss->et();
	ss->setTime( time ); 
        prov = 0x2000;
        if(et>10e3 && time>-8 && time<16) prov |= 0x200;
        if(et<=10e3 && fabs(time)<8) prov |= 0x200; 
    } else ss->setTime(  999.0  ); // makes sure it will fail some BCID

    ss->setQuality( qualities[i] );
    if (calo_sc_id->is_tile(ss->ID()))
      {
	ss->setProvenance( 0 );
	ss->setGain( (CaloGain::CaloGain) 0 );	
      }
    else
      {
	ss->setProvenance( prov );
	ss->setGain( CaloGain::LARHIGHGAIN );
      }
    superCellContainer->push_back(ss);

  }

  return StatusCode::SUCCESS;
}

