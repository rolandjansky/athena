/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************/

//<doc><file>	$Id: CaloIdMgrDetDescrCnv.cxx,v 1.7 2009-02-10 14:09:21 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $


#include "CaloIdCnv/CaloIdMgrDetDescrCnv.h"
#include "CaloIdentifier/CaloIdManager.h"


// infrastructure includes
#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/errorcheck.h"

// Identifier includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/Tile_SuperCell_ID.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/LArEM_SuperCell_ID.h"
#include "CaloIdentifier/LArHEC_SuperCell_ID.h"
#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"
#include "CaloIdentifier/JTower_ID.h"
#include "CaloIdentifier/GTower_ID.h"


long int
CaloIdMgrDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode
CaloIdMgrDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(messageService(), "CaloIdMgrDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endreq;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endreq;
	return sc;
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

StatusCode
CaloIdMgrDetDescrCnv::finalize()
{
    MsgStream log(messageService(), "CaloIdMgrDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endreq;

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------


namespace {


template <class T>
StatusCode set_helper (StoreGateSvc* detStore,
                       const char* sgkey,
                       CaloIdManager* mgr,
                       MsgStream& log)
{
  const T* id = 0;
  CHECK_WITH_CONTEXT( detStore->retrieve (id, sgkey), "CaloIdMgrDetDescrCnv" );
  mgr->set_helper (id);
  log << MSG::DEBUG << "Found Calo ID helper " << sgkey << endreq;
  return StatusCode::SUCCESS;
}


} // anonymous namespace

StatusCode
CaloIdMgrDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
    MsgStream log(messageService(), "CaloIdMgrDetDescrCnv");
    log << MSG::INFO
	<< "in createObj: creating a CaloDescrManager object in the detector store"
	<< endreq;

    // Create a new CaloIdManager

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
	log << MSG::FATAL << "Could not cast to DetDescrAddress." << endreq;
	return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string mgrKey  = *( ddAddr->par() );
    if ("" == mgrKey) {
	log << MSG::DEBUG << "No Manager key " << endreq;
    }
    else {
	log << MSG::DEBUG << "Manager key is " << mgrKey << endreq;
    }

    // Create the manager
    CaloIdManager* caloIdMgr = new CaloIdManager();

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(caloIdMgr);

    // get DetectorStore service
    StoreGateSvc * detStore = 0;
    CHECK( serviceLocator()->service("DetectorStore", detStore) );

    // Initialize manager with all helpers.
#define SET_HELPER(H) CHECK( set_helper<H> (detStore, #H, caloIdMgr, log) )

    SET_HELPER (CaloCell_ID);

    bool is_test_beam = false;
    std::vector<std::string> file_names =
      caloIdMgr->getCaloCell_ID()->file_names();
    for (size_t i = 0; i < file_names.size(); i++) {
      if (file_names[i].find ("_H8_") != std::string::npos) {
        is_test_beam =true;
        break;
      }
    }
      
    SET_HELPER (CaloDM_ID);
    SET_HELPER (CaloLVL1_ID);
    SET_HELPER (LArEM_ID);
    SET_HELPER (LArHEC_ID);
    SET_HELPER (LArFCAL_ID);
    SET_HELPER (LArMiniFCAL_ID);
    SET_HELPER (TileID);
    SET_HELPER (TTOnlineID);
    SET_HELPER (JTower_ID);
    SET_HELPER (GTower_ID);

    if (!is_test_beam) {
      SET_HELPER (CaloCell_SuperCell_ID);
      SET_HELPER (LArEM_SuperCell_ID);
      SET_HELPER (LArHEC_SuperCell_ID);
      SET_HELPER (LArFCAL_SuperCell_ID);
      SET_HELPER (Tile_SuperCell_ID);
    }

#undef SET_HELPER


    log << MSG::INFO << " Finished " << endreq;

    // Initialize the calo mgr
    //  We protect here in case this has been initialized elsewhere
    if (!caloIdMgr->isInitialized()) {

      log << MSG::INFO << "Initializing CaloIdMgr from values in CaloIdMgrDetDescrCnv "
	  << endreq;

      // Initialize the manager ...

      // in this one, only strictly calo work is done
      caloIdMgr->initialize();

    }


    return StatusCode::SUCCESS;

}

//--------------------------------------------------------------------

long
CaloIdMgrDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID&
CaloIdMgrDetDescrCnv::classID() {
    return ClassID_traits<CaloIdManager>::ID();
}

//--------------------------------------------------------------------
CaloIdMgrDetDescrCnv::CaloIdMgrDetDescrCnv(ISvcLocator* svcloc)
    :
    DetDescrConverter(ClassID_traits<CaloIdManager>::ID(), svcloc)
{}
