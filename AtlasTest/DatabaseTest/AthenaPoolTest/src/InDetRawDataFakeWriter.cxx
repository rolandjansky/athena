/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRawDataFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O, writes InDetRawData to
 * transient store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: InDetRawDataFakeWriter.cxx,v 1.14 2008-06-20 16:41:09 schaffer Exp $
 *
 */

// Silicon trackers includes
#include "InDetRawDataFakeWriter.h"

#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"


//InDetRawData includes
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"

//#include "InDetRawData/InDetRawDataCLASS_DEF.h"

// Id includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

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
InDetRawDataFakeWriter::InDetRawDataFakeWriter(const std::string &name, 
				       ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0)
{}

// Initialize method:
StatusCode InDetRawDataFakeWriter::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "InDetRawDataFakeWriter::initialize()" << endreq;

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
    const DataHandle<SCT_ID> sct_id;
    sc = detStore->retrieve(sct_id, "SCT_ID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not get SCT_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the SCT_ID helper. " << endreq;
    }

    m_sctId = sct_id;

    // Get the trt helper from the detector store
    const DataHandle<TRT_ID> trt_id;
    sc = detStore->retrieve(trt_id, "TRT_ID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not get TRT_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the TRT_ID helper. " << endreq;
    }

    m_trtId = trt_id;

    // create the IdentifiableContainers to contain the rdo collections
    try {
	m_pixCont=new PixelRDO_Container(m_pixelId->wafer_hash_max());
    } catch (std::bad_alloc) {
	log << MSG::ERROR << "Could not create a new Pixel RawDataContainer !" 
	    << endreq;
	return StatusCode::FAILURE;
    }
  
    try {
	m_sctCont=new SCT_RDO_Container(m_sctId->wafer_hash_max());
    } catch (std::bad_alloc) {
	log << MSG::ERROR << "Could not create a new SCT  RawDataContainer !" 
	    << endreq;
	return StatusCode::FAILURE;
    }
  
    try {
	m_trtCont=new TRT_RDO_Container(m_trtId->straw_layer_hash_max());
    } catch (std::bad_alloc) {
	log << MSG::ERROR << "Could not create a new TRT RawDataContainer !" 
	    << endreq;
	return StatusCode::FAILURE;
    }
  
    // preventing SG from deleting object:
    m_pixCont->addRef();
    m_sctCont->addRef();
    m_trtCont->addRef();

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode InDetRawDataFakeWriter::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeWriter::execute()" << endreq;


    // create pixel rdos
    StatusCode sc = createPixels();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot create PixelRDOs" 
	    << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Created PixelRDOs" << endreq;
    }

    // create pixel rdos
    sc = createSCTs();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot create SCT_RDOs" 
	    << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Created SCT_RDOs" << endreq;
    }

    // create pixel rdos
    sc = createTRTs();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot create TRT_RDOs" 
	    << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Created TRT_RDOs" << endreq;
    }

    sc = printRDOs();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot print out RDOs" 
	    << endreq;
	return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeWriter::createPixels() const
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeWriter::createPixels()" << endreq;


    // IDC creation and registration, done once per job:

    // the IDC is created in the initialize, must clear the IDC every event:
    m_pixCont->cleanup();


    // register the Pixel IdentifiableContainer into StoreGate
    StatusCode sc=m_storeGate->record(m_pixCont,"PixelRDOs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "PixelRDOs" 
	    << "' could not be registered in StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "PixelRDOs"
	    << "' registered in StoreGate" << endreq;
    }

    // Create and record the element link vector
    PixelRDOElemLinkVec* linkVec = new PixelRDOElemLinkVec;
    sc = m_storeGate->record(linkVec,"PixelRDOELs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "PixelRDOElemLinkVec could not be registered in StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "PixelRDOElemLinkVec registered in StoreGate" << endreq;
    }

    // loop over 10 different pixel wafers
    int deltaWafer = m_pixelId->wafer_hash_max()/15;
    IdentifierHash waferHash = deltaWafer;

    InDetRawDataFakeCreator creator;
  
    IdContext cntx = m_pixelId->wafer_context();

    for (int i=0; i < 10; ++i, waferHash += deltaWafer) {
      
	// Print out contents of RDO
	log << MSG::DEBUG << "Creating RDO collection: " 
	    << m_pixelId->show_to_string(m_pixelId->wafer_id(waferHash), &cntx) << " " 
	    << MSG::hex << m_pixelId->wafer_id(waferHash) << MSG::dec << " "
	    << MSG::hex << (unsigned int)waferHash << MSG::dec << endreq;

	// create a new pixel RDO collection
	const InDetRawDataCollection<PixelRDORawData> *p_rdocoll = 
	    creator.createPixelRawDataColl(waferHash, m_pixelId, log);

	// register the rdo collection in StoreGate
	// add rdo collection to the container
	IdentifierHash id_hash = p_rdocoll->identifyHash();
	sc = m_pixCont->addCollection(p_rdocoll, id_hash);
 	if (sc.isFailure()) {
 	    log << MSG::ERROR << "Pixel RDOs could not be added to the container !"
 		<< endreq;
 	    return StatusCode::FAILURE;
 	} else {
 	    log << MSG::DEBUG << "Pixel RDOs '" << (unsigned int)id_hash << "' added to container"
 		<< endreq;
 	}

	// Loop over RDOs and add ELs to vector
	for (unsigned int irdo = 0; irdo < p_rdocoll->size(); ++irdo) {
	    unsigned int itest = irdo % 3;
	    if (itest == 2) {
		// Create el with element and container
		log << MSG::VERBOSE << "Pixel RDOs: create EL without index" 
		    << endreq;
		PixelRDOElemLinkVec::elem_type el((*p_rdocoll)[irdo], *m_pixCont);
		linkVec->pixelRDOs().push_back(el);
	    }
	    else {
		// Create el with hash and sometimes index
		IdentContIndex index;
		index.setCollHash(waferHash);
		if (itest == 1) index.setObjIndex(irdo);
		log << MSG::VERBOSE << "Pixel RDOs: create EL - hash, index, hashAndIndex: " 
		    << index.collHash() << " " << index.objIndex() << " " 
		    << MSG::hex << index.hashAndIndex() << MSG::dec
		    << endreq;
		// Create el with element and index
		PixelRDOElemLinkVec::elem_type el(*m_pixCont, index.hashAndIndex());
		el.setElement((*p_rdocoll)[irdo]);
		linkVec->pixelRDOs().push_back(el);
	    }
	}
    }

    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeWriter::createSCTs() const
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeWriter::execute()" << endreq;


    // IDC creation and registration, done once per job:

  // the IDC is created in the initialize, must clear the IDC every event:
    m_sctCont->cleanup();


    // register the Sct IdentifiableContainer into StoreGate
    StatusCode sc=m_storeGate->record(m_sctCont,"SCT_RDOs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "SCT_RDOs" 
	    << "' could not be registered in StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "SCT_RDOs"
	    << "' registered in StoreGate" << endreq;
    }

    // loop over 10 different sct wafers
    int deltaWafer = m_sctId->wafer_hash_max()/15;
    IdentifierHash waferHash = deltaWafer;

    InDetRawDataFakeCreator creator;

    IdContext cntx = m_sctId->wafer_context();

    for (int i=0; i < 10; ++i, waferHash += deltaWafer) {
      
	// Print out contents of RDO
	log << MSG::DEBUG << "Creating RDO collection: " 
	    << m_sctId->show_to_string(m_sctId->wafer_id(waferHash), &cntx) << " " 
	    << MSG::hex << m_sctId->wafer_id(waferHash) << MSG::dec << " "
	    << MSG::hex << (unsigned int)waferHash << MSG::dec << endreq;

	// create a new sct RDO collection
	const InDetRawDataCollection<SCT_RDORawData> *p_rdocoll = 
	    creator.createSCT_RawDataColl(waferHash, m_sctId, log);

	// register the rdo collection in StoreGate
	IdentifierHash id_hash = p_rdocoll->identifyHash();
	sc = m_sctCont->addCollection(p_rdocoll, id_hash);
 	if (sc.isFailure()) {
 	    log << MSG::ERROR << "SCT RDOs could not be added to the container !"
 		<< endreq;
 	    return StatusCode::FAILURE;
 	} else {
 	    log << MSG::DEBUG << "SCT RDOs '" << (unsigned int)id_hash << "' added to container"
 		<< endreq;
 	}
    }

    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeWriter::createTRTs() const
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "InDetRawDataFakeWriter::execute()" << endreq;


    // IDC creation and registration, done once per job:

  // the IDC is created in the initialize, must clear the IDC every event:
    m_trtCont->cleanup();


    // register the TRT_ IdentifiableContainer into StoreGate
    StatusCode sc=m_storeGate->record(m_trtCont,"TRT_RDOs");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "TRT_RDOs" 
	    << "' could not be registered in StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "TRT_RDOs"
	    << "' registered in StoreGate" << endreq;
    }

    // loop over 10 different trt straw layers
    int deltaStrLay = m_trtId->straw_layer_hash_max()/15;
    IdentifierHash strLayHash = deltaStrLay;

    InDetRawDataFakeCreator creator;
    
    IdContext cntx = m_trtId->straw_layer_context();
  
    for (int i=0; i < 10; ++i, strLayHash += deltaStrLay) {
      
	// Print out contents of RDO
	log << MSG::DEBUG << "Creating RDO collection: " 
	    << m_trtId->show_to_string(m_trtId->layer_id(strLayHash), &cntx) << " " 
	    << MSG::hex << m_trtId->layer_id(strLayHash) << MSG::dec << " "
	    << MSG::hex << (unsigned int)strLayHash << MSG::dec << endreq;

	// create a new trt RDO collection
	const InDetRawDataCollection<TRT_RDORawData> *p_rdocoll = 
	    creator.createTRT_RawDataColl(strLayHash, m_trtId, log);

	// register the rdo collection in StoreGate
	IdentifierHash id_hash = p_rdocoll->identifyHash();
	sc = m_trtCont->addCollection(p_rdocoll, id_hash);
 	if (sc.isFailure()) {
 	    log << MSG::ERROR << "TRT RDOs could not be added to the container !"
 		<< endreq;
 	    return StatusCode::FAILURE;
 	} else {
 	    log << MSG::DEBUG << "TRT RDOs '" << (unsigned int)id_hash << "' added to container"
 		<< endreq;
 	}
    }

    return StatusCode::SUCCESS;
}




