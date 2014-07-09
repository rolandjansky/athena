/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/TypeNameConversions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Utility functions for converting between type names, type_info,
 *        and CLIDs.
 */


#include "D3PDMakerUtils/TypeNameConversions.h"
#include "SGTools/BaseInfo.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "GaudiKernel/System.h"
#include "TClass.h"


namespace D3PD {


/**
 * @brief Convert from a class name to a type_info.
 * @param name The class name to convert.
 * @param ti[out] The resulting type_info..
 * @param context Context name, for error reporting.
 * @param clidsvc Handle to the CLID service.
 */
StatusCode nameToTypeinfo (const std::string& name,
                           std::type_info const* &ti,
                           const std::string& context,
                           ServiceHandle<IClassIDSvc> clidsvc
                             /*= ServiceHandle<IClassIDSvc>("ClassIDSvc")*/)
{
  ti = 0;

  // See if root knows the type.
  const TClass* cls = TClass::GetClass (name.c_str());
  if (cls) {
    ti = cls->GetTypeInfo();
    if (ti) return StatusCode::SUCCESS;
  }

  // Try to look it up using BaseInfo.  First need to convert to a CLID.
  CLID clid;
  if (nameToCLID (name, clid, context, clidsvc).isSuccess()) {
    const SG::BaseInfoBase* bib = SG::BaseInfoBase::find (clid);
    if (bib) {
      ti = &bib->typeinfo();
      return StatusCode::SUCCESS;
    }
  }

  REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, context)
    << "Can't convert type name " << name << " to type_info.";

  return StatusCode::FAILURE;
}


/**
 * @brief Convert from a class name to a CLID.
 * @param name The class name to convert.
 * @param clid[out] The resulting CLID.
 * @param context Context name, for error reporting.
 * @param clidsvc Handle to the CLID service.
 */
StatusCode nameToCLID (const std::string& name,
                       CLID& clid,
                       const std::string& context,
                       ServiceHandle<IClassIDSvc> clidsvc
                         /*= ServiceHandle<IClassIDSvc>("ClassIDSvc")*/)
{
  clid = CLID_NULL;
  CHECK_WITH_CONTEXT( clidsvc.retrieve(), context );

  if (clidsvc->getIDOfTypeName (name, clid).isSuccess())
    return StatusCode::SUCCESS;
  if (clidsvc->getIDOfTypeInfoName (name, clid).isSuccess())
    return StatusCode::SUCCESS;

  REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, context)
    << "Can't find CLID for type name " << name;
  return StatusCode::FAILURE;
}


/**
 * @brief Convert from a CLID to a type_info; we also already know the name.
 * @param clid The CLID to convert.
 * @param name The name of the class to convert.
 * @param ti[out] The resulting type_info.
 * @param context Context name, for error reporting.
 */
StatusCode clidToTypeinfo (CLID clid,
                           const std::string& name,
                           std::type_info const* &ti,
                           const std::string& context)
{
  ti = 0;
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find (clid);
  if (bib) {
    ti = &bib->typeinfo();
    return StatusCode::SUCCESS;
  }

  // Hmm.. no BaseInfo.  Maybe Root knows it.
  const TClass* cls =  TClass::GetClass (name.c_str());
  if (cls){
    ti = cls->GetTypeInfo();
    if (ti)
      return StatusCode::SUCCESS;
  }

  REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, context)
    << "Can't find typeinfo for CLID "
    << clid << " (type " << name << ").";
  return StatusCode::FAILURE;
}


/**
 * @brief Convert from a type_info to a name.
 * @param ti The type_info to convert.
 */
std::string typeinfoToName (const std::type_info& ti)
{
  // Handle fundamental types.
  if (ti == typeid(float))
    return "float";
  else if (ti == typeid(double))
    return "double";
  else if (ti == typeid(int))
    return "int";
  else if (ti == typeid(bool))
    return "bool";
  else if (ti == typeid(char))
    return "char";
  else if (ti == typeid(signed char))
    return "signed char";
  else if (ti == typeid(unsigned char))
    return "unsigned char";
  else if (ti == typeid(short))
    return "short";
  else if (ti == typeid(unsigned short))
    return "unsigned short";
  else if (ti == typeid(unsigned int))
    return "unsigned int";
  else if (ti == typeid(long))
    return "long";
  else if (ti == typeid(unsigned long))
    return "unsigned long";
  else if (ti == typeid(long long))
    return "long long";
  else if (ti == typeid(unsigned long long))
    return "unsigned long long";
  else if (ti == typeid(long double))
    return "long double";
  else if (ti == typeid(wchar_t))
    return "wchar_t";

  // Otherwise try to look it up with root.
  std::string eltname;
  TClass* eltcls = TClass::GetClass (ti);
  if (eltcls) {
    eltname = eltcls->GetName();
  }
  else {
    std::string eltname = System::typeinfoName (ti);
    eltcls = TClass::GetClass (eltname.c_str());
    if (eltcls)
      eltname = eltcls->GetName();
  }
  return eltname;
}


} // namespace D3PD
