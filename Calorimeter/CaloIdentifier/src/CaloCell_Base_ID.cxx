/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/CaloCell_Base_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2012
 * @brief 
 */


#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloIdentifier/CaloNeighbours.h"


CaloCell_Base_ID::CaloCell_Base_ID(const LArEM_Base_ID*   em_id, 
                                   const LArHEC_Base_ID*  hec_id, 
                                   const LArFCAL_Base_ID* fcal_id, 
                                   const LArMiniFCAL_ID*  minifcal_id,
                                   const Tile_Base_ID*    tile_id,
				   bool supercell)
  : m_emHelper(em_id),
    m_hecHelper(hec_id),
    m_fcalHelper(fcal_id),
    m_minifcalHelper(minifcal_id),
    m_tileHelper(tile_id),
    m_cell_hash_max(0),
    m_region_hash_max(0),
    m_caloNeighbours (0),
    m_supercell(supercell ? 1 : 0)
{
  m_helpers[LAREM] = em_id;
  m_helpers[LARHEC] = hec_id;
  m_helpers[LARFCAL] = fcal_id;
  m_helpers[TILE] = tile_id;
  m_helpers[LARMINIFCAL] = minifcal_id;

  for (int i=0; i < NSUBCALO; i++) {
    if (i == 0) {
      m_cell_min[i] = 0;
      m_reg_min[i] = 0;
    }
    else {
      m_cell_min[i] = m_cell_max[i-1];
      m_reg_min[i] = m_reg_max[i-1];
    }

    m_cell_max[i] = m_cell_min[i] + m_helpers[i]->channels().hash_max();
    m_reg_max[i]  = m_reg_min[i]  + m_helpers[i]->regions().hash_max();
  }

  m_cell_hash_max   = m_cell_max[NSUBCALO-1];
  m_region_hash_max = m_reg_max[NSUBCALO-1];

  // count number of helpers with enabled neighbors and instantiate 
  // super neighbors in case this is more than 1

  int nNeigh(0);
  
  if ( m_emHelper->do_neighbours() ) nNeigh++;
  if ( m_hecHelper->do_neighbours() ) nNeigh++;
  if ( m_fcalHelper->do_neighbours() ) nNeigh++;
  if ( m_minifcalHelper->do_neighbours() ) nNeigh++;
  if ( m_tileHelper->do_neighbours() ) nNeigh++;
  
  if ( nNeigh > 1 ) { 
    m_caloNeighbours = new CaloNeighbours();
  }
}


CaloCell_Base_ID::~CaloCell_Base_ID()
{
  if ( m_caloNeighbours ) delete m_caloNeighbours;
}


int CaloCell_Base_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
{
  
  MsgStream log(m_msgSvc, "CaloCell_Base_ID");
  if(m_msgSvc)log << MSG::DEBUG << "Initialize" << endmsg;
  
  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }
  
  // init base object
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);
  
  // Register version of the different dictionaries 
  if (register_dict_tag(dict_mgr, "LArCalorimeter")) return(1);
  if (register_dict_tag(dict_mgr, "TileCalorimeter")) return(1);
  
  //
  // ... initialize cell / region vectors
  //
  m_region_vec.clear();
  m_region_vec.reserve(m_region_hash_max);

  m_cell_vec.clear();
  m_cell_vec.reserve(m_cell_hash_max);

  for (int i=0; i < NSUBCALO; i++) {
    m_region_vec.insert (m_region_vec.end(),
                         m_helpers[i]->regions().begin(),
                         m_helpers[i]->regions().end());
    m_cell_vec.insert (m_cell_vec.end(),
                       m_helpers[i]->channels().begin(),
                       m_helpers[i]->channels().end());
  }
  assert (m_region_vec.size() == m_region_hash_max);
  assert (m_cell_vec.size() == m_cell_hash_max);

  // initialize CaloCell neighbours
  if(m_caloNeighbours) {
    std::string neighbourFile;
    if ( !m_supercell ) {
      neighbourFile = dict_mgr.find_metadata("FULLATLASNEIGHBORS");
    }
    else {
      // use this file name for Super Cells - should be replaced by db tag ...
      neighbourFile = "SuperCaloNeighborsSuperCells-April2014.dat";
    }
    if (!neighbourFile.size()) throw std::runtime_error("CaloCell_ID: Cannot find the CaloNeighbour file name");
    if(m_msgSvc)log << MSG::DEBUG << "Initializing Super3D Neighbors from file " << neighbourFile << endmsg;
    m_caloNeighbours->initialize(this, neighbourFile);
  }
 
  return 0;
}


