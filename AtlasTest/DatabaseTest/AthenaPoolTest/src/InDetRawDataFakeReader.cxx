/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRawDataFakeReader.cxx
 *
 * @brief Test Algorithm for POOL I/O, reads InDetRawData from
 * transient store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: InDetRawDataFakeReader.cxx,v 1.9 2007-03-03 19:09:16 schaffer Exp $
 *
 */

// Silicon trackers includes
#include "InDetRawDataFakeReader.h"

#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"

//InDetRawData includes
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"

//  #include "InDetRawData/InDetRawDataContainer.h"
//  #include "InDetRawData/InDetRawDataCollection.h"
//  #include "InDetRawData/Pixel1RawData.h"
//  #include "InDetRawData/InDetRawDataCLASS_DEF.h"
//  #include "InDetRawData/PixelRDORawData.h"
//  #include "InDetRawUtils/PixelHid2RESrcID.h"

// Id includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
//#include "InDetDetDescr/InDet_DetDescrManager.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"

// test includes
#include "InDetRawDataFakeCreator.h"

#include <map>

// Constructor with parameters:
InDetRawDataFakeReader::InDetRawDataFakeReader(const std::string &name, 
				       ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0),
    m_pixelId(0),
    m_sctId(0),
    m_trtId(0)

{}

// Initialize method:
StatusCode InDetRawDataFakeReader::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "InDetRawDataFakeReader::initialize()" << endreq;

    // get StoreGate service
    StatusCode sc=service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
	log << MSG::ERROR << "StoreGate service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {}

    // get DetectorStore service
    StoreGateSvc* detStore;
    sc=service("DetectorStore",detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR << "DetectorStore service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << " Found DetectorStore " << endreq;
    }
    
    // Get the pixel helper from the detector store
    const DataHandle<PixelID> pixel_id;
    sc = detStore->retrieve(pixel_id, "PixelID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not get PixelID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the PixelID helper. " << endreq;
    }

    m_pixelId = pixel_id;

    // Get the sct helper from the detector store
    const DataHandle<SCT_ID> sctId;
    sc = detStore->retrieve(sctId, "SCT_ID");
    if (sc.isFailure()) {
  	log << MSG::ERROR << "Could not get SCT_ID helper !" << endreq;
  	return StatusCode::FAILURE;
    } 
    else {
  	log << MSG::DEBUG << " Found the SCT_ID helper. " << endreq;
    }

    m_sctId = sctId;

    // Get the trt helper from the detector store
    const DataHandle<TRT_ID> trtId;
    sc = detStore->retrieve(trtId, "TRT_ID");
    if (sc.isFailure()) {
  	log << MSG::ERROR << "Could not get TRT_ID helper !" << endreq;
  	return StatusCode::FAILURE;
    } 
    else {
  	log << MSG::DEBUG << " Found the TRT_ID helper. " << endreq;
    }

    m_trtId = trtId;


    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode InDetRawDataFakeReader::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeReader::execute()" << endreq;

    // Check pixel
    StatusCode sc = checkPixel();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Failed pixel check. " << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Pixel check OK " << endreq;
    }

    // Check sct
    sc = checkSCT();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Failed SCT check. " << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "SCT check OK " << endreq;
    }

    // Check TRT
    sc = checkTRT();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Failed trt check. " << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "TRT check OK " << endreq;
    }

    
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeReader::checkPixel() const
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeReader::checkPixel()" << endreq;

    // retieve the Pixel IdentifiableContainer each event
    const PixelRDO_Container* pixCont;
    StatusCode sc=m_storeGate->retrieve(pixCont,"PixelRDOs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "PixelRDOs" 
	    << "' could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "PixelRDOs"
	    << "' retrieved from StoreGate" << endreq;
    }

  
    // loop on all RDO collections read in and check them
    int nmatch = 0;
    bool error = false;
    InDetRawDataFakeCreator creator;
    IdContext cntx = m_pixelId->wafer_context();

    log << MSG::DEBUG << "PixelRDO_Container size " 
	<< pixCont->size() << " "
	<< endreq;

    // Check for non-zero length of container
    if (pixCont->begin() == pixCont->end()) {
	log << MSG::ERROR << "Container '" << "PixelRDOs" 
	    << "' is EMPTY !" << endreq;
	return StatusCode::FAILURE;
    }


     // Retrieve the digits by class, not by name
