// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tpcnvLoad.h 613944 2014-08-28 21:17:20Z ssnyder $

/**
 * @file AthenaROOTAccess/src/tpcnvLoad.cxx
 * @author Sebastien Binet
 * @date Aug 2010
 * @brief load a T/P cnv via the Reflex plugin service.
 */

#ifndef ATHENAROOTACCESS_TPCNVLOAD_H
#define ATHENAROOTACCESS_TPCNVLOAD_H

#include "AthenaKernel/ITPCnvBase.h"
#include <string>
#include <vector>


namespace AthenaROOTAccess {


namespace TPCnv {

  class Registry
  {
  public:
    typedef std::vector<ITPCnvBase*> TpCnvs_t;
  private:
    /** a registry of ITPCnvBase* instances
     */
    TpCnvs_t m_cnvs;

    Registry();

  public:

    /** d-tor
     */
    ~Registry();

    /** the only way to get a registry instance.
     */
    static Registry& instance();

    /** @brief load the T/P converter class named `cls`
     *  return NULL on failure.
     *  the converter is OWNED by the registry
     */
    ITPCnvBase* load_tpcnv(const std::string& cls);

    /** @brief given a persistent class name, return the proper P->T converter
     */
    ITPCnvBase *p2t_cnv(const std::string& persClassName);
  
    /** @brief given a transient class name, return the proper T->P converter
     */
    ITPCnvBase *t2p_cnv(const std::string& transClassName);

    /** @brief given a transient class name,
     *         return the proper persistent class name.
     */
    std::string p2t_name(const std::string& persClassName);
  };
  
} // namespace TPCnv

} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_TPCNVLOAD_H