int   CaloCell_Base_ID::calo_sample   (const Identifier         id) const
{
  int calo_sampl = (int) Unknown;
  
  if(m_emHelper->is_em_barrel(id)) {
    calo_sampl = m_emHelper->sampling(id)+(int)PreSamplerB; 
  } 
  else if (m_emHelper->is_em_endcap_outer(id)) {
    calo_sampl = m_emHelper->sampling(id)+(int)PreSamplerE; 
  } 
  else if (m_emHelper->is_em_endcap_inner(id)) {
    calo_sampl = m_emHelper->sampling(id)+(int)EME1;
  }
  
  else if(m_hecHelper->is_lar_hec(id)) {
    calo_sampl = m_hecHelper->sampling(id) + (int) HEC0; 
  }

  else if(m_minifcalHelper->is_lar_minifcal(id)) {
    // must do minifcal before fcal because miniFCAL IS FCAL
    calo_sampl = m_minifcalHelper->depth(id) - 1 + (int) MINIFCAL0; 
  }
  else if(m_fcalHelper->is_lar_fcal(id)) {
    calo_sampl = m_fcalHelper->module(id) - 1 + (int) FCAL0; 
  }
  
  else if (m_tileHelper->is_tile_barrel( id )) {
    calo_sampl = TileBar0 + m_tileHelper->sample(id);
  } 

  else if (m_tileHelper->is_tile_extbarrel( id )) {
    calo_sampl = TileExt0 + m_tileHelper->sample(id);
  }
  
  else if (m_tileHelper->is_tile_gap( id )) {
    calo_sampl = TileGap1 - 1 + m_tileHelper->sample(id);
  } 
  
  return calo_sampl;
}

int   CaloCell_Base_ID::calo_sample    (const IdentifierHash caloHash) const
{
  Identifier id = cell_id (caloHash);
  return calo_sample(id);
}


int
CaloCell_Base_ID::get_neighbours(const IdentifierHash caloHashId,
                                 const LArNeighbours::neighbourOption& option,
                                 std::vector<IdentifierHash>& neighbourList) const
{
  int result = 1; 

  int subCalo = NOT_VALID;
  IdentifierHash subHash = subcalo_cell_hash (caloHashId, subCalo);

  switch (subCalo) {
  case LAREM:
    em_idHelper()->get_neighbours(subHash, option, neighbourList);
    break;

  case LARHEC:
    hec_idHelper()->get_neighbours(subHash, option, neighbourList);
    break;

  case LARFCAL:
    fcal_idHelper()->get_neighbours(subHash, option, neighbourList);
    break;

  case TILE:
    tile_idHelper()->get_neighbours(subHash, option, neighbourList);
    break;

  case LARMINIFCAL:
    minifcal_idHelper()->get_neighbours(subHash, option, neighbourList);
    break;

  default:
    neighbourList.resize(0);
    return result;
  }

  unsigned int shift = caloHashId - subHash;
  int neighbourIndex = neighbourList.size();

  if (neighbourIndex > 0) {
    if (shift != 0) {
      for (int iN = 0 ; iN <neighbourIndex ; ++iN) {
        neighbourList[iN] += shift;
      }
    }
    result = 0 ;
  }

  if ( m_caloNeighbours ) {
    if ( (option & LArNeighbours::prevSuperCalo) ){
      result = m_caloNeighbours->get_prevInCalo(caloHashId,neighbourList);
      if ( result != 0 )  
	return result;
    }
    
    if ( (option & LArNeighbours::nextSuperCalo) ){
      result = m_caloNeighbours->get_nextInCalo(caloHashId,neighbourList);
      if ( result != 0 )  
	return result;
    }
  }

  return result;
}

