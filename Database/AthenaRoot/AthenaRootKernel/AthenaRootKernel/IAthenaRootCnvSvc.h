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

// fwk includes
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IAddressCreator.h"
#include "DataModelRoot/RootType.h"

// stl includes
#include <typeinfo>

// fwd declares
class Token;
class ITPCnvBase;

/** @class IAthenaRootCnvSvc
 *  @brief This class provides the interface between Athena and RootSvc.
 **/
class IAthenaRootCnvSvc : virtual public ::IConversionSvc, virtual public ::IAddressCreator {
public:
  virtual ~IAthenaRootCnvSvc();

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    static const InterfaceID s_iid("IAthenaRootCnvSvc", 1, 0);
    return s_iid;
  }

  ///@{ RootType-based API
  /// Load the class (dictionary) from Root.
  virtual RootType getType(const CLID& clid) const = 0;
  virtual RootType getType(const std::type_info& type) const = 0;

  /// Write object of a given class to Root, using optional T/P converter.
  virtual const Token* writeObject(const std::string& key,
		  const RootType& typeDesc,
		  const void* pObj,
		  ITPCnvBase* tpConverter = 0) = 0;
  ///@}

  /// Get T/P converter for a transient class.
  virtual ITPCnvBase* getTPConverter(const CLID& clid) const = 0;
};

#endif /* !ATHENAROOTKERNEL_IATHENAROOTCNVSVC_H */
