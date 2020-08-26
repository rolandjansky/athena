///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AthDictLoaderSvc.cxx 
// Implementation file for class AthDictLoaderSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaServices includes
#include "AthDictLoaderSvc.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/BaseInfo.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthDictLoaderSvc::AthDictLoaderSvc (const std::string& name, 
                                    ISvcLocator* pSvcLocator) : 
  ::AthService( name, pSvcLocator ),
  m_dsodb (nullptr),
  m_doRecursiveLoad (true),
  m_clidSvc ("ClassIDSvc", name)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty ("DoRecursiveLoad",
                   m_doRecursiveLoad = true,
                   "Switch to recursively load (or not) all dictionaries for "
                   "all types composing a given one. \n"
                   "  ie: load dict of Bar in 'struct Foo {Bar*b;};' \n"
                   "Default is 'true'.");
}

// Destructor
///////////////
AthDictLoaderSvc::~AthDictLoaderSvc()
{}

// Athena Service's Hooks
////////////////////////////
StatusCode AthDictLoaderSvc::initialize()
{
  ATH_MSG_INFO ("in initialize...");
  m_dsodb = Ath::DsoDb::instance();
  if (!m_dsodb) {
    ATH_MSG_ERROR("could not acquire Dso-registry");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO ("acquired Dso-registry");

  if (!m_clidSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve [" << m_clidSvc.typeAndName() << "]");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode AthDictLoaderSvc::finalize()
{
  ATH_MSG_INFO ("in finalize...");
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
AthDictLoaderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IDictLoaderSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IDictLoaderSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return ::AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/** @brief check a @c Reflex dictionary exists for a given type
 */
bool
AthDictLoaderSvc::has_type (const std::string& type_name)
{
  if (!m_dsodb)
    return false;

  bool has_type = m_dsodb->has_type(type_name);
  if (!has_type) {
    ATH_MSG_DEBUG ("no reflex dict. for type [" << type_name << "]");
  }

  return has_type;
}

/** @brief check a @c Reflex dictionary exists for a given type
 */
bool
AthDictLoaderSvc::has_type (const std::type_info& typeinfo)
{
  return has_type (System::typeinfoName(typeinfo).c_str());
}

/** @brief check a @c Reflex dictionary exists for a given type
 */
bool
AthDictLoaderSvc::has_type (CLID clid)
{
  bool ret = false;
  std::string name = "<N/A>";
  if (m_clidSvc->getTypeNameOfID(clid, name).isSuccess()) {
    ret = has_type (name);
    if (ret) {
      return ret;
    }
  }

  // try out the typeinfoname...
  if (m_clidSvc->getTypeInfoNameOfID(clid, name).isSuccess()) {
    ret = has_type (name);
    if (ret) {
      return ret;
    }
  }
  
  return ret;
}

/** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
 *         by any necessary means.
 */
const RootType
AthDictLoaderSvc::load_type (const std::string& type_name)
{
  ATH_MSG_DEBUG ("loading [" << type_name << "]...");

  // MN: short-cutting all the dance with type names done in DSODB...
  // may need verification
  // return RootType::ByName (m_dsodb->load_type(type_name));
  return RootType::ByNameNoQuiet(type_name);
}

/** @brief retrieve a @c Reflex::Type by @c std::type_info (auto)loading the
 *         dictionary by any necessary means.
 *         This method is preferred over the above one as it is guaranteed to
 *         succeed *IF* the dictionary for that type has been generated.
 */
const RootType
AthDictLoaderSvc::load_type (const std::type_info& typeinfo)
{
  ATH_MSG_DEBUG 
    ("loading [" << System::typeinfoName(typeinfo) << " (from typeinfo)]...");
  return load_type (System::typeinfoName(typeinfo));
}

/** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
 *         by any necessary means.
 */
const RootType
AthDictLoaderSvc::load_type (CLID clid)
{
  std::string name = "<N/A>";
  if (!m_clidSvc->getTypeNameOfID(clid, name).isSuccess()) {
    ATH_MSG_INFO ("could not retrieve typename for clid [" << clid << "]");
    // try out the bare std::type_info if available...
    const SG::BaseInfoBase* bib = SG::BaseInfoBase::find(clid);
    if (bib) {
      return load_type(bib->typeinfo());
    }
    // fail early...
    return RootType();
  }
  
  ATH_MSG_DEBUG("loading [" << name << " (from clid="<<clid<<")]...");

  RootType type = load_type(name);
  if (type) {
    return type;
  }

  // try out the typeinfo-name
  m_clidSvc->getTypeInfoNameOfID(clid, name).ignore();
  type = load_type(name);
  if (type) {
    return type;
  }

  // try out the bare std::type_info if available...
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find(clid);
  if (bib) {
    return load_type(bib->typeinfo());
  }
  return type;
}
