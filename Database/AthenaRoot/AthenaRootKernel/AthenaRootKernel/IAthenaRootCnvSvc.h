///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAROOTKERNEL_IATHENAROOTCNVSVC_H
#define ATHENAROOTKERNEL_IATHENAROOTCNVSVC_H 1

/** @file IAthenaRootCnvSvc.h
 *  @brief This file contains the class definition for the IAthenaRootCnvSvc interface class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// stl includes
#include <typeinfo>

// fwk includes
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IAddressCreator.h"
#include "DataModelRoot/RootType.h"

// fwd declares
class TClass;
class ITPCnvBase;

/** @class IAthenaRootCnvSvc
 *  @brief This class provides the interface between Athena and RootSvc.
 **/
class IAthenaRootCnvSvc : 
  virtual public ::IConversionSvc, 
  virtual public ::IAddressCreator 
{

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() 
  { 
    /// Declaration of the interface ID 
    /// ( interface id, major version, minor version)
    static const InterfaceID s_iid("IAthenaRootCnvSvc", 1, 0);
    return s_iid; 
  }

  virtual ~IAthenaRootCnvSvc();

  /// Get T/P converter for a transient class.
  virtual ITPCnvBase* getTPConverter(const CLID& clid) const = 0;

  ///@{ CINT-based API
  /// Load the class (dictionary) from Root.
  virtual TClass* getClass(const CLID& clid) const = 0;
  virtual TClass* getClass(const std::type_info& type) const = 0;

  /// Write object of a given class to Root, using optional T/P converter.
  virtual unsigned long writeObject(const std::string& placement,
                                    TClass* classDesc,
                                    const void* pObj,
                                    ITPCnvBase* tpConverter = 0) = 0;
  ///@}

  ///@{ RootType-based API
  /// Load the class (dictionary) from Root.
  virtual RootType getType(const CLID& clid) const = 0;
  virtual RootType getType(const std::type_info& type) const = 0;

  /// Write object of a given class to Root, using optional T/P converter.
  virtual unsigned long writeObject(const std::string& placement,
                            RootType typeDesc,
                            const void* pObj,
                            ITPCnvBase* tpConverter = 0) = 0;
  ///@}
};

#endif /* !ATHENAROOTKERNEL_IATHENAROOTCNVSVC_H */
