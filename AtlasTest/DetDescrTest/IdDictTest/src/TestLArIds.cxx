/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/
// AL Notes:
// TestLArIds::TestLArIds()
// string& TestLArIds::name()
// StatusCode TestLArIds::testEmIds()
// StatusCode TestLArIds::testEmNeighbors()
// StatusCode TestLArIds::testHecIds()
// StatusCode TestLArIds::testHecNeighbors()
// StatusCode TestLArIds::testFcalIds()
// StatusCode TestLArIds::testFcalNeighbors()
// StatusCode TestLArIds::testLaronlineIds()
// 
//<<<<<< INCLUDES                                                       >>>>>>

#include "TestLArIds.h"

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "LArIdentifier/LArOnlineID.h"

// Athena-related includes
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"

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

TestLArIds::TestLArIds()
    :
    m_name("IdDictTest_LAr"),
    m_idDictMgr(0), 
    m_emHelper(0),
    m_hecHelper(0),
    m_fcalHelper(0),
    m_miniFcalHelper(0),
    m_laronlineHelper(0),
    m_msgSvc(0),
    m_chronoStSvc(0)
{
}

TestLArIds::TestLArIds(const IdDictManager* idDictMgr, 
                       const LArEM_ID* emHelper,
                       const LArHEC_ID* hecHelper,
                       const LArFCAL_ID* fcalHelper,
                       const LArMiniFCAL_ID* miniFcalHelper,
                       const LArOnlineID* laronlineHelper,
                       IMessageSvc*     msgSvc,
                       IChronoStatSvc*  chronoStSvc)
    :
    m_name("IdDictTest_LAr"),
    m_idDictMgr(idDictMgr), 
    m_emHelper(emHelper),
    m_hecHelper(hecHelper),
    m_fcalHelper(fcalHelper),
    m_miniFcalHelper(miniFcalHelper),
    m_laronlineHelper(laronlineHelper),
    m_msgSvc(msgSvc),
    m_chronoStSvc(chronoStSvc)
{}

const std::string& TestLArIds::name (void) const
{
    return m_name;
}



