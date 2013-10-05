/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>   $Id: TestInDetIds.cxx,v 1.26 2007-01-19 16:10:34 dquarrie Exp $
//<version>     $Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"

// DetDescr includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SiliconID.h"
#include "TestInDetIds.h"

// Athena-related includes
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
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

TestInDetIds::TestInDetIds()
    :
    m_name("IdDictTest_InDet"),
    m_idDictMgr(0), 
    m_siliconID(0),
    m_chronoStSvc(0)
{
}

TestInDetIds::TestInDetIds(const IdDictManager* idDictMgr, 
                           const PixelID*       pixIDHelper,
                           const SCT_ID*        sctIDHelper,
                           const TRT_ID*        trtIDHelper, 
                           const SiliconID*     siliconID,
                           IMessageSvc*         msgSvc,
                           IChronoStatSvc*      chronoStSvc)
    :
    m_name("IdDictTest_InDet"),
    m_idDictMgr(idDictMgr), 
    m_pixIDHelper(pixIDHelper),
    m_sctIDHelper(sctIDHelper),
    m_trtIDHelper(trtIDHelper), 
    m_siliconID(siliconID),
    m_msgSvc(msgSvc),
    m_chronoStSvc(chronoStSvc)
{}

const std::string&
TestInDetIds::name (void) const
{
    return m_name;
}

