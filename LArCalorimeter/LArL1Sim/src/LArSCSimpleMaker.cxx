/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "boost/foreach.hpp"
#include <cmath>
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <iostream>

/**
 * @brief Standard Gaudi algorithm constructor.
 */
LArSCSimpleMaker::LArSCSimpleMaker(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_scidtool ("CaloSuperCellIDTool"),
    m_sem_mgr(0),
    m_calo_id_manager(0)
{
  declareProperty ("SCIDTool", m_scidtool,
                   "Offline / supercell mapping tool.");
  declareProperty ("CellContainer", m_cellContainer = "AllCalo",
                   "SG key for the input calorimeter cell container.");
  declareProperty ("SCellContainer", m_sCellContainer = "SCellContainer",
                   "SG key for the output supercell LAr channel container.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArSCSimpleMaker::initialize()
{

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

  const CaloCell_SuperCell_ID * calo_sc_id   =  m_calo_id_manager->getCaloCell_SuperCell_ID(); 
  const CaloCell_ID * calo_cell_id =  m_calo_id_manager->getCaloCell_ID(); 

  const Tile_SuperCell_ID * tile_sc_id  =  m_calo_id_manager->getTile_SuperCell_ID(); 
  const TileID * tile_cell_id           =  m_calo_id_manager->getTileID(); 

  MsgStream  msglog(messageService(),name());

  DataPool<CaloCell> dataPool;
  if (dataPool.allocated()==0)
    dataPool.reserve (calo_sc_id->calo_cell_hash_max());

  const DataHandle<CaloCellContainer> cells;
  CHECK( evtStore()->retrieve(cells, m_cellContainer) );
  if ( msgLvl (MSG::DEBUG) )
    msg(MSG::DEBUG) << "Got container Size : " << cells->size() << endreq;
  
  int hash_max = calo_sc_id->calo_cell_hash_max(); 
  std::vector<float> energies (hash_max,0);
  std::vector<uint16_t> provenances  (hash_max,0);
  std::vector<uint16_t> gains (hash_max,0);
  std::vector<uint16_t> qualities (hash_max,0);


  BOOST_FOREACH (const CaloCell* cell, *cells) {
    Identifier cell_id = cell->ID(); 
    Identifier sCellID  = m_scidtool->offlineToSuperCellID (cell_id);

    if (!sCellID.is_valid()) {
      // msg(MSG::WARNING) << " SC ID not valid  " << sCellID.get_identifier32().get_compact() << " offline id = " << cell->ID().get_identifier32().get_compact()<< endreq;
      // calo_sc_id->print(cell->ID()); 
      continue;
    }
    IdentifierHash hash;
    hash = calo_sc_id->calo_cell_hash (sCellID);
    assert (hash < energies.size() );
    energies[hash] += cell->energy(); 
    provenances[hash] |= cell->provenance(); 
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

      energies[hash1] += cell->energy()/2.; 
      energies[hash2] += cell->energy()/2.; 
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

  for (unsigned int i=0; i < energies.size(); i++) {

    const CaloDetDescrElement* dde = m_sem_mgr->get_element (i);
    if (!dde) {
      // msg(MSG::WARNING) << " Not valid DDE, hash index =  "<< i << endreq;
      continue;
    }

    CaloCell* ss = dataPool.nextElementPtr();
    ss->setCaloDDE( m_sem_mgr->get_element (i));
    ss->setEnergy( energies[i] );
    ss->setTime(  0.  );

    ss->setQuality( qualities[i] );
    if (calo_sc_id->is_tile(ss->ID()))
      {
	ss->setProvenance( 0 );
	ss->setGain( (CaloGain::CaloGain) 0 );	
      }
    else
      {
	ss->setProvenance( provenances[i] );
	ss->setGain( (CaloGain::CaloGain)  gains[i] );
      }
    superCellContainer->push_back(ss);

  }

  return StatusCode::SUCCESS;
}

