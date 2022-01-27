///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "RootUtils/WithRootErrorHandler.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "TClassEdit.h"


namespace {

bool startsWith (const std::string& a, const std::string& b)
{
  return (a.compare (0, b.size(), b) == 0);
}

} // anonymous namespace

// Constructors
////////////////
AthDictLoaderSvc::AthDictLoaderSvc (const std::string& name, 
                                    ISvcLocator* pSvcLocator) : 
  ::AthService( name, pSvcLocator ),
  m_dsodb (nullptr),
  m_clidSvc ("ClassIDSvc", name),
  m_tpCnvSvc("AthTPCnvSvc", name)
{
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

  ATH_CHECK( m_clidSvc.retrieve() );
  ATH_CHECK( m_tpCnvSvc.retrieve() );

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
 *         If @c recursive is true, then recursively load contained types.
 */
const RootType
AthDictLoaderSvc::load_type (const std::string& type_name,
                             bool recursive /* = false*/)
{
  ATH_MSG_DEBUG ("loading [" << type_name << "]...");

  // MN: short-cutting all the dance with type names done in DSODB...
  // may need verification
  // return RootType::ByName (m_dsodb->load_type(type_name));
  RootType rt = RootType::ByNameNoQuiet(type_name);

  if (recursive) {
    load_recursive (rt);
  }
  return rt;
}

/** @brief retrieve a @c Reflex::Type by @c std::type_info (auto)loading the
 *         dictionary by any necessary means.
 *         This method is preferred over the above one as it is guaranteed to
 *         succeed *IF* the dictionary for that type has been generated.
 *         If @c recursive is true, then recursively load contained types.
 */
const RootType
AthDictLoaderSvc::load_type (const std::type_info& typeinfo,
                             bool recursive /*= false*/)
{
  ATH_MSG_DEBUG 
    ("loading [" << System::typeinfoName(typeinfo) << " (from typeinfo)]...");
  return load_type (System::typeinfoName(typeinfo), recursive);
}

/** @brief retrieve a @c Reflex::Type by name (auto)loading the dictionary
 *         by any necessary means.
 *         If @c recursive is true, then recursively load contained types.
 */
const RootType
AthDictLoaderSvc::load_type (CLID clid, bool recursive /*= false*/)
{
  std::string name = "<N/A>";
  if (!m_clidSvc->getTypeNameOfID(clid, name).isSuccess()) {
    ATH_MSG_INFO ("could not retrieve typename for clid [" << clid << "]");
    // try out the bare std::type_info if available...
    const SG::BaseInfoBase* bib = SG::BaseInfoBase::find(clid);
    if (bib) {
      return load_type(bib->typeinfo(), recursive);
    }
    // fail early...
    return RootType();
  }
  
  ATH_MSG_DEBUG("loading [" << name << " (from clid="<<clid<<")]...");

  RootType type = load_type(name, recursive);
  if (type) {
    return type;
  }

  // try out the typeinfo-name
  m_clidSvc->getTypeInfoNameOfID(clid, name).ignore();
  type = load_type(name, recursive);
  if (type) {
    return type;
  }

  // try out the bare std::type_info if available...
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find(clid);
  if (bib) {
    return load_type(bib->typeinfo(), recursive);
  }
  // cppcheck-suppress identicalConditionAfterEarlyExit
  return type;
}


void AthDictLoaderSvc::load_recursive (const RootType& typ)
{
  Memo_t memo;
  RootUtils::WithRootErrorHandler hand ([] (int, bool, const char*, const char*) { return false; });
  load_recursive1 (typ, memo);
}
  
void AthDictLoaderSvc::load_recursive1 (const std::string& tnam,
                                        Memo_t& memo)
{
  if (startsWith (tnam, "const ")) {
    load_recursive1 (RootType::ByNameNoQuiet (tnam.substr (6, std::string::npos)),
                     memo);
  }
  else {
    load_recursive1 (RootType::ByNameNoQuiet (tnam), memo);
  }
}
void AthDictLoaderSvc::load_recursive1 (const RootType& typ, Memo_t& memo)
{
  if (!typ) return;
  if (typ.IsFundamental() || typ.IsPointer() || typ.IsEnum()) return;
  if (!typ.IsClass()) return;
  std::string nam = typ.Name();
  if (!memo.insert (nam).second) return;
  if (nam == "string" || nam == "std::string" ||
      startsWith (nam, "basic_string<") ||
      startsWith (nam, "std::basic_string<"))
  {
    return;
  }
  if (startsWith (nam, "vector<") || startsWith (nam, "std::vector<")) {
    TClassEdit::TSplitType split (nam.c_str());
    if (split.fElements.size() > 1) {
      load_recursive1 (split.fElements[1], memo);
      return;
    }
  }
  else if (startsWith (nam, "DataVector<")) {
    TClassEdit::TSplitType split (nam.c_str());
    if (split.fElements.size() > 1) {
      load_recursive1 (split.fElements[1], memo);
      return;
    }
  }
  else if (startsWith (nam, "pair<") || startsWith (nam, "std::pair<")) {
    TClassEdit::TSplitType split (nam.c_str());
    if (split.fElements.size() > 2) {
      load_recursive1 (split.fElements[1], memo);
      load_recursive1 (split.fElements[2], memo);
      return;
    }
  }
  else if (startsWith (nam, "map<") || startsWith (nam, "std::map<")) {
    TClassEdit::TSplitType split (nam.c_str());
    if (split.fElements.size() > 2) {
      load_recursive1 (split.fElements[1], memo);
      load_recursive1 (split.fElements[2], memo);
      // For ROOT persistency.
      std::string pname = "std::pair<" + split.fElements[1] + "," + split.fElements[2] + ">";
      load_type (pname);
      return;
    }
  }
  else if (startsWith (nam, "LArConditionsContainer<")) {
    TClassEdit::TSplitType split (nam.c_str());
    if (split.fElements.size() > 1) {
      std::string pname = "LArConditionsSubset<" + split.fElements[1] + ">";
      load_recursive1 (pname, memo);

      std::unique_ptr<ITPCnvBase> tpcnv = m_tpCnvSvc->t2p_cnv_unique (pname);
      if (tpcnv) {
        load_type (tpcnv->persistentTInfo(), true);
      }
      return;
    }
  }

  size_t nbase = typ.BaseSize();
  for (size_t i = 0; i < nbase; i++) {
    load_recursive1 (typ.BaseAt(i).Name(), memo);
  }

  size_t nmem = typ.DataMemberSize();
  for (size_t i = 0; i < nmem; i++) {
    RootType mem = typ.DataMemberAt(i).TypeOf();
    load_recursive1 (mem, memo);
  }
}