//      const DataHandle<InDetRawDataCollection<Pixel1RawData> > rdoCollectionsBegin;
//      const DataHandle<InDetRawDataCollection<Pixel1RawData> > rdoCollectionsEnd;
//      sc = m_storeGate->retrieve(rdoCollectionsBegin, rdoCollectionsEnd);
//      if (sc.isFailure()) {
//  	log << MSG::WARNING << "Could not find the RDOs"<< endreq;
//  	return StatusCode::FAILURE;
//      }
//      else {
//  	log << MSG::DEBUG << "Retrieved pixel collections from SG"<< endreq;
//      }
     

    // loop on all rdo collections. 
//     for(; rdoCollectionsBegin!=rdoCollectionsEnd; ++rdoCollectionsBegin) {
	    
      for(PixelRDO_Container::const_iterator it=pixCont->begin() ; 
        	it!=pixCont->end() ; ++it) {

	const InDetRawDataCollection<PixelRDORawData>* RDO_Collection(&(**it));
//	const InDetRawDataCollection<Pixel1RawData>* RDO_Collection(&(*rdoCollectionsBegin.cptr()));

	log << MSG::DEBUG << "Read RDO collection: size, ids " 
	    << RDO_Collection->size() << " "
	    << m_pixelId->show_to_string(RDO_Collection->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec << endreq;

	// create a new pixel RDO collection
	const InDetRawDataCollection<PixelRDORawData> *rdocoll = 
	    creator.createPixelRawDataColl(RDO_Collection->identifyHash(), m_pixelId, log);

	log << MSG::DEBUG << "Test RDO collection: size, ids " 
	    << rdocoll->size() << " "
	    << m_pixelId->show_to_string(rdocoll->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec << endreq;


	// Use one of the specific clustering AlgTools to make clusters
	typedef InDetRawDataCollection<Pixel1RawData> RDO1_Collection_type;
	typedef InDetRawDataCollection<PixelRDORawData> RDO_Collection_type;

	// loop on all RDOs
//	RDO1_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin(); 
//	RDO1_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
 	RDO_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin(); 
 	RDO_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
	RDO_Collection_type::const_iterator nextRDO1 = rdocoll->begin(); 
	RDO_Collection_type::const_iterator lastRDO1 = rdocoll->end();
	for(; nextRDO!=lastRDO && nextRDO1!=lastRDO1 ; ++nextRDO, ++nextRDO1) {
	    // Look for match
	    if (((*nextRDO)->identify() != (*nextRDO1)->identify()) ||
		((*nextRDO)->getWord() != (*nextRDO1)->getWord())) {
		
		log << MSG::ERROR << "RDO's do NOT match: ids, word " 
		    << m_pixelId->show_to_string((*nextRDO)->identify()) << " " 
		    << m_pixelId->show_to_string((*nextRDO1)->identify()) << " " 
		    << MSG::hex << (*nextRDO)->getWord() << " "
		    << MSG::hex << (*nextRDO1)->getWord() << " "
		    << endreq;
		error = true;
	    }
	    else {
		nmatch++;
	    }
	}
	log << MSG::DEBUG << "Matching RDOs " << nmatch << endreq;
    
	delete rdocoll;
    }

    // print out pixel element links

    // Retrive the element link vector
    const PixelRDOElemLinkVec* linkVec = 0;
    sc = m_storeGate->retrieve(linkVec, "PixelRDOELs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "PixelRDOElemLinkVec could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "PixelRDOElemLinkVec retrieved from StoreGate" << endreq;
    }
    
    log << MSG::DEBUG << "PixelRDOElemLinkVec found with "
        << linkVec->pixelRDOs().size() 
        << "  RDOs " << endreq;
    for(unsigned int i = 0; i < linkVec->pixelRDOs().size(); ++i) {
        // Print out rdos from element link
	PixelRDOElemLinkVec::elem_type rdo  = linkVec->pixelRDOs()[i];
        PixelRDORawData* ncrdo = const_cast<PixelRDORawData*>(*rdo);
        log << MSG::DEBUG << "RDO ID=" << m_pixelId->show_to_string((*rdo)->identify());
        log << MSG::DEBUG << "RDO ToT=" << ncrdo->getToT() << endreq;
    }


    if (error) {
	log << MSG::INFO << "ERROR reading and checking Pixel RDO collections  " << endreq;
    }
    else {
	log << MSG::INFO << "Successfully read and checked Pixel RDO collections  " << endreq;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeReader::checkSCT() const
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeReader::execute()" << endreq;


    // retieve the SCT IdentifiableContainer each event
    const SCT_RDO_Container* sctCont;
    StatusCode sc=m_storeGate->retrieve(sctCont,"SCT_RDOs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "SCT_RDOs" 
	    << "' could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "SCT_RDOs"
	    << "' retrieved from StoreGate" << endreq;
    }

  
    // loop on all RDO collections read in and check them
    int nmatch = 0;
    bool error = false;
    InDetRawDataFakeCreator creator;
    IdContext cntx = m_sctId->wafer_context();

    log << MSG::DEBUG << "SCT_RDO_Container size " 
	<< sctCont->size() << " "
	<< endreq;

    // Check for non-zero length of container
    if (sctCont->begin() == sctCont->end()) {
	log << MSG::ERROR << "Container '" << "SCT_RDOs" 
	    << "' is EMPTY !" << endreq;
	return StatusCode::FAILURE;
    }

    for(SCT_RDO_Container::const_iterator it=sctCont->begin() ; 
      	it!=sctCont->end() ; ++it) {

	const InDetRawDataCollection<SCT_RDORawData>* RDO_Collection(&(**it));

	log << MSG::DEBUG << "Read RDO collection: size, ids " 
	    << RDO_Collection->size() << " "
	    << m_sctId->show_to_string(RDO_Collection->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec << endreq;

	// create a new sct RDO collection
	const InDetRawDataCollection<SCT_RDORawData> *rdocoll = 
	    creator.createSCT_RawDataColl(RDO_Collection->identifyHash(), m_sctId, log);

	log << MSG::DEBUG << "Test RDO collection: size, ids " 
	    << rdocoll->size() << " "
	    << m_sctId->show_to_string(rdocoll->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec << endreq;


	// Use one of the specific clustering AlgTools to make clusters
	typedef InDetRawDataCollection<SCT_RDORawData> RDO_Collection_type;

	// loop on all RDOs
	RDO_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin(); 
	RDO_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
	RDO_Collection_type::const_iterator nextRDO1 = rdocoll->begin(); 
	RDO_Collection_type::const_iterator lastRDO1 = rdocoll->end();
	for(; nextRDO!=lastRDO && nextRDO1!=lastRDO1 ; ++nextRDO, ++nextRDO1) {
	    // Look for match
	    if (((*nextRDO)->identify() != (*nextRDO1)->identify()) ||
		((*nextRDO)->getGroupSize() != (*nextRDO1)->getGroupSize()) ||
		((*nextRDO)->getStrip() != (*nextRDO1)->getStrip())) {
		
		log << MSG::ERROR << "RDO's do NOT match: ids, group, strip " 
		    << m_sctId->show_to_string((*nextRDO)->identify()) << " " 
		    << m_sctId->show_to_string((*nextRDO1)->identify()) << " " 
		    << MSG::hex 
		    << (*nextRDO)->getGroupSize() << " "
		    << (*nextRDO1)->getGroupSize() << " "
		    << (*nextRDO)->getStrip() << " "
		    << (*nextRDO1)->getStrip() << " "
		    << MSG::dec
		    << endreq;
		error = true;
	    }
	    else {
		nmatch++;
	    }
	}
	log << MSG::DEBUG << "Matching RDOs " << nmatch << endreq;
    
	delete rdocoll;
    }

    if (error) {
	log << MSG::INFO << "ERROR reading and checking SCT RDO collections  " << endreq;
    }
    else {
	log << MSG::INFO << "Successfully read and checked SCT RDO collections  " << endreq;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeReader::checkTRT() const
{ 
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeReader::execute()" << endreq;


    // retieve the TRT IdentifiableContainer each event
    const TRT_RDO_Container* trtCont;
    StatusCode sc=m_storeGate->retrieve(trtCont,"TRT_RDOs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "TRT_RDOs" 
	    << "' could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "TRT_RDOs"
	    << "' retrieved from StoreGate" << endreq;
    }

  
    // loop on all RDO collections read in and check them
    int nmatch = 0;
    bool error = false;
    InDetRawDataFakeCreator creator;
    IdContext cntx = m_trtId->straw_layer_context();

    log << MSG::DEBUG << "TRT_RDO_Container size " 
	<< trtCont->size() << " "
	<< endreq;

    // Check for non-zero length of container
    if (trtCont->begin() == trtCont->end()) {
	log << MSG::ERROR << "Container '" << "TRT_RDOs" 
	    << "' is EMPTY !" << endreq;
	return StatusCode::FAILURE;
    }

    for(TRT_RDO_Container::const_iterator it=trtCont->begin() ; 
      	it!=trtCont->end() ; ++it) {

	const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(&(**it));

	log << MSG::DEBUG << "Read RDO collection: size, ids " 
	    << RDO_Collection->size() << " "
	    << m_trtId->show_to_string(RDO_Collection->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec << endreq;

	// create a new trt RDO collection
	const InDetRawDataCollection<TRT_RDORawData> *rdocoll = 
	    creator.createTRT_RawDataColl(RDO_Collection->identifyHash(), m_trtId, log);

	log << MSG::DEBUG << "Test RDO collection: size, ids " 
	    << rdocoll->size() << " "
	    << m_trtId->show_to_string(rdocoll->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec << endreq;


	// Use one of the specific clustering AlgTools to make clusters
	typedef InDetRawDataCollection<TRT_RDORawData>   RDO_Collection_type;

	// loop on all RDOs
	RDO_Collection_type::const_iterator  nextRDO  = RDO_Collection->begin(); 
	RDO_Collection_type::const_iterator  lastRDO  = RDO_Collection->end();
	RDO_Collection_type::const_iterator nextRDO1 = rdocoll->begin(); 
	RDO_Collection_type::const_iterator lastRDO1 = rdocoll->end();
	for(; nextRDO!=lastRDO && nextRDO1!=lastRDO1 ; ++nextRDO, ++nextRDO1) {
	    // Look for match
	    if (((*nextRDO)->identify() != (*nextRDO1)->identify()) ||
		((*nextRDO)->highLevel() != (*nextRDO1)->highLevel()) ||
		((*nextRDO)->timeOverThreshold() != (*nextRDO1)->timeOverThreshold()) ||
		((*nextRDO)->driftTimeBin() != (*nextRDO1)->driftTimeBin())) {
		
		log << MSG::ERROR << "RDO's do NOT match: ids, highlvl, TOT, drift " 
		    << m_trtId->show_to_string((*nextRDO)->identify()) << " " 
		    << m_trtId->show_to_string((*nextRDO1)->identify()) << " " 
		    << MSG::hex 
		    << (*nextRDO)->highLevel() << " "
		    << (*nextRDO1)->highLevel() << " "
		    << (*nextRDO)->timeOverThreshold() << " "
		    << (*nextRDO1)->timeOverThreshold() << " "
		    << (*nextRDO)->driftTimeBin() << " "
		    << (*nextRDO1)->driftTimeBin() << " "
		    << MSG::dec
		    << endreq;
		error = true;
	    }
	    else {
		nmatch++;
	    }
	}
	log << MSG::DEBUG << "Matching RDOs " << nmatch << endreq;
    
	delete rdocoll;
    }

    if (error) {
	log << MSG::INFO << "ERROR reading and checking TRT RDO collections  " << endreq;
    }
    else {
	log << MSG::INFO << "Successfully read and checked TRT RDO collections  " << endreq;
    }
    
    return StatusCode::SUCCESS;
}




// Finalize method:
StatusCode InDetRawDataFakeReader::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "InDetRawDataFakeReader::finalize()" << endreq;

    return StatusCode::SUCCESS;
}