// Print rdos
StatusCode InDetRawDataFakeWriter::printRDOs() const
{

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "InDetRawDataFakeWriter::printRDOs()" << endreq;

    // loop on all RDO collections

    // pixels

    for(PixelRDO_Container::const_iterator it=m_pixCont->begin() ; 
      	it!=m_pixCont->end() ; ++it) {

	const InDetRawDataCollection< PixelRDORawData > *rdocoll=&(**it);
      
	log << MSG::DEBUG << "PixelRDORawData collection found with id "
	    << m_pixelId->show_to_string(rdocoll->identify())
	    << " and " << rdocoll->size() << " RDOs" << endreq;
      
	// loop on all RDOs
	DataVector<PixelRDORawData>::const_iterator rdo;
	for(rdo=rdocoll->begin() ; 
	    rdo!=rdocoll->end() ; ++rdo) {
	
	    log << MSG::DEBUG << "RDO ID=" << m_pixelId->show_to_string((*rdo)->identify());
	    PixelRDORawData* ncrdo = const_cast<PixelRDORawData*>(*rdo);
	    log << MSG::DEBUG << "RDO ToT=" << ncrdo->getToT() << " id:" << 
		(*rdo)->identify().getString() << endreq;
	
	}
    }

    // pixel element links

    // Retrive the element link vector
    const PixelRDOElemLinkVec* linkVec = 0;
    StatusCode sc = m_storeGate->retrieve(linkVec, "PixelRDOELs");
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
        log << MSG::DEBUG << "RDO ToT=" << ncrdo->getToT() 
            << " id:" << (*rdo)->identify().getString() << endreq;
    }


    // scts

    for(SCT_RDO_Container::const_iterator it=m_sctCont->begin() ; 
      	it!=m_sctCont->end() ; ++it) {

	const InDetRawDataCollection< SCT_RDORawData > *rdocoll=&(**it);
      
	log << MSG::DEBUG << "SCT_RDORawData collection found with id "
	    << m_sctId->show_to_string(rdocoll->identify())
	    << " and " << rdocoll->size() << " RDOs" << endreq;
      
	// loop on all RDOs
	DataVector<SCT_RDORawData>::const_iterator rdo;
	for(rdo=rdocoll->begin() ; 
	    rdo!=rdocoll->end() ; ++rdo) {
	
	    log << MSG::DEBUG << "RDO ID=" << m_sctId->show_to_string((*rdo)->identify());
	    log << MSG::DEBUG << "RDO grp=" << (*rdo)->getGroupSize() << " strip:" << 
		(*rdo)->getStrip() << endreq;
	
	}
    }

    // trts

    for(TRT_RDO_Container::const_iterator it=m_trtCont->begin() ; 
      	it!=m_trtCont->end() ; ++it) {

	const InDetRawDataCollection< TRT_RDORawData > *rdocoll=&(**it);
      
	log << MSG::DEBUG << "TRT_RDORawData collection found with id "
	    << m_trtId->show_to_string(rdocoll->identify())
	    << " and " << rdocoll->size() << " RDOs" << endreq;
      
	// loop on all RDOs
	DataVector<TRT_RDORawData>::const_iterator rdo;
	for(rdo=rdocoll->begin() ; 
	    rdo!=rdocoll->end() ; ++rdo) {
	
	    log << MSG::DEBUG << "RDO ID=" << m_trtId->show_to_string((*rdo)->identify());
	    log << MSG::DEBUG << "RDO hilvl=" << (*rdo)->highLevel() << " ToT:" << 
		(*rdo)->timeOverThreshold() << " drift: " << (*rdo)->driftTimeBin() 
		<< endreq;
	
	}
    }

    log << MSG::DEBUG << "InDetRawDataFakeWriter::printRDOs() end" << endreq;

    return StatusCode::SUCCESS;

}


// Finalize method:
StatusCode InDetRawDataFakeWriter::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "InDetRawDataFakeWriter::finalize()" << endreq;

    return StatusCode::SUCCESS;
}