StatusCode TestLArIds::testEmIds(void) const
/*========================================*/
{
  MsgStream  log(m_msgSvc,name());  
  log << MSG::INFO 
      << " ======================================  " 
      << endreq;
  log << MSG::INFO 
      << " Testing Em Barrel ids'   " 
      << endreq;

  bool error = false;  
  
  log << MSG::INFO
      << " channel_hash_max = (ref: ATLAS=173312, H8=3424, H6=112)" 
      << m_emHelper->channel_hash_max()
      << " region_hash_max = (ref: ATLAS=36, H8=6, H6=2)" 
      << m_emHelper->region_hash_max()
      << endreq;
  
  log << MSG::INFO
      << " disc_channel_hash_max = (ref: ATLAS=178688, H8=3456, H6=128)" 
      << m_emHelper->disc_channel_hash_max()
      << endreq;
  
/* barrel_ec        +/-1             positive/negative barrel - A/C side or P/M half barrel
*  "                +/-2             positive/negative endcap outer wheel - A/C side 
*  "                +/-3             positive/negative endcap inner wheel - A/C side
*/

  // Check is_barrel, is_endcap...
  // WARNING these tests are misleading in case of TB dictionaries
  Identifier  new_id ;
  try {
    new_id =  m_emHelper->channel_id (1, 0, 0, 0, 0);
    if(!m_emHelper->is_em_barrel(new_id)) {
      log << MSG::ERROR 
          << " testEMIds new_id IS barrel " 
          << m_emHelper->show_to_string(new_id)
          << endreq;
      error = true;
    }
  }
  catch(LArID_Exception & except){
    log << MSG::ERROR 
        <<  " LArId exception creating new_id  " 
        << m_emHelper->show_to_string(new_id) << "   "
        << (std::string)except
        << " not a problem if H6 testbeam dict"
        << endreq;
  }
  
  try {
    new_id =  m_emHelper->channel_id (1, 0, 0, 0, 0);
    if(m_emHelper->is_em_endcap(new_id)) {
      log << MSG::ERROR 
          << " testEMIds new_id is NOT endcap " 
          << m_emHelper->show_to_string(new_id)
          << endreq;
      error = true;
    }
  }
  catch(LArID_Exception & except){
    log << MSG::ERROR 
        <<  " LArId exception creating new_id2  " 
        << m_emHelper->show_to_string(new_id) << "   "
        << (std::string)except
        << " not a problem if H6 testbeam dict"
        << endreq;
  }
  
  try {
    new_id =  m_emHelper->channel_id (3, 1, 0, 0, 0);
    if(!m_emHelper->is_em_endcap_inner(new_id)) {
      log << MSG::ERROR 
          << " testEMIds new_id IS endcap inner" 
          << m_emHelper->show_to_string(new_id)
          << endreq;
      error = true;
    }
  }
  catch(LArID_Exception & except){
    log << MSG::ERROR 
        <<  " LArId exception creating new_id3  " 
        << m_emHelper->show_to_string(new_id) << "   "
        << (std::string)except
        << " not a problem if H8 testbeam dict"
        << endreq;
  }


  // regions
  IdContext regionContext = m_emHelper->region_context();
  std::vector<Identifier>::const_iterator itId = m_emHelper->reg_begin();
  std::vector<Identifier>::const_iterator itIdEnd = m_emHelper->reg_end();
  
  int nChan=0;  
  for(; itId!=itIdEnd;++itId){

    Identifier reg_id = *itId;
    if(m_emHelper->is_lar_em(reg_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_emHelper->get_hash ( reg_id, hashId, &regionContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "EM: error in get_hash, reg_id = "
            << m_emHelper->show_to_string(reg_id) 
            << endreq;
        error = true;
      }

      log << MSG::DEBUG << "EM region " << hashId << ": eta and phi granularities  = " 
          << m_emHelper->etaGranularity(hashId) << "*" 
          << m_emHelper->phiGranularity(hashId) << endreq; 

      IdentifierHash hashId2 = m_emHelper->region_hash(reg_id);
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << "EM: hashId2 incorrect: " << hashId2
            << "         should be " << hashId
            << "         reg_id = " << m_emHelper->show_to_string(reg_id) 
            << endreq;
        error = true;
      }
      
      Identifier reg_id2;
      not_ok =  m_emHelper->get_id ( hashId, reg_id2, &regionContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "EM: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( reg_id2 != reg_id ) {
        log << MSG::ERROR 
            << "EM: reg_id2 incorrect: " <<  m_emHelper->show_to_string(reg_id2) 
            << "         should be " <<  m_emHelper->show_to_string(reg_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
      reg_id2 = m_emHelper->region_id(hashId);
      if ( reg_id2 != reg_id ) {
        log << MSG::ERROR 
            << "EM: reg_id2 incorrect: " <<  m_emHelper->show_to_string(reg_id2) 
            << "         should be " <<  m_emHelper->show_to_string(reg_id) 
            << endreq;
        error = true;
      }
      
    }
  }


  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_em is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of regions in the loop = (ref: ATLAS=36, H8=6, H6=2)" 
        << nChan
        << endreq;
  }

  // channels
  IdContext channelContext = m_emHelper->channel_context();
  itId = m_emHelper->em_begin();
  itIdEnd = m_emHelper->em_end();
  
  nChan=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;
    if(m_emHelper->is_lar_em(ch_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_emHelper->get_hash ( ch_id, hashId, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "EM: error in get_hash, ch_id = "
            << m_emHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      IdentifierHash hashId2 = m_emHelper->channel_hash(ch_id);
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << " EM: hashI2 incorrect: " << hashId2
            << "         should be " << hashId
            << "           ch_id = " <<  m_emHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      
      Identifier ch_id2;
      not_ok =  m_emHelper->get_id ( hashId, ch_id2, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "EM: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << " EM: ch_id2 incorrect: " <<  m_emHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_emHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }

      ch_id2 = m_emHelper->channel_id(hashId);
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << " EM: ch_id2 incorrect: " <<  m_emHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_emHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      
      int bec  = m_emHelper->barrel_ec(ch_id);
      int samp = m_emHelper->sampling(ch_id);
      int reg  = m_emHelper->region(ch_id);
      int eta  = m_emHelper->eta(ch_id);
      int phi  = m_emHelper->phi(ch_id);
      
      Identifier chanId ;
      try {
        chanId = m_emHelper->channel_id (bec, samp, reg, eta, phi);
      }
      catch(LArID_Exception & except){
        error = true;
        log << MSG::ERROR 
            <<  " LArId exception " 
            << (std::string)except
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "EM: chanId incorrect: " <<  m_emHelper->show_to_string(chanId) 
            << "         should be " <<  m_emHelper->show_to_string(ch_id) 
            << " input val of bec, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier regId;
      Identifier chanId2;
      try {
        regId = m_emHelper->region_id (bec, samp, reg);
        chanId2 = m_emHelper->channel_id (regId, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating regId or chanId2 " 
            << m_emHelper->show_to_string(regId) << "   "<< m_emHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " bec, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "EM: chanId2 incorrect: " <<  m_emHelper->show_to_string(chanId2) 
            << "          should be " <<  m_emHelper->show_to_string(chanId) 
            << "          regId     " <<  m_emHelper->show_to_string(regId) 
            << " input val of bec, samp, reg, eta, phi= " << bec << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_em is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of channels in the loop = (ref: ATLAS=173312, H8=3424, H6=112)" 
        << nChan
        << endreq;
  }

  // disconnected channels
  itId = m_emHelper->disc_em_begin();
  itIdEnd = m_emHelper->disc_em_end();
  
  nChan=0;

  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;

    if(!m_emHelper->is_connected(ch_id)) {
      
      nChan++;
      IdentifierHash hashId = m_emHelper->disc_channel_hash(ch_id);
      
      Identifier ch_id2 = m_emHelper->disc_channel_id(hashId);
      //      std::cout << "ch_id, hashId, ch_id2= " << m_emHelper->show_to_string(ch_id) 
      //        << ", " << hashId << ", " << m_emHelper->show_to_string(ch_id2) << std::endl;

      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "EM: ch_id2 incorrect: " <<  m_emHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_emHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
            
      if(nChan == 1) {
          log << MSG::INFO << "Testing show_to_string with LArEM_ID, ch_id= " 
              << m_emHelper->show_to_string(ch_id)  << endreq; 
      }
      int pn   = m_emHelper->barrel_ec(ch_id);
      int samp = m_emHelper->sampling(ch_id);
      int reg  = m_emHelper->region(ch_id);
      int eta  = m_emHelper->disc_eta(ch_id);
      int phi  = m_emHelper->disc_phi(ch_id);
      
      Identifier chanId;
      try {
        chanId = m_emHelper->disc_channel_id (pn, samp, reg, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating chanId  " 
            << m_emHelper->show_to_string(chanId) << "   "
            << (std::string)except
            << " pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi << " "
            << endreq;
      }
      //      std::cout << "chanId = " << m_emHelper->show_to_string(chanId) << std::endl;
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "EM: chanId incorrect: " <<  m_emHelper->show_to_string(chanId) 
            << "         should be " <<  m_emHelper->show_to_string(ch_id) 
            << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier regId ;
      Identifier chanId2 ;
      try {
        regId = m_emHelper->disc_region_id (pn, samp, reg);
        //      std::cout << "regId = " << m_emHelper->show_to_string(regId) << std::endl;
        chanId2 = m_emHelper->channel_id (regId, eta, phi);
        //      std::cout << "chanId2 = " << m_emHelper->show_to_string(chanId2) << std::endl;
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating regId or chanId2 " 
            << m_emHelper->show_to_string(regId) << "   "<< m_emHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "EM: chanId2 incorrect: " <<  m_emHelper->show_to_string(chanId2) 
            << "          should be " <<  m_emHelper->show_to_string(chanId) 
            << "          regId     " <<  m_emHelper->show_to_string(regId) 
            << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
    }
      //    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_connected is always 'true' for EM channels"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of disconnected channels in the loop = (ref: ATLAS=5376, H8=32, H6=16)" 
        << nChan
        << endreq;
  }

  if(error) {
    log << MSG::ERROR 
        << "Check of em ids FAILED " 
        << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO 
        << "Check of em ids OK " 
        << endreq;
    
  }
  return StatusCode::SUCCESS;
}



StatusCode      
TestLArIds::testEmNeighbors     (void) const
{
  MsgStream  log(m_msgSvc,name());

  log << MSG::INFO 
      << " Testing Em neighbours " 
      << endreq;

  std::vector<IdentifierHash> neighbourList;
  //
  // ... loop on cells hash
  //
  for (unsigned int iCell=0; iCell<m_emHelper->channel_hash_max(); iCell++) {
    m_emHelper->get_neighbours(iCell, LArNeighbours::all3D, neighbourList);
    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    std::set<IdentifierHash> neighbSet ;
    // loop on neighbours found
    Identifier id=m_emHelper->channel_id(iCell);
    for (;last!=first; first++){
      // check that there is no duplicate cell
      if(!(neighbSet.insert(*first)).second) {
        log << MSG::ERROR 
            << "Check of em neighbours FAILED: tried to insert duplicate neighbours in cell " 
            << iCell << " id " << m_emHelper->show_to_string(id)
            << endreq;
        return StatusCode::FAILURE;
      }
    }
  }
    
  log << MSG::INFO 
      << "Check of EM neighbours OK " 
      << endreq;
  return StatusCode::SUCCESS;
}




StatusCode TestLArIds::testHecIds(void) const
/* ========================================== */
{
  MsgStream  log(m_msgSvc,name());
  
  log << MSG::INFO 
      << " ======================================  " 
      << endreq;
  log << MSG::INFO 
      << " Testing HEC ids'   " 
      << endreq;
  
  bool error = false;
  
  log << MSG::INFO
      << " channel_hash_max = (ref: ATLAS=5632, H8=0, H6=464)" 
      << m_hecHelper->channel_hash_max()
      << " region_hash_max = (ref: ATLAS=16, H8=0, H6=7)" 
      << m_hecHelper->region_hash_max()
      << endreq;
  
  log << MSG::INFO
      << " disc_channel_hash_max = (ref: ATLAS=6144, H8=0, H6=512)" 
      << m_hecHelper->disc_channel_hash_max()
      << endreq;
  
  // regions
  IdContext regionContext = m_hecHelper->region_context();
  std::vector<Identifier>::const_iterator itId = m_hecHelper->reg_begin();
  std::vector<Identifier>::const_iterator itIdEnd = m_hecHelper->reg_end();
  
  int nChan=0;  
  for(; itId!=itIdEnd;++itId){

    Identifier reg_id = *itId;
    if(m_hecHelper->is_lar_hec(reg_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_hecHelper->get_hash ( reg_id, hashId, &regionContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "HEC: error in get_hash, reg_id = "
            << m_hecHelper->show_to_string(reg_id) 
            << endreq;
        error = true;
      }
      log << MSG::DEBUG  << "HEC region " << hashId << ": eta and phi granularities = " 
          << m_hecHelper->etaGranularity(hashId) << "*" 
          << m_hecHelper->phiGranularity(hashId) << endreq;
      
      IdentifierHash hashId2 = m_hecHelper->region_hash(reg_id);
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << "HEC: hashId2 incorrect: " << hashId2
            << "         should be " << hashId
            << "         reg_id = " << m_hecHelper->show_to_string(reg_id) 
            << endreq;
        error = true;
      }

      Identifier reg_id2;
      not_ok =  m_hecHelper->get_id ( hashId, reg_id2, &regionContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "HEC: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( reg_id2 != reg_id ) {
        log << MSG::ERROR 
            << " HEC: reg_id2 incorrect: " <<  m_hecHelper->show_to_string(reg_id2) 
            << "         should be " <<  m_hecHelper->show_to_string(reg_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }

      reg_id2 = m_hecHelper->region_id(hashId);
      if ( reg_id2 != reg_id ) {
        log << MSG::ERROR 
            << "HEC: reg_id2 incorrect: " <<  m_hecHelper->show_to_string(reg_id2) 
            << "         should be " <<  m_hecHelper->show_to_string(reg_id) 
            << endreq;
        error = true;
      }
      
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_hec is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of regions in the loop = (ref: ATLAS=16, H8=0, H6=7 )" 
        << nChan
        << endreq;
  }

  // channels
  IdContext channelContext = m_hecHelper->channel_context();
  itId = m_hecHelper->hec_begin();
  itIdEnd = m_hecHelper->hec_end();
  
  nChan=0;


  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;

    if(m_hecHelper->is_lar_hec(ch_id)) {
      if(m_hecHelper->is_connected(ch_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_hecHelper->get_hash ( ch_id, hashId, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "HEC: error in get_hash, ch_id = "
            << m_hecHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      IdentifierHash hashId2 = m_hecHelper->channel_hash(ch_id);
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << " HEC: hashI2 incorrect: " << hashId2
            << "         should be " << hashId
            << "           ch_id = " <<  m_hecHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      
      Identifier ch_id2;
      not_ok =  m_hecHelper->get_id ( hashId, ch_id2, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "HEC: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "HEC: ch_id2 incorrect: " <<  m_hecHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_hecHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
      ch_id2 = m_hecHelper->channel_id(hashId);
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << " HEC: ch_id2 incorrect: " <<  m_hecHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_hecHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
            
      if(nChan == 1) {
          log << MSG::INFO << "Testing show_to_string with LArHEC_ID, ch_id= " 
              << m_hecHelper->show_to_string(ch_id)  << endreq; 
      }

      int pn   = m_hecHelper->pos_neg(ch_id);
      int samp = m_hecHelper->sampling(ch_id);
      int reg  = m_hecHelper->region(ch_id);
      int eta  = m_hecHelper->eta(ch_id);
      int phi  = m_hecHelper->phi(ch_id);
      
      Identifier chanId;
      try {
        chanId = m_hecHelper->channel_id (pn, samp, reg, eta, phi);
      }
      catch(LArID_Exception & except){
        error = true;
        log << MSG::ERROR 
            <<  " LArId exception " 
            << (std::string)except
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "HEC: chanId incorrect: " <<  m_hecHelper->show_to_string(chanId) 
            << "         should be " <<  m_hecHelper->show_to_string(ch_id) 
            << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier regId;
      Identifier chanId2;
      try {
        regId = m_hecHelper->region_id (pn, samp, reg);
        chanId2 = m_hecHelper->channel_id (regId, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating regId or chanId2 " 
            << m_hecHelper->show_to_string(regId) << "   "<< m_hecHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "HEC: chanId2 incorrect: " <<  m_hecHelper->show_to_string(chanId2) 
            << "          should be " <<  m_hecHelper->show_to_string(chanId) 
            << "          regId     " <<  m_hecHelper->show_to_string(regId) 
            << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      }
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_hec is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of channels in the loop = (ref: ATLAS=5632, H8=0, H6=464)" 
        << nChan
        << endreq;
  }

  // disconnected channels
  itId = m_hecHelper->disc_hec_begin();
  itIdEnd = m_hecHelper->disc_hec_end();
  
  nChan=0;

  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;

    if(!m_hecHelper->is_connected(ch_id)) {
      
      nChan++;
      IdentifierHash hashId = m_hecHelper->disc_channel_hash(ch_id);
      
      Identifier ch_id2 = m_hecHelper->disc_channel_id(hashId);
      //      std::cout << "ch_id, hashId, ch_id2= " << m_hecHelper->show_to_string(ch_id) 
      //        << ", " << hashId << ", " << m_hecHelper->show_to_string(ch_id2) << std::endl;

      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "HEC: ch_id2 incorrect: " <<  m_hecHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_hecHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
            
      int pn  = m_hecHelper->pos_neg(ch_id);
      int samp = m_hecHelper->sampling(ch_id);
      int reg  = m_hecHelper->region(ch_id);
      int eta  = m_hecHelper->disc_eta(ch_id);
      int phi  = m_hecHelper->disc_phi(ch_id);
      
      Identifier chanId;
      try {
        chanId = m_hecHelper->disc_channel_id (pn, samp, reg, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating chanId  " 
            << m_hecHelper->show_to_string(chanId) << "   "
            << (std::string)except
            << " pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "HEC: chanId incorrect: " <<  m_hecHelper->show_to_string(chanId) 
            << "         should be " <<  m_hecHelper->show_to_string(ch_id) 
            << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier regId;
      Identifier chanId2;
      try {
        regId = m_hecHelper->disc_region_id (pn, samp, reg);
        chanId2 = m_hecHelper->channel_id (regId, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating regId or chanId2 " 
            << m_hecHelper->show_to_string(regId) << "   "<< m_hecHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "HEC: chanId2 incorrect: " <<  m_hecHelper->show_to_string(chanId2) 
            << "          should be " <<  m_hecHelper->show_to_string(chanId) 
            << "          regId     " <<  m_hecHelper->show_to_string(regId) 
            << " input val of pn, samp, reg, eta, phi= " << pn << " " << samp << " " << reg << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
    }
      //    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_connected is always 'true'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of disconnected channels in the loop = (ref: ATLAS=512, H8=0, H6=48)" 
        << nChan
        << endreq;
  }

  if(error) {
    log << MSG::ERROR 
        << "Check of HEC ids FAILED " 
        << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO 
        << "Check of HEC ids OK " 
        << endreq;
    
  }
  return StatusCode::SUCCESS;
}

StatusCode TestLArIds::testHecNeighbors (void) const
/*==================================================*/
{
  MsgStream  log(m_msgSvc,name());

  log << MSG::INFO 
      << " Testing Hec neighbours " 
      << endreq;

  std::vector<IdentifierHash> neighbourList;
  //
  // ... loop on cells hash
  //
  for (unsigned int iCell=0; iCell<m_hecHelper->channel_hash_max(); iCell++) {
    m_hecHelper->get_neighbours(iCell, LArNeighbours::all3D, neighbourList);
    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    std::set<IdentifierHash> neighbSet ;
    // loop on neighbours found
    Identifier id=m_hecHelper->channel_id(iCell);
    for (;last!=first; first++){
      // check that there is no duplicate cell
      if(!(neighbSet.insert(*first)).second) {
        log << MSG::ERROR 
            << "Check of hec neighbours FAILED: tried to insert duplicate neighbours in cell " 
            << iCell << " id " << m_hecHelper->show_to_string(id)
            << endreq;
        return StatusCode::FAILURE;
      }
    }
  }
    
  log << MSG::INFO 
      << "Check of HEC neighbours OK " 
      << endreq;
  return StatusCode::SUCCESS;
}





StatusCode TestLArIds::testFcalIds(void) const
/*==========================================*/
{
  MsgStream  log(m_msgSvc,name());

  log << MSG::INFO 
      << " ======================================  " 
      << endreq;
  log << MSG::INFO 
      << " Testing FCAL ids'   " 
      << endreq;

  bool error = false;
  log << MSG::INFO
      << " channel_hash_max = (ref: ATLAS=3524, H8=0, H6=314)" 
      << m_fcalHelper->channel_hash_max()
      << " module_hash_max = (ref: ATLAS=6, H8=0, H6=2)" 
      << m_fcalHelper->module_hash_max()
      << endreq;
  log << MSG::INFO
      << " disc_channel_hash_max = (ref: ATLAS=3584, H8=0, H6=384)" 
      << m_fcalHelper->disc_channel_hash_max()
      << endreq;
  
  // modules
  IdContext moduleContext = m_fcalHelper->module_context();
  std::vector<Identifier>::const_iterator itId = m_fcalHelper->mod_begin();
  std::vector<Identifier>::const_iterator itIdEnd = m_fcalHelper->mod_end();
  
  int nChan=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier mod_id = *itId;
    if(m_fcalHelper->is_lar_fcal(mod_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_fcalHelper->get_hash ( mod_id, hashId, &moduleContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "FCAL: error in get_hash, mod_id = "
            << m_fcalHelper->show_to_string(mod_id) 
            << endreq;
        error = true;
      }
      log << MSG::DEBUG << "FCAL module " << hashId << ": eta and phi granularites = " 
          << m_fcalHelper->etaGranularity(hashId) << "*" 
          << m_fcalHelper->phiGranularity(hashId) << endreq; 

      IdentifierHash hashId2 = m_fcalHelper->module_hash(mod_id);
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << "FCAL: hashId2 incorrect: " << hashId2
            << "         should be " << hashId
            << "         mod_id = " << m_fcalHelper->show_to_string(mod_id) 
            << endreq;
        error = true;
      }
      
      Identifier mod_id2;
      not_ok =  m_fcalHelper->get_id ( hashId, mod_id2, &moduleContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "FCAL: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( mod_id2 != mod_id ) {
        log << MSG::ERROR 
            << "FCAL: mod_id2 incorrect: " <<  m_fcalHelper->show_to_string(mod_id2) 
            << "         should be " <<  m_fcalHelper->show_to_string(mod_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
      mod_id2 = m_fcalHelper->module_id(hashId);
      if ( mod_id2 != mod_id ) {
        log << MSG::ERROR 
            << "FCAL: mod_id2 incorrect: " <<  m_fcalHelper->show_to_string(mod_id2) 
            << "         should be " <<  m_fcalHelper->show_to_string(mod_id) 
            << endreq;
        error = true;
      }
      
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_fcal is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of modules in the loop = (ref: ATLAS=6, H8=0, H6=2)" 
        << nChan
        << endreq;
  }

  // channels
  IdContext channelContext = m_fcalHelper->channel_context();
  itId = m_fcalHelper->fcal_begin();
  itIdEnd = m_fcalHelper->fcal_end();
  
  nChan=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;
    if(m_fcalHelper->is_lar_fcal(ch_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_fcalHelper->get_hash ( ch_id, hashId, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "error in get_hash, ch_id = "
            << m_fcalHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      IdentifierHash hashId2 = m_fcalHelper->channel_hash(ch_id);
      
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << " FCAL: hashI2 incorrect: " << hashId2
            << "         should be " << hashId
            << "           ch_id = " <<  m_fcalHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      
      Identifier ch_id2;
      not_ok =  m_fcalHelper->get_id ( hashId, ch_id2, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "FCAL: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "FCAL: ch_id2 incorrect: " <<  m_fcalHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_fcalHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
      ch_id2 = m_fcalHelper->channel_id(hashId);
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << " FCAL: ch_id2 incorrect: " <<  m_fcalHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_fcalHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      if(nChan == 1) {
          log << MSG::INFO << "Testing show_to_string with LArFCAL_ID, ch_id= " 
              << m_fcalHelper->show_to_string(ch_id)  << endreq; 
      }
      
      int pn   = m_fcalHelper->pos_neg(ch_id);
      int mod  = m_fcalHelper->module(ch_id);
      int eta  = m_fcalHelper->eta(ch_id);
      int phi  = m_fcalHelper->phi(ch_id);
      
      Identifier chanId;
      try {
        chanId = m_fcalHelper->channel_id (pn, mod, eta, phi);
      }
      catch(LArID_Exception & except){
        error = true;
        log << MSG::ERROR 
            <<  " LArId exception " 
            << (std::string)except
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "FCAL: chanId incorrect: " <<  m_fcalHelper->show_to_string(chanId) 
            << "         should be " <<  m_fcalHelper->show_to_string(ch_id) 
            << " input val of pn, mod, eta, phi= " << pn << " " << " " << mod << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier modId;
      Identifier chanId2;
      try {
        modId = m_fcalHelper->module_id (pn, mod);
        chanId2 = m_fcalHelper->channel_id (modId, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating modId or chanId2 " 
            << m_fcalHelper->show_to_string(modId) << "   "<< m_fcalHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, mod, eta, phi= " << pn << " " << mod << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "FCAL: chanId2 incorrect: " <<  m_fcalHelper->show_to_string(chanId2) 
            << "          should be " <<  m_fcalHelper->show_to_string(chanId) 
            << "          modId     " <<  m_fcalHelper->show_to_string(modId) 
            << " input val of pn, mod, eta, phi= " << pn << " " << " " << mod << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_fcal is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of channels in the loop = (ref: ATLAS=3524, H8=0, H6=314)" 
        << nChan
        << endreq;
  }

  // disconnected channels
  itId = m_fcalHelper->disc_fcal_begin();
  itIdEnd = m_fcalHelper->disc_fcal_end();
  
  nChan=0;

  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;

    //    if(m_fcalHelper->is_lar_fcal(ch_id)) {
    if(!m_fcalHelper->is_connected(ch_id)) {
      
      nChan++;
      IdentifierHash hashId = m_fcalHelper->disc_channel_hash(ch_id);
      
      Identifier ch_id2 = m_fcalHelper->disc_channel_id(hashId);

      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "FCAL: ch_id2 incorrect: " <<  m_fcalHelper->show_to_string(ch_id2) 
            << "         should be " <<  m_fcalHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
            
      int pn  = m_fcalHelper->pos_neg(ch_id);
      int mod  = m_fcalHelper->module(ch_id);
      int eta  = m_fcalHelper->disc_eta(ch_id);
      int phi  = m_fcalHelper->disc_phi(ch_id);
      
      Identifier chanId;
      try {
        chanId = m_fcalHelper->disc_channel_id (pn, mod, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating chanId  " 
            << m_fcalHelper->show_to_string(chanId) << "   "
            << (std::string)except
            << " pn, mod, eta, phi= " << pn << " " << mod << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "FCAL: chanId incorrect: " <<  m_fcalHelper->show_to_string(chanId) 
            << "         should be " <<  m_fcalHelper->show_to_string(ch_id) 
            << " input val of pn, mod, eta, phi= " << pn << " " << mod << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier modId;
      Identifier chanId2;
      try {
        modId = m_fcalHelper->disc_module_id (pn, mod);
        chanId2 = m_fcalHelper->channel_id (modId, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating modId or chanId2 " 
            << m_fcalHelper->show_to_string(modId) << "   "<< m_fcalHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, mod, eta, phi= " << pn << " " << mod << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "FCAL: chanId2 incorrect: " <<  m_fcalHelper->show_to_string(chanId2) 
            << "          should be " <<  m_fcalHelper->show_to_string(chanId) 
            << "          modId     " <<  m_fcalHelper->show_to_string(modId) 
            << " input val of pn, mod, eta, phi= " << pn << " " << mod << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
    }
      //    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_connected is always 'true' for FCAL channels"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of disconnected channels in the loop = (ref: ATLAS=60, H8=0, H6=70)" 
        << nChan
        << endreq;
  }

    if(error) {
        log << MSG::ERROR 
            << "Check of FCAL ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of FCAL ids OK " 
            << endreq;

    }
    return StatusCode::SUCCESS;
}




StatusCode TestLArIds::testFcalNeighbors(void) const
/* ============================================ */
{
  MsgStream  log(m_msgSvc,name());

  log << MSG::INFO 
      << " Testing Fcal neighbours " 
      << endreq;

  std::vector<IdentifierHash> neighbourList;
  //
  // ... loop on cells hash
  //
  for (unsigned int iCell=0; iCell<m_fcalHelper->channel_hash_max(); iCell++) {
    m_fcalHelper->get_neighbours(iCell, LArNeighbours::all3D, neighbourList);
    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    std::set<IdentifierHash> neighbSet ;
    // loop on neighbours found
    Identifier id=m_fcalHelper->channel_id(iCell);
    for (;last!=first; first++){
      // check that there is no duplicate cell
      if(!(neighbSet.insert(*first)).second) {
        log << MSG::ERROR 
            << "Check of fcal neighbours FAILED: tried to insert duplicate neighbours in cell " 
            << iCell << " id " << m_fcalHelper->show_to_string(id)
            << endreq;
        return StatusCode::FAILURE;
      }
    }
  }
    
  log << MSG::INFO 
      << "Check of FCAL neighbours OK " 
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TestLArIds::testMiniFcalIds(void) const
/*==========================================*/
{
  MsgStream  log(m_msgSvc,name());

  log << MSG::INFO 
      << " ======================================  " 
      << endreq;
  log << MSG::INFO 
      << " Testing MiniFCAL ids'   " 
      << endreq;

  if (!m_miniFcalHelper->is_initialized()) {
      log << MSG::INFO
          << " LArMiniFCAL_ID helper has NOT been initialized - there must not be a description for MiniFCAL ids"
      << endreq;
      return StatusCode::SUCCESS;
  }

  bool error = false;
  log << MSG::INFO
      << " channel_hash_max = (ref: ATLAS=4392) " 
      << m_miniFcalHelper->channel_hash_max()
      << " module_hash_max = (ref: ATLAS=2) " 
      << m_miniFcalHelper->module_hash_max()
      << endreq;
  log << MSG::INFO
      << " disc_channel_hash_max = (ref: ATLAS=4392) " 
      << m_miniFcalHelper->disc_channel_hash_max()
      << endreq;
  
  // modules
  IdContext moduleContext = m_miniFcalHelper->module_context();
  std::vector<Identifier>::const_iterator itId = m_miniFcalHelper->mod_begin();
  std::vector<Identifier>::const_iterator itIdEnd = m_miniFcalHelper->mod_end();
  
  int nChan=0;
  
  for(; itId!=itIdEnd;++itId){

    Identifier mod_id = *itId;
    if(m_miniFcalHelper->is_lar_fcal(mod_id)) {
      
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_miniFcalHelper->get_hash ( mod_id, hashId, &moduleContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "MiniFCAL: error in get_hash, mod_id = "
            << m_miniFcalHelper->show_to_string(mod_id) 
            << endreq;
        error = true;
      }
      log << MSG::DEBUG << "MiniFCAL module " << hashId << ": eta and phi granularites = " 
          << m_miniFcalHelper->etaGranularity(hashId) << "*" 
          << m_miniFcalHelper->phiGranularity(hashId) << endreq; 

      IdentifierHash hashId2 = m_miniFcalHelper->module_hash(mod_id);
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << "MiniFCAL: hashId2 incorrect: " << hashId2
            << "                   should be " << hashId
            << "                    mod_id = " << m_miniFcalHelper->show_to_string(mod_id) 
            << endreq;
        error = true;
      }
      
      Identifier mod_id2;
      not_ok =  m_miniFcalHelper->get_id ( hashId, mod_id2, &moduleContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "FCAL: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
      
      if ( mod_id2 != mod_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: mod_id2 incorrect: " <<  m_miniFcalHelper->show_to_string(mod_id2) 
            << "                   should be " <<  m_miniFcalHelper->show_to_string(mod_id) 
            << "                    hashId = " << hashId
            << endreq;
        error = true;
      }
      mod_id2 = m_miniFcalHelper->module_id(hashId);
      if ( mod_id2 != mod_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: mod_id2 incorrect: " <<  m_miniFcalHelper->show_to_string(mod_id2) 
            << "                   should be " <<  m_miniFcalHelper->show_to_string(mod_id) 
            << endreq;
        error = true;
      }
      
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_fcal is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of modules in the loop = (ref: ATLAS=2) " 
        << nChan
        << endreq;
  }

  // channels
  IdContext channelContext = m_miniFcalHelper->channel_context();
  itId = m_miniFcalHelper->minifcal_begin();
  itIdEnd = m_miniFcalHelper->minifcal_end();

  nChan=0;

  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;
    if(m_miniFcalHelper->is_lar_minifcal(ch_id)) {
   
      nChan++;
      IdentifierHash hashId;
      int not_ok = m_miniFcalHelper->get_hash ( ch_id, hashId, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "error in get_hash, ch_id = "
            << m_miniFcalHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      IdentifierHash hashId2 = m_miniFcalHelper->channel_hash(ch_id);
   
      if ( hashId2 != hashId ) {
        log << MSG::ERROR 
            << "MiniFCAL: hashI2 incorrect: " << hashId2
            << "                  should be " << hashId
            << "                    ch_id = " <<  m_miniFcalHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
   
      Identifier ch_id2;
      not_ok =  m_miniFcalHelper->get_id ( hashId, ch_id2, &channelContext);
      if(not_ok) {
        log << MSG::ERROR 
            <<  "MiniFCAL: error in get_id, hashId = "
            << hashId
            << endreq;
        error = true;
        }
   
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: ch_id2 incorrect: " <<  m_miniFcalHelper->show_to_string(ch_id2) 
            << "                  should be " <<  m_miniFcalHelper->show_to_string(ch_id) 
            << "                   hashId = " << hashId
            << endreq;
        error = true;
      }
      ch_id2 = m_miniFcalHelper->channel_id(hashId);
      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: ch_id2 incorrect: " <<  m_miniFcalHelper->show_to_string(ch_id2) 
            << "                  should be " <<  m_miniFcalHelper->show_to_string(ch_id) 
            << endreq;
        error = true;
      }
      if(nChan == 1) {
          log << MSG::INFO << "Testing show_to_string with LArFCAL_ID, ch_id= " 
              << m_miniFcalHelper->show_to_string(ch_id)  << endreq; 
      }
   
      int pn     = m_miniFcalHelper->pos_neg(ch_id);
      int mod    = m_miniFcalHelper->module(ch_id);
      int depth  = m_miniFcalHelper->depth(ch_id);
      int eta    = m_miniFcalHelper->eta(ch_id);
      int phi    = m_miniFcalHelper->phi(ch_id);
   
      Identifier chanId;
      try {
          chanId = m_miniFcalHelper->channel_id (pn, mod, depth, eta, phi);
      }
      catch(LArID_Exception & except){
        error = true;
        log << MSG::ERROR 
            <<  " LArId exception " 
            << (std::string)except
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: chanId incorrect: " <<  m_miniFcalHelper->show_to_string(chanId) 
            << "                  should be " <<  m_miniFcalHelper->show_to_string(ch_id) 
            << " input val of pn, mod, depth, eta, phi= " << pn << " " << " " << mod << " " << depth << " " << eta << " " << phi
            << endreq;
        error = true;
      }
   
      Identifier modId;
      Identifier chanId2;
      try {
        modId = m_miniFcalHelper->module_id (pn, mod);
        chanId2 = m_miniFcalHelper->channel_id (modId, depth, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  "LArId exception creating modId or chanId2 " 
            << m_miniFcalHelper->show_to_string(modId) << "   "<< m_miniFcalHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, mod, depth, eta, phi= " << pn << " " << mod << " " << depth << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "MiniFCAL: chanId2 incorrect: " <<  m_miniFcalHelper->show_to_string(chanId2) 
            << "                   should be " <<  m_miniFcalHelper->show_to_string(chanId) 
            << "                   modId     " <<  m_miniFcalHelper->show_to_string(modId) 
            << " input val of pn, mod, depth, eta, phi= " << pn << " " << " " << mod << " " << depth << " " << eta << " " << phi
            << endreq;
        error = true;
      }
   
   
    }
  }

  if(nChan == 0) {
    log << MSG::ERROR
        <<  " Problem with initialization : is_lar_fcal is always 'false'"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of channels in the loop = (ref: ATLAS=3524, H8=0, H6=314)" 
        << nChan
        << endreq;
  }

  // disconnected channels
  itId    = m_miniFcalHelper->disc_minifcal_begin();
  itIdEnd = m_miniFcalHelper->disc_minifcal_end();
  
  nChan=0;

  for(; itId!=itIdEnd;++itId){

    Identifier ch_id = *itId;

    //    if(m_miniFcalHelper->is_lar_fcal(ch_id)) {
    if(!m_miniFcalHelper->is_connected(ch_id)) {
      
      nChan++;
      IdentifierHash hashId = m_miniFcalHelper->disc_channel_hash(ch_id);
      
      Identifier ch_id2 = m_miniFcalHelper->disc_channel_id(hashId);

      if ( ch_id2 != ch_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: ch_id2 incorrect: " <<  m_miniFcalHelper->show_to_string(ch_id2) 
            << "                  should be " <<  m_miniFcalHelper->show_to_string(ch_id) 
            << "           hashId = " << hashId
            << endreq;
        error = true;
      }
            
      int pn     = m_miniFcalHelper->pos_neg(ch_id);
      int mod    = m_miniFcalHelper->module(ch_id);
      int depth  = m_miniFcalHelper->depth(ch_id);
      int eta    = m_miniFcalHelper->disc_eta(ch_id);
      int phi    = m_miniFcalHelper->disc_phi(ch_id);
      
      Identifier chanId;
      try {
          chanId = m_miniFcalHelper->disc_channel_id (pn, mod, depth, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating chanId  " 
            << m_miniFcalHelper->show_to_string(chanId) << "   "
            << (std::string)except
            << " pn, mod, depth, eta, phi= " << pn << " " << mod << " " << depth << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId != ch_id ) {
        log << MSG::ERROR 
            << "MiniFCAL: chanId incorrect: " <<  m_miniFcalHelper->show_to_string(chanId) 
            << "                  should be " <<  m_miniFcalHelper->show_to_string(ch_id) 
            << " input val of pn, mod, depth, eta, phi= " << pn << " " << mod << " " << depth << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
      Identifier modId;
      Identifier chanId2;
      try {
        modId = m_miniFcalHelper->disc_module_id (pn, mod);
        chanId2 = m_miniFcalHelper->channel_id (modId, depth, eta, phi);
      }
      catch(LArID_Exception & except){
        log << MSG::ERROR 
            <<  " LArId exception creating modId or chanId2 " 
            << m_miniFcalHelper->show_to_string(modId) << "   "<< m_miniFcalHelper->show_to_string(chanId2) << " "
            << (std::string)except
            << " pn, mod, depth, eta, phi= " << pn << " " << mod << " " << depth << " " << eta << " " << phi << " "
            << endreq;
      }
      if ( chanId2 != chanId ) {
        log << MSG::ERROR 
            << "MiniFCAL: chanId2 incorrect: " <<  m_miniFcalHelper->show_to_string(chanId2) 
            << "                   should be " <<  m_miniFcalHelper->show_to_string(chanId) 
            << "                   modId     " <<  m_miniFcalHelper->show_to_string(modId) 
            << " input val of pn, mod, depth, eta, phi= " << pn << " " << mod << " " << depth << " " << eta << " " << phi
            << endreq;
        error = true;
      }
      
    }
      //    }
  }

  if(nChan == 0) {
//    log << MSG::ERROR
    log << MSG::WARNING
//        <<  " Problem with initialization : is_connected is always 'true' for MiniFCAL channels"
        <<  " Possible problem with initialization : is_connected is always 'true' for MiniFCAL channels"
        << endreq;
  } else {
    log << MSG::INFO
        << " number of disconnected channels in the loop = (ref: ATLAS=60, H8=0, H6=70)" 
        << nChan
        << endreq;
  }

    if(error) {
        log << MSG::ERROR 
            << "Check of MiniFCAL ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of MiniFCAL ids OK " 
            << endreq;

    }
    return StatusCode::SUCCESS;
}



StatusCode TestLArIds::testLaronlineIds(void) const
  /* ============================================ */
{
  MsgStream  log(m_msgSvc,name());
  const LArOnlineID* laronline_id  = m_laronlineHelper;
  log << MSG::INFO 
      << " /////// ======================================= " 
      << endreq;
  log << MSG::INFO 
      << " /////// ==> Now testing within testLaronlineIds " 
      << endreq;
  log << MSG::INFO
      << " /////// ==> Channel_hash_max (ref ATLAS=188416, H8=2048, H6=1024) = " 
      << laronline_id->channelHashMax() << endreq;
  log << MSG::INFO
      << " /////// ==> Feb_hash_max = (ref: ATLAS=1524, H8=16, H6=8) = " 
        << laronline_id->febHashMax() << endreq;
  log << MSG::INFO
      << " /////// ==> Feedthrough_hash_max (ref: ATLAS=114, H8=2, H6=0 known bug, should be 2) = " 
      << laronline_id->feedthroughHashMax() << endreq;
  bool error = false;
  
  /* Test of Channel Identifiers */
  std::vector<HWIdentifier>::const_iterator itId = laronline_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator itIdEnd = laronline_id->channel_end();
  
  int n_ChannelId=0;
  // bool error_ch1=false;
  // bool error_ch2=false;  
  // bool error_ch3=false;  
  // bool error_ch4=false;

  for(; itId!=itIdEnd;++itId)
    {
      HWIdentifier l_channelId = *itId;
      n_ChannelId++;

      /* Test of channel from channelHash Id */
      IdentifierHash hashId = laronline_id->channel_Hash( l_channelId );
      HWIdentifier l_channelId1 = laronline_id->channel_Id( hashId );
      if ( l_channelId1 != l_channelId ) {
        log << MSG::ERROR 
            << " ////// ==> ChannelId1 incorrect: " <<  laronline_id->show_to_string(l_channelId1) 
            << "            should be: " <<  laronline_id->show_to_string(l_channelId) 
            << "            hashId : " << hashId
            << endreq;
        // error_ch1 = true;
          error = true;
        }

        /* Test of channel from channel_Id(barrel,side,feedthr,slot,channel) */
        int i_barrel_ec   = laronline_id->barrel_ec(l_channelId);
        int i_side        = laronline_id->pos_neg(l_channelId);
        int i_feedthrough = laronline_id->feedthrough(l_channelId);
        int i_slot        = laronline_id->slot(l_channelId);
        int i_channel     = laronline_id->channel(l_channelId);

        HWIdentifier l_channelId2;
        try {
          l_channelId2 = laronline_id->channel_Id (i_barrel_ec, 
                                                   i_side,
                                                   i_feedthrough,
                                                   i_slot,
                                                   i_channel );      
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_channelId " 
              << laronline_id->show_to_string(l_channelId) << "   "
              << (std::string)except
              << " barrel_ec, side, feedthrough, slot, channel= " << i_barrel_ec << " " 
                                                                  << i_side << " " 
                                                                  << i_feedthrough << " " 
                                                                  << i_slot << " "
                                                                  << i_channel 
              << endreq;
        }
        if ( l_channelId2 != l_channelId ) 
          {
            log << MSG::ERROR 
                << " ////// ==> channelId3 incorrect: " <<  laronline_id->show_to_string(l_channelId2) 
                << "                     should be: " <<  laronline_id->show_to_string(l_channelId) 
                << endreq;
            // error_ch2 = true;
            error = true;
          }     
        
        /* Test of channel from channel_Id(feedthrough,slot,channel) */
        HWIdentifier l_feedthroughId_ch = laronline_id->feedthrough_Id( l_channelId );
        HWIdentifier l_channelId3 ;
        try {
          l_channelId3 = laronline_id->channel_Id( l_feedthroughId_ch, i_slot, i_channel );
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_channelId3 " 
              << laronline_id->show_to_string(l_channelId3) << "   "
              << (std::string)except
              << " feedthroughId, slot, channel= " << laronline_id->show_to_string(l_feedthroughId_ch) << " "
              << i_slot << " "
              << i_channel 
              << endreq;
        }
        if ( l_channelId3 != l_channelId ) 
          {
            log << MSG::ERROR 
                << " ////// ==> channelId3 incorrect: " <<  laronline_id->show_to_string(l_channelId3) 
                << "            should be: " <<  laronline_id->show_to_string(l_channelId) 
                << endreq;
            // error_ch3 = true;
            error = true;
          }     

        /* Test of channel from channel_Id(feb , channel) */
        HWIdentifier l_febId = laronline_id->feb_Id( l_channelId );
        HWIdentifier l_channelId4;
        try {
          l_channelId4 = laronline_id->channel_Id( l_febId, i_channel );
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_channelId4 " 
              << laronline_id->show_to_string(l_channelId4) << "   "
              << (std::string)except
              << " febId, channel= " << laronline_id->show_to_string(l_febId) << " "
              << i_channel 
              << endreq;
        }
        if ( l_channelId4 != l_channelId ) 
          {
            log << MSG::ERROR 
                << " ////// ==> channelId4 incorrect: " <<  laronline_id->show_to_string(l_channelId4) 
                << "            should be: " <<  laronline_id->show_to_string(l_channelId) 
                << endreq;
            // error_ch4 = true;
            error = true;
          }     
    }

    log << MSG::INFO
        << " number of channels in the loop = (ref: ATLAS=188416, H8=2048, H6=1024)" 
        << n_ChannelId
        << endreq;

    /* Test of FEB HWIdentifier */
    std::vector<HWIdentifier>::const_iterator itFEB = laronline_id->feb_begin();
    std::vector<HWIdentifier>::const_iterator itFEBEnd = laronline_id->feb_end();    
    int n_febId=0;
    // bool error_feb1 = false;
    // bool error_feb2 = false;
    // bool error_feb3 = false;
    // bool error_feb4 = false;
    for(; itFEB!=itFEBEnd;++itFEB)
      {
        HWIdentifier l_febId = *itFEB;
        n_febId++;

        /* Test of channel from febHash Id */
        IdentifierHash hashId = laronline_id->feb_Hash( l_febId );
        HWIdentifier l_febId1 = laronline_id->feb_Id( hashId );
        if ( l_febId1 != l_febId ) {
          log << MSG::ERROR 
              << " ////// ==>  FebId1 incorrect: " <<  laronline_id->show_to_string(l_febId1) 
              << "        should be: " <<  laronline_id->show_to_string(l_febId) 
              << "          hashId : " << hashId
              << endreq;
          // error_feb1 = true;
          error = true;
        }

        /* Test of channel from feb_Id(barrel,side,feedthr,slot) */
        int i_barrel_ec   = laronline_id->barrel_ec(l_febId);
        int i_side        = laronline_id->pos_neg(l_febId);
        int i_feedthrough = laronline_id->feedthrough(l_febId);
        int i_slot        = laronline_id->slot(l_febId);
        int i_channel     = laronline_id->channel(l_febId);
        HWIdentifier l_febId2 ;
        try {
          l_febId2 = laronline_id->feb_Id (i_barrel_ec, 
                                           i_side,
                                           i_feedthrough,
                                           i_slot );
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_febId2 " 
              << laronline_id->show_to_string(l_febId2) << "   "
              << (std::string)except
              << " barrel_ec, side, feedthrough, slot= " << i_barrel_ec << " " 
              << i_side << " " 
              << i_feedthrough << " " 
              << i_slot 
              << endreq;
        }
        if ( l_febId2 != l_febId ) 
          {
            log << MSG::ERROR 
                << " ////// ==>  febId2 incorrect: " <<  laronline_id->show_to_string(l_febId2) 
                << "        should be: " <<  laronline_id->show_to_string(l_febId) 
                << endreq;
            // error_feb2 = true;
            error = true;
          }     

        /* Test feb from feb_Id(channelId) */
        HWIdentifier l_channelId_feb ;
        try {
          l_channelId_feb = laronline_id->channel_Id( l_febId, i_channel );
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_channelId_feb " 
              << laronline_id->show_to_string(l_channelId_feb) << "   "
              << (std::string)except
              << " i_channel= " << i_channel 
              << endreq;
        }
        HWIdentifier l_febId3 = laronline_id->feb_Id( l_channelId_feb );
        if ( l_febId3 != l_febId ) 
          {
            log << MSG::ERROR 
                << " ////// ==>  febId3 incorrect: " <<  laronline_id->show_to_string(l_febId3) 
                << "        should be: " <<  laronline_id->show_to_string(l_febId) 
                << endreq;
            // error_feb3 = true;
            error = true;
          }     

        /* Test feb from feb_Id(feedthroughId,slot) */
        HWIdentifier l_feedthroughId_feb = laronline_id->feedthrough_Id( l_febId );
        HWIdentifier l_febId4 ;
        try {
          l_febId4 = laronline_id->feb_Id( l_feedthroughId_feb, i_slot);
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_febId4 " 
              << laronline_id->show_to_string(l_febId4) << "   "
              << (std::string)except
              << " slot= " << i_slot 
              << endreq;
        }
        if ( l_febId4 != l_febId ) 
          {
            log << MSG::ERROR 
                << " ////// ==>  febId4 incorrect: " <<  laronline_id->show_to_string(l_febId4) 
                << "        should be: " <<  laronline_id->show_to_string(l_febId) 
                << endreq;
            // error_feb4 = true;
            error = true;
          }     

      }

    log << MSG::INFO
        << " number of febs in the loop = (ref: ATLAS=1524, H8=16, H6=8)" 
        << n_febId
        << endreq;

    /* Test OF FEEDTHROUGH HWIdentifiers */

    std::vector<HWIdentifier>::const_iterator itFT = laronline_id->feedthrough_begin();
    std::vector<HWIdentifier>::const_iterator itFTEnd = laronline_id->feedthrough_end();    
    int n_feedthroughId=0;
    // error = false;
    for(; itFT!=itFTEnd;++itFT)
      {
        HWIdentifier l_feedthroughId = *itFT;
        n_feedthroughId++;
        IdentifierHash hashId = laronline_id->feedthrough_Hash( l_feedthroughId );
        HWIdentifier l_feedthroughId2 = laronline_id->feedthrough_Id( hashId );
        if ( l_feedthroughId2 != l_feedthroughId ) {
          log << MSG::ERROR 
              << " ////// ==>  FeedthroughId2 incorrect: " <<  laronline_id->show_to_string(l_feedthroughId2) 
              << "                should be: " <<  laronline_id->show_to_string(l_feedthroughId) 
              << "                  hashId = " << hashId
              << endreq;
          error = true;
        }
        int i_barrel_ec   = laronline_id->barrel_ec(l_feedthroughId);
        int i_side        = laronline_id->pos_neg(l_feedthroughId);
        int i_feedthrough = laronline_id->feedthrough(l_feedthroughId);
        HWIdentifier l_feedthroughId3 ;
        try {
         l_feedthroughId3 = laronline_id->feedthrough_Id (i_barrel_ec, 
                                                          i_side, 
                                                          i_feedthrough );
        }
        catch(LArOnlID_Exception & except){
          log << MSG::ERROR 
              <<  " LArOnlId exception creating l_feedthroughId3 " 
              << laronline_id->show_to_string(l_feedthroughId3) << "   "
              << (std::string)except
              << " bec= " << i_barrel_ec << " "
              << " side= " << i_side << " "
              << " ft= " << i_feedthrough << " "
              << endreq;
        }
        if ( l_feedthroughId3 != l_feedthroughId ) 
          {
            log << MSG::ERROR 
                << " ////// ==> feedthroughId3 incorrect: " <<  laronline_id->show_to_string(l_feedthroughId3) 
                << "                should be: " <<  laronline_id->show_to_string(l_feedthroughId) 
                << endreq;
            error = true;
          }     
      }
    log << MSG::INFO
        << " number of FTs in the loop = (ref: ATLAS=114, H8=2, H6=0 known bug should be 2)" 
        << n_feedthroughId
        << endreq;

    
    log << MSG::INFO << " /////// ==> finally check the exception throwing... " << endreq;
    try {
        /* HWIdentifier l_wrong_channelId = */ laronline_id->channel_Id (0,-1,0,0,0); 
    }
    catch(LArOnlID_Exception & except){
      log << MSG::INFO  << (std::string) except << endreq ;
    }
    try {
        HWIdentifier l_good_febId = laronline_id->feb_Id (0,0,0,0); 
        /* HWIdentifier l_wrong_channelId = */laronline_id->channel_Id (l_good_febId,-99); 
    }
    catch(LArOnlID_Exception & except){
      log << MSG::INFO  << (std::string) except << endreq ;
    }
    try {
        HWIdentifier l_good_ftId =  laronline_id->feedthrough_Id (0,0,0); 
        /* HWIdentifier l_wrong_channelId = */ laronline_id->channel_Id (l_good_ftId,0,-99); 
    }
    catch(LArOnlID_Exception & except){
      log << MSG::INFO  << (std::string) except << endreq ;
    }
    try {
        /* HWIdentifier l_wrong_febId = */ laronline_id->feb_Id (0,-1,0,0); 
    }
    catch(LArOnlID_Exception & except){
      log << MSG::INFO  << (std::string) except << endreq ;
    }
    try {
        /* HWIdentifier l_wrong_ftId = */ laronline_id->feedthrough_Id (0,-1,0); 
    }
    catch(LArOnlID_Exception & except){
      log << MSG::INFO  << (std::string) except << endreq ;
    }
    log << MSG::INFO  << " /////// ==> ... which is ok " << endreq;


  log << MSG::INFO 
      << " /////// ======================================= " 
      << endreq;


  if(error) {
    log << MSG::ERROR 
        << "Check of LAr online ids FAILED " 
        << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO 
        << "Check of LAr online ids OK " 
        << endreq;
    
  }



  return StatusCode::SUCCESS;
}// end


