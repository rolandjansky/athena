/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tpcnvLoad.cxx 613944 2014-08-28 21:17:20Z ssnyder $
/**
 * @file AthenaROOTAccess/src/tpcnvLoad.cxx
 * @author Sebastien Binet
 * @date Aug 2010
 * @brief load a T/P cnv via the Reflex plugin service.
 */



#include "AthenaROOTAccess/tpcnvLoad.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "Gaudi/PluginService.h"
#include "TError.h"


namespace AthenaROOTAccess {

namespace TPCnv {

Registry::Registry()
{}

Registry::~Registry()
{
  for (TpCnvs_t::iterator i = m_cnvs.begin(), iend=m_cnvs.end();
       i != iend;
       ++i) {
    delete *i;
  }
}

Registry& 
Registry::instance()
{
  static Registry self;
  return self;
}

/** @brief load the T/P converter class named `cls`
 *  return NULL on failure.
 *  the converter is OWNED by the registry
 */
ITPCnvBase* 
Registry::load_tpcnv(const std::string& cls)
{
  ITPCnvBase* cnv = ITPCnvBase::Factory::create (cls);
  if (cnv == 0) {
    ::Warning("tpcnvLoad", 
              "could not load class [%s] via Reflex::PluginService",
              cls.c_str());
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}

/** @brief given a persistent class name, return the proper P->T converter
 */
ITPCnvBase*
Registry::p2t_cnv(const std::string& persClassName)
{
  ITPCnvBase* cnv = ITPCnvBase::Factory::create ("_ARA_PERS_" + persClassName);
  if (!cnv)
    cnv = ITPCnvBase::Factory::create ("_PERS_" + persClassName);
  if (cnv == 0) {
    ::Warning("tpcnvLoad", 
              "could not load converter for persistent class [%s]",
              persClassName.c_str());
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}
  
/** @brief given a transient class name, return the proper T->P converter
 */
ITPCnvBase*
Registry::t2p_cnv(const std::string& transClassName)
{
  ITPCnvBase* cnv = ITPCnvBase::Factory::create ("_ARA_TRANS_" + transClassName);
  if (!cnv)
    cnv = ITPCnvBase::Factory::create ("_TRANS_" + transClassName);
  if (cnv == 0) {
    ::Warning("tpcnvLoad", 
              "could not load converter for transient class [%s]",
              transClassName.c_str());
  }
  else {
    m_cnvs.push_back (cnv);
  }
  return cnv;
}
  
/** @brief given a transient class name,
 *         return the proper persistent class name.
 */
std::string Registry::p2t_name(const std::string& persClassName)
{
  std::string out;
  ITPCnvBase* cnv = ITPCnvBase::Factory::create ("_ARA_PERS_" + persClassName);
  if (!cnv)
    cnv = ITPCnvBase::Factory::create ("_PERS_" + persClassName);
  if (cnv != 0) {
    out = SG::normalizedTypeinfoName (cnv->transientTInfo());
    delete cnv;
  }
  return out;
}

} // !namespace TPCnv

} // !namespace AthenaROOTAccess


