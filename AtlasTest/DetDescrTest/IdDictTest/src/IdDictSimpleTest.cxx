/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict converter package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: IdDictSimpleTest.cxx,v 1.4 2005-04-28 17:07:12 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDictSimpleTest.h"

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"

// DetDescr includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SiliconID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBID.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

// Athena-related includes
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"


//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

/////////////////////////////////////////////////////////////////////

void 	
IdDictSimpleTest::tab 		(size_t level) const
{ 
    MsgStream  log(msgSvc(),name());
    for (size_t i = 0; i < level; ++i) log << " "; 
} 

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictSimpleTest::IdDictSimpleTest(const std::string& name, ISvcLocator* pSvcLocator) 
    :
    Algorithm(name, pSvcLocator)
{}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictSimpleTest::~IdDictSimpleTest()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode 
IdDictSimpleTest::initialize()
{
    MsgStream  log(msgSvc(),name());

    log << MSG::INFO 
	<< "Start initialization" 
	<< endreq;

    // Access InDet_DetDescrManager from detector store
    StoreGateSvc* detStore = 0;
    const IdDictManager* idDictMgr;
    StatusCode sc = service( "DetectorStore", detStore );
    if ( sc.isSuccess( ) ) {
	// Get the dictionary manager from the detector store
	sc = detStore->retrieve(idDictMgr, "IdDict");
	if (sc.isFailure()) {
	    log << MSG::FATAL << "Could not get IdDictManager !" << endreq;
	    return StatusCode::FAILURE;
	} 
	else {
	    log << MSG::DEBUG << " Found the IdDictManager. " << endreq;
	    log << MSG::DEBUG << " Tag is " << idDictMgr->manager()->tag() << endreq;
	}
    } 
    else {
	log << MSG::ERROR << "Could not locate DetectorStore" << endreq;
	return StatusCode::FAILURE;
    }

    // Get the pixel helper from the detector store
    const PixelID* pixelId;
    sc = detStore->retrieve(pixelId, "PixelID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the PixelID helper. " << endreq;
	printTags("PixelID", pixelId);
    }

    IdContext pixelWaferContext = pixelId->wafer_context();
    
    Identifier id = pixelId->wafer_id(-2, 0, 3, 0);
    log << MSG::DEBUG << " Id " << pixelId->show_to_string(id) << endreq;
    log << MSG::DEBUG << " Id " << pixelId->show_to_string(id,&pixelWaferContext) << endreq;
    

    // Get the sct helper from the detector store
    const SCT_ID* sctId;
    sc = detStore->retrieve(sctId, "SCT_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get SCT_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the SCT_ID helper. " << endreq;
	printTags("SCT_ID", sctId);
    }

    IdContext sctWaferContext = sctId->wafer_context();
    
    id = sctId->wafer_id(-2, 0, 3, 0, 1);
    log << MSG::DEBUG << " Id " << sctId->show_to_string(id) << endreq;
    log << MSG::DEBUG << " Id " << sctId->show_to_string(id,&sctWaferContext) << endreq;


    // Get the pixel helper  from the detector store
    const SiliconID* siliconId;
    sc = detStore->retrieve(siliconId, "SiliconID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get SiliconID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the SiliconID helper. " << endreq;
	printTags("SiliconID", siliconId);
    }

    unsigned int pixMax = pixelId->wafer_hash_max();
    unsigned int silMax = siliconId->wafer_hash_max();

    IdentifierHash h1 = (unsigned int)0.1*pixMax;
    Identifier id1    = siliconId->wafer_id(h1);
    IdentifierHash h2 = (unsigned int)0.6*pixMax;
    Identifier id2    = siliconId->wafer_id(h2);
    IdentifierHash h3 = (unsigned int)0.25*(silMax - pixMax) + pixMax;
    Identifier id3    = siliconId->wafer_id(h3);
    IdentifierHash h4 = (unsigned int)0.75*(silMax - pixMax) + pixMax;
    Identifier id4    = siliconId->wafer_id(h4);
    // Add in a blayer id
    Identifier     blay   = pixelId->wafer_id( 0, 0, 2, 3 );
    IdentifierHash blayH  = siliconId->wafer_hash(blay);


    IdContext context;
    if (siliconId->is_hash_pixel(h1)) context = pixelWaferContext;
    else context = sctWaferContext;
    log << MSG::DEBUG << " Hash " << h1 
	<< " Id " << siliconId->show_to_string(id1, &context) 
	<< " is_barrel " << siliconId->is_barrel(id1) 
	<< " is_blayer " << siliconId->is_blayer(id1) 
	<< endreq;
    if (siliconId->is_hash_pixel(h2)) context = pixelWaferContext;
    else context = sctWaferContext;
    log << MSG::DEBUG << " Hash " << h2 
	<< " Id " << siliconId->show_to_string(id2, &context) 
	<< " is_barrel " << siliconId->is_barrel(id2) 
	<< " is_blayer " << siliconId->is_blayer(id2) 
	<< endreq;
    if (siliconId->is_hash_pixel(h3)) context = pixelWaferContext;
    else context = sctWaferContext;
    log << MSG::DEBUG << " Hash " << h3 
	<< " Id " << siliconId->show_to_string(id3, &context) 
	<< " is_barrel " << siliconId->is_barrel(id3) 
	<< " is_blayer " << siliconId->is_blayer(id3) 
	<< endreq;
    if (siliconId->is_hash_pixel(h4)) context = pixelWaferContext;
    else context = sctWaferContext;
    log << MSG::DEBUG << " Hash " << h4 
	<< " Id " << siliconId->show_to_string(id4, &context) 
	<< " is_barrel " << siliconId->is_barrel(id4) 
	<< " is_blayer " << siliconId->is_blayer(id4) 
	<< endreq;
    if (siliconId->is_hash_pixel(blayH)) context = pixelWaferContext;
    else context = sctWaferContext;
    log << MSG::DEBUG << " Hash " << blayH 
	<< " Id " << siliconId->show_to_string(blay, &context) 
	<< " is_barrel " << siliconId->is_barrel(blay) 
	<< " is_blayer " << siliconId->is_blayer(blay) 
	<< endreq;

    // Get the trt helper from the detector store
    const TRT_ID* trtId;
    sc = detStore->retrieve(trtId, "TRT_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get TRT_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the TRT_ID helper. " << endreq; 
	printTags("TRT_ID", trtId);
   }

    // Get the CaloCell helper from the detector store
    const CaloCell_ID* calocellId;
    sc = detStore->retrieve(calocellId, "CaloCell_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get CaloCell_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CaloCell_ID helper. " << endreq;
	printTags("CaloCell_ID", calocellId);
    }

    // Get the CaloDM_ID helper from the detector store
    const CaloDM_ID* calodmId;
    sc = detStore->retrieve(calodmId, "CaloDM_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get CaloDM_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CaloDM_ID helper. " << endreq;
	printTags("CaloDM_ID", calodmId);
    }

    // Get the CaloLVL1_ID helper from the detector store
    const CaloLVL1_ID* calolvl1Id;
    sc = detStore->retrieve(calolvl1Id, "CaloLVL1_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get CaloLVL1_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CaloLVL1_ID helper. " << endreq;
	printTags("CaloLVL1_ID", calolvl1Id);
    }

    // Get the trt helper from the detector store
    const LArEM_ID* laremId;
    sc = detStore->retrieve(laremId, "LArEM_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get LArEM_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArEM_ID helper. " << endreq;
	printTags("LArEM_ID", laremId);
    }

    // Get the LArHEC_ID helper from the detector store
    const LArHEC_ID* larhecId;
    sc = detStore->retrieve(larhecId, "LArHEC_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get LArHEC_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArHEC_ID helper. " << endreq;
	printTags("LArHEC_ID", larhecId);
    }

    // Get the LArFCAL_ID helper from the detector store
    const LArFCAL_ID* larfcalId;
    sc = detStore->retrieve(larfcalId, "LArFCAL_ID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get LArFCAL_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArFCAL_ID helper. " << endreq;
	printTags("LArFCAL_ID", larfcalId);
    }

    // Get the LArOnline_ID helper from the detector store
    const LArOnlineID* larOnlineId;
    sc = detStore->retrieve(larOnlineId, "LArOnlineID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get LArOnlineID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArOnlineID helper. " << endreq;
	printTags("LArOnlineID", larOnlineId);
    }

    // Get the TileID helper from the detector store
    const TileID* tileId;
    sc = detStore->retrieve(tileId, "TileID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get TileID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the TileID helper. " << endreq;
	printTags("TileID", tileId);
    }

    // Get the TileHWID helper from the detector store
    const TileHWID* tileHWID;
    sc = detStore->retrieve(tileHWID, "TileHWID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get TileHWID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the TileHWID helper. " << endreq;
	printTags("TileHWID", tileHWID);
    }

    // Get the TileTBID helper from the detector store
    const TileTBID* tileTBID;
    sc = detStore->retrieve(tileTBID, "TileTBID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get TileTBID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the TileTBID helper. " << endreq;
	printTags("TileTBID", tileTBID);
    }

    // Get the MdtIdHelper helper from the detector store
    const MdtIdHelper* mdtId;
    sc = detStore->retrieve(mdtId, "MDTIDHELPER");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get MdtIdHelper helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the MdtIdHelper helper. " << endreq;
	printTags("MDT_ID", mdtId);
    }

    // Get the RpcIdHelper helper from the detector store
    const RpcIdHelper* rpcId;
    sc = detStore->retrieve(rpcId, "RPCIDHELPER");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get RpcIdHelper helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the RpcIdHelper helper. " << endreq;
	printTags("RPC_ID", rpcId);
    }


    // Get the CscIdHelper helper from the detector store
    const CscIdHelper* cscId;
    sc = detStore->retrieve(cscId, "CSCIDHELPER");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get CscIdHelper helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CscIdHelper helper. " << endreq;
	printTags("CSC_ID", cscId);
    }


    // Get the tgc helper from the detector store
    const TgcIdHelper* tgcId;
    sc = detStore->retrieve(tgcId, "TGCIDHELPER");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get TgcIdHelper helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the TgcIdHelper helper. " << endreq;
	printTags("TGC_ID", tgcId);
    }

    log << MSG::INFO
	<< "Initialization completed successfully"
	<< endreq;

    return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////
