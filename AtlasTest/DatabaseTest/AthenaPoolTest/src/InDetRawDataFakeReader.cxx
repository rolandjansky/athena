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

// test includes
#include "InDetRawDataFakeCreator.h"

#include <map>

// Constructor with parameters:
InDetRawDataFakeReader::InDetRawDataFakeReader(const std::string &name, 
				       ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_pixelId(0),
    m_sctId(0),
    m_trtId(0)

{}

// Initialize method:
StatusCode InDetRawDataFakeReader::initialize()
{
    ATH_MSG_INFO("InDetRawDataFakeReader::initialize()" );

    // Get the pixel helper from the detector store
    const DataHandle<PixelID> pixel_id;
    ATH_CHECK( detStore()->retrieve(pixel_id, "PixelID") );
    m_pixelId = pixel_id;

    const DataHandle<SCT_ID> sctId;
    ATH_CHECK( detStore()->retrieve(sctId, "SCT_ID") );
    m_sctId = sctId;

    const DataHandle<TRT_ID> trtId;
    ATH_CHECK(  detStore()->retrieve(trtId, "TRT_ID") );
    m_trtId = trtId;

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode InDetRawDataFakeReader::execute() 
{
    ATH_MSG_DEBUG("InDetRawDataFakeReader::execute()" );

    ATH_CHECK( checkPixel() );
    ATH_CHECK( checkSCT() );
    ATH_CHECK( checkTRT() );
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeReader::checkPixel() const
{
    ATH_MSG_DEBUG("InDetRawDataFakeReader::checkPixel()" );

    const PixelRDO_Container* pixCont = nullptr;
    ATH_CHECK( evtStore()->retrieve(pixCont,"PixelRDOs") );

    // loop on all RDO collections read in and check them
    int nmatch = 0;
    bool error = false;
    InDetRawDataFakeCreator creator;
    IdContext cntx = m_pixelId->wafer_context();

    ATH_MSG_DEBUG("PixelRDO_Container size " 
	<< pixCont->size() << " " );

    // Check for non-zero length of container
    if (pixCont->begin() == pixCont->end()) {
	ATH_MSG_ERROR("Container '" << "PixelRDOs" << "' is EMPTY !" );
	return StatusCode::FAILURE;
    }



    // loop on all rdo collections. 
//     for(; rdoCollectionsBegin!=rdoCollectionsEnd; ++rdoCollectionsBegin) {
	    
      for(PixelRDO_Container::const_iterator it=pixCont->begin() ; 
        	it!=pixCont->end() ; ++it) {

	const InDetRawDataCollection<PixelRDORawData>* RDO_Collection(&(**it));
//	const InDetRawDataCollection<Pixel1RawData>* RDO_Collection(&(*rdoCollectionsBegin.cptr()));

	ATH_MSG_DEBUG("Read RDO collection: size, ids " 
	    << RDO_Collection->size() << " "
	    << m_pixelId->show_to_string(RDO_Collection->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec );

	// create a new pixel RDO collection
        MsgStream log(msgSvc(), name());
	const InDetRawDataCollection<PixelRDORawData> *rdocoll = 
          creator.createPixelRawDataColl(RDO_Collection->identifyHash(), m_pixelId, msg());

	ATH_MSG_DEBUG("Test RDO collection: size, ids " 
	    << rdocoll->size() << " "
	    << m_pixelId->show_to_string(rdocoll->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec );


	// Use one of the specific clustering AlgTools to make clusters
	//typedef InDetRawDataCollection<Pixel1RawData> RDO1_Collection_type;
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
		
		ATH_MSG_ERROR("RDO's do NOT match: ids, word " 
		    << m_pixelId->show_to_string((*nextRDO)->identify()) << " " 
		    << m_pixelId->show_to_string((*nextRDO1)->identify()) << " " 
		    << MSG::hex << (*nextRDO)->getWord() << " "
		    << MSG::hex << (*nextRDO1)->getWord() << " "
		    );
		error = true;
	    }
	    else {
		nmatch++;
	    }
	}
	ATH_MSG_DEBUG("Matching RDOs " << nmatch );
    
	delete rdocoll;
    }

    // print out pixel element links

    // Retrive the element link vector
    const PixelRDOElemLinkVec* linkVec = 0;
    ATH_CHECK( evtStore()->retrieve(linkVec, "PixelRDOELs") );
    
    ATH_MSG_DEBUG("PixelRDOElemLinkVec found with "
        << linkVec->pixelRDOs().size() 
        << "  RDOs " );
    for(unsigned int i = 0; i < linkVec->pixelRDOs().size(); ++i) {
        // Print out rdos from element link
	PixelRDOElemLinkVec::elem_type rdo  = linkVec->pixelRDOs()[i];
        PixelRDORawData* ncrdo = const_cast<PixelRDORawData*>(*rdo);
        ATH_MSG_DEBUG("RDO ID=" << m_pixelId->show_to_string((*rdo)->identify())
                      << "RDO ToT=" << ncrdo->getToT() );
    }


    if (error) {
	ATH_MSG_INFO("ERROR reading and checking Pixel RDO collections  " );
    }
    else {
	ATH_MSG_INFO("Successfully read and checked Pixel RDO collections  " );
    }
    
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeReader::checkSCT() const
{
    ATH_MSG_DEBUG("InDetRawDataFakeReader::execute()" );

    const SCT_RDO_Container* sctCont = nullptr;
    ATH_CHECK( evtStore()->retrieve(sctCont,"SCT_RDOs") );
  
    // loop on all RDO collections read in and check them
    int nmatch = 0;
    bool error = false;
    InDetRawDataFakeCreator creator;
    IdContext cntx = m_sctId->wafer_context();

    ATH_MSG_DEBUG("SCT_RDO_Container size " 
	<< sctCont->size() << " "	);

    // Check for non-zero length of container
    if (sctCont->begin() == sctCont->end()) {
	ATH_MSG_ERROR("Container '" << "SCT_RDOs" << "' is EMPTY !" );
	return StatusCode::FAILURE;
    }

    for(SCT_RDO_Container::const_iterator it=sctCont->begin() ; 
      	it!=sctCont->end() ; ++it) {

	const InDetRawDataCollection<SCT_RDORawData>* RDO_Collection(&(**it));

	ATH_MSG_DEBUG("Read RDO collection: size, ids " 
	    << RDO_Collection->size() << " "
	    << m_sctId->show_to_string(RDO_Collection->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec );

	// create a new sct RDO collection
        MsgStream log(msgSvc(), name());
	const InDetRawDataCollection<SCT_RDORawData> *rdocoll = 
          creator.createSCT_RawDataColl(RDO_Collection->identifyHash(), m_sctId, msg());

	ATH_MSG_DEBUG("Test RDO collection: size, ids " 
	    << rdocoll->size() << " "
	    << m_sctId->show_to_string(rdocoll->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec );


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
		((*nextRDO)->getGroupSize() != (*nextRDO1)->getGroupSize())) {
		
		ATH_MSG_ERROR("RDO's do NOT match: ids, group, strip " 
		    << m_sctId->show_to_string((*nextRDO)->identify()) << " " 
		    << m_sctId->show_to_string((*nextRDO1)->identify()) << " " 
		    << MSG::hex 
		    << (*nextRDO)->getGroupSize() << " "
		    << (*nextRDO1)->getGroupSize() << " "
		    << MSG::dec
		    );
		error = true;
	    }
	    else {
		nmatch++;
	    }
	}
	ATH_MSG_DEBUG("Matching RDOs " << nmatch );
    
	delete rdocoll;
    }

    if (error) {
	ATH_MSG_INFO("ERROR reading and checking SCT RDO collections  " );
    }
    else {
	ATH_MSG_INFO("Successfully read and checked SCT RDO collections  " );
    }
    
    return StatusCode::SUCCESS;
}

StatusCode InDetRawDataFakeReader::checkTRT() const
{ 
    ATH_MSG_DEBUG("InDetRawDataFakeReader::execute()" );

    const TRT_RDO_Container* trtCont = nullptr;
    ATH_CHECK( evtStore()->retrieve(trtCont,"TRT_RDOs") );
  
    // loop on all RDO collections read in and check them
    int nmatch = 0;
    bool error = false;
    InDetRawDataFakeCreator creator;
    IdContext cntx = m_trtId->straw_layer_context();

    ATH_MSG_DEBUG("TRT_RDO_Container size " 
	<< trtCont->size() << " " );

    // Check for non-zero length of container
    if (trtCont->begin() == trtCont->end()) {
	ATH_MSG_ERROR("Container '" << "TRT_RDOs" 
	    << "' is EMPTY !" );
	return StatusCode::FAILURE;
    }

    for(TRT_RDO_Container::const_iterator it=trtCont->begin() ; 
      	it!=trtCont->end() ; ++it) {

	const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(&(**it));

	ATH_MSG_DEBUG("Read RDO collection: size, ids " 
	    << RDO_Collection->size() << " "
	    << m_trtId->show_to_string(RDO_Collection->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)RDO_Collection->identifyHash() << MSG::dec );

	// create a new trt RDO collection
        MsgStream log(msgSvc(), name());
	const InDetRawDataCollection<TRT_RDORawData> *rdocoll = 
          creator.createTRT_RawDataColl(RDO_Collection->identifyHash(), m_trtId, msg());

	ATH_MSG_DEBUG("Test RDO collection: size, ids " 
	    << rdocoll->size() << " "
	    << m_trtId->show_to_string(rdocoll->identify(), &cntx) << " " 
	    << MSG::hex << (unsigned int)rdocoll->identifyHash() << MSG::dec );


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
		
		ATH_MSG_ERROR("RDO's do NOT match: ids, highlvl, TOT, drift " 
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
		    );
		error = true;
	    }
	    else {
		nmatch++;
	    }
	}
	ATH_MSG_DEBUG("Matching RDOs " << nmatch );
    
	delete rdocoll;
    }

    if (error) {
	ATH_MSG_INFO("ERROR reading and checking TRT RDO collections  " );
    }
    else {
	ATH_MSG_INFO("Successfully read and checked TRT RDO collections  " );
    }
    
    return StatusCode::SUCCESS;
}




// Finalize method:
StatusCode InDetRawDataFakeReader::finalize() 
{
    ATH_MSG_INFO("InDetRawDataFakeReader::finalize()" );
    return StatusCode::SUCCESS;
}

