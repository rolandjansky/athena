/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetMgrDetDescrCnv/src/CaloSuperCellMgrDetDescrCnv.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Converter for CaloSuperCellMgrDetDescrCnv.
 *        Creates the supercell geometry from the offline geometry.
 */


#undef NDEBUG


#include "CaloSuperCellMgrDetDescrCnv.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/ICaloSuperCellAlignTool.h"
#include "CaloDetDescrUtils/CaloSuperCellUtils.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "GeoModelInterfaces/IGeoAlignTool.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/errorcheck.h"
#include <cassert>


/**
 * @brief Return the service type.  Required by the base class.
 */
long int 
CaloSuperCellMgrDetDescrCnv::repSvcType() const
{
  return (storageType());
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode 
CaloSuperCellMgrDetDescrCnv::initialize()
{
  CHECK( DetDescrConverter::initialize() );
  CHECK( m_scidtool.retrieve() );
  return StatusCode::SUCCESS; 
}


/**
 * @brief Standard Gaudi finalize method.
 */
StatusCode 
CaloSuperCellMgrDetDescrCnv::finalize()
{
    return StatusCode::SUCCESS; 
}


/**
 * @brief Called by the converter infrastructure to create an object.
 * @param pAddr Address of the object to create.
 * @param pObj[out] Set to a reference to the created helper.
 *
 * This is all boilerplate; the real work is done in @c createManager.
 */
StatusCode
CaloSuperCellMgrDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    MsgStream log(msgSvc(), "CaloSCMgrDetDescrCnv");
    int outputLevel = msgSvc()->outputLevel( "CaloSCMgrDetDescrCnv" );

    // Create a new CaloSCDescrManager

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
	log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
	return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string mgrKey  = *( ddAddr->par() );
    
    if (outputLevel <= MSG::DEBUG) {
    if ("" == mgrKey) {
	log << MSG::DEBUG << "No Manager key " << endmsg;
    }
    else {
	log << MSG::DEBUG << "Manager key is " << mgrKey << endmsg;
    }
    }
    
    // Create the manager
    CaloSuperCellDetDescrManager* mgr = 0;
    CHECK( createManager (mgrKey, mgr, pObj) );


    return StatusCode::SUCCESS; 

}


/**
 * @brief Storage type and class ID (used by CnvFactory)
 */
long 
CaloSuperCellMgrDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}


/**
 * @brief Return the CLID of the class we create.
 * Required by the converter infrastructure.
 */
const CLID& 
CaloSuperCellMgrDetDescrCnv::classID() { 
    return ClassID_traits<CaloSuperCellDetDescrManager>::ID(); 
}


/**
 * @brief Constructor.
 * @param clid The CLID if the class we're constructing.
 * @param svcloc Gaudi service locator.
 */
CaloSuperCellMgrDetDescrCnv::CaloSuperCellMgrDetDescrCnv(ISvcLocator* svcloc) 
    :
  DetDescrConverter(ClassID_traits<CaloSuperCellDetDescrManager>::ID(), svcloc),
  m_scidtool ("CaloSuperCellIDTool"),
  m_scAlignTool ("CaloSuperCellAlignTool")
{}


/**
 * @brief Create the manager object.
 * @param helperKey StoreGate key of the manager.
 * @param idhelper[out] Set to a pointer to the new manager.
 * @param pObj[out] Set to a reference to the created manager.
 *
 * This actually creates the manager object.
 */
StatusCode
CaloSuperCellMgrDetDescrCnv::createManager(const std::string& /*mgrKey*/,
                                           CaloSuperCellDetDescrManager* & mgr,
                                           DataObject*& pObj)
{
  // Get ID helpers.
  StoreGateSvc * detStore = 0;
  CHECK( serviceLocator()->service("DetectorStore", detStore) );
  const CaloIdManager* caloId_mgr = 0;
  CHECK( detStore->retrieve(caloId_mgr, "CaloIdManager") );

  // Create the manager and initialize the helpers.
  mgr = new CaloSuperCellDetDescrManager;
  mgr->set_helper(caloId_mgr->getCaloCell_SuperCell_ID());
  mgr->set_helper(caloId_mgr);
  mgr->initialize();

  // return the manager to the caller.
  pObj = SG::asStorable (mgr);

  // Create descriptors / elements (with null geometry).
  createDescriptors (mgr);
  createElements (mgr);

  // Update the geometry from the offline manager.
  const CaloDetDescrManager* cellmgr = 0;
  CHECK( detStore->retrieve(cellmgr, "CaloMgr") );

  CHECK( m_scAlignTool.retrieve() );
  CHECK( m_scAlignTool->doUpdate (mgr, cellmgr) );

  return StatusCode::SUCCESS;
}

