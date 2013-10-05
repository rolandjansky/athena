/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TestMuonIds.cxx,v 1.9 2007-08-31 07:53:34 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "TestMuonIds.h"

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"

// DetDescr includes
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

// Athena-related includes
#include "StoreGate/StoreGateSvc.h"
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


TestMuonIds::TestMuonIds()
    :
    m_name("IdDictTest_Muon"),
    m_idDictMgr(0), 
    m_msgSvc(0),
    m_chronoStSvc(0)
{
}

TestMuonIds::TestMuonIds(const IdDictManager* idDictMgr, 
                         StoreGateSvc*   detStore,
			 IMessageSvc* 	msgSvc,
			 IChronoStatSvc*  chronoStSvc)
    :
    m_name("IdDictTest_Muon"),
    m_idDictMgr(idDictMgr), 
    m_detStore(detStore),
    m_msgSvc(msgSvc),
    m_chronoStSvc(chronoStSvc)
{}

const std::string&
TestMuonIds::name (void) const
{
    return m_name;
}

StatusCode 	
TestMuonIds::testMDTIds		(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing MDT det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const MdtIdHelper*  mdt_id   = 0;
    StatusCode sc = m_detStore->retrieve(mdt_id, "MDTIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get MdtIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 

    // Save context for MDT module id
    IdContext	context = mdt_id->module_context();    

    bool error = false;


    // Check is_barrel

    // neg endcap
    Identifier  new_id =  mdt_id->elementID(13, -2, 7);
    if(mdt_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testMDTIds new id is NOT endcap " 
	    << mdt_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // barrel
    new_id =  mdt_id->elementID(0, 2, 3);
    if(!mdt_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testMDTIds new id is NOT barrel " 
	    << mdt_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // pos endcap
    new_id =  mdt_id->elementID(14, 1, 5);
    if(mdt_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testMDTIds new id is NOT endcap " 
	    << mdt_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of MDT isBarrel  FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of MDT isBarrel OK " 
	    << endreq;

    }
    std::vector<Identifier> elementIds = mdt_id->idVector();
    std::vector<Identifier>::const_iterator first = elementIds.begin();
    std::vector<Identifier>::const_iterator last  = elementIds.end();
    for (; first != last; ++first) {
      Identifier id = (*first);
      mdt_id->test_id(id,context);
      if (!mdt_id->validElement(id)) {
	log << MSG::ERROR
	    << " testMDTIds invalid identifier "
	    << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      int stationName = mdt_id->stationName(id);
      std::string name = mdt_id->stationNameString(stationName);
      if (mdt_id->isBarrel(id) && name[0] != 'B') {
	log << MSG::ERROR
	    << " testMDTIds problem in isBarrel() and stationName() "
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (mdt_id->isEndcap(id) && name[0] != 'E') {
	log << MSG::ERROR
	    << " testMDTIds problem in isEndCap() and stationName() "
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (mdt_id->stationNameIndex(name) != mdt_id->stationName(id)) {
	log << MSG::ERROR
	    << " testMDTIds problem in stationNameIndex() and stationName() "
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int etaMin = mdt_id->stationEtaMin(id);
      const int etaMax = mdt_id->stationEtaMax(id);
      const int eta    = mdt_id->stationEta(id);
      if (eta < etaMin || eta > etaMax) {
	log << MSG::ERROR
	    << " testMDTIds problem in etaMin, etaMax or eta "
	    << "etaMin = " << etaMin
	    << "etaMax = " << etaMax
	    << "eta    = " << eta << " "
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int phiMin = mdt_id->stationPhiMin(id);
      const int phiMax = mdt_id->stationPhiMax(id);
      const int phi    = mdt_id->stationPhi(id);
      if (phi < phiMin || phi > phiMax) {
	log << MSG::ERROR
	    << " testMDTIds problem in phiMin, phiMax or phi "
	    << "phiMin = " << phiMin
	    << "phiMax = " << phiMax
	    << "phi    = " << phi << " "
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int technology = mdt_id->technology(id);
      if (technology != mdt_id->technologyIndex("MDT")) {
	log << MSG::ERROR
	    << " testMDTIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << mdt_id->technologyIndex("MDT")
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if ("MDT" != mdt_id->technologyString(0) || technology != 0) {
	log << MSG::ERROR
	    << " testMDTIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << mdt_id->technologyIndex("MDT")
            << mdt_id->show_to_string(id)
	    << endreq;
	error = true;
      }
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of MDT ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of MDT ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}


StatusCode 	
TestMuonIds::testMDTDetElem	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing MDT det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const MdtIdHelper*  mdt_id   = 0;
    StatusCode sc = m_detStore->retrieve(mdt_id, "MDTIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get MdtIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
//    mdt_id->test_module_packing();


    log << MSG::INFO 
	<< "IDDICT of MDT elements ids "
	<< endreq;

    std::set<Identifier> ids;


    int nids = 0;
    
    MdtIdHelper::const_id_iterator idIt       = mdt_id->module_begin();
    MdtIdHelper::const_id_iterator idItEnd    = mdt_id->module_end();
	
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
	ids.insert((*idIt));
	nids++;
    }

    idIt       = mdt_id->module_begin();
    idItEnd    = mdt_id->module_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    bool error = false;
    
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
	if ((*firstIds) != (*idIt) ) {
	    log << MSG::ERROR 
		<< " IDs not the same: N " << ncount
		<< " id manually sorted " << mdt_id->show_to_string((*firstIds))
		<< " id already sorted " << mdt_id->show_to_string((*idIt))
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
	<< "Number of IDDICT det elem MDT ids " << nids
	<< endreq;
    log << MSG::INFO 
	<< "Number of non-duplicate IDDICT MDT ids " << ids.size()
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
    MdtIdHelper::size_type hashMax = mdt_id->module_hash_max();

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
	mdt_id->get_module_hash ((*firstId), hashedId);
	if(ncount != hashedId) {
	    log << MSG::ERROR 
		<< " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
		<< mdt_id->show_to_string((*firstId))
		<< endreq;
	    error = true;
	}

	// Get packed again
	Identifier id;
        IdContext idContext = mdt_id->module_context();
	mdt_id->get_id(hashedId, id, &idContext);
	Identifier packedId1 = id;
	if (packedId != packedId1) {
	    log << MSG::ERROR 
		<< " Unable to get packed from hash: N " << ncount
		<< " hash " << hashedId
		<< " id " << mdt_id->show_to_string((*firstId))
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
	<< "Number of non-duplicate PACKED IDDICT MDT ids " << packedIds.size()
	<< endreq;

    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    unsigned int nelems = 0;
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
		<< " id " << mdt_id->show_to_string((*firstId))
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
	<< " Successful end of MDT test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testMDTNeighbors	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing MDT det elem neighbor ids " 
	<< endreq;

    // Test packing of ids
    const MdtIdHelper*  mdt_id   = 0;
    StatusCode sc = m_detStore->retrieve(mdt_id, "MDTIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get MdtIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    
    // Save context for MDT module id
    IdContext	idContext = mdt_id->module_context();    

    // Check neighbors
//     mdt_el_it first1 = m_muonMgr->mdt_elements_begin();
//     mdt_el_it last1  = m_muonMgr->mdt_elements_end();
//     unsigned int mdtMax = mdt_id->module_hash_max();
//     unsigned int nelems = 0;
    bool error = false;
    int ntest = 0;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	bool prevPhiFlag = false;
// 	bool nextPhiFlag = false;
// 	bool prevEtaFlag = false;
// 	bool nextEtaFlag = false;
	
// 	IdentifierHash id = ((*first1)->identifyHash());
// 	if (nelems < mdtMax) {
// 	    // Pixels
// 	    IdentifierHash prevPhi;
// 	    if (!mdt_id->get_prev_in_phi(id, prevPhi)) {
// 		// check that previous phi is the correct one
// 		Identifier id1;
// 		if (!mdt_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!mdt_id->get_id(prevPhi, prevId, &idContext)){
// 			int phi    = mdt_id->stationPhi(id1);
// 			int prePhi = mdt_id->stationPhi(prevId);
// 			if (phi != prePhi + 1) {
// 			    if (prePhi != mdt_id->stationPhiMax(id1)) {
// 				log << MSG::ERROR 
// 				    << " MDTDetElem previous incorrect: phi " 
// 				    << phi
// 				    << " prev phi " << prePhi
// 				    << " id " << mdt_id->show_to_string(id1) 
// 				    << " prev id " << mdt_id->show_to_string(prevId) 
// 				    << " phi max " << mdt_id->stationPhiMax(id1)
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				prevPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    prevPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a previous phi
// 		log << MSG::ERROR 
// 		    << " MdtDetElem previous not found " 
// 		    << endreq;
// 		error = true;
// 	    }

// 	    IdentifierHash nextPhi;
// 	    if (!mdt_id->get_next_in_phi(id, nextPhi)) {
// 		// check that next phi is the correct one
// 		Identifier id1;
// 		if (!mdt_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!mdt_id->get_id(nextPhi, nextId, &idContext)){
// 			int phi    =mdt_id->stationPhi(id1);
// 			int nextPhi = mdt_id->stationPhi(nextId);
// 			if (phi != nextPhi - 1) {
// 			    if (nextPhi != 0) {
// 				log << MSG::ERROR 
// 				    << " MdtDetElem next incorrect: phi " 
// 				    << phi
// 				    << " next phi " << nextPhi
// 				    << " id " << mdt_id->show_to_string(id1) 
// 				    << " next id " << mdt_id->show_to_string(nextId) 
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				nextPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    nextPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a next phi
// 		log << MSG::ERROR 
// 		    << " MdtDetElem next not found " 
// 		    << endreq;
// 		error = true;
// 	    }
	    

// 	    IdentifierHash prevEta;
// 	    if (!mdt_id->get_prev_in_eta(id, prevEta)) {
// 		// check that previous eta is the correct one
// 		Identifier id1;
// 		if (!mdt_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!mdt_id->get_id(prevEta, prevId, &idContext)){
// 			int eta    = mdt_id->stationEta(id1);
// 			int preEta = mdt_id->stationEta(prevId);
// 			if (eta != preEta + 1) {
// 			    log << MSG::ERROR 
// 				<< " MdtDetElem previous incorrect: eta " 
// 				<< eta
// 				<< " prev eta " << preEta
// 				<< " id " << mdt_id->show_to_string(id1) 
// 				<< " prev id " << mdt_id->show_to_string(prevId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    prevEtaFlag = true;
// 			}
// 		    }
// 		    else {
// 			int eta    = mdt_id->stationEta(id1);
// 			log << MSG::ERROR 
// 			    << " MdtDetElem unable to get prevEta id: eta " 
// 			    << eta
// 			    << " prev eta " << prevEta
// 			    << " id " << mdt_id->show_to_string(id1) 
// 			    << endreq;
// 		    }
// 		}
// 		else {
// 		    log << MSG::ERROR 
// 			<< " MdtDetElem unable to get id: " 
// 			<< " id " << id
// 			<< endreq;
// 		}
// 	    }
// 	    else {
// 		// could not get previous eta - must be first one
// 		Identifier id1;
// 		if (!mdt_id->get_id(id, id1, &idContext)){
// 		    int eta    = mdt_id->stationEta(id1);
// 		    if (eta != mdt_id->stationEtaMin(id1)) {
// 			log << MSG::ERROR 
// 			    << " MdtDetElem previous incorrect: eta " 
// 			    << eta
// 			    << " min eta " << mdt_id->stationEtaMin(id1)
// 			    << " id " << mdt_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			prevEtaFlag = true;
// 		    }
// 		}
// 	    }

// 	    IdentifierHash nextEta;
// 	    if (!mdt_id->get_next_in_eta(id, nextEta)) {
// 		// check that next eta is the correct one
// 		Identifier id1;
// 		if (!mdt_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!mdt_id->get_id(nextEta, nextId, &idContext)){
// 			int eta    = mdt_id->stationEta(id1);
// 			int nextEta = mdt_id->stationEta(nextId);
// 			if (eta != nextEta - 1) {
// 			    log << MSG::ERROR 
// 				<< " MdtDetElem next incorrect: eta " 
// 				<< eta
// 				<< " next eta " << nextEta
// 				<< " id " << mdt_id->show_to_string(id1) 
// 				<< " next id " << mdt_id->show_to_string(nextId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    nextEtaFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// could not get next eta - must be last one
// 		Identifier id1;
// 		if (!mdt_id->get_id(id, id1, &idContext)){
// 		    int eta    = mdt_id->stationEta(id1);
// 		    if (eta != mdt_id->stationEtaMax(id1)) {
// 			log << MSG::ERROR 
// 			    << " MdtDetElem next incorrect: eta " 
// 			    << eta
// 			    << " max eta " << mdt_id->stationEtaMax(id1)
// 			    << " id " << mdt_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			nextEtaFlag = true;
// 		    }
// 		}
// 	    }
// 	    if (prevPhiFlag && nextPhiFlag && prevEtaFlag && nextEtaFlag) {
// 		ntest++;
// 	    }
// 	    else {
// 		Identifier id1;
// 		mdt_id->get_id(id, id1, &idContext);
// 		log << MSG::ERROR 
// 		    << " MDT elem neighbor test: flags: prevPhi, nextPhi, prevEta, nextEta " 
// 		    << prevPhiFlag << " " << nextPhiFlag << " " 
// 		    << prevEtaFlag << " " << nextEtaFlag
// 		    << " id " << mdt_id->show_to_string(id1) 
// 		    << endreq;
// 	    }
// 	}
//    }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of neighbors for MDT DetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of neighbors for " << ntest << " MDT DetElems in mgr OK " 
	    << endreq;
    }


    log << MSG::INFO 
	<< " Successful end of MDT neighbor test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testCSCIds		(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing CSC det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const CscIdHelper*  csc_id   = 0;
    StatusCode sc = m_detStore->retrieve(csc_id, "CSCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get CscIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 

    // Save context for CSC module id
    IdContext	context = csc_id->module_context();    

    bool error = false;


    // Check is_barrel

    // neg endcap
    Identifier  new_id =  csc_id->elementID(50, -1, 3);
    if(csc_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testCSCIds new id is NOT endcap " 
	    << csc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // barrel
    new_id =  csc_id->elementID(51, -1, 4);
    if(csc_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testCSCIds new id is NOT endcap " 
	    << csc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // pos endcap
    new_id =  csc_id->elementID(50, 1, 5);
    if(csc_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testCSCIds new id is NOT endcap " 
	    << csc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of CSC isBarrel  FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of CSC isBarrel OK " 
	    << endreq;

    }
    std::vector<Identifier> elementIds = csc_id->idVector();
    std::vector<Identifier>::const_iterator first = elementIds.begin();
    std::vector<Identifier>::const_iterator last  = elementIds.end();
    for (; first != last; ++first) {
      Identifier id = (*first);
      csc_id->test_id(id,context);
      if (!csc_id->validElement(id)) {
	log << MSG::ERROR
	    << " testCSCIds invalid identifier "
	    << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      int stationName = csc_id->stationName(id);
      std::string name = csc_id->stationNameString(stationName);
      if (csc_id->isEndcap(id) && name[0] != 'C') {
	log << MSG::ERROR
	    << " testCSCIds problem in isEndcap() and stationName() "
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (!csc_id->isBarrel(id) && name[0] != 'C') {
	log << MSG::ERROR
	    << " testCSCIds problem in isBarrel() and stationName() "
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (csc_id->stationNameIndex(name) != csc_id->stationName(id)) {
	log << MSG::ERROR
	    << " testCSCIds problem in stationNameIndex() and stationName() "
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int etaMin = csc_id->stationEtaMin(id);
      const int etaMax = csc_id->stationEtaMax(id);
      const int eta    = csc_id->stationEta(id);
      if (eta < etaMin || eta > etaMax) {
	log << MSG::ERROR
	    << " testCSCIds problem in etaMin, etaMax or eta "
	    << "etaMin = " << etaMin
	    << "etaMax = " << etaMax
	    << "eta    = " << eta << " "
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int phiMin = csc_id->stationPhiMin(id);
      const int phiMax = csc_id->stationPhiMax(id);
      const int phi    = csc_id->stationPhi(id);
      if (phi < phiMin || phi > phiMax) {
	log << MSG::ERROR
	    << " testCSCIds problem in phiMin, phiMax or phi "
	    << "phiMin = " << phiMin
	    << "phiMax = " << phiMax
	    << "phi    = " << phi << " "
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int technology = csc_id->technology(id);
      if (technology != csc_id->technologyIndex("CSC")) {
	log << MSG::ERROR
	    << " testCSCIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << csc_id->technologyIndex("CSC")
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if ("CSC" != csc_id->technologyString(1) || technology != 1) {
	log << MSG::ERROR
	    << " testCSCIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << csc_id->technologyIndex("CSC")
            << csc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of CSC ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of CSC ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}


StatusCode 	
TestMuonIds::testCSCDetElem	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing CSC det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const CscIdHelper*  csc_id   = 0;
    StatusCode sc = m_detStore->retrieve(csc_id, "CSCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get CscIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    csc_id->test_module_packing();
    
    // Save context for CSC module id
    IdContext	idContext = csc_id->module_context();    

    // Check the MuonMgr's CscDetElem hash ids
//     csc_el_it first1 = m_muonMgr->csc_elements_begin();
//     csc_el_it last1  = m_muonMgr->csc_elements_end();
//     unsigned int cscMax = csc_id->module_hash_max();
     unsigned int nelems = 0;
     bool error = false;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	IdentifierHash hashId = ((*first1)->identifyHash());
// 	if (nelems < cscMax) {
// 	    if(hashId != nelems) {
// 		log << MSG::ERROR 
// 		    << " Pixel CscDetElem hash incorrect " << hashId
// 		    << " nelems " << nelems
// 		    << " csc hash Max " << cscMax
// 		    << endreq;
// 		error = true;
// 	    }
// 	}
//     }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of hashes for CSCDetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of hashes for CscDetElems in mgr OK " 
	    << endreq;
    }

    // Get set of identifiers from elements
    std::set<Identifier> ids;


    int nids = 0;
    
    CscIdHelper::const_id_iterator idIt       = csc_id->module_begin();
    CscIdHelper::const_id_iterator idItEnd    = csc_id->module_end();
	
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
	ids.insert((*idIt));
	nids++;
    }

    idIt       = csc_id->module_begin();
    idItEnd    = csc_id->module_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    error = false;
    
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
	if ((*firstIds) != (*idIt) ) {
	    log << MSG::ERROR 
		<< " IDs not the same: N " << ncount
		<< " id manually sorted " << csc_id->show_to_string((*firstIds))
		<< " id already sorted " << csc_id->show_to_string((*idIt))
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
	<< "Number of IDDICT det elem CSC ids " << nids
	<< endreq;
    log << MSG::INFO 
	<< "Number of non-duplicate IDDICT CSC ids " << ids.size()
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
    CscIdHelper::size_type hashMax = csc_id->module_hash_max();

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
	csc_id->get_hash ((*firstId), hashedId, &idContext);
	if(ncount != hashedId) {
	    log << MSG::ERROR 
		<< " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
		<< csc_id->show_to_string((*firstId))
		<< endreq;
	    error = true;
	}

	// Get packed again
	Identifier id;
	csc_id->get_id(hashedId, id, &idContext);
	Identifier packedId1 = id;
	if (packedId != packedId1) {
	    log << MSG::ERROR 
		<< " Unable to get packed from hash: N " << ncount
		<< " hash " << hashedId
		<< " id " << csc_id->show_to_string((*firstId))
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
	<< "Number of non-duplicate PACKED IDDICT CSC ids " << packedIds.size()
	<< endreq;

    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    nelems = 0;
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
		<< " id " << csc_id->show_to_string((*firstId))
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
	<< " Successful end of CSC test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testCSCNeighbors	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing CSC det elem neighbor ids " 
	<< endreq;

    // Test packing of ids
    const CscIdHelper*  csc_id   = 0;
    StatusCode sc = m_detStore->retrieve(csc_id, "CSCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get CscIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    
    // Save context for CSC module id
    IdContext	idContext = csc_id->module_context();    

    // Check neighbors
//     csc_el_it first1 = m_muonMgr->csc_elements_begin();
//     csc_el_it last1  = m_muonMgr->csc_elements_end();
//     unsigned int cscMax = csc_id->module_hash_max();
//     unsigned int nelems = 0;
     bool error = false;
     int ntest = 0;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	bool prevPhiFlag = false;
// 	bool nextPhiFlag = false;
// 	bool prevEtaFlag = false;
// 	bool nextEtaFlag = false;
	
// 	IdentifierHash id = ((*first1)->identifyHash());
// 	if (nelems < cscMax) {
// 	    // Pixels
// 	    IdentifierHash prevPhi;
// 	    if (!csc_id->get_prev_in_phi(id, prevPhi)) {
// 		// check that previous phi is the correct one
// 		Identifier id1;
// 		if (!csc_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!csc_id->get_id(prevPhi, prevId, &idContext)){
// 			int phi    = csc_id->stationPhi(id1);
// 			int prePhi = csc_id->stationPhi(prevId);
// 			if (phi != prePhi + 1) {
// 			    if (prePhi != csc_id->stationPhiMax(id1)) {
// 				log << MSG::ERROR 
// 				    << " CSCDetElem previous incorrect: phi " 
// 				    << phi
// 				    << " prev phi " << prePhi
// 				    << " id " << csc_id->show_to_string(id1) 
// 				    << " prev id " << csc_id->show_to_string(prevId) 
// 				    << " phi max " << csc_id->stationPhiMax(id1)
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				prevPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    prevPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a previous phi
// 		log << MSG::ERROR 
// 		    << " CscDetElem previous not found " 
// 		    << endreq;
// 		error = true;
// 	    }

// 	    IdentifierHash nextPhi;
// 	    if (!csc_id->get_next_in_phi(id, nextPhi)) {
// 		// check that next phi is the correct one
// 		Identifier id1;
// 		if (!csc_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!csc_id->get_id(nextPhi, nextId, &idContext)){
// 			int phi    =csc_id->stationPhi(id1);
// 			int nextPhi = csc_id->stationPhi(nextId);
// 			if (phi != nextPhi - 1) {
// 			    if (nextPhi != 0) {
// 				log << MSG::ERROR 
// 				    << " CscDetElem next incorrect: phi " 
// 				    << phi
// 				    << " next phi " << nextPhi
// 				    << " id " << csc_id->show_to_string(id1) 
// 				    << " next id " << csc_id->show_to_string(nextId) 
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				nextPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    nextPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a next phi
// 		log << MSG::ERROR 
// 		    << " CscDetElem next not found " 
// 		    << endreq;
// 		error = true;
// 	    }
	    

// 	    IdentifierHash prevEta;
// 	    if (!csc_id->get_prev_in_eta(id, prevEta)) {
// 		// check that previous eta is the correct one
// 		Identifier id1;
// 		if (!csc_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!csc_id->get_id(prevEta, prevId, &idContext)){
// 			int eta    = csc_id->stationEta(id1);
// 			int preEta = csc_id->stationEta(prevId);
// 			if (eta != preEta + 1) {
// 			    log << MSG::ERROR 
// 				<< " CscDetElem previous incorrect: eta " 
// 				<< eta
// 				<< " prev eta " << preEta
// 				<< " id " << csc_id->show_to_string(id1) 
// 				<< " prev id " << csc_id->show_to_string(prevId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    prevEtaFlag = true;
// 			}
// 		    }
// 		    else {
// 			int eta    = csc_id->stationEta(id1);
// 			log << MSG::ERROR 
// 			    << " CscDetElem unable to get prevEta id: eta " 
// 			    << eta
// 			    << " prev eta " << prevEta
// 			    << " id " << csc_id->show_to_string(id1) 
// 			    << endreq;
// 		    }
// 		}
// 		else {
// 		    log << MSG::ERROR 
// 			<< " CscDetElem unable to get id: " 
// 			<< " id " << id
// 			<< endreq;
// 		}
// 	    }
// 	    else {
// 		// could not get previous eta - must be first one
// 		Identifier id1;
// 		if (!csc_id->get_id(id, id1, &idContext)){
// 		    int eta    = csc_id->stationEta(id1);
// 		    if (eta != csc_id->stationEtaMin(id1)) {
// 			log << MSG::ERROR 
// 			    << " CscDetElem previous incorrect: eta " 
// 			    << eta
// 			    << " min eta " << csc_id->stationEtaMin(id1)
// 			    << " id " << csc_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			prevEtaFlag = true;
// 		    }
// 		}
// 	    }

// 	    IdentifierHash nextEta;
// 	    if (!csc_id->get_next_in_eta(id, nextEta)) {
// 		// check that next eta is the correct one
// 		Identifier id1;
// 		if (!csc_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!csc_id->get_id(nextEta, nextId, &idContext)){
// 			int eta    = csc_id->stationEta(id1);
// 			int nextEta = csc_id->stationEta(nextId);
// 			if (eta != nextEta - 1) {
// 			    log << MSG::ERROR 
// 				<< " CscDetElem next incorrect: eta " 
// 				<< eta
// 				<< " next eta " << nextEta
// 				<< " id " << csc_id->show_to_string(id1) 
// 				<< " next id " << csc_id->show_to_string(nextId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    nextEtaFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// could not get next eta - must be last one
// 		Identifier id1;
// 		if (!csc_id->get_id(id, id1, &idContext)){
// 		    int eta    = csc_id->stationEta(id1);
// 		    if (eta != csc_id->stationEtaMax(id1)) {
// 			log << MSG::ERROR 
// 			    << " CscDetElem next incorrect: eta " 
// 			    << eta
// 			    << " max eta " << csc_id->stationEtaMax(id1)
// 			    << " id " << csc_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			nextEtaFlag = true;
// 		    }
// 		}
// 	    }
// 	    if (prevPhiFlag && nextPhiFlag && prevEtaFlag && nextEtaFlag) {
// 		ntest++;
// 	    }
// 	    else {
// 		Identifier id1;
// 		csc_id->get_id(id, id1, &idContext);
// 		log << MSG::ERROR 
// 		    << " CSC elem neighbor test: flags: prevPhi, nextPhi, prevEta, nextEta " 
// 		    << prevPhiFlag << " " << nextPhiFlag << " " 
// 		    << prevEtaFlag << " " << nextEtaFlag
// 		    << " id " << csc_id->show_to_string(id1) 
// 		    << endreq;
// 	    }
// 	}
//     }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of neighbors for CSC DetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of neighbors for " << ntest << " CSC DetElems in mgr OK " 
	    << endreq;
    }


    log << MSG::INFO 
	<< " Successful end of CSC neighbor test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testRPCIds		(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing RPC det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const RpcIdHelper*  rpc_id   = 0;
    StatusCode sc = m_detStore->retrieve(rpc_id, "RPCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RpcIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 

    // Save context for RPC module id
    IdContext	context = rpc_id->module_context();    

    bool error = false;


    // Check is_barrel

    // neg barrel
    Identifier  new_id =  rpc_id->elementID(2, -6, 1, 1);
    if(!rpc_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testRPCIds new id is NOT endcap " 
	    << rpc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // barrel
    new_id =  rpc_id->elementID(3, 5, 1, 2);
    if(!rpc_id->isBarrel(new_id)) {
	log << MSG::ERROR 
	    << " testRPCIds new id is NOT barrel " 
	    << rpc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of RPC isBarrel  FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of RPC isBarrel OK " 
	    << endreq;

    }
    std::vector<Identifier> elementIds = rpc_id->idVector();
    std::vector<Identifier>::const_iterator first = elementIds.begin();
    std::vector<Identifier>::const_iterator last  = elementIds.end();
    for (; first != last; ++first) {
      Identifier id = (*first);
      rpc_id->test_id(id,context);
      if (!rpc_id->validElement(id)) {
	log << MSG::ERROR
	    << " testRPCIds invalid identifier "
	    << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      int stationName = rpc_id->stationName(id);
      std::string name = rpc_id->stationNameString(stationName);
      if (rpc_id->isBarrel(id) && name[0] != 'B') {
	log << MSG::ERROR
	    << " testRPCIds problem in isBarrel() and stationName() "
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (!rpc_id->isEndcap(id) && name[0] != 'B') {
	log << MSG::ERROR
	    << " testRPCIds problem in isEndCap() and stationName() "
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (rpc_id->stationNameIndex(name) != rpc_id->stationName(id)) {
	log << MSG::ERROR
	    << " testRPCIds problem in stationNameIndex() and stationName() "
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int etaMin = rpc_id->stationEtaMin(id);
      const int etaMax = rpc_id->stationEtaMax(id);
      const int eta    = rpc_id->stationEta(id);
      if (eta < etaMin || eta > etaMax) {
	log << MSG::ERROR
	    << " testRPCIds problem in etaMin, etaMax or eta "
	    << "etaMin = " << etaMin
	    << "etaMax = " << etaMax
	    << "eta    = " << eta << " "
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int phiMin = rpc_id->stationPhiMin(id);
      const int phiMax = rpc_id->stationPhiMax(id);
      const int phi    = rpc_id->stationPhi(id);
      if (phi < phiMin || phi > phiMax) {
	log << MSG::ERROR
	    << " testRPCIds problem in phiMin, phiMax or phi "
	    << "phiMin = " << phiMin
	    << "phiMax = " << phiMax
	    << "phi    = " << phi << " "
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int technology = rpc_id->technology(id);
      if (technology != rpc_id->technologyIndex("RPC")) {
	log << MSG::ERROR
	    << " testRPCIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << rpc_id->technologyIndex("RPC")
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if ("RPC" != rpc_id->technologyString(2) || technology != 2) {
	log << MSG::ERROR
	    << " testRPCIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << rpc_id->technologyIndex("RPC")
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int doubletRMin = rpc_id->doubletRMin(id);
      const int doubletRMax = rpc_id->doubletRMax(id);
      const int doubletR    = rpc_id->doubletR(id);
      if (doubletR < doubletRMin || doubletR > doubletRMax) {
	log << MSG::ERROR
	    << " testRPCIds problem in doubletRMin, doubletRMax or doubletR "
	    << "doubletRMin = " << doubletRMin
	    << "doubletRMax = " << doubletRMax
	    << "doubletR    = " << doubletR << " "
            << rpc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of RPC ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of RPC ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}


StatusCode 	
TestMuonIds::testRPCDetElem	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing RPC det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const RpcIdHelper*  rpc_id   = 0;
    StatusCode sc = m_detStore->retrieve(rpc_id, "RPCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RpcIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    rpc_id->test_module_packing();
    
    // Save context for RPC module id
    IdContext	idContext = rpc_id->module_context();    

    // Check the MuonMgr's RpcDetElem hash ids
//     rpc_el_it first1 = m_muonMgr->rpc_elements_begin();
//     rpc_el_it last1  = m_muonMgr->rpc_elements_end();
//     unsigned int rpcMax = rpc_id->module_hash_max();
     unsigned int nelems = 0;
     bool error = false;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	IdentifierHash hashId = ((*first1)->identifyHash());
// 	if (nelems < rpcMax) {
// 	    if(hashId != nelems) {
// 		log << MSG::ERROR 
// 		    << " Pixel RpcDetElem hash incorrect " << hashId
// 		    << " nelems " << nelems
// 		    << " rpc hash Max " << rpcMax
// 		    << endreq;
// 		error = true;
// 	    }
// 	}
//     }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of hashes for RPCDetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of hashes for RpcDetElems in mgr OK " 
	    << endreq;
    }

    // Get set of identifiers from elements


    log << MSG::INFO 
	<< "IDDICT of RPC elements ids "
	<< endreq;

    std::set<Identifier> ids;


    int nids = 0;
    
    RpcIdHelper::const_id_iterator idIt       = rpc_id->module_begin();
    RpcIdHelper::const_id_iterator idItEnd    = rpc_id->module_end();
	
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
	ids.insert((*idIt));
	nids++;
    }

    idIt       = rpc_id->module_begin();
    idItEnd    = rpc_id->module_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    error = false;
    
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
	if ((*firstIds) != (*idIt) ) {
	    log << MSG::ERROR 
		<< " IDs not the same: N " << ncount
		<< " id manually sorted " << rpc_id->show_to_string((*firstIds))
		<< " id already sorted " << rpc_id->show_to_string((*idIt))
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
	<< "Number of IDDICT det elem RPC ids " << nids
	<< endreq;
    log << MSG::INFO 
	<< "Number of non-duplicate IDDICT RPC ids " << ids.size()
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
    RpcIdHelper::size_type hashMax = rpc_id->module_hash_max();

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
	rpc_id->get_hash ((*firstId), hashedId, &idContext);
	if(ncount != hashedId) {
	    log << MSG::ERROR 
		<< " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
		<< rpc_id->show_to_string((*firstId))
		<< endreq;
	    error = true;
	}

	// Get packed again
	Identifier id;
	rpc_id->get_id(hashedId, id, &idContext);
	Identifier packedId1 = id;
	if (packedId != packedId1) {
	    log << MSG::ERROR 
		<< " Unable to get packed from hash: N " << ncount
		<< " hash " << hashedId
		<< " id " << rpc_id->show_to_string((*firstId))
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
	<< "Number of non-duplicate PACKED IDDICT RPC ids " << packedIds.size()
	<< endreq;

    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    nelems = 0;
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
		<< " id " << rpc_id->show_to_string((*firstId))
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
	<< " Successful end of RPC test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testRPCNeighbors	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing RPC det elem neighbor ids " 
	<< endreq;

    // Test packing of ids
    const RpcIdHelper*  rpc_id   = 0;
    StatusCode sc = m_detStore->retrieve(rpc_id, "RPCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RpcIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    
    // Save context for RPC module id
    IdContext	idContext = rpc_id->module_context();    

    // Check neighbors
//     typedef MuonDetDescrManager::rpc_elem_const_iterator  rpc_el_it;
//     rpc_el_it first1 = m_muonMgr->rpc_elements_begin();
//     rpc_el_it last1  = m_muonMgr->rpc_elements_end();
//     unsigned int rpcMax = rpc_id->module_hash_max();
//     unsigned int nelems = 0;
     bool error = false;
     int ntest = 0;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	bool prevPhiFlag = false;
// 	bool nextPhiFlag = false;
// 	bool prevEtaFlag = false;
// 	bool nextEtaFlag = false;
	
// 	IdentifierHash id = ((*first1)->identifyHash());
// 	if (nelems < rpcMax) {
// 	    // Pixels
// 	    IdentifierHash prevPhi;
// 	    if (!rpc_id->get_prev_in_phi(id, prevPhi)) {
// 		// check that previous phi is the correct one
// 		Identifier id1;
// 		if (!rpc_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!rpc_id->get_id(prevPhi, prevId, &idContext)){
// 			int phi    = rpc_id->stationPhi(id1);
// 			int prePhi = rpc_id->stationPhi(prevId);
// 			if (phi != prePhi + 1) {
// 			    if (prePhi != rpc_id->stationPhiMax(id1)) {
// 				log << MSG::ERROR 
// 				    << " RPCDetElem previous incorrect: phi " 
// 				    << phi
// 				    << " prev phi " << prePhi
// 				    << " id " << rpc_id->show_to_string(id1) 
// 				    << " prev id " << rpc_id->show_to_string(prevId) 
// 				    << " phi max " << rpc_id->stationPhiMax(id1)
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				prevPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    prevPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a previous phi
// 		log << MSG::ERROR 
// 		    << " RpcDetElem previous not found " 
// 		    << endreq;
// 		error = true;
// 	    }

// 	    IdentifierHash nextPhi;
// 	    if (!rpc_id->get_next_in_phi(id, nextPhi)) {
// 		// check that next phi is the correct one
// 		Identifier id1;
// 		if (!rpc_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!rpc_id->get_id(nextPhi, nextId, &idContext)){
// 			int phi    =rpc_id->stationPhi(id1);
// 			int nextPhi = rpc_id->stationPhi(nextId);
// 			if (phi != nextPhi - 1) {
// 			    if (nextPhi != 0) {
// 				log << MSG::ERROR 
// 				    << " RpcDetElem next incorrect: phi " 
// 				    << phi
// 				    << " next phi " << nextPhi
// 				    << " id " << rpc_id->show_to_string(id1) 
// 				    << " next id " << rpc_id->show_to_string(nextId) 
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				nextPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    nextPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a next phi
// 		log << MSG::ERROR 
// 		    << " RpcDetElem next not found " 
// 		    << endreq;
// 		error = true;
// 	    }
	    

// 	    IdentifierHash prevEta;
// 	    if (!rpc_id->get_prev_in_eta(id, prevEta)) {
// 		// check that previous eta is the correct one
// 		Identifier id1;
// 		if (!rpc_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!rpc_id->get_id(prevEta, prevId, &idContext)){
// 			int eta    = rpc_id->stationEta(id1);
// 			int preEta = rpc_id->stationEta(prevId);
// 			if (eta != preEta + 1) {
// 			    log << MSG::ERROR 
// 				<< " RpcDetElem previous incorrect: eta " 
// 				<< eta
// 				<< " prev eta " << preEta
// 				<< " id " << rpc_id->show_to_string(id1) 
// 				<< " prev id " << rpc_id->show_to_string(prevId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    prevEtaFlag = true;
// 			}
// 		    }
// 		    else {
// 			int eta    = rpc_id->stationEta(id1);
// 			log << MSG::ERROR 
// 			    << " RpcDetElem unable to get prevEta id: eta " 
// 			    << eta
// 			    << " prev eta " << prevEta
// 			    << " id " << rpc_id->show_to_string(id1) 
// 			    << endreq;
// 		    }
// 		}
// 		else {
// 		    log << MSG::ERROR 
// 			<< " RpcDetElem unable to get id: " 
// 			<< " id " << id
// 			<< endreq;
// 		}
// 	    }
// 	    else {
// 		// could not get previous eta - must be first one
// 		Identifier id1;
// 		if (!rpc_id->get_id(id, id1, &idContext)){
// 		    int eta    = rpc_id->stationEta(id1);
// 		    if (eta != rpc_id->stationEtaMin(id1)) {
// 			log << MSG::ERROR 
// 			    << " RpcDetElem previous incorrect: eta " 
// 			    << eta
// 			    << " min eta " << rpc_id->stationEtaMin(id1)
// 			    << " id " << rpc_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			prevEtaFlag = true;
// 		    }
// 		}
// 	    }

// 	    IdentifierHash nextEta;
// 	    if (!rpc_id->get_next_in_eta(id, nextEta)) {
// 		// check that next eta is the correct one
// 		Identifier id1;
// 		if (!rpc_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!rpc_id->get_id(nextEta, nextId, &idContext)){
// 			int eta    = rpc_id->stationEta(id1);
// 			int nextEta = rpc_id->stationEta(nextId);
// 			if (eta != nextEta - 1) {
// 			    log << MSG::ERROR 
// 				<< " RpcDetElem next incorrect: eta " 
// 				<< eta
// 				<< " next eta " << nextEta
// 				<< " id " << rpc_id->show_to_string(id1) 
// 				<< " next id " << rpc_id->show_to_string(nextId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    nextEtaFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// could not get next eta - must be last one
// 		Identifier id1;
// 		if (!rpc_id->get_id(id, id1, &idContext)){
// 		    int eta    = rpc_id->stationEta(id1);
// 		    if (eta != rpc_id->stationEtaMax(id1)) {
// 			log << MSG::ERROR 
// 			    << " RpcDetElem next incorrect: eta " 
// 			    << eta
// 			    << " max eta " << rpc_id->stationEtaMax(id1)
// 			    << " id " << rpc_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			nextEtaFlag = true;
// 		    }
// 		}
// 	    }
// 	    if (prevPhiFlag && nextPhiFlag && prevEtaFlag && nextEtaFlag) {
// 		ntest++;
// 	    }
// 	    else {
// 		Identifier id1;
// 		rpc_id->get_id(id, id1, &idContext);
// 		log << MSG::ERROR 
// 		    << " RPC elem neighbor test: flags: prevPhi, nextPhi, prevEta, nextEta " 
// 		    << prevPhiFlag << " " << nextPhiFlag << " " 
// 		    << prevEtaFlag << " " << nextEtaFlag
// 		    << " id " << rpc_id->show_to_string(id1) 
// 		    << endreq;
// 	    }
// 	}
//     }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of neighbors for RPC DetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of neighbors for " << ntest << " RPC DetElems in mgr OK " 
	    << endreq;
    }


    log << MSG::INFO 
	<< " Successful end of RPC neighbor test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testTGCIds		(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing TGC det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const TgcIdHelper*  tgc_id   = 0;
    StatusCode sc = m_detStore->retrieve(tgc_id, "TGCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get TgcIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 

    // Save context for TGC module id
    IdContext	context = tgc_id->module_context();    

    bool error = false;


    // Check is_barrel

    // neg forward
    Identifier  new_id =  tgc_id->elementID(41, -1, 2);
    if(!tgc_id->isForward(new_id)) {
	log << MSG::ERROR 
	    << " testTGCIds new id is NOT forward " 
	    << tgc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // neg endcap
    new_id =  tgc_id->elementID(42, -2, 2);
    if(tgc_id->isForward(new_id)) {
	log << MSG::ERROR 
	    << " testTGCIds new id is NOT endcap " 
	    << tgc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }

    // pos forward
    new_id =  tgc_id->elementID(43, 1, 9);
    if(!tgc_id->isForward(new_id)) {
	log << MSG::ERROR 
	    << " testTGCIds new id is NOT forward " 
	    << tgc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }
    // pos endcap
    new_id =  tgc_id->elementID(44, 3, 47);
    if(tgc_id->isForward(new_id)) {
	log << MSG::ERROR 
	    << " testTGCIds new id is NOT endcap " 
	    << tgc_id->show_to_string(new_id)
	    << endreq;
	error = true;
    }

    if(error) {
	log << MSG::ERROR 
	    << "Check of TGC isEndcap/isForward  FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of TGC isEndcap/isForward OK " 
	    << endreq;

    }
    std::vector<Identifier> elementIds = tgc_id->idVector();
    std::vector<Identifier>::const_iterator first = elementIds.begin();
    std::vector<Identifier>::const_iterator last  = elementIds.end();
    for (; first != last; ++first) {
      Identifier id = (*first);
      tgc_id->test_id(id,context);
      if (!tgc_id->validElement(id)) {
	log << MSG::ERROR
	    << " testTGCIds invalid identifier "
	    << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      int stationName = tgc_id->stationName(id);
      std::string name = tgc_id->stationNameString(stationName);
      if (!tgc_id->isForward(id) && name[2] != 'E') {
	log << MSG::ERROR
	    << " testTGCIds problem in isEndcap() and stationName() "
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (tgc_id->isForward(id) && name[2] != 'F') {
	log << MSG::ERROR
	    << " testTGCIds problem in isForward() and stationName() "
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if (tgc_id->stationNameIndex(name) != tgc_id->stationName(id)) {
	log << MSG::ERROR
	    << " testTGCIds problem in stationNameIndex() and stationName() "
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int etaMin = tgc_id->stationEtaMin(id);
      const int etaMax = tgc_id->stationEtaMax(id);
      const int eta    = tgc_id->stationEta(id);
      if (eta < etaMin || eta > etaMax) {
	log << MSG::ERROR
	    << " testTGCIds problem in etaMin, etaMax or eta "
	    << "etaMin = " << etaMin
	    << "etaMax = " << etaMax
	    << "eta    = " << eta << " "
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int phiMin = tgc_id->stationPhiMin(id);
      const int phiMax = tgc_id->stationPhiMax(id);
      const int phi    = tgc_id->stationPhi(id);
      if (phi < phiMin || phi > phiMax) {
	log << MSG::ERROR
	    << " testTGCIds problem in phiMin, phiMax or phi "
	    << "phiMin = " << phiMin
	    << "phiMax = " << phiMax
	    << "phi    = " << phi << " "
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      const int technology = tgc_id->technology(id);
      if (technology != tgc_id->technologyIndex("TGC")) {
	log << MSG::ERROR
	    << " testTGCIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << tgc_id->technologyIndex("TGC")
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
      if ("TGC" != tgc_id->technologyString(3) || technology != 3) {
	log << MSG::ERROR
	    << " testTGCIds problem in technology() "
	    << "technolog(id) = " << technology
	    << "technologyIndex = " << tgc_id->technologyIndex("TGC")
            << tgc_id->show_to_string(id)
	    << endreq;
	error = true;
      }
    }
    if(error) {
	log << MSG::ERROR 
	    << "Check of TGC ids FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of TGC ids OK " 
	    << endreq;

    }
    return StatusCode::SUCCESS;
}


StatusCode 	
TestMuonIds::testTGCDetElem	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing TGC det elem ids'   " 
	<< endreq;

    // Test packing of ids
    const TgcIdHelper*  tgc_id   = 0;
    StatusCode sc = m_detStore->retrieve(tgc_id, "TGCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get TgcIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    tgc_id->test_module_packing();
    
    // Save context for TGC module id
    IdContext	idContext = tgc_id->module_context();    

    // Check the MuonMgr's TgcDetElem hash ids
//     typedef MuonDetDescrManager::tgc_elem_const_iterator  tgc_el_it;
//     tgc_el_it first1 = m_muonMgr->tgc_elements_begin();
//     tgc_el_it last1  = m_muonMgr->tgc_elements_end();
//     unsigned int tgcMax = tgc_id->module_hash_max();
     unsigned int nelems = 0;
     bool error = false;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	IdentifierHash hashId = ((*first1)->identifyHash());
// 	if (nelems < tgcMax) {
// 	    if(hashId != nelems) {
// 		log << MSG::ERROR 
// 		    << " Pixel TgcDetElem hash incorrect " << hashId
// 		    << " nelems " << nelems
// 		    << " tgc hash Max " << tgcMax
// 		    << endreq;
// 		error = true;
// 	    }
// 	}
//     }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of hashes for TGCDetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of hashes for TgcDetElems in mgr OK " 
	    << endreq;
    }

    // Get set of identifiers from elements


    log << MSG::INFO 
	<< "IDDICT of TGC elements ids "
	<< endreq;

    std::set<Identifier> ids;


    int nids = 0;
    
    TgcIdHelper::const_id_iterator idIt       = tgc_id->module_begin();
    TgcIdHelper::const_id_iterator idItEnd    = tgc_id->module_end();
	
    // Check the sorted list of ids

    for (; idIt != idItEnd; ++idIt) {
	ids.insert((*idIt));
	nids++;
    }

    idIt       = tgc_id->module_begin();
    idItEnd    = tgc_id->module_end();
    std::set<Identifier>::iterator    firstIds = ids.begin();
    std::set<Identifier>::iterator    lastIds  = ids.end();
    unsigned int ncount = 0;
    error = false;
    
    for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
	if ((*firstIds) != (*idIt) ) {
	    log << MSG::ERROR 
		<< " IDs not the same: N " << ncount
		<< " id manually sorted " << tgc_id->show_to_string((*firstIds))
		<< " id already sorted " << tgc_id->show_to_string((*idIt))
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
	<< "Number of IDDICT det elem TGC ids " << nids
	<< endreq;
    log << MSG::INFO 
	<< "Number of non-duplicate IDDICT TGC ids " << ids.size()
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
    TgcIdHelper::size_type hashMax = tgc_id->module_hash_max();

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
	tgc_id->get_hash ((*firstId), hashedId, &idContext);
	if(ncount != hashedId) {
	    log << MSG::ERROR 
		<< " Error: hash is NOT equal to ncount:  " << hashedId << " " << ncount << " " 
		<< tgc_id->show_to_string((*firstId))
		<< endreq;
	    error = true;
	}

	// Get packed again
	Identifier id;
	tgc_id->get_id(hashedId, id, &idContext);
	Identifier packedId1 = id;
	if (packedId != packedId1) {
	    log << MSG::ERROR 
		<< " Unable to get packed from hash: N " << ncount
		<< " hash " << hashedId
		<< " id " << tgc_id->show_to_string((*firstId))
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
	<< "Number of non-duplicate PACKED IDDICT TGC ids " << packedIds.size()
	<< endreq;

    // 
    //  Now loop over both packed and unpacked sets and assure that
    //  the order is the same
    //

    nelems = 0;
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
		<< " id " << tgc_id->show_to_string((*firstId))
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
	<< " Successful end of TGC test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode 	
TestMuonIds::testTGCNeighbors	(void) const
{
    MsgStream  log(m_msgSvc,name());

    log << MSG::INFO 
	<< " Testing TGC det elem neighbor ids " 
	<< endreq;

    // Test packing of ids
    const TgcIdHelper*  tgc_id   = 0;
    StatusCode sc = m_detStore->retrieve(tgc_id, "TGCIDHELPER"); 
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get TgcIdHelper from DetectorStore !" << endreq;
        return StatusCode::FAILURE;
    } 
    
    // Save context for TGC module id
    IdContext	idContext = tgc_id->module_context();    

    // Check neighbors
//     typedef MuonDetDescrManager::tgc_elem_const_iterator  tgc_el_it;
//     tgc_el_it first1 = m_muonMgr->tgc_elements_begin();
//     tgc_el_it last1  = m_muonMgr->tgc_elements_end();
//     unsigned int tgcMax = tgc_id->module_hash_max();
//     unsigned int nelems = 0;
     bool error = false;
     int ntest = 0;
//     for(; first1 != last1; ++first1, ++nelems) {
// 	bool prevPhiFlag = false;
// 	bool nextPhiFlag = false;
// 	bool prevEtaFlag = false;
// 	bool nextEtaFlag = false;
	
// 	IdentifierHash id = ((*first1)->identifyHash());
// 	if (nelems < tgcMax) {
// 	    // Pixels
// 	    IdentifierHash prevPhi;
// 	    if (!tgc_id->get_prev_in_phi(id, prevPhi)) {
// 		// check that previous phi is the correct one
// 		Identifier id1;
// 		if (!tgc_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!tgc_id->get_id(prevPhi, prevId, &idContext)){
// 			int phi    = tgc_id->stationPhi(id1);
// 			int prePhi = tgc_id->stationPhi(prevId);
// 			if (phi != prePhi + 1) {
// 			    if (prePhi != tgc_id->stationPhiMax(id1)) {
// 				log << MSG::ERROR 
// 				    << " TGCDetElem previous incorrect: phi " 
// 				    << phi
// 				    << " prev phi " << prePhi
// 				    << " id " << tgc_id->show_to_string(id1) 
// 				    << " prev id " << tgc_id->show_to_string(prevId) 
// 				    << " phi max " << tgc_id->stationPhiMax(id1)
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				prevPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    prevPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a previous phi
// 		log << MSG::ERROR 
// 		    << " TgcDetElem previous not found " 
// 		    << endreq;
// 		error = true;
// 	    }

// 	    IdentifierHash nextPhi;
// 	    if (!tgc_id->get_next_in_phi(id, nextPhi)) {
// 		// check that next phi is the correct one
// 		Identifier id1;
// 		if (!tgc_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!tgc_id->get_id(nextPhi, nextId, &idContext)){
// 			int phi    =tgc_id->stationPhi(id1);
// 			int nextPhi = tgc_id->stationPhi(nextId);
// 			if (phi != nextPhi - 1) {
// 			    if (nextPhi != 0) {
// 				log << MSG::ERROR 
// 				    << " TgcDetElem next incorrect: phi " 
// 				    << phi
// 				    << " next phi " << nextPhi
// 				    << " id " << tgc_id->show_to_string(id1) 
// 				    << " next id " << tgc_id->show_to_string(nextId) 
// 				    << endreq;
// 				error = true;
// 			    }
// 			    else {
// 				nextPhiFlag = true;
// 			    }
// 			}
// 			else {
// 			    nextPhiFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// should never get here - always a next phi
// 		log << MSG::ERROR 
// 		    << " TgcDetElem next not found " 
// 		    << endreq;
// 		error = true;
// 	    }
	    

// 	    IdentifierHash prevEta;
// 	    if (!tgc_id->get_prev_in_eta(id, prevEta)) {
// 		// check that previous eta is the correct one
// 		Identifier id1;
// 		if (!tgc_id->get_id(id, id1, &idContext)){
// 		    Identifier prevId;
// 		    if (!tgc_id->get_id(prevEta, prevId, &idContext)){
// 			int eta    = tgc_id->stationEta(id1);
// 			int preEta = tgc_id->stationEta(prevId);
// 			if (eta != preEta + 1) {
// 			    log << MSG::ERROR 
// 				<< " TgcDetElem previous incorrect: eta " 
// 				<< eta
// 				<< " prev eta " << preEta
// 				<< " id " << tgc_id->show_to_string(id1) 
// 				<< " prev id " << tgc_id->show_to_string(prevId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    prevEtaFlag = true;
// 			}
// 		    }
// 		    else {
// 			int eta    = tgc_id->stationEta(id1);
// 			log << MSG::ERROR 
// 			    << " TgcDetElem unable to get prevEta id: eta " 
// 			    << eta
// 			    << " prev eta " << prevEta
// 			    << " id " << tgc_id->show_to_string(id1) 
// 			    << endreq;
// 		    }
// 		}
// 		else {
// 		    log << MSG::ERROR 
// 			<< " TgcDetElem unable to get id: " 
// 			<< " id " << id
// 			<< endreq;
// 		}
// 	    }
// 	    else {
// 		// could not get previous eta - must be first one
// 		Identifier id1;
// 		if (!tgc_id->get_id(id, id1, &idContext)){
// 		    int eta    = tgc_id->stationEta(id1);
// 		    if (eta != tgc_id->stationEtaMin(id1)) {
// 			log << MSG::ERROR 
// 			    << " TgcDetElem previous incorrect: eta " 
// 			    << eta
// 			    << " min eta " << tgc_id->stationEtaMin(id1)
// 			    << " id " << tgc_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			prevEtaFlag = true;
// 		    }
// 		}
// 	    }

// 	    IdentifierHash nextEta;
// 	    if (!tgc_id->get_next_in_eta(id, nextEta)) {
// 		// check that next eta is the correct one
// 		Identifier id1;
// 		if (!tgc_id->get_id(id, id1, &idContext)){
// 		    Identifier nextId;
// 		    if (!tgc_id->get_id(nextEta, nextId, &idContext)){
// 			int eta    = tgc_id->stationEta(id1);
// 			int nextEta = tgc_id->stationEta(nextId);
// 			if (eta != nextEta - 1) {
// 			    log << MSG::ERROR 
// 				<< " TgcDetElem next incorrect: eta " 
// 				<< eta
// 				<< " next eta " << nextEta
// 				<< " id " << tgc_id->show_to_string(id1) 
// 				<< " next id " << tgc_id->show_to_string(nextId) 
// 				<< endreq;
// 			    error = true;
// 			}
// 			else {
// 			    nextEtaFlag = true;
// 			}
// 		    }
// 		}
// 	    }
// 	    else {
// 		// could not get next eta - must be last one
// 		Identifier id1;
// 		if (!tgc_id->get_id(id, id1, &idContext)){
// 		    int eta    = tgc_id->stationEta(id1);
// 		    if (eta != tgc_id->stationEtaMax(id1)) {
// 			log << MSG::ERROR 
// 			    << " TgcDetElem next incorrect: eta " 
// 			    << eta
// 			    << " max eta " << tgc_id->stationEtaMax(id1)
// 			    << " id " << tgc_id->show_to_string(id1) 
// 			    << endreq;
// 			error = true;
// 		    }
// 		    else {
// 			nextEtaFlag = true;
// 		    }
// 		}
// 	    }
// 	    if (prevPhiFlag && nextPhiFlag && prevEtaFlag && nextEtaFlag) {
// 		ntest++;
// 	    }
// 	    else {
// 		Identifier id1;
// 		tgc_id->get_id(id, id1, &idContext);
// 		log << MSG::ERROR 
// 		    << " TGC elem neighbor test: flags: prevPhi, nextPhi, prevEta, nextEta " 
// 		    << prevPhiFlag << " " << nextPhiFlag << " " 
// 		    << prevEtaFlag << " " << nextEtaFlag
// 		    << " id " << tgc_id->show_to_string(id1) 
// 		    << endreq;
// 	    }
// 	}
//     }
    
    if(error) {
	log << MSG::ERROR 
	    << "Check of neighbors for TGC DetElems in mgr FAILED " 
	    << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO 
	    << "Check of neighbors for " << ntest << " TGC DetElems in mgr OK " 
	    << endreq;
    }


    log << MSG::INFO 
	<< " Successful end of TGC neighbor test   " 
	<< endreq;
    
    return StatusCode::SUCCESS;
}

