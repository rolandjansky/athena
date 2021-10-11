///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthTPCnvSvc.cxx 
// Implementation file for class AthTPCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AthTPCnvSvc.h"
#include "GaudiKernel/Converter.h"


namespace {

std::string prefix (Athena::TPCnvType::Value type)
{
  switch (type)
  {
  case Athena::TPCnvType::ARA:
    return "_ARA";
  case Athena::TPCnvType::Trigger:
    return "_TRIG";
  default:
    break;
  }
  return "";
}

} // anonymous namespace


// Constructors
////////////////
AthTPCnvSvc::AthTPCnvSvc( const std::string& name, 
                          ISvcLocator* pSvcLocator ) : 
  base_class ( name, pSvcLocator ),
  m_clidSvc("ClassIDSvc/ClassIDSvc", name)
{
}

// Destructor
///////////////
AthTPCnvSvc::~AthTPCnvSvc()
{
  for (auto p : m_cnvs)
    delete p;
}


/** @brief load the T/P converter class named `cls`
 *  return NULL on failure.
 *  the converter is OWNED by the T/P converter service
 */
ITPCnvBase* 
AthTPCnvSvc::load_tpcnv(const std::string& cls)
{
  ITPCnvBase* cnv = ITPCnvBase::Factory::create (cls).release();
  if (cnv == nullptr) {
    ATH_MSG_INFO("could not load class [" << cls
                 << "] via Reflex::PluginService");
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}

/** @brief return the T/P converter for a transient class (NULL if failure)
 *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
 */ 
ITPCnvBase* 
AthTPCnvSvc::t2p_cnv(const std::string& transClassName,
                     Athena::TPCnvType::Value type /*= Athena::TPCnvType::Athena*/)

{
  ITPCnvBase* cnv = ITPCnvBase::Factory::create (prefix(type) + "_TRANS_" + transClassName).release();
  if (cnv == nullptr && type != Athena::TPCnvType::Athena)
    return t2p_cnv (transClassName);
  if (cnv == nullptr) {
    ATH_MSG_WARNING("Could not load converter for transient class ["
                    << transClassName << "]");
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}

/** @brief return the T/P converter for a transient class (NULL if failure)
 *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
 */ 
ITPCnvBase* 
AthTPCnvSvc::t2p_cnv(const CLID& transClid,
                     Athena::TPCnvType::Value type /*= Athena::TPCnvType::Athena*/)
{
  std::string trans_type;
  if (!m_clidSvc->getTypeNameOfID(transClid, trans_type).isSuccess()) {
    ATH_MSG_INFO("could not get a type-name for clid [" << transClid << "]");
    return nullptr;
  }

  ITPCnvBase* cnv = ITPCnvBase::Factory::create (prefix(type) + "_TRANS_" + trans_type).release();
  if (cnv == nullptr) {
    // try a typeinfo-name before bailing out...
    if (!m_clidSvc->getTypeInfoNameOfID(transClid, trans_type).isSuccess()) {
      ATH_MSG_INFO("could not get a typeinfo-name for clid ["
                   << transClid << "]");
      return nullptr;
    }
    cnv = ITPCnvBase::Factory::create (prefix(type) + "_TRANS_" + trans_type).release();
  }
  if (cnv == nullptr && type != Athena::TPCnvType::Athena)
    return t2p_cnv (transClid);
  if (cnv == nullptr) {
    ATH_MSG_WARNING("could not load converter for transient CLID ["
                    << transClid << "] (" << trans_type << ")");
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}

/** @brief return the T/P converter for a persistent class (NULL if failure)
 *         `ITPCnvSvc` owns the `ITPCnvBase` pointer
 */ 
ITPCnvBase* 
AthTPCnvSvc::p2t_cnv(const std::string& persClassName,
                     Athena::TPCnvType::Value type /*= Athena::TPCnvType::Athena*/)
{
  ITPCnvBase* cnv = ITPCnvBase::Factory::create (prefix(type) + "_PERS_" + persClassName).release();
  if (cnv == nullptr && type != Athena::TPCnvType::Athena)
    return p2t_cnv (persClassName);
  if (cnv == nullptr) {
    ATH_MSG_WARNING("Could not load converter for persistent class ["
                    << persClassName << "]");
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}


/** @brief Return the T/P converter for a transient class.
 *         Ownership is returned to the caller.
 */ 
std::unique_ptr<ITPCnvBase>
AthTPCnvSvc::t2p_cnv_unique(const std::string& transClassName) const
{
  return ITPCnvBase::Factory::create ("_TRANS_" + transClassName);
}


/** @brief Return the T/P converter for a transient class.
 *         Returns null on failure (with no warning printed).
 *         Ownership is returned to the caller.
 */ 
std::unique_ptr<ITPCnvBase>
AthTPCnvSvc::t2p_cnv_unique(const CLID transClid) const
{
  std::string trans_type;
  if (!m_clidSvc->getTypeNameOfID(transClid, trans_type).isSuccess()) {
    return nullptr;
  }

  std::unique_ptr<ITPCnvBase> cnv = ITPCnvBase::Factory::create (/*prefix(type) +*/ "_TRANS_" + trans_type);
  if (cnv == nullptr) {
    // try a typeinfo-name before bailing out...
    if (!m_clidSvc->getTypeInfoNameOfID(transClid, trans_type).isSuccess()) {
      return nullptr;
    }
    cnv = ITPCnvBase::Factory::create (/*prefix(type) +*/ "_TRANS_" + trans_type);
  }
  /*
  ** FIXME: support other types.
  if (cnv == nullptr && type != Athena::TPCnvType::Athena)
    return t2p_cnv (transClid);
  */
  return cnv;
}