StatusCode      
TestInDetIds::testPixelIds              (void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
        << " Testing pixel det elem ids'   " 
        << endreq;

    // Save context for pixel wafer id
    IdContext   idContext = m_pixIDHelper->wafer_context();    
    bool error = false;

    // Check is_barrel

    // neg endcap
    Identifier  new_id =  m_pixIDHelper->wafer_id (-2, 0, 0, 0);
    if(m_pixIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testPixelIds new id is NOT endcap " 
            << m_pixIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // barrel
    new_id =  m_pixIDHelper->wafer_id (0, 0, 0, 0);
    if(!m_pixIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testPixelIds new id is NOT barrel " 
            << m_pixIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // pos endcap
    new_id =  m_pixIDHelper->wafer_id (2, 0, 0, 0);
    if(m_pixIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testPixelIds new id is NOT endcap " 
            << m_pixIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    if(error) {
        log << MSG::ERROR 
            << "Check of pixel is_barrel  FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of pixel is_barrel OK " 
            << endreq;

    }


    // Test for number of pixels
    new_id =  m_pixIDHelper->wafer_id (0, 0, 0, 0);
    int eta_index_max = 191;
    if (m_idDictMgr->manager()->tag() == "initial_layout" ||
        m_idDictMgr->manager()->tag() == "final_layout"   ||
        m_idDictMgr->manager()->tag() == "destaged_layout") eta_index_max = 143;
    if(327           != m_pixIDHelper->phi_index_max(new_id) ||
       eta_index_max != m_pixIDHelper->eta_index_max(new_id)) {
        log << MSG::ERROR 
            << "phi/eta index max is INCORRECT - should be 327/" << eta_index_max << ": found " 
            << m_pixIDHelper->show_to_string(new_id)
            << " " << m_pixIDHelper->phi_index_max(new_id)
            << " " << m_pixIDHelper->eta_index_max(new_id)
            << " tag " << m_idDictMgr->manager()->tag() 
            << endreq;
        error = true;
    }
    if (m_idDictMgr->manager()->tag() != "initial_layout") {
        new_id =  m_pixIDHelper->wafer_id (0, 1, 0, 0);
        if(327 != m_pixIDHelper->phi_index_max(new_id) ||
           143 != m_pixIDHelper->eta_index_max(new_id)) {
            log << MSG::ERROR 
                << "phi/eta index max is INCORRECT - should be 327/143: found " 
                << m_pixIDHelper->show_to_string(new_id)
                << " " << m_pixIDHelper->phi_index_max(new_id)
                << " " << m_pixIDHelper->eta_index_max(new_id)
                << " layout " << m_idDictMgr->manager()->tag()
                << endreq;
            error = true;
        }
    }
    new_id =  m_pixIDHelper->wafer_id (2, 0, 0, 0);
    if(327 != m_pixIDHelper->phi_index_max(new_id) ||
       143 != m_pixIDHelper->eta_index_max(new_id)) {
        log << MSG::ERROR 
            << "phi/eta index max is INCORRECT - should be 327/143: found " 
            << m_pixIDHelper->show_to_string(new_id)
            << " " << m_pixIDHelper->phi_index_max(new_id)
            << " " << m_pixIDHelper->eta_index_max(new_id)
            << endreq;
        error = true;
    }
    if (m_idDictMgr->manager()->tag() != "initial_layout") {
        new_id =  m_pixIDHelper->wafer_id (2, 1, 0, 0);
        if(327 != m_pixIDHelper->phi_index_max(new_id) ||
           143 != m_pixIDHelper->eta_index_max(new_id)) {
            log << MSG::ERROR 
                << "phi/eta index max is INCORRECT - should be 327/143: found " 
                << m_pixIDHelper->show_to_string(new_id)
                << " " << m_pixIDHelper->phi_index_max(new_id)
                << " " << m_pixIDHelper->eta_index_max(new_id)
                << " layout " << m_idDictMgr->manager()->tag()
                << endreq;
            error = true;
        }
    }
    new_id =  m_pixIDHelper->wafer_id (2, 2, 0, 0);
    if(327 != m_pixIDHelper->phi_index_max(new_id) ||
       143 != m_pixIDHelper->eta_index_max(new_id)) {
        log << MSG::ERROR 
            << "phi/eta index max is INCORRECT - should be 327/143: found " 
            << m_pixIDHelper->show_to_string(new_id)
            << " " << m_pixIDHelper->phi_index_max(new_id)
            << " " << m_pixIDHelper->eta_index_max(new_id)
            << endreq;
        error = true;
    }
    if(error) {
        log << MSG::ERROR 
            << "Check of pixel phi/eta index max  FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of pixel phi/eta index max OK " 
            << endreq;
    }

    return StatusCode::SUCCESS;
}


StatusCode      
TestInDetIds::testPixelDetElem  (void) const
{// testPixelDetElem
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
        << " Testing pixel det elem ids'   " 
        << endreq;

    m_pixIDHelper->test_wafer_packing();
    
    // Save context for pixel wafer id
    IdContext   idContext = m_pixIDHelper->wafer_context();    

    std::set<Identifier> ids;
     
    int nids = 0;
    
    PixelID::const_id_iterator idIt       = m_pixIDHelper->wafer_begin();
    PixelID::const_id_iterator idItEnd    = m_pixIDHelper->wafer_end();
        
    // Check the sorted list of ids
    for (; idIt != idItEnd; ++idIt) {
        ids.insert((*idIt));
        nids++;
    }

    idIt       = m_pixIDHelper->wafer_begin();
    idItEnd    = m_pixIDHelper->wafer_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    bool error = false;
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
        if ((*firstIds) != (*idIt) ) {
            log << MSG::ERROR 
                << " IDs not the same: N " << ncount
                << " id manually sorted " << m_pixIDHelper->show_to_string((*firstIds))
                << " id already sorted " << m_pixIDHelper->show_to_string((*idIt))
                << endreq;
            error = true;
        }
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for matching ids from helper and detelems FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for matching ids from helper and detelems OK " 
            << endreq;
    }
    
    log << MSG::INFO 
        << "Number of IDDICT det elem pixel ids " << nids
        << endreq;
    log << MSG::INFO 
        << "Number of non-duplicate IDDICT pixel ids " << ids.size()
        << endreq;

    //
    // Create set of packed ids
    //
    ExpandedIdentifier unpackedId;
    // Check that the order of packed ids is the same as expanded
    // identifiers
    std::set<Identifier> packedIds;
    std::set<Identifier>::iterator    firstId = ids.begin();
    std::set<Identifier>::iterator    lastId  = ids.end();
    ncount = 0;
    error = false;
    // Check hash max
    PixelID::size_type hashMax = m_pixIDHelper->wafer_hash_max();

    for(; firstId != lastId; ++firstId, ncount++) {
      // Get packed
      Identifier packedId = (*firstId);
        if(!(packedIds.insert(packedId)).second) {
            log << MSG::ERROR 
                << " Error: duplicated packed id " << ncount
                << " packed " << MSG::hex << packedId << MSG::dec 
                << endreq;
            error = true;
        }
        
        // Test hash
        IdentifierHash hashedId;
        if(m_pixIDHelper->get_hash ((*firstId), hashedId, &idContext)) {
            log << MSG::ERROR 
                << " Error: cannot find packed id's hash. Unpacked id " << m_pixIDHelper->show_to_string((*firstId), &idContext)
                << " hashed id " << MSG::hex << hashedId << MSG::dec 
                << endreq;
        }
        if(ncount != hashedId) {
            log << MSG::ERROR 
                << " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
                << m_pixIDHelper->show_to_string((*firstId), &idContext)
                << endreq;
            error = true;
        }

        // Get packed again
        Identifier id;
        m_pixIDHelper->get_id(hashedId, id, &idContext);
        Identifier packedId1 = id;
        if (packedId != packedId1) {
            log << MSG::ERROR 
                << " Unable to get packed from hash: N " << ncount
                << " hash " << hashedId
                << " id " << m_pixIDHelper->show_to_string((*firstId), &idContext)
                << " packed " << MSG::hex << packedId << MSG::dec
                << " new packed " << MSG::hex << packedId1 << MSG::dec
                << " is eq " << (packedId == packedId1)
                << " begin, end of context " << idContext.begin_index() << " " << idContext.end_index()
                << endreq;
            error = true;
        }

        // Check that hash is less than max
        if(hashMax <= hashedId) {
            log << MSG::ERROR 
                << " Error: hash is OUT OF LIMITS: hash, max:  " << hashedId << " " << hashMax << " " 
                << (std::string)unpackedId
                << endreq;
            error = true;
        }
    }
    
    // We should now be at hash max
    if(hashMax != ncount) {
        log << MSG::ERROR 
            << " Error: hashMax is NOT equal to ncount: max, ncount:  " << hashMax << " " << ncount << " " 
            << (std::string)unpackedId
            << endreq;
        error = true;
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for hashed ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for hashed ids OK " 
            << endreq;
    }

    log << MSG::INFO 
        << "Number of non-duplicate PACKED IDDICT pixel ids " << packedIds.size()
        << endreq;

    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    int nelems = 0;
    error = false;
    firstId = ids.begin();
    std::set<Identifier>::iterator  packedIdIt   = packedIds.begin();
    std::set<Identifier>::iterator  packedIdLast = packedIds.end();
    for(; firstId != lastId; ++firstId, ++nelems, ++packedIdIt) {
        if (packedIdIt == packedIdLast) {
            log << MSG::ERROR 
                << " Error: Ended loop over packed ids, nelem " << nelems
                << endreq;
            error = true;
            break;
        }
        Identifier packedId = (*firstId);
        if (packedId != (*packedIdIt)) {
            log << MSG::ERROR 
                << " Error: unequal ids - "
                << " id " << m_pixIDHelper->show_to_string((*firstId), &idContext)
                << " packed " << MSG::hex << (*packedIdIt)
                << " unpacked " << (std::string)unpackedId
                << endreq;
            error = true;
        }
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for ordering of packed ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for ordering of packed ids OK " 
            << endreq;
    }

    log << MSG::INFO 
        << " Successful end of pixel test   " 
        << endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode      
TestInDetIds::testSCTIds                (void) const
{//testSCTIDs
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
        << " Testing sct det elem ids'   " 
        << endreq;

    bool error = false;

    // Save context for sct wafer id
    IdContext   idContext = m_sctIDHelper->wafer_context();    

    // Check is_barrel

    // neg endcap
    Identifier  new_id =  m_sctIDHelper->wafer_id (-2, 0, 0, 0, 0);
    if(m_sctIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testSCTIds new id is NOT endcap " 
            << m_sctIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // barrel
    new_id =  m_sctIDHelper->wafer_id (0, 0, 0, 1, 0);
    if(!m_sctIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testSCTIds new id is NOT barrel " 
            << m_sctIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // pos endcap
    new_id =  m_sctIDHelper->wafer_id (2, 0, 0, 0, 0);
    if(m_sctIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testSCTIds new id is NOT endcap " 
            << m_sctIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    if(!error) {
        log << MSG::INFO 
            << "Check of sct is_barrel OK " 
            << endreq;

    }

    // Test for number of strips
    new_id =  m_sctIDHelper->wafer_id (0, 0, 0, 1, 0);
    if(767 != m_sctIDHelper->strip_max(new_id)) {
        log << MSG::ERROR 
            << "strip max is INCORRECT - should be 767: found " 
            << m_sctIDHelper->show_to_string(new_id)
            << " " << m_sctIDHelper->strip_max(new_id)
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of sct strip max OK " 
            << endreq;
    }

    return StatusCode::SUCCESS;
}

StatusCode      
TestInDetIds::testSCTDetElem            (void) const
{//testSCTDetElem
    
    MsgStream  log(m_msgSvc,name());
    bool error = false;
    log << MSG::INFO 
        << " Testing sct det elem ids'   " 
        << endreq;

    // Save context for SCT wafer id
    IdContext   idContext = m_sctIDHelper->wafer_context();    

    std::set<Identifier> ids;

    int nids = 0;
    
    SCT_ID::const_id_iterator idIt       = m_sctIDHelper->wafer_begin();
    SCT_ID::const_id_iterator idItEnd    = m_sctIDHelper->wafer_end();
        
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
        ids.insert((*idIt));
        nids++;
    }

    idIt       = m_sctIDHelper->wafer_begin();
    idItEnd    = m_sctIDHelper->wafer_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
        if ((*firstIds) != (*idIt) ) {
            log << MSG::ERROR 
                << " IDs not the same: N " << ncount
                << " id manually sorted " << m_sctIDHelper->show_to_string((*firstIds), &idContext)
                << " id already sorted " << m_sctIDHelper->show_to_string((*idIt), &idContext)
                << endreq;
            error = true;
        }
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for matching ids from helper and detelems FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for matching ids from helper and detelems OK " 
            << endreq;
    }
    
    log << MSG::INFO 
        << "Number of IDDICT det elem sct ids " << nids
        << endreq;
    log << MSG::INFO 
        << "Number of non-duplicate IDDICT sct ids " << ids.size()
        << endreq;

    firstIds = ids.begin();
    lastIds  = ids.end();

    //
    // Create set of packed ids
    //
    ExpandedIdentifier unpackedId;
    // Check that the order of packed ids is the same as expanded
    // identifiers
    std::set<Identifier> packedIds;
    std::set<Identifier>::iterator    firstId = ids.begin();
    std::set<Identifier>::iterator    lastId  = ids.end();
    ncount = 0;
    error = false;
    // Check hash max
    SCT_ID::size_type hashMax = m_sctIDHelper->wafer_hash_max();

    for(; firstId != lastId; ++firstId, ncount++) {
        // Get packed
        Identifier packedId = (*firstId);
        if(!(packedIds.insert(packedId)).second) {
            log << MSG::ERROR 
                << " Error: duplicated packed id " << ncount
                << " packed " << MSG::hex << packedId  << MSG::dec
                << endreq;
            error = true;
        }
        
        // Test hash
        IdentifierHash hashedId;
        if(m_sctIDHelper->get_hash ((*firstId), hashedId, &idContext)) {
            log << MSG::ERROR 
                << " Error: cannot find packed id's hash. Unpacked id " << m_sctIDHelper->show_to_string((*firstId), &idContext)
                << " hashed id " << MSG::hex << hashedId << MSG::dec 
                << endreq;
        }
        if(ncount != hashedId) {
            log << MSG::ERROR 
                << " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
                << m_sctIDHelper->show_to_string((*firstId), &idContext)
                << endreq;
            error = true;
        }

        // Get packed again
        Identifier id;
        m_sctIDHelper->get_id(hashedId, id, &idContext);
        Identifier packedId1 = id;
        if (packedId != packedId1) {
            log << MSG::ERROR 
                << " Unable to get packed from hash: N " << ncount
                << " hash " << hashedId
                << " id " << m_sctIDHelper->show_to_string((*firstId), &idContext)
                << " packed " << MSG::hex << packedId << MSG::dec
                << " new packed " << MSG::hex << packedId1 << MSG::dec
                << " is eq " << (packedId == packedId1)
                << " begin, end of context " << idContext.begin_index() << " " << idContext.end_index()
                << endreq;
            error = true;
        }

        if (hashedId < 10) {
            log << MSG::DEBUG 
                << " Dumping hash and id from it: " 
                << " hash " << hashedId
                << " id " << m_sctIDHelper->show_to_string((*firstId), &idContext)
                << " packed " << MSG::hex << packedId << MSG::dec
                << " new packed " << MSG::hex << packedId1 << MSG::dec
                << " is eq " << (packedId == packedId1)
                << " begin, end of context " << idContext.begin_index() << " " << idContext.end_index()
                << endreq;
        }
        

        // Check that hash is less than max
        if(hashMax <= hashedId) {
            log << MSG::ERROR 
                << " Error: hash is OUT OF LIMITS: hash, max:  " << hashedId << " " << hashMax << " " 
                << (std::string)unpackedId
                << endreq;
            error = true;
        }
    }

    // We should now be at hash max
    if(hashMax != ncount) {
        log << MSG::ERROR 
            << " Error: hashMax is NOT equal to ncount: max, ncount:  " << hashMax << " " << ncount << " " 
            << (std::string)unpackedId
            << endreq;
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for hashed ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for hashed ids OK " 
            << endreq;
    }

    log << MSG::INFO 
        << "Number of non-duplicate PACKED IDDICT sct ids " << packedIds.size()
        << endreq;


    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    int nelems = 0;
    error = false;
    firstId = ids.begin();
    std::set<Identifier>::iterator  packedIdIt   = packedIds.begin();
    std::set<Identifier>::iterator  packedIdLast = packedIds.end();
    for(; firstId != lastId; ++firstId, ++nelems, ++packedIdIt) {
        if (packedIdIt == packedIdLast) {
            log << MSG::ERROR 
                << " Error: Ended loop over packed ids, nelem " << nelems
                << endreq;
            error = true;
            break;
        }
        Identifier packedId = (*firstId);
        if (packedId != (*packedIdIt)) {
            log << MSG::ERROR 
                << " Error: unequal ids - "
                << " id " << m_sctIDHelper->show_to_string((*firstId), &idContext)      
                << " packed " << MSG::hex << (*packedIdIt)
                << " unpacked " << (std::string)unpackedId
                << endreq;
            error = true;
        }
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for ordering of packed ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for ordering of packed ids OK " 
            << endreq;
    }

    log << MSG::INFO 
        << " Successful end of sct test   " 
        << endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode      
TestInDetIds::testSCTNeighbors  (void) const
{//testSCTNeighbors
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
        << " Testing sct det elem neighbor ids " 
        << endreq;

    m_sctIDHelper->test_wafer_packing();
    
    return StatusCode::SUCCESS;
}

StatusCode  
TestInDetIds::testSiliconIds          (void) const
{

    MsgStream  log(m_msgSvc,name());
    bool error = false;

    if (m_siliconID->test_wafer_hashes()) error = true;

    if(error) {
        log << MSG::ERROR 
            << "Check of Silicon IDs FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of Silicon IDs OK " 
            << endreq;
    }

    log << MSG::INFO 
        << " Successful end of silicon id test   " 
        << endreq;
    
    return StatusCode::SUCCESS;
}


StatusCode      
TestInDetIds::testTRTDetElem            (void) const
{//testTRTDetElem
    
    MsgStream  log(m_msgSvc,name());
    bool error = false;
    log << MSG::INFO 
        << " Testing trt det elem ids'   " 
        << endreq;

    TRT_ID* trt_id = const_cast<TRT_ID*>(m_trtIDHelper);
    trt_id->test_trt_ids();

    // Save context for TRT module id
    IdContext   idContext = m_trtIDHelper->module_context();    


    // Test is_barrel

    // neg endcap
    Identifier  new_id =  m_trtIDHelper->layer_id(-2, 0, 0, 0);
    if(m_trtIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testTRTIds new id is NOT endcap " 
            << m_trtIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // neg barrel
    new_id = m_trtIDHelper->layer_id(-1, 0, 0, 0);
    if(!m_trtIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testTRTIds new id is NOT barrel " 
            << m_trtIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // pos barrel
    new_id = m_trtIDHelper->layer_id(1, 0, 0, 0);
    if(!m_trtIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testTRTIds new id is NOT barrel " 
            << m_trtIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    // pos endcap
    new_id =  m_trtIDHelper->layer_id (2, 0, 0, 0);
    if(m_trtIDHelper->is_barrel(new_id)) {
        log << MSG::ERROR 
            << " testTRTIds new id is NOT endcap " 
            << m_trtIDHelper->show_to_string(new_id)
            << endreq;
        error = true;
    }
    if(!error) {
        log << MSG::INFO 
            << "Check of trt is_barrel OK " 
            << endreq;

    }

    // Test for number of straw layers
    new_id =  m_trtIDHelper->module_id (-1, 0, 0);
    if(18 != m_trtIDHelper->straw_layer_max(new_id)) {
        log << MSG::ERROR 
            << "straw layer max is INCORRECT - should be 18: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_layer_max(new_id)
            << endreq;
        error = true;
    }
    new_id =  m_trtIDHelper->module_id (-1, 0, 1);
    if(23 != m_trtIDHelper->straw_layer_max(new_id)) {
        log << MSG::ERROR 
            << "straw layer max is INCORRECT - should be 23: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_layer_max(new_id)
            << endreq;
        error = true;
    }
    new_id =  m_trtIDHelper->module_id (-1, 0, 2);
    if(29 != m_trtIDHelper->straw_layer_max(new_id)) {
        log << MSG::ERROR 
            << "straw layer max is INCORRECT - should be 29: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_layer_max(new_id)
            << endreq;
        error = true;
    }
    new_id =  m_trtIDHelper->module_id (-2, 0, 0);
    if(15 != m_trtIDHelper->straw_layer_max(new_id)) {
        log << MSG::ERROR 
            << "straw layer max is INCORRECT - should be 15: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_layer_max(new_id)
            << endreq;
        error = true;
    }
    new_id =  m_trtIDHelper->module_id (-2, 0, 7);
    if(7 != m_trtIDHelper->straw_layer_max(new_id)) {
        log << MSG::ERROR 
            << "straw layer max is INCORRECT - should be 7: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_layer_max(new_id)
            << endreq;
        error = true;
    }

    // Number of straws in each layer of mod 1
    int nstraws1[] = {15,16,16,16,16,
                      17,17,17,17,17,
                      18,18,18,18,18,
                      19,19,19,18};
    for (int i = 0; i < 19; ++i) {
        new_id =  m_trtIDHelper->layer_id (-1, 0, 0, i);
        if((nstraws1[i]-1) != m_trtIDHelper->straw_max(new_id)) {
            log << MSG::ERROR 
                << "straw max is INCORRECT - should be " << nstraws1[i]-1 <<" : found " 
                << m_trtIDHelper->show_to_string(new_id)
                << " " << m_trtIDHelper->straw_max(new_id)
                << endreq;
            error = true;
        }
    }
    // Number of straws in each layer of mod 2
    int nstraws2[] = {19,20,20,20,20,20,
                      21,21,21,21,21,
                      22,22,22,22,22,
                      23,23,23,23,23,
                      24,24,23};
    for (int i = 0; i < 24; ++i) {
        new_id =  m_trtIDHelper->layer_id (-1, 0, 1, i);
        if((nstraws2[i]-1) != m_trtIDHelper->straw_max(new_id)) {
            log << MSG::ERROR 
                << "straw max is INCORRECT - should be " << nstraws2[i]-1 <<" : found " 
                << m_trtIDHelper->show_to_string(new_id)
                << " " << m_trtIDHelper->straw_max(new_id)
                << endreq;
            error = true;
        }
    }
    // Number of straws in each layer of mod 3
    int nstraws3[] = {23,24,24,24,24,
                      25,25,25,25,25,
                      26,26,26,26,26,
                      27,27,27,27,27,
                      28,28,28,28,28,
                      29,29,29,29,28};
    for (int i = 0; i < 30; ++i) {
        new_id =  m_trtIDHelper->layer_id (-1, 0, 2, i);
        if((nstraws3[i]-1) != m_trtIDHelper->straw_max(new_id)) {
            log << MSG::ERROR 
                << "straw max is INCORRECT - should be " << nstraws3[i]-1 <<" : found " 
                << m_trtIDHelper->show_to_string(new_id)
                << " " << m_trtIDHelper->straw_max(new_id)
                << endreq;
            error = true;
        }
    }
    new_id =  m_trtIDHelper->layer_id (-2, 0, 0, 0);
    if(23 != m_trtIDHelper->straw_max(new_id)) {
        log << MSG::ERROR 
            << "straw max is INCORRECT - should be 23: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_max(new_id)
            << endreq;
        error = true;
    }
    new_id =  m_trtIDHelper->layer_id (-2, 0, 13, 0);
    if(23 != m_trtIDHelper->straw_max(new_id)) {
        log << MSG::ERROR 
            << "straw max is INCORRECT - should be 23: found " 
            << m_trtIDHelper->show_to_string(new_id)
            << " " << m_trtIDHelper->straw_max(new_id)
            << endreq;
        error = true;
    }

    // Make sure we have straw wheel 15, and then do test -
    // INITIAL_LAYOUT is missing wheels 14 to 17
    if (m_idDictMgr->manager()->tag() != "initial_layout" &&
        m_idDictMgr->manager()->tag() != "destaged_layout") {
        new_id =  m_trtIDHelper->layer_id (-2, 0, 15, 0);
        if(17 != m_trtIDHelper->straw_max(new_id)) {
            log << MSG::ERROR 
                << "straw  max is INCORRECT - should be 17: found " 
                << m_trtIDHelper->show_to_string(new_id)
                << " " << m_trtIDHelper->straw_max(new_id)
                << " layout " << m_idDictMgr->manager()->tag()
                << endreq;
            error = true;
        }
    }

    if(error) {
        log << MSG::ERROR 
            << "Check of trt straw and straw layer max FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check of trt straw and straw layer max OK " 
            << endreq;

    }
    

    log << MSG::INFO 
        << " Successful end of trt test   " 
        << endreq;

    return StatusCode::SUCCESS;
}

StatusCode      
TestInDetIds::testTRTDetElemSL  (void) const
{//testTRTDetElemSL
    
    MsgStream  log(m_msgSvc,name());
    bool error = false;
    log << MSG::INFO 
        << " Testing trt straw layer ids'   " 
        << endreq;

    IdContext   idContext = m_trtIDHelper->straw_layer_context();    

    std::set<Identifier> ids;

    int nids = 0;
   
    TRT_ID::const_id_iterator idIt       = m_trtIDHelper->straw_layer_begin();
    TRT_ID::const_id_iterator idItEnd    = m_trtIDHelper->straw_layer_end();
        
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
        ids.insert((*idIt));
        nids++;
    }

    idIt       = m_trtIDHelper->straw_layer_begin();
    idItEnd    = m_trtIDHelper->straw_layer_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
        if ((*firstIds) != (*idIt) ) {
            log << MSG::ERROR 
                << " IDs not the same: N " << ncount
                << " id manually sorted " << m_trtIDHelper->show_to_string((*firstIds))
                << " id already sorted " << m_trtIDHelper->show_to_string((*idIt))
                << endreq;
            error = true;
        }
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for matching ids from helper and detelems FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for matching ids from helper and detelems OK " 
            << endreq;
    }
    
    log << MSG::INFO 
        << "Number of IDDICT trt straw layer det elem  ids " << nids
        << endreq;
    log << MSG::INFO 
        << "Number of non-duplicate IDDICT trt straw layer ids " << ids.size()
        << endreq;
    //
    // Create set of packed ids
    //
    ExpandedIdentifier unpackedId;
    // Check that the order of packed ids is the same as expanded
    // identifiers
    std::set<Identifier> packedIds;
    std::set<Identifier>::iterator    firstId = ids.begin();
    std::set<Identifier>::iterator    lastId  = ids.end();
    ncount = 0;
    error = false;
    // Check hash max
    TRT_ID::size_type hashMax = m_trtIDHelper->straw_layer_hash_max();
    for(; firstId != lastId; ++firstId, ncount++) {
        // Get packed
        Identifier packedId = (*firstId);
        if(!(packedIds.insert(packedId)).second) {
            log << MSG::ERROR 
                << " Error: duplicated packed id "  << ncount
                << " packed " << MSG::hex << packedId << MSG::dec
                << endreq;
            error = true;
        }
        
        // Test hash
        IdentifierHash hashedId;
        if(m_trtIDHelper->get_hash ((*firstId), hashedId, &idContext)) {
            log << MSG::ERROR 
                << " Error: cannot find packed id's hash. Unpacked id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " hashed id " << MSG::hex << hashedId << MSG::dec 
                << endreq;
        }
        if(ncount != hashedId) {
            log << MSG::ERROR 
                << " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
                << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << endreq;
            error = true;
        }

        // Get packed again
        Identifier id;
        m_trtIDHelper->get_id(hashedId, id, &idContext);
        Identifier packedId1 = id;
        if (packedId != packedId1) {
            log << MSG::ERROR 
                << " Unable to get packed from hash: N " << ncount
                << " hash " << hashedId
                << " id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " packed " << MSG::hex << packedId << MSG::dec
                << " new packed " << MSG::hex << packedId1 << MSG::dec
                << " is eq " << (packedId == packedId1)
                << " begin, end of context " << idContext.begin_index() << " " << idContext.end_index()
                << endreq;
            error = true;
        }

    }

    // We should now be at hash max
    if(hashMax != ncount) {
        log << MSG::ERROR 
            << " Error: hashMax is NOT equal to ncount: max, ncount:  " << hashMax << " " << ncount << " " 
            << (std::string)unpackedId
            << endreq;
    }

    if(error) {
        log << MSG::ERROR 
            << "Check for hashed ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for hashed ids OK " 
            << endreq;
    }

    log << MSG::INFO 
        << "Number of non-duplicate PACKED IDDICT trt ids " << packedIds.size()
        << endreq;


    //
    //  Do some timing checks for hash
    //

    //
    //  Scenario 1) start with Identifier and compare finding hash in
    //  map with calculating the hash.
    //


    m_chronoStSvc->chronoStart( "Hash from map - TRT straw layer " );


    //  Get hash from set
    firstId = ids.begin();
    for(; firstId != lastId; ++firstId) {
        IdentifierHash hashedId;
        if(m_trtIDHelper->get_hash ((*firstId), hashedId, &idContext)) {
            log << MSG::ERROR 
                << " Error: cannot find packed id's hash. Unpacked id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " hashed id " << MSG::hex << hashedId << MSG::dec 
                << endreq;
        }
    }
    
    m_chronoStSvc->chronoStop ( "Hash from map - TRT straw layer " );
    m_chronoStSvc->chronoPrint( "Hash from map - TRT straw layer " );

    m_chronoStSvc->chronoStart( "Hash calculated - TRT straw layer " );

    //  Calculate hash
    firstId = ids.begin();
    for(; firstId != lastId; ++firstId) {

        IdentifierHash hashedId;
        ExpandedIdentifier test_id;
        m_trtIDHelper->get_expanded_id((*firstId), test_id, &idContext);
        if(m_trtIDHelper->get_straw_layer_hash_calc (test_id, hashedId)) {
            log << MSG::ERROR 
                << " Error: cannot find packed id's hash. Unpacked id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " hashed " << MSG::hex << hashedId << MSG::dec 
                << endreq;
        }
    }
    
    m_chronoStSvc->chronoStop ( "Hash calculated - TRT straw layer " );
    m_chronoStSvc->chronoPrint( "Hash calculated - TRT straw layer " );

    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    int nelems = 0;
    error = false;
    firstId = ids.begin();
    std::set<Identifier>::iterator  packedIdIt   = packedIds.begin();
    std::set<Identifier>::iterator  packedIdLast = packedIds.end();
    for(; firstId != lastId; ++firstId, ++nelems, ++packedIdIt) {
        if (packedIdIt == packedIdLast) {
            log << MSG::ERROR 
                << " Error: Ended loop over packed ids, nelem " << nelems
                << endreq;
            error = true;
            break;
        }
        Identifier packedId = (*firstId);
        if (packedId != (*packedIdIt)) {
            log << MSG::ERROR 
                << " Error: unequal ids - "
                << " id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " packed 1 " << MSG::hex << packedId
                << " packed 2 " << MSG::hex << (*packedIdIt) << MSG::dec
                << endreq;
            error = true;
        }
    }
    
    if(error) {
        log << MSG::ERROR 
            << "Check for ordering of packed ids FAILED " 
            << endreq;
        return StatusCode::FAILURE;
    }
    else {
        log << MSG::INFO 
            << "Check for ordering of packed ids OK " 
            << endreq;
    }

    log << MSG::INFO 
        << " Successful end of trt straw layer test   " 
        << endreq;

    return StatusCode::SUCCESS;
}


StatusCode      
TestInDetIds::testTRTStraw              (void) const
{//testTRTStraw
    
    MsgStream  log(m_msgSvc,name());
    log << MSG::INFO 
        << " Testing trt straw ids'   " 
        << endreq;

//      log << MSG::INFO 
//      << "range -> " << (std::string) fullRange
//      << endreq;


    IdContext   idContext = m_trtIDHelper->straw_context();    

    // Get set of identifiers from elements
    // Try loop over fullRange and check each id that comes out

    std::set<Identifier> detMgrIds;
    std::set<Identifier> ids;
    int nids = 0;

    TRT_ID::const_expanded_id_iterator idIt       = m_trtIDHelper->straw_begin();
    TRT_ID::const_expanded_id_iterator idItEnd    = m_trtIDHelper->straw_end();
        
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
        const ExpandedIdentifier& expId = (*idIt);
        Identifier id = m_trtIDHelper->straw_id(expId[2], 
                                         expId[3], 
                                         expId[4],
                                         expId[5], 
                                         expId[6]);
        ids.insert(id);
        nids++;
    }

    log << MSG::INFO 
        << "Number of IDDICT det straw ids " << nids
        << endreq;
    log << MSG::INFO 
        << "Number of non-duplicate IDDICT trt straw ids " << ids.size()
        << endreq;
    //
    // Create set of packed ids
    //
    ExpandedIdentifier unpackedId;
    // Check that the order of packed ids is the same as expanded
    // identifiers
    std::set<Identifier> packedIds;
    std::set<Identifier>::iterator    firstId = ids.begin();
    std::set<Identifier>::iterator    lastId  = ids.end();
    unsigned int ncount = 0;
    // Check hash max
    TRT_ID::size_type hashMax = m_trtIDHelper->straw_hash_max();

    for(; firstId != lastId; ++firstId, ncount++) {
        // Get packed
        Identifier packedId = (*firstId);
        if(!(packedIds.insert(packedId)).second) {
            log << MSG::ERROR 
                << " Error: duplicated packed id " << ncount
                << " packed " << MSG::hex << packedId
                << endreq;
        }
        

        // Test hash
        IdentifierHash hashedId;
        if(m_trtIDHelper->get_hash ((*firstId), hashedId, &idContext)) {
            log << MSG::ERROR 
                << " Error: cannot find packed id's hash. Unpacked id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " hashed id " << MSG::hex << hashedId << MSG::dec 
                << endreq;
        }
        //IdentifierHash hashedId = m_trtIDHelper->straw_layer_hash ((*firstId));
        if(ncount != hashedId) {
            log << MSG::ERROR 
                << " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
                << (std::string)unpackedId
                << endreq;
        }

        // Get packed again ******* CURRENTLY UNABLE TO GO FROM HASH
        // TO ID FOR STRAWS **************

        // Check that hash is less than max
        if(hashMax <= hashedId) {
            log << MSG::ERROR 
                << " Error: hash is OUT OF LIMITS: hash, max:  " << hashedId << " " << hashMax << " " 
                << (std::string)unpackedId
                << endreq;
        }
    }

    // We should now be at hash max
    if(hashMax != ncount) {
        log << MSG::ERROR 
            << " Error: hashMax is NOT equal to ncount: max, ncount:  " << hashMax << " " << ncount << " " 
            << (std::string)unpackedId
            << endreq;
    }


    log << MSG::INFO 
        << "Number of non-duplicate PACKED IDDICT trt straw ids " << packedIds.size()
        << endreq;


    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    int nelems = 0;
    firstId = ids.begin();
    std::set<Identifier>::iterator  packedIdIt   = packedIds.begin();
    std::set<Identifier>::iterator  packedIdLast = packedIds.end();
    for(; firstId != lastId; ++firstId, ++nelems, ++packedIdIt) {
        if (packedIdIt == packedIdLast) {
            log << MSG::ERROR 
                << " Error: Ended loop over packed ids, nelem " << nelems
                << endreq;
            break;
        }
        Identifier packedId = (*firstId);
        if (packedId != (*packedIdIt)) {
            log << MSG::ERROR 
                << " Error: unequal ids - "
                << " id " << m_trtIDHelper->show_to_string((*firstId), &idContext)
                << " packed 1 " << MSG::hex << packedId
                << " packed 2 " << MSG::hex << (*packedIdIt) << MSG::dec
                << endreq;
        }
    }
    
    log << MSG::INFO 
        << " Successful end of trt straw test   " 
        << endreq;

    return StatusCode::SUCCESS;
}


StatusCode      
TestInDetIds::testMinMax                (void) const
{//testMinMax
    
    MsgStream  log(m_msgSvc,name());
    log << MSG::INFO 
        << " Testing min max for pixels and sct " 
        << endreq;

    return StatusCode::SUCCESS;
}

