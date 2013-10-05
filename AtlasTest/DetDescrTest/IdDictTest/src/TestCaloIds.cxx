/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TestCaloIds.cxx,v 1.37 2007-10-16 06:10:23 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "TestCaloIds.h"

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"

// DetDescr includes
#include "CaloIdentifier/CaloLVL1_ID.h"
//#include "CaloTTDetDescr/CaloTTDescrManager.h"
//#include "CaloTTDetDescr/CaloTTDescrRegion.h"
//#include "CaloTTDetDescr/CaloTTDescriptor.h"

#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
//#include "CaloDetDescr/CaloDetDescrManager.h"

// Athena-related includes
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include <assert.h>
#include <set>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

TestCaloIds::TestCaloIds()
    :
    m_name("IdDictTest_Calo"),
    m_idDictMgr(0), 
    //    m_caloTTMgr(0),
    //    m_caloMgr(0),
    m_lvl1Helper(0),
    m_dmHelper(0),
    m_cellHelper(0),
    m_msgSvc(0),
    m_chronoStSvc(0)
{
}

TestCaloIds::TestCaloIds(const IdDictManager* idDictMgr, 
			 const CaloLVL1_ID* lvl1Helper,
			 const CaloDM_ID* dmHelper,
			 const CaloCell_ID* cellHelper,
			 IMessageSvc* 	msgSvc,
			 IChronoStatSvc*  chronoStSvc)
    :
    m_name("IdDictTest_Calo"),
    m_idDictMgr(idDictMgr), 
    m_lvl1Helper(lvl1Helper),
    m_dmHelper(dmHelper),
    m_cellHelper(cellHelper),
    m_msgSvc(msgSvc),
    m_chronoStSvc(chronoStSvc)
{}

const std::string&
TestCaloIds::name (void) const
{
    return m_name;
}

