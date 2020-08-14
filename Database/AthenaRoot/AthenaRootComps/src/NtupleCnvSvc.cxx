///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NtupleCnvSvc.cxx 
// Implementation file for class Athena::NtupleCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "NtupleCnvSvc.h"
#include "LeafCnv.h"
#include "RootBranchAddress.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "Gaudi/Property.h"

// ROOT includes
#include "TFile.h"

namespace Athena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
NtupleCnvSvc::NtupleCnvSvc( const std::string& name, 
                            ISvcLocator* pSvcLocator ) : 
  ::AthCnvSvc( name, pSvcLocator, ROOT_StorageType )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  // std::cerr << "::NtupleCnvSvc(" << name << ")...\n";
}

// Destructor
///////////////
NtupleCnvSvc::~NtupleCnvSvc()
{}

// Athena Service's Hooks
////////////////////////////
StatusCode
NtupleCnvSvc::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  return AthCnvSvc::initialize();
}

StatusCode
NtupleCnvSvc::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  return AthCnvSvc::finalize();
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
NtupleCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IConversionSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IConversionSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return ::AthCnvSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/// update the service state (well, just check the @c IOpaqueAddress is in
/// fact a @c GenericAddress )
StatusCode 
NtupleCnvSvc::updateServiceState(IOpaqueAddress* pAddress)
{
  ATH_MSG_DEBUG("updateServiceState(" << pAddress << ")...");
  if (pAddress) {
    return dynamic_cast<GenericAddress*>(pAddress) 
      ? StatusCode::SUCCESS
      : StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

/// Implementation of IConverter: Create the transient representation of an 
/// object from persistent state.
/// @param pAddress [IN] pointer to IOpaqueAddress of the representation.
/// @param refpObject [OUT] pointer to DataObject to be created.
StatusCode 
NtupleCnvSvc::createObj(IOpaqueAddress* pAddress, DataObject*& refpObject)
{
  ATH_MSG_DEBUG("-->createObj(" << pAddress->par()[0] << "/" << pAddress->par()[1]
                << ", " << refpObject << ")...");
  GenericAddress *addr = dynamic_cast<GenericAddress*>(pAddress);
  if (!addr) {
    ATH_MSG_DEBUG(pAddress->par()[0] << "/" << pAddress->par()[1]
                  << " is NOT a GenericAddress!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("  par:  " << pAddress->par()[0] << "/" << pAddress->par()[1]
                << endmsg <<
                "  ipar: " << *pAddress->ipar());
  return AthCnvSvc::createObj(pAddress, refpObject);
}

/// Implementation of IConverter: Convert the transient object to the
/// requested representation.
/// @param pObject [IN] pointer to DataObject.
/// @param refpAddress [OUT] pointer to IOpaqueAddress of the representation
///        to be created.
StatusCode 
NtupleCnvSvc::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress)
{
  ATH_MSG_DEBUG("::createRep(" << refpAddress << ", " << pObject << ")...");
  //std::cerr << "::NCS::createRep(" << this->name() << ", " << pObject << ")...\n";
  if (0==pObject) {
    ATH_MSG_ERROR("::createRep: received null ptr to DataObject");
    return StatusCode::FAILURE;
  }
  return ::AthCnvSvc::createRep(pObject, refpAddress);
}

/// Implementation of IConverter: Resolve the references of the converted
/// object.
/// @param pAddress [IN] pointer to IOpaqueAddress of the representation to
///        be resolved.
/// @param pObject [IN] pointer to DataObject to be created.
StatusCode 
NtupleCnvSvc::fillRepRefs(IOpaqueAddress* pAddress, DataObject* pObject)
{
  ATH_MSG_DEBUG("::fillRepRefs(" << pAddress << ", " << pObject << ")...");
  if (0==pAddress || 0==pObject) {
    ATH_MSG_ERROR("::fillRepRefs: null ptr received! (paddr=" << pAddress
                  << ", pobj=" << pObject << ")");
    return StatusCode::FAILURE;
  }
  return ::AthCnvSvc::fillRepRefs(pAddress, pObject);
}

/// Connect the output connection specification to the service with open mode.
/// @param outputConnectionSpec [IN] the name of the output connection 
///        specification as string.
/// @param openMode [IN] the open mode of the file as string.
StatusCode 
NtupleCnvSvc::connectOutput(const std::string& outputConnectionSpec,
			    const std::string& openMode)
{
  ATH_MSG_DEBUG("::connectOutput(" 
                << outputConnectionSpec << ", " 
                << openMode << ")");
  // std::cerr << "::connectOutput(" << outputConnectionSpec << ", openmode="
  //       << openMode << ")...\n";
  TFile *f = TFile::Open(outputConnectionSpec.c_str(), "recreate");
  if (f == 0) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/// Connect the output connection specification to the service with open mode.
/// @param outputConnectionSpec [IN] the name of the output connection
///        specification as string.
StatusCode
NtupleCnvSvc::connectOutput(const std::string& outputConnectionSpec)
{
  ATH_MSG_DEBUG("::connectOutput(" 
                << outputConnectionSpec << ")");
  // std::cerr << "::connectOutput(" << outputConnectionSpec << ")...\n";
  TFile *f = TFile::Open(outputConnectionSpec.c_str(), "recreate");
  if (f == 0) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Commit pending output.
StatusCode 
NtupleCnvSvc::commitOutput(const std::string& outputConnectionSpec,
                        bool doCommit)
{
  ATH_MSG_DEBUG("::commitOutput(" << outputConnectionSpec
               << ", " << doCommit << ")...");
  return StatusCode::SUCCESS;
}
//@}


/// @c IAddressCreator interfaces
//@{

/// Create a Generic address using explicit arguments to identify a single
/// object.
/// @param svcType [IN] service type of the address.
/// @param clid [IN] class id for the address.
/// @param par [IN] string containing the database name.
/// @param ip [IN] object identifier.
/// @param refpAddress [OUT] converted address.
StatusCode
NtupleCnvSvc::createAddress(long svcType,
                         const CLID& clid,
                         const std::string* par,
                         const unsigned long* ip,
                         IOpaqueAddress*& refpAddress)
{
  ATH_MSG_DEBUG("::createAddress(" << svcType << ", " << clid 
                << ", " << par << ", " << ip << ", " << refpAddress
                << ")...");
  return ::AthCnvSvc::createAddress(svcType, clid, par, ip, refpAddress);
}

/// Convert address to string form
/// @param pAddress [IN] address to be converted.
/// @param refAddress [OUT] converted string form.
StatusCode 
NtupleCnvSvc::convertAddress(const IOpaqueAddress* pAddress,
                          std::string& refAddress)
{
  ATH_MSG_DEBUG("::convertAddress(" << pAddress << ", " 
                << refAddress << ")...");
  if (pAddress) {
    refAddress = *pAddress->par();
    return StatusCode::SUCCESS;
  }
  ATH_MSG_ERROR("::convertAddress: null pointer to IOpaqueAddress !");
  return StatusCode::FAILURE;
}

/// Create address from string form
/// @param svcType [IN] service type of the address.
/// @param clid [IN] class id for the address.
/// @param refAddress [IN] string form to be converted.
/// @param refpAddress [OUT] converted address.
StatusCode 
NtupleCnvSvc::createAddress(long svcType,
                            const CLID& clid,
                            const std::string& refAddress,
                            IOpaqueAddress*& refpAddress)
{
  ATH_MSG_DEBUG("::createAddress(" << svcType << ", " << clid 
               << ", " << refAddress << ", " << refpAddress
               << ")...");
  if (svcType != ROOT_StorageType) {
    ATH_MSG_ERROR("::createAddress: svcType != root-type");
    ATH_MSG_ERROR("    svcType   = [" << svcType << "]");
    ATH_MSG_ERROR("    root-type = [" << ROOT_StorageType << "]");
    return StatusCode::FAILURE;
  }
  refpAddress = new GenericAddress(ROOT_StorageType, clid, refAddress);
  return StatusCode::SUCCESS;
}
//@}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// Create new Converter using factory (override AthCnvSvc::createConverter)
IConverter* 
NtupleCnvSvc::createConverter(long typ, 
                              const CLID& clid, 
                              const ICnvFactory* fac)
{
  ATH_MSG_DEBUG("::createConverter(" << typ << ", " << clid
                << ", " << fac << ")...");
  IConverter* cnv = new Athena::LeafCnv(clid, serviceLocator());
  if (cnv) {
    return cnv;
  }
  return AthCnvSvc::createConverter(typ, clid, fac);
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace Athena
