///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// xAODCnv.cxx 
// Implementation file for class xAODCnv
// Author: Johannes Elmsheuser, Will Buttinger
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "xAODCnv.h"
#include "xAODBranchAddress.h"
#include "RootDataBucketBranch.h"

// STL includes
#include <sstream>
#include <typeinfo>
#include <stdexcept>

// framework
#include "GaudiKernel/DataObject.h"
//#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/DataBucketBase.h"

// ROOT 
#include "TBranch.h"
#include "RootUtils/TBranchElementClang.h"
#include "TLeaf.h"
#include "TTree.h"

namespace {
  std::string name_from_clid (const CLID clid)
  {
    std::ostringstream o;
    o << "AthenaRootxAODCnv_" << clid;
    return o.str();
  }
}

namespace Athena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
xAODCnv::xAODCnv( const CLID& clid,
                  ISvcLocator* pSvcLocator ) : 
  ::Converter( POOL_ROOTTREE_StorageType, clid, pSvcLocator ),
  ::AthMessaging( msgSvc(),    name_from_clid(clid) ),
  m_type()
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

xAODCnv::xAODCnv( ISvcLocator* pSvcLocator ) : 
  ::Converter   ( POOL_ROOTTREE_StorageType, CLID_NULL, pSvcLocator ),
  ::AthMessaging( msgSvc(),    name_from_clid(CLID_NULL) ),
  m_type()
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  ATH_MSG_DEBUG("Constructor xAODCnv...");

}

// Destructor
///////////////
xAODCnv::~xAODCnv()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

// Athena Service's Hooks
////////////////////////////
StatusCode xAODCnv::initialize()
{
  // configure our MsgStream
  //msg().setLevel( MSG::INFO );

  ATH_MSG_DEBUG("Initializing xAODCnv...");

  ATH_MSG_DEBUG("Initializing base class...");
  if ( ::Converter::initialize().isFailure() ) {
    ATH_MSG_ERROR("Could not intialize base class !!");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE("Base class initialized");
  }

  // retrieve the dict-loader-svc
  typedef ServiceHandle<IDictLoaderSvc> IDictLoaderSvc_t;
  IDictLoaderSvc_t dictSvc("AthDictLoaderSvc", 
                           "Athena::xAODCnv");
                           // name_from_clid(objType()));
  if ( !dictSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("could not retrieve [" << dictSvc.typeAndName() << "] !");
    return StatusCode::FAILURE;
  }

  RootType ty = dictSvc->load_type(this->objType());
  ATH_MSG_VERBOSE("loaded dictionary for clid ["
                  << this->objType() << "]: name=["
                  << ty.Name() << "]");
  m_type = ty;

  ATH_MSG_DEBUG("converter correctly initialized - clid=[" << objType() << "]");
  return StatusCode::SUCCESS;
}

// StatusCode xAODCnv::finalize()
// {
//   m_msg << MSG::INFO 
//      << "Finalizing " << name() << "..." 
//      << endmsg;

//   return StatusCode::SUCCESS;
// }

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
// StatusCode 
// xAODCnv::queryInterface(const InterfaceID& riid, void** ppvInterface) 
// {
//   if ( IConverter::interfaceID().versionMatch(riid) ) {
//     *ppvInterface = dynamic_cast<IConverter*>(this);
//   } else {
//     // Interface is not directly available : try out a base class
//     return ::Service::queryInterface(riid, ppvInterface);
//   }
//   addRef();
//   return StatusCode::SUCCESS;
// }

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

long
xAODCnv::repSvcType() const
{
  ATH_MSG_DEBUG("Calling xAODCnv::repSvcType");
  return POOL_ROOTTREE_StorageType;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/// Create a transient object from a persistent representation.
/// @param pAddr [IN] IOpaqueAddress of persistent representation.
/// @param pObj [OUT] pointer to the transient object.
StatusCode 
xAODCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  ATH_MSG_DEBUG("::createObj(" << pAddr << ", " << pObj << ")");
  Athena::xAODBranchAddress *rba = dynamic_cast<Athena::xAODBranchAddress*>(pAddr);
  if (!rba) {
    ATH_MSG_DEBUG(*pAddr->par() << " is NOT a GenericAddress!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("loading branch: [" << rba->par()[0]
                << "/" << rba->par()[1] << "]...");
  //rba->setBranchAddress(m_type);
  ATH_MSG_DEBUG("xAODCnv::createObj 1");
  // FIXME JE
  rba->setTEventAddress();
  ATH_MSG_DEBUG("loading branch: [" << rba->par()[0]
                << "/" << rba->par()[1] << "]... [done]");
  ATH_MSG_DEBUG("xAODCnv::createObj 2");
  Athena::DataBucketBranch *dbb = new DataBucketBranch(rba->clID(), m_type, rba->m_ptr);
   dbb->allowMismatchCLID = true; //FIXME WB : hack to allow requests from other clid types
  ATH_MSG_DEBUG("xAODCnv::createObj 3");
  pObj = dbb;
  ATH_MSG_DEBUG("xAODCnv::createObj 4");
  return StatusCode::SUCCESS;
}

/// Create a persistent representation for a transient object.
/// @param pObj [IN] pointer to the transient object.
/// @param pAddr [OUT] IOpaqueAddress of persistent representation.
StatusCode 
xAODCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  ATH_MSG_DEBUG("Calling xAODCnv::createRep");

  ATH_MSG_INFO("::createRep(" << pObj << ", " << pAddr << ")");
  if (0==pObj) {
    ATH_MSG_ERROR("::createRep: received null ptr to dobj");
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << " -> clid: [" << pObj->clID() << "]" << endmsg
                 << " -> name: [" << pObj->name() << "]" << endmsg;

  /*
  Athena::DataBucketBranch *dbb = 0;
  dbb = dynamic_cast<Athena::DataBucketBranch*>(pObj);
  if (!dbb) {
    ATH_MSG_INFO("dataobject at [" << pObj->name() << "] is not"
                 << " a DataBucketBranch ! [clid=" << pObj->clID() << "]");
    return StatusCode::FAILURE;
  }
  */

  // FIXME:
  // Athena::RootBranchAddress* addr = new Athena::RootBranchAddress
  //   (POOL_ROOTTREE_StorageType, 
  //    dbb->clID(), 
  //    m_tupleName.value(), 
  //    br_name, 
  //    (unsigned long)(value_ptr),
  //    (unsigned long)(m_nbrEvts-1));
#if 1
  GenericAddress* addr = new GenericAddress(POOL_ROOTTREE_StorageType, 
                                            pObj->clID(),
                                            pObj->name());
#else
  Athena::xAODBranchAddress* addr = new Athena::xAODBranchAddress
    (POOL_ROOTTREE_StorageType, id, 
     "foo",//m_tupleName.value(), 
     dbb->name(), 
     (unsigned long)(value_ptr),
     (unsigned long)(m_nbrEvts-1));
#endif
  pAddr = addr;
  return StatusCode::SUCCESS;
}

long 
xAODCnv::storageType()
{
  std::cout << "Calling xAODCnv::storageType" << std::endl;

  return POOL_ROOTTREE_StorageType;

}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace Athena