StatusCode   
TestCaloIds::testLArDM_Ids		(void) const
{
    MsgStream  log(m_msgSvc,name());

    // Use log1 with a different name to print out pointers - this
    // avoids including them in the test checks, since the pointer
    // values change from run-to-run
    MsgStream  log1(m_msgSvc,"TestCaloIds::testLArDM_Ids");

    log << MSG::INFO 
	<< " ======================================  " 
	<< endreq;
    log << MSG::INFO 
	<< " Testing LAr DM ids'   " 
	<< endreq;

    bool error = false;

    const CaloDM_ID* dmId = m_dmHelper;
    log1 << MSG::DEBUG
	<< "m_dmHelper= " << m_dmHelper
	<< endreq;

    if (!m_dmHelper) error = true;

  log << MSG::INFO
      << " lar_zone_hash_max = (ref: ATLAS=39298)" 
      << dmId->lar_zone_hash_max()
      << " lar_region_hash_max = (ref: ATLAS=68)" 
      << dmId->lar_region_hash_max()
      << endreq;
  
  // regions
  std::vector<Identifier>::const_iterator itId = dmId->lar_region_begin();
  std::vector<Identifier>::const_iterator itIdEnd = dmId->lar_region_end();
  
  int nReg=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier reg_id = *itId;
    if(dmId->is_lar_dm(reg_id)) {
      
      nReg++;
      IdentifierHash hashId = dmId->lar_region_hash(reg_id);
      Identifier reg_id2 = dmId->lar_region_id(hashId);
      if ( reg_id2 != reg_id ) {
	log << MSG::ERROR 
	    << "LARDM: reg_id2 incorrect: " <<  dmId->show_to_string(reg_id2) 
	    << "         should be " <<  dmId->show_to_string(reg_id) 
	    << endreq;
	error = true;
      }
    }
  }

  if(nReg == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_lar_dm is always 'false'"
	<< endreq;
    error = true;
  } else {
    log << MSG::INFO
	<< " number of regions in the loop = (ref: ATLAS=68)" 
	<< nReg
	<< endreq;
  }

  // zones
  itId = dmId->lar_zone_begin();
  itIdEnd = dmId->lar_zone_end();
  
  int nZon=0;
  for(; itId!=itIdEnd;++itId){

    Identifier zo_id = *itId;
    if(dmId->is_lar_dm(zo_id)) {
      
      nZon++;
      IdentifierHash hashId = dmId->lar_zone_hash(zo_id);
      Identifier zo_id2 = dmId->lar_zone_id(hashId);
      if ( zo_id2 != zo_id ) {
	log << MSG::ERROR 
	    << " LARDM: zo_id2 incorrect: " <<  dmId->show_to_string(zo_id2) 
	    << "         should be " <<  dmId->show_to_string(zo_id) 
	    << endreq;
	error = true;
      }
            
      if(nZon == 1) {
	  log << MSG::INFO
	      << "Testing show_to_string with CaloDM_ID, zo_id= " << dmId->show_to_string(zo_id)  << std::endl;
      }

      int pn   = dmId->pos_neg_z(zo_id);
      int type = dmId->dmat(zo_id);
      int samp = dmId->sampling(zo_id);
      int reg  = dmId->region(zo_id);
      int eta  = dmId->eta(zo_id);
      int phi  = dmId->phi(zo_id);
      
      Identifier zo_id3;
      try {
	zo_id3 = dmId->zone_id (pn, type, samp, reg, eta, phi);
      }
      catch(CaloID_Exception & except){
	log << MSG::ERROR 
	    <<  " CaloId exception creating zo_id3  " 
	    << dmId->show_to_string(zo_id3) << "   "
	    << (std::string)except
	    << " pn, type, samp, reg, eta, phi= " << pn << " " << type << " " << samp << " " << reg << " " << eta << " " << phi << " "
	    << endreq;
      }
      if ( zo_id3 != zo_id ) {
	log << MSG::ERROR 
	    << "LARDM: zo_id3 incorrect: " <<  dmId->show_to_string(zo_id3) 
	    << "         should be " <<  dmId->show_to_string(zo_id) 
	    << " input val of pn, type, samp, reg, eta, phi= " << pn << " " << type << " " << samp << " " << reg << " " << eta << " " << phi
	    << endreq;
	error = true;
      }
      
      Identifier regId;
      Identifier zo_id4;
      try {
	regId = dmId->region_id (pn, type, samp, reg);
	zo_id4 = dmId->zone_id (regId, eta, phi);
      }
      catch(CaloID_Exception & except){
	log << MSG::ERROR 
	    <<  " CaloId exception creating regId or zo_id4 " 
	    << dmId->show_to_string(regId) << "   "<< dmId->show_to_string(zo_id4) << " "
	    << (std::string)except
	    << " pn, type, samp, reg, eta, phi= " << pn << " " << type << " " << samp << " " << reg << " " << eta << " " << phi << " "
	    << endreq;
      }
      if ( zo_id4 != zo_id3 ) {
	log << MSG::ERROR 
	    << "LARDM: zo_id4 incorrect: " <<  dmId->show_to_string(zo_id4) 
	    << "          should be " <<  dmId->show_to_string(zo_id3) 
	    << "          regId     " <<  dmId->show_to_string(regId) 
	    << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
	    << endreq;
	error = true;
      }
    }
  }

  if(nZon == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_lar_dm is always 'false'"
	<< endreq;
    error = true;
  } else {
    log << MSG::INFO
	<< " number of zones in the loop = (ref: ATLAS=39298)" 
	<< nZon
	<< endreq;
  }

    if(error) {
	log << MSG::ERROR 
	    << "Check of LArDM ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of LArDM ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}

StatusCode 	
TestCaloIds::testTileDM_Ids		(void) const
{
    MsgStream  log(m_msgSvc,name());

    // Use log1 with a different name to print out pointers - this
    // avoids including them in the test checks, since the pointer
    // values change from run-to-run
    MsgStream  log1(m_msgSvc,"TestCaloIds::testTileDM_Ids");

    log << MSG::INFO 
	<< " ======================================  " 
	<< endreq;
    log << MSG::INFO 
	<< " Testing Tile DM ids'   " 
	<< endreq;

    bool error = false;

    const CaloDM_ID* dmId = m_dmHelper;
    log1 << MSG::DEBUG
	<< "m_dmHelper= " << m_dmHelper
	<< endreq;

    if (!m_dmHelper) error = true;

  log << MSG::INFO
      << " tile_zone_hash_max = (ref: ATLAS=7424)" 
      << dmId->tile_zone_hash_max()
      << " tile_region_hash_max = (ref: ATLAS=18)" 
      << dmId->tile_region_hash_max()
      << endreq;
  
  // regions
  std::vector<Identifier>::const_iterator itId = dmId->tile_region_begin();
  std::vector<Identifier>::const_iterator itIdEnd = dmId->tile_region_end();
  
  int nReg=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier reg_id = *itId;
    if(dmId->is_tile_dm(reg_id)) {
      
      nReg++;
      IdentifierHash hashId = dmId->tile_region_hash(reg_id);
      Identifier reg_id2 = dmId->tile_region_id(hashId);
      if ( reg_id2 != reg_id ) {
	log << MSG::ERROR 
	    << "TILEDM: reg_id2 incorrect: " <<  dmId->show_to_string(reg_id2) 
	    << "         should be " <<  dmId->show_to_string(reg_id) 
	    << endreq;
	error = true;
      }
    }
  }

  if(nReg == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_tile_dm is always 'false'"
	<< endreq;
    error = true;
  } else {
    log << MSG::INFO
	<< " number of regions in the loop = (ref: ATLAS=18)" 
	<< nReg
	<< endreq;
  }

  // zones
  itId = dmId->tile_zone_begin();
  itIdEnd = dmId->tile_zone_end();
  
  int nZon=0;
  for(; itId!=itIdEnd;++itId){

    Identifier zo_id = *itId;
    if(dmId->is_tile_dm(zo_id)) {
      
      nZon++;
      IdentifierHash hashId = dmId->tile_zone_hash(zo_id);
      Identifier zo_id2 = dmId->tile_zone_id(hashId);
      if ( zo_id2 != zo_id ) {
	log << MSG::ERROR 
	    << " TILEDM: zo_id2 incorrect: " <<  dmId->show_to_string(zo_id2) 
	    << "         should be " <<  dmId->show_to_string(zo_id) 
	    << endreq;
	error = true;
      }
            
      int pn   = dmId->pos_neg_z(zo_id);
      int type = dmId->dmat(zo_id);
      int samp = dmId->sampling(zo_id);
      int reg  = dmId->region(zo_id);
      int eta  = dmId->eta(zo_id);
      int phi  = dmId->phi(zo_id);
      
      Identifier zo_id3 ;
      try {
	zo_id3 = dmId->zone_id (pn, type, samp, reg, eta, phi);
      }
      catch(CaloID_Exception & except){
	log << MSG::ERROR 
	    <<  " CaloId exception creating zo_id3  " 
	    << dmId->show_to_string(zo_id3) << "   "
	    << (std::string)except
	    << " pn, type, samp, reg, eta, phi= " << pn << " " << type << " " << samp << " " << reg << " " << eta << " " << phi << " "
	    << endreq;
      }
      if ( zo_id3 != zo_id ) {
	log << MSG::ERROR 
	    << "TILEDM: zo_id3 incorrect: " <<  dmId->show_to_string(zo_id3) 
	    << "         should be " <<  dmId->show_to_string(zo_id) 
	    << " input val of pn, type, samp, reg, eta, phi= " << pn << " " << type << " " << samp << " " << reg << " " << eta << " " << phi
	    << endreq;
	error = true;
      }
      
      Identifier regId ;
      Identifier zo_id4 ;
      try {
	regId = dmId->region_id (pn, type, samp, reg);
	zo_id4 = dmId->zone_id (regId, eta, phi);
      }
      catch(CaloID_Exception & except){
	log << MSG::ERROR 
	    <<  " CaloId exception creating regId or zo_id4 " 
	    << dmId->show_to_string(regId) << "   "<< dmId->show_to_string(zo_id4) << " "
	    << (std::string)except
	    << " pn, type, samp, reg, eta, phi= " << pn << " " << type << " " << samp << " " << reg << " " << eta << " " << phi << " "
	    << endreq;
      }
      if ( zo_id4 != zo_id3 ) {
	log << MSG::ERROR 
	    << "TILEDM: zo_id4 incorrect: " <<  dmId->show_to_string(zo_id4) 
	    << "          should be " <<  dmId->show_to_string(zo_id3) 
	    << "          regId     " <<  dmId->show_to_string(regId) 
	    << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
	    << endreq;
	error = true;
      }
    }
  }

  if(nZon == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_tile_dm is always 'false'"
	<< endreq;
    error = true;
  } else {
    log << MSG::INFO
	<< " number of zones in the loop = (ref: ATLAS=7424)" 
	<< nZon
	<< endreq;
  }

    if(error) {
	log << MSG::ERROR 
	    << "Check of TileDM ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of TileDM ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}

StatusCode 	
TestCaloIds::testCaloCell_Ids		(void) const
{
    MsgStream  log(m_msgSvc,name());

    // Use log1 with a different name to print out pointers - this
    // avoids including them in the test checks, since the pointer
    // values change from run-to-run
    MsgStream  log1(m_msgSvc,"TestCaloIds::testCaloCell_Ids");

    log << MSG::INFO 
	<< " ======================================  " 
	<< endreq;
    log << MSG::INFO 
	<< " Testing CaloCell ids'   " 
	<< endreq;

    bool error = false;

    const CaloCell_ID* cellId = m_cellHelper;
    log1 << MSG::DEBUG
	<< "m_cellHelper= " << m_cellHelper
	<< endreq;

    if (!m_cellHelper) error = true;

  log << MSG::INFO
      << " calo_cell_hash_max = (ref: ATLAS=187716, H8=8672, H6=6138)" 
      << cellId->calo_cell_hash_max()
      << " calo_region_hash_max = (ref: ATLAS=64, H8=12, H6=17)" 
      << cellId->calo_region_hash_max()
      << endreq;
  
  // regions
  std::vector<Identifier>::const_iterator itId = cellId->reg_begin();
  std::vector<Identifier>::const_iterator itIdEnd = cellId->reg_end();
  
  int nReg=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier reg_id = *itId;
    //    std::cout << cellId->show_to_string(reg_id) << std::endl;
    if(cellId->is_lar(reg_id) || cellId->is_tile(reg_id)) {
      
      nReg++;
      IdentifierHash hashId = cellId->calo_region_hash(reg_id);
      Identifier reg_id2 = cellId->region_id(hashId);
      if ( reg_id2 != reg_id ) {
	log << MSG::ERROR 
	    << "CALOCELL: reg_id2 incorrect: " <<  cellId->show_to_string(reg_id2) 
	    << "         should be " <<  cellId->show_to_string(reg_id) 
	    << endreq;
	error = true;
      }

	log << MSG::DEBUG
	    << "Calo region " << hashId << ": eta and phi granularities  = " 
	    << cellId->etaGranularity(reg_id) << "*" 
	    << cellId->phiGranularity(reg_id) << endreq;
    }
  }

  if(nReg == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_lar || is_tile is always 'false'"
	<< endreq;
    error = true;
  } else {
    log << MSG::INFO
	<< " number of regions in the loop = (ref: ATLAS=64, H8=12, H6=17 )" 
	<< nReg
	<< endreq;
  }

  // cells
  itId = cellId->cell_begin();
  itIdEnd = cellId->cell_end();
  
  int nChan=0;
  IdentifierHash min=0;
  IdentifierHash max=0;
  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;
    if(cellId->is_lar(ch_id) || cellId->is_tile(ch_id)) {
      
      nChan++;
      IdentifierHash hashId = cellId->calo_cell_hash(ch_id);
      Identifier ch_id2 = cellId->cell_id(hashId);
      if ( ch_id2 != ch_id ) {
	log << MSG::ERROR 
	    << " CALOCELL: ch_id2 incorrect: " <<  cellId->show_to_string(ch_id2) 
	    << "         should be " <<  cellId->show_to_string(ch_id) 
	    << endreq;
	error = true;
      }

      int subc   = cellId->sub_calo(ch_id);
      int subc2  = cellId->sub_calo(hashId);
      if ( subc != subc2 ) {
	  log << MSG::ERROR
	      << " subc(2) incorrect, subc, subc2 = " << subc << " " << subc2
	      << " ch_id = " <<  cellId->show_to_string(ch_id) 
	      << endreq;
      }

      IdentifierHash min2=0;
      IdentifierHash max2=0;
      cellId->calo_cell_hash_range(ch_id, min, max);
      cellId->calo_cell_hash_range(subc, min2, max2);
      if ( min != min2 || max != max2 ) {
	  log << MSG::ERROR
	      << "problem with min/max, subc, min, min2, max, max2 = " 
	      << subc << " " << min << " " << min2 << " " << max << " " << max2 << endreq;
	error = true;
      }

      if(cellId->is_em(ch_id)) {
	int subc3=-1;
	IdentifierHash emHash = cellId->subcalo_cell_hash(hashId, subc3) ;
	if(emHash < min || emHash >= max) {
	  log << MSG::ERROR
	      << " em range problem, min, max, emHash = " 
	      << min << " " << max << " " << emHash << endreq;
	  error = true;
	}
	if ( subc != subc3 ) {
	  log << MSG::ERROR
	      << " subc(3) incorrect, subc, subc3 = " << subc << " " << subc3
	      << " ch_id = " <<  cellId->show_to_string(ch_id) 
	      << endreq;
	}
	IdentifierHash caloCellHash = cellId->calo_cell_hash(CaloCell_ID::LAREM, emHash) ;
	if(hashId != caloCellHash) {
	  log << MSG::ERROR
	      << " em return trip check failed = " << caloCellHash 
	      << " " << emHash << endreq;
	  error = true;
	}
      }
      else if(cellId->is_hec(ch_id)) {
	int subc3=-1;
	IdentifierHash hecHash = cellId->subcalo_cell_hash(hashId, subc3) ;
	/*
	// range valid only in ATLAS
	if(173440 < min-hecHash || 173440 >= max-hecHash) {
	  std::cout << " hec range problem, min, max,hecHash = " << min << " " << max << " " << hecHash << std::endl;
	  error = true;
	}
	*/
	if ( subc != subc3 ) {
	  log << MSG::ERROR
	      << " subc(3) incorrect, subc, subc3 = " << subc << " " << subc3
	      << " ch_id = " <<  cellId->show_to_string(ch_id) 
	      << endreq;
	}
	IdentifierHash caloCellHash = cellId->calo_cell_hash(CaloCell_ID::LARHEC, hecHash) ;
	if(hashId != caloCellHash) {
	  log << MSG::ERROR
	      << " hec return trip check failed = " << caloCellHash
	      << " " << hecHash << endreq;
	  error = true;
	}
      }
      else if(cellId->is_fcal(ch_id)) {
	int subc3=-1;
	IdentifierHash fcalHash = cellId->subcalo_cell_hash(hashId, subc3) ;
	/*
	// range valid only in ATLAS
	if(179072 < min-fcalHash || 179072 >= max-fcalHash) {
	  std::cout << " fcal range problem, min, max, fcalHash = " << min << " " << max << " " << fcalHash << std::endl;
	  error = true;
	}
	*/
	if ( subc != subc3 ) {
	  log << MSG::ERROR
	      << " subc(3) incorrect, subc, subc3 = " << subc << " " << subc3
	      << " ch_id = " <<  cellId->show_to_string(ch_id) 
	      << endreq;
	}
	IdentifierHash caloCellHash = cellId->calo_cell_hash(CaloCell_ID::LARFCAL, fcalHash) ;
	if(hashId != caloCellHash) {
	  log << MSG::ERROR
	      << " fcal return trip check failed = " << caloCellHash
	      << " " << fcalHash << endreq;
	  error = true;
	}
	else if(cellId->is_tile(ch_id)) {
	  int subc3=-1;
	  IdentifierHash tileHash = cellId->subcalo_cell_hash(hashId, subc3) ;
	  if ( subc3 != subc ) {
	  log << MSG::ERROR
	      << "(tile)subc3 incorrect: " << subc3
	      << "         should be " <<  subc
	      << " hashId = " << hashId
	      << endreq;
	    error = true;
	  }
	/*
	// range valid only in ATLAS
	  if(182596 < min-tileHash || 182596 >= max-tileHash) {
	    std::cout << " tile range problem, min, max, tileHash = " << min << " " << max << " " << tileHash << std::endl;
	    error = true;
	  }
	*/
	  IdentifierHash caloCellHash = cellId->calo_cell_hash(CaloCell_ID::TILE, tileHash) ;
	  if(hashId != caloCellHash) {
	  log << MSG::ERROR
	      << " tile return trip check failed = " << caloCellHash
	      << " " << tileHash << endreq;
	    error = true;
	  }
	}
      }

      if(nChan == 1) {
	  log << MSG::INFO
	      << "Testing show_to_string with CaloCell_ID, ch_id= " 
	      << cellId->show_to_string(ch_id)  << endreq;
      }

      int pn   = 888888;
      int samp = 888888;
      int reg  = 888888;
      int eta  = 888888;
      int phi  = 888888;
      if(cellId->is_tile(ch_id)) {
	pn   = cellId->section(ch_id);
	samp = cellId->side(ch_id);
	reg  = cellId->module(ch_id);
	eta  = cellId->tower(ch_id);
	phi  = cellId->sample(ch_id);
      }
      else {
	pn   = cellId->pos_neg(ch_id);
	samp = cellId->sampling(ch_id);
	reg  = cellId->region(ch_id);
	eta  = cellId->eta(ch_id);
	phi  = cellId->phi(ch_id);
      }
      
      Identifier ch_id3 ;
      try {
	ch_id3 = cellId->cell_id (subc, pn, samp, reg, eta, phi);
      }
      catch(CaloID_Exception & except){
	error = true;
	log << MSG::ERROR 
	    <<  " CaloId exception 0" 
	    << (std::string)except
	    << endreq;
      }
      if ( ch_id3 != ch_id ) {
	log << MSG::ERROR 
	    << "CALOCELL: ch_id3 incorrect: " <<  cellId->show_to_string(ch_id3) 
	    << "         should be " <<  cellId->show_to_string(ch_id) 
	    << " input val of subc, pn, samp, reg, eta, phi= " << subc << " " << pn << " " << samp << " " << reg << " " << eta << " " << phi
	    << endreq;
	error = true;
      }
      
      Identifier regId ;
      try {
	regId = cellId->region_id (subc, pn, samp, reg);
      }
      catch(CaloID_Exception & except){
	error = true;
	log << MSG::ERROR 
	    <<  " CaloId exception 1" 
	    << (std::string)except
	    << endreq;
      }
      if(!cellId->is_tile(ch_id)) {           
	Identifier ch_id4 ;
	try {
	  ch_id4 = cellId->cell_id (regId, eta, phi);
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 2" 
	      << (std::string)except
	      << endreq;
	}
	if ( ch_id4 != ch_id3 ) {
	  log << MSG::ERROR 
	      << "CALOCELL: ch_id4 incorrect: " <<  cellId->show_to_string(ch_id4) 
	      << "          should be " <<  cellId->show_to_string(ch_id3) 
	      << "          regId     " <<  cellId->show_to_string(regId) 
	      << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
	      << endreq;
	  error = true;
	}
      }

      Identifier regId2 = cellId->region_id (ch_id);
      if ( regId2 != regId ) {
	log << MSG::ERROR 
	    << "CALOCELL: regId2 incorrect: " <<  cellId->show_to_string(regId2) 
	    << "         should be " <<  cellId->show_to_string(regId) 
	    << endreq;
	error = true;
      }
    }
  }

  //sub-calos
  for (int subCalo = 0; subCalo < CaloCell_ID::NSUBCALO; subCalo++) {   
    itId = cellId->cell_begin(subCalo);
    itIdEnd = cellId->cell_end(subCalo);
    int nSubCalo = 0;
    for(; itId!=itIdEnd;++itId){
      nSubCalo++;
    }
      cellId->calo_cell_hash_range(subCalo,min,max);
      if ( nSubCalo != (int)(max-min) ) {
	  log << MSG::ERROR
	      << "nSubCalo incorrect: " <<  nSubCalo
	      << "         should be max - min " <<  max << " - " << min
	      << endreq;
	  error = true;
      }
  }

  if(nChan == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_lar || is_tile is always 'false'"
	<< endreq;
    error = true;
  } else {
    log << MSG::INFO
	<< " number of cells in the loop = (ref: ATLAS= 187716, H8=8672, H6=6138)" 
	<< nChan
	<< endreq;
  }

    if(error) {
	log << MSG::ERROR 
	    << "Check of Calo Cell ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of Calo Cell ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}


StatusCode 	
TestCaloIds::testLvl1Ids		(void) const
{
    MsgStream  log(m_msgSvc,name());

    // Use log1 with a different name to print out pointers - this
    // avoids including them in the test checks, since the pointer
    // values change from run-to-run
    MsgStream  log1(m_msgSvc,"TestCaloIds::testLvl1Ids");

    log << MSG::INFO 
	<< " ======================================  " 
	<< endreq;
    log << MSG::INFO 
	<< " Testing lvl1 ids'   " 
	<< endreq;

    bool error = false;

    //const CaloLVL1_ID* caloId = m_lvl1Helper;
    log1 << MSG::DEBUG
	<<  "m_lvl1Helper= " << m_lvl1Helper
	<< endreq;

    log << MSG::INFO
	<< " tower_hash_max = (ref: 7168)" 
	<< m_lvl1Helper->tower_hash_max()
	<< " , layer_hash_max = (ref: 28160)" 
	<< m_lvl1Helper->layer_hash_max()
	<< endreq;
  


    //
    // ... in tower context
    //
    IdContext towerContext = m_lvl1Helper->tower_context();
    std::vector<Identifier>::const_iterator itId = m_lvl1Helper->tower_begin();
    std::vector<Identifier>::const_iterator itIdEnd = m_lvl1Helper->tower_end();
  
    int nTow=0;
    for(; itId!=itIdEnd;++itId){
      Identifier ttId = *itId ;
      if(m_lvl1Helper->is_lvl1_trig_towers(ttId)) {

	nTow++;

	int posNeg = m_lvl1Helper->pos_neg_z(ttId);
	int sampling = m_lvl1Helper->sampling(ttId);
	int region = m_lvl1Helper->region(ttId);
	int eta = m_lvl1Helper->eta(ttId);
	int phi = m_lvl1Helper->phi(ttId);
	
	Identifier ttId2;
	try {
	  ttId2 = m_lvl1Helper->tower_id (posNeg, sampling, region, eta, phi) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 2 " 
	      << (std::string)except
	      << endreq;
	}
	if(ttId2 != ttId) {
	  log << MSG::ERROR 
	      << "error ttId2 incorrect "
	      << " ttId2 " << m_lvl1Helper->show_to_string(ttId2) 
	      << " should be " << m_lvl1Helper->show_to_string(ttId) 
	      << " input values of posNeg, sampling, regiom, eta, phi = "
	      << posNeg << " "
	      << sampling << " "
	      << region << " "
	      << eta << " "
	      << phi
	      << endreq;
	  error = true;
	}
	
	Identifier regId;
	try {
	  regId = m_lvl1Helper->region_id (posNeg, sampling, region) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 3 " 
	      << (std::string)except
	      << endreq;
	}
	try {
	  ttId2 = m_lvl1Helper->tower_id (regId,eta,phi) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 4 " 
	      << (std::string)except
	      << endreq;
	}
	if(ttId2 != ttId) {
	  log << MSG::ERROR 
	      << "error ttId2 incorrect "
	      << " ttId2 " << m_lvl1Helper->show_to_string(ttId2) 
	      << " should be " << m_lvl1Helper->show_to_string(ttId) 
	      << " input values of posNeg, sampling, regiom, eta, phi = "
	      << posNeg << " "
	      << sampling << " "
	      << region << " "
	      << eta << " "
	      << phi
	      << endreq;
	  error = true;
	}
	//
	// ... now test 'return trip' to hash
	//
	IdentifierHash hashId;
	
	int not_ok = m_lvl1Helper->get_hash ( ttId, hashId, &towerContext);
	if(not_ok) {
	  log << MSG::ERROR 
	      << "error in get_hash"
	      << " ttId " << m_lvl1Helper->show_to_string(ttId) 
	      << endreq;
	  error = true;
	}
	IdentifierHash hashId2 = m_lvl1Helper->tower_hash(ttId);
	if ( hashId2 != hashId ) {
	  log << MSG::ERROR 
	      << " LVl1: hashI2 incorrect: " << hashId2
	      << "         should be " << hashId
	      << "           ttId = " <<  m_lvl1Helper->show_to_string(ttId) 
	      << endreq;
	  error = true;
	}
	
	Identifier ttId3;
	not_ok =  m_lvl1Helper->get_id ( hashId, ttId3, &towerContext);
	if(not_ok) {
	  log << MSG::ERROR 
	      << "error in get_id"
	      << " id " << m_lvl1Helper->show_to_string(ttId3) 
	      << " hash " << hashId 
	      << endreq;
	  error = true;
	}
	assert (ttId == ttId3);
	
	ttId3 =  m_lvl1Helper->tower_id(hashId);
	assert (ttId == ttId3);
      }
    }
    if(nTow == 0) {
      log << MSG::ERROR
	  <<  " Problem with initialization : is_lvl1_trig_towers is always 'false'"
	  << endreq;
      error = true;
    }
    else {
      log << MSG::INFO
	  <<  " nTow= " << nTow
	  << endreq;
    }
		
    //
    // ... in layer context
    //
    IdContext layerContext = m_lvl1Helper->layer_context();
    itId = m_lvl1Helper->layer_begin();
    itIdEnd = m_lvl1Helper->layer_end();
    int nLay=0;
    for(; itId!=itIdEnd;++itId){

      Identifier ExtTtId = *itId ;
      if(m_lvl1Helper->is_lvl1_trig_towers(ExtTtId)) {

	nLay++;

	int posNeg = m_lvl1Helper->pos_neg_z(ExtTtId);
	int sampling = m_lvl1Helper->sampling(ExtTtId);
	int region = m_lvl1Helper->region(ExtTtId);
	int eta = m_lvl1Helper->eta(ExtTtId);
	int phi = m_lvl1Helper->phi(ExtTtId);
	int lay = m_lvl1Helper->layer(ExtTtId);
	
	Identifier ExtTtId2;
	try {
	  ExtTtId2 = m_lvl1Helper->layer_id (posNeg, sampling, region, eta, phi, lay) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 5 " 
	      << (std::string)except
	      << endreq;
	}
	
	Identifier ttId;
	try {
	  ttId = m_lvl1Helper->tower_id (posNeg, sampling, region, eta, phi) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 6 " 
	      << (std::string)except
	      << endreq;
	}
	
	try {
	  ExtTtId2 = m_lvl1Helper->layer_id (ttId, lay) ;
	}
	catch(CaloID_Exception & except){
	  error = true;
	  log << MSG::ERROR 
	      <<  " CaloId exception 7 " 
	      << (std::string)except
	      << endreq;
	}
	assert ( ExtTtId2 == ExtTtId);
	
	Identifier ttId2 = m_lvl1Helper->tower_id (ExtTtId) ;
	if (ttId2 != ttId) {
	  log << MSG::ERROR 
	      << "error in tower_id(layer_id)"
	      << " ttId  " << m_lvl1Helper->show_to_string(ttId) 
	      << " ttId2 " << m_lvl1Helper->show_to_string(ttId2) 
	      << endreq;
	  error = true;
	}
	
	//
	// ... then return trip to hash in layer context
	//
	IdentifierHash hashId;
	int not_ok = m_lvl1Helper->get_hash ( ExtTtId, hashId, &layerContext);
	if(not_ok) {
	  log << MSG::ERROR 
	      << "error in get_hash"
	      << " ExtTtId " << m_lvl1Helper->show_to_string(ExtTtId) 
	      << " hashId " << hashId
	      << endreq;
	  error = true;
	}
	
	not_ok =  m_lvl1Helper->get_id ( hashId, ExtTtId2, &layerContext);
	if(not_ok) {
	  log << MSG::ERROR 
	      << "error in get_id"
	      << " id " << m_lvl1Helper->show_to_string(ExtTtId2) 
	      << " hash " << MSG::hex << hashId << MSG::dec
	      << endreq;
	  error = true;
	}
	
	assert (ExtTtId == ExtTtId2);
      }
    }


   if(nLay == 0) {
    log << MSG::ERROR
	<<  " Problem with initialization : is_lvl1_trig_towers is always 'false'"
	<< endreq;
    error = true;
  }
    else {
      log << MSG::INFO
	  <<  " nLay= " << nLay
	  << endreq;
    }


    Identifier towerId = m_lvl1Helper->tower_id(1, 0, 0, 0, 0);
    
    log << MSG::INFO 
	<< " Print out of id " << m_lvl1Helper->show_to_string(towerId)
	<< endreq;

    
    if(error) {
	log << MSG::ERROR 
	    << "Check of lvl1 ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of lvl1 ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}

StatusCode 	
TestCaloIds::testLvl1Neighbors	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " ======================================  " 
	<< endreq;
    log << MSG::INFO 
	<< " Testing lvl1 neighbor ids " 
	<< endreq;

    //    const CaloLVL1_ID* calo_id  = m_caloTTMgr->get_lvl1_id();
    const CaloLVL1_ID* calo_id = m_lvl1Helper;
    
    // Save context for calo module id
    IdContext	idContext = calo_id->tower_context();    

    // Check neighbors
    std::vector<Identifier>::const_iterator first1=calo_id->tower_begin();
    std::vector<Identifier>::const_iterator last1 =calo_id->tower_end();

    bool error = false;
    int ntest = 0;
    for(; first1 != last1; ++first1) {
	bool prevPhiFlag     = false;
	bool nextPhiFlag     = false;
	bool prevEtaFlag     = false;
	bool nextEtaFlag     = false;
	
	IdentifierHash id;
	if (!calo_id->get_hash((*first1),id,&idContext)) {
	  // CaloLVL1s

	  IdentifierHash prevPhi;
	  if (!calo_id->get_prev_in_phi(id, prevPhi)) {
	    // Check that previous phi is the correct one
	    Identifier id1;
	    if (!calo_id->get_id(id, id1, &idContext)){
	      Identifier prevId;
	      if (!calo_id->get_id(prevPhi, prevId, &idContext)){
		int phi    = calo_id->phi(id1);
		int prePhi = calo_id->phi(prevId);
		if (phi != prePhi + 1) {
		  if (prePhi != calo_id->phi_max(id1)) {
		    log << MSG::ERROR 
			<< " CaloLVL1 previous incorrect: phi " 
			<< phi
			<< " prev phi " << prePhi
			<< " id " << calo_id->show_to_string(id1) 
			<< " prev id " << calo_id->show_to_string(prevId) 
			<< " phi max " << calo_id->phi_max(id1)
			<< endreq;
		    error = true;
		  }
		  else {
		    prevPhiFlag = true;
		  }
		}
		else {
		  prevPhiFlag = true;
		}
	      }
	    }
	  }
	  else {
	    // should never get here - always a previous phi
	    log << MSG::ERROR 
		<< " CaloLVL1 previous not found " 
		<< endreq;
	    error = true;
	  }
	}   else {
	  log << MSG::ERROR 
	      << " could not get hash, id "
	      << calo_id->show_to_string(*first1)
	      << endreq;
	  error = true;
	}
	
	IdentifierHash nextPhi;
	if (!calo_id->get_next_in_phi(id, nextPhi)) {
	  // check that next phi is the correct one
	  Identifier id1;
	  if (!calo_id->get_id(id, id1, &idContext)){
	    Identifier nextId;
	    if (!calo_id->get_id(nextPhi, nextId, &idContext)){
	      int phi    = calo_id->phi(id1);
	      int nextPhi = calo_id->phi(nextId);
	      if (phi != nextPhi - 1) {
		if (nextPhi != 0) {
		  log << MSG::ERROR 
		      << " CaloLVL1 next incorrect: phi " 
		      << phi
		      << " next phi " << nextPhi
		      << " id " << calo_id->show_to_string(id1) 
		      << " next id " << calo_id->show_to_string(nextId) 
		      << endreq;
		  error = true;
		}
		else {
		  nextPhiFlag = true;
		}
	      }
	      else {
		nextPhiFlag = true;
	      }
	    }
	  }
	}
	else {
	  // should never get here - always a next phi
	  log << MSG::ERROR 
	      << " CaloLVL1 next not found " 
	      << endreq;
	  error = true;
	}
	

	IdentifierHash prevEta;
	if (!calo_id->get_prev_in_eta(id, prevEta)) {
	  // check that previous eta is the correct one
	  Identifier id1;   //primary id
	  if (!calo_id->get_id(id, id1, &idContext)){
	    Identifier prevId; // secondary id
	    if (!calo_id->get_id(prevEta, prevId, &idContext)){
	      int eta    = calo_id->eta(id1);
	      int preEta = calo_id->eta(prevId);
	      if (eta != preEta + 1) {
		log << MSG::ERROR 
		    << " CaloLVL1 previous incorrect: eta " 
		    << eta
		    << " prev eta " << preEta
		    << " id " << calo_id->show_to_string(id1) 
		    << " prev id " << calo_id->show_to_string(prevId) 
		    << endreq;
		error = true;
	      }
	      else {
		prevEtaFlag = true;
	      }
	    }
	  }
	}
	else {
	  // could not get previous eta - must be first one
	  Identifier id1;
	  if (!calo_id->get_id(id, id1, &idContext)){
	    int eta    = calo_id->eta(id1);
	    if (eta != calo_id->eta_min(id1)) {
	      log << MSG::ERROR 
		  << " CaloLVL1 previous incorrect: eta " 
		  << eta
		  << " min eta " << calo_id->eta_min(id1)
		  << " id " << calo_id->show_to_string(id1) 
		  << endreq;
	      error = true;
	    }
	    else {
	      prevEtaFlag = true;
	    }
	  }
	}
	
	IdentifierHash nextEta;
	if (!calo_id->get_next_in_eta(id, nextEta)) {
	  // check that next eta is the correct one
	  Identifier id1;
	  if (!calo_id->get_id(id, id1, &idContext)){
	    Identifier nextId;
	    if (!calo_id->get_id(nextEta, nextId, &idContext)){
	      int eta    = calo_id->eta(id1);
	      int nextEta = calo_id->eta(nextId);
	      if (eta != nextEta - 1 ) {
		log << MSG::ERROR 
		    << " CaloLVL1 next incorrect: eta " 
		    << eta
		    << " next eta " << nextEta
		    << " id " << calo_id->show_to_string(id1) 
		    << " next id " << calo_id->show_to_string(nextId) 
		    << endreq;
		error = true;
	      }
	      else {
		nextEtaFlag = true;
	      }
	    }
	  }
	}
	else {
	  // could not get next eta - must be last one
	  Identifier id1;
	  if (!calo_id->get_id(id, id1, &idContext)){
	    int eta    = calo_id->eta(id1);
	    if (eta != calo_id->eta_max(id1)) {
	      log << MSG::ERROR 
		  << " CaloLVL1 next incorrect: eta " 
		  << eta
		  << " max eta " << calo_id->eta_max(id1)
		  << " id " << calo_id->show_to_string(id1) 
		  << endreq;
	      error = true;
	    }
	    else {
	      nextEtaFlag = true;
	    }
	  }
	}

	if (prevPhiFlag && nextPhiFlag && prevEtaFlag && nextEtaFlag ) {
	  ntest++;
	}
	else {
	  Identifier id1;
	  calo_id->get_id(id, id1, &idContext);
	  log << MSG::ERROR
	      << " CaloLVL1 neighbor test: flags: prevPhi, nextPhi, prevEta, nextEta " 
	      << prevPhiFlag << " " << nextPhiFlag << " " 
	      << prevEtaFlag << " " << nextEtaFlag 
	      << " id " << calo_id->show_to_string(id1) 
	      << endreq;
	}
    }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of lvl1 neighbors FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of neighbors for " << ntest << " lvl1 ids OK " 
	    << endreq;
    }


    log << MSG::INFO 
	<< " Successful end of lvl1 neighbor test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}


