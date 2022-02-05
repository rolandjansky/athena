/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: Denis Oliveira Damazio                              +
// + Institute .....: BNL                                                 +
// + Creation date .: 17/06/2012                                          +
// + Moved to MT : 20/02/2021
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArSCSimpleMaker.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "boost/foreach.hpp"
#include <cmath>
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include <random>

#include <mutex>

/**
 * @brief Standard Gaudi algorithm constructor.
 */
LArSCSimpleMaker::LArSCSimpleMaker(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_sem_mgr(0),
    m_calo_id_manager(0)
{
}

StatusCode LArSCSimpleMaker::initialize(){
	ATH_CHECK( m_cellContainerKey.initialize() );
	ATH_CHECK( m_sCellContainerKey.initialize() );
	ATH_CHECK( m_scidtool.retrieve() );
	ATH_CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );
	ATH_CHECK( detStore()->retrieve (m_calo_id_manager, "CaloIdManager") );
	if (m_compNoise )ATH_CHECK( m_noise_per_cell_Key.initialize() );
        if ( m_addBCID ) ATH_CHECK( m_bcidAvgKey.initialize() );
	return StatusCode::SUCCESS;
}

/**
 * @brief Algorithm execute method.
 */
StatusCode LArSCSimpleMaker::execute(const EventContext& context) const
{

  // conditions
  const CaloNoiseSigmaDiff* noise_per_cell = nullptr;
  if ( m_compNoise ){
    SG::ReadCondHandle<CaloNoiseSigmaDiff> noise_per_cell_handle(m_noise_per_cell_Key,context); 
    if ( not noise_per_cell_handle.isValid() ) {
	ATH_MSG_DEBUG("Found invalid read handle for CaloNoiseSigmaDiff");
	return StatusCode::FAILURE;
    }
    noise_per_cell = noise_per_cell_handle.cptr();
  } // end of if n_compNoise

  // Not really a condition, but almost
  const CaloBCIDAverage* caloLumiBCID = nullptr;
  if (m_addBCID ) {
    SG::ReadHandle<CaloBCIDAverage> caloLumiBCIDHandle(m_bcidAvgKey, context);
    if ( not caloLumiBCIDHandle.isValid() ) {
	ATH_MSG_DEBUG("Found invalid read handle for CaloBCIDAverage");
	return StatusCode::FAILURE;
    }
    caloLumiBCID = caloLumiBCIDHandle.cptr();
  } // end of if m_addBCID

  const CaloCell_SuperCell_ID * calo_sc_id   =  m_calo_id_manager->getCaloCell_SuperCell_ID(); 
  const CaloCell_ID * calo_cell_id =  m_calo_id_manager->getCaloCell_ID(); 

  const Tile_SuperCell_ID *  tile_sc_id  =  m_calo_id_manager->getTile_SuperCell_ID(); 
  const TileID * tile_cell_id           =  m_calo_id_manager->getTileID(); 

  int hash_max = calo_sc_id->calo_cell_hash_max(); 
  DataPool<CaloCell> dataPool;
  if (dataPool.allocated()==0)
    dataPool.reserve (hash_max);

  auto cellsHandle = SG::makeHandle( m_cellContainerKey, context );
  if ( not cellsHandle.isValid() ) {
    ATH_MSG_ERROR("Did not get CaloCellContainer input");
    return StatusCode::FAILURE;
  }

  const CaloCellContainer* cells = cellsHandle.cptr();
  ATH_MSG_DEBUG("Got a CaloCellContainer input");
  
  std::vector<float> energies (hash_max,0);
  std::vector<float> enTime (hash_max,0);
  std::vector<float> enForTime (hash_max,0);
  std::vector<bool> timeDef (hash_max,false);
  std::vector<uint16_t> gains (hash_max,0);
  std::vector<uint16_t> qualities (hash_max,0);
  std::vector<float> sigma_noise_per_scell(hash_max,0);

  for (const CaloCell* cell : *cells) {
    Identifier cell_id = cell->ID(); 
    Identifier sCellID  = m_scidtool->offlineToSuperCellID (cell_id);

    if (!sCellID.is_valid()) {
      // ATH_MSG_WARNING( " SC ID not valid  " << sCellID.get_identifier32().get_compact() << " offline id = " << cell->ID().get_identifier32().get_compact() );
      // calo_sc_id->print(cell->ID()); 
      continue;
    }
    IdentifierHash hash, cell_hash;
    hash = calo_sc_id->calo_cell_hash (sCellID);
    assert (hash < energies.size() );
    float pedestalshift = 0.0;
    if ( m_addBCID ) pedestalshift = caloLumiBCID->average(cell_id);
    energies[hash] += cell->energy() + pedestalshift; 
    if ( m_compNoise && cell->gain() == CaloGain::LARHIGHGAIN ){
	cell_hash = calo_cell_id->calo_cell_hash(cell_id);
	sigma_noise_per_scell[hash]+=(*noise_per_cell)[cell_hash];
    }
    uint16_t prov = cell->provenance();
    if ( ((prov & 0x2000) == 0x2000) && (cell->et()>50) ) {
	// time defined for one of the cells
	timeDef[hash] = true;
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
    }


  } 

  SG::WriteHandle<CaloCellContainer> scellContainerHandle( m_sCellContainerKey, context);
  auto superCellContainer = std::make_unique<CaloCellContainer> ();

  superCellContainer->reserve(energies.size());

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

    //CaloCell* ss = dataPool.nextElementPtr();
    CaloCell* ss = new CaloCell();
    ss->setCaloDDE( m_sem_mgr->get_element (i));
    ss->setEnergy( energies[i] );
    uint16_t prov (0);
    if ( timeDef[i]  ){
        float time = enTime[i] / enForTime[i]; // if timeDef is true, enForTime is > 10
        float et = ss->et();
	ss->setTime( time ); 
        prov = 0x2000;
        if(et>10e3 && time>-8 && time<16) prov |= 0x200;
        else if(et<=10e3 && fabs(time)<8) prov |= 0x200; 
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
  ATH_CHECK( scellContainerHandle.record( std::move(superCellContainer) ) );

  return StatusCode::SUCCESS;
}