StatusCode 
IdDictSimpleTest::execute()
{
    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

StatusCode 
IdDictSimpleTest::finalize()
{

    MsgStream  log(msgSvc(),name());
    log << MSG::DEBUG 
	<< "finalize completed successfully" 
	<< endreq;

    return StatusCode::SUCCESS;

}


void IdDictSimpleTest::printTags      (const std::string& dictName,
				       const AtlasDetectorID* idHelper) const
{
    MsgStream  log(msgSvc(),name());
    ///  Dictionary names
    std::vector<std::string>  dictNames = idHelper->dict_names();
    ///  File names for subdet dictionaries
    std::vector<std::string>  fileNames = idHelper->file_names();
    ///  Version tags for subdet dictionaries
    std::vector<std::string>  dictTags  = idHelper->dict_tags();

    log << MSG::DEBUG 
	<< "Dict: " << dictName
	<< " Num dict names : " << dictNames.size()
	<< " Num file names : " << fileNames.size()
	<< " Num dict tags  : " << dictTags.size()
	<< endreq;
    log << MSG::DEBUG 
	<< "Dict names: "
	<< endreq;
    for (unsigned int i = 0; i < dictNames.size(); ++i) {
      log << MSG::DEBUG 
	  << " " << dictNames[i]
	  << endreq;
    }
    log << MSG::DEBUG 
	<< "File names: "
	<< endreq;
    for (unsigned int i = 0; i < fileNames.size(); ++i) {
      log << MSG::DEBUG 
	  << " " << fileNames[i]
	  << endreq;
    }
    log << MSG::DEBUG 
	<< "Dict tags: "
	<< endreq;
    for (unsigned int i = 0; i < dictTags.size(); ++i) {
      log << MSG::DEBUG 
	  << " " << dictTags[i]
	  << endreq;
    }
}
