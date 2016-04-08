///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAROOTKERNEL_IROOTSVC_H
#define ATHENAROOTKERNEL_IROOTSVC_H 1

/** @file IRootSvc.h
 *  @brief This file contains the class definition for the IRootSvc interface class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// fwk includes
#include "GaudiKernel/IInterface.h"

#include "DataModelRoot/RootType.h"

// stl includes
#include <typeinfo>
#include <string>

// fwd declares
class Token;
class Placement;
namespace Athena { class RootConnection; }

/** @class IRootSvc
 *  @brief This class provides the interface to the ROOT software.
 **/
class IRootSvc : virtual public IInterface {
public:
  virtual ~IRootSvc();

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    static const InterfaceID s_iid("IRootSvc", 1, 0);
    return s_iid;
  }

  ///@{ RootType-base interface
  /// Load the type (dictionary) from Root.
  virtual RootType getType(const std::type_info& type) const = 0;

  /// Read object from Root.
  virtual void* readObject(const Token& token, void*& pObj) = 0;

  /// Write object of a given class to Root.
  virtual const Token* writeObject(const Placement& placement, const RootType& type, const void* pObj) = 0;

  /// Create an object of a given `RootType`.
  virtual void* createObject(const RootType& type) const = 0;

  /// Destruct a given object of type `RootType`.
  virtual void destructObject(const RootType& type, void* pObj) const = 0;

  ///@}

  /// Open the file `fname` with open mode `mode`
  virtual StatusCode open(const std::string& fname, const std::string& mode) = 0;

  /// Connect the file `fname` to the service.
  virtual StatusCode connect(const std::string& fname) = 0;

  /// Commit data and flush buffer.
  virtual StatusCode commitOutput() = 0;

  /// Disconnect the file `fname` from the service.
  virtual StatusCode disconnect(const std::string& fname) = 0;

  /// Get the RootConnection associated with file `fname`
  /// @returns NULL if no such file is known to this service
  virtual Athena::RootConnection* connection(const std::string& fname) = 0;
};

#endif /* !ATHENAROOTKERNEL_IROOTSVC_H */
