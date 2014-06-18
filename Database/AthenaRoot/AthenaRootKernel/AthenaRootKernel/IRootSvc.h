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

// stl includes
#include <typeinfo>
#include <string>

// fwk includes
#include "GaudiKernel/IInterface.h"
#include "AthenaRootKernel/IIoSvc.h"
#include "DataModelRoot/RootType.h"

// fwd declares
class TClass;
namespace Athena { class RootConnection; }

/** @class IRootSvc
 *  @brief This class provides the interface to the ROOT software.
 **/
class IRootSvc : virtual public IInterface
{

public:

  virtual ~IRootSvc();

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Public members: 
  /////////////////////////////////////////////////////////////////// 

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() 
  { 
    static const InterfaceID s_iid("IRootSvc", 1, 0);
    return s_iid;
  }

  ///@{ RootType-base interface
  /// Load the type (dictionary) from Root.
  virtual RootType getType(const std::type_info& type) const = 0;

  /// Create an object of a given `RootType`.
  virtual void* createObject(const RootType type) const = 0;

  /// Write object of a given class to Root.
  virtual unsigned long writeObject(const std::string& placement,
                            const RootType type,
                            const void* pObj) = 0;

  /// Destruct a given object of type `RootType`.
  virtual void destructObject(RootType type, void* pObj) const = 0;
  
  ///@}

  ///@{ CINT-based interface
  /// Load the class (dictionary) from Root.
  virtual TClass* getClass(const std::type_info& type) const = 0;

  /// Create an object of a given TClass.
  virtual void* createObject(const TClass* classDesc) const = 0;

  /// Write object of a given class to Root.
  virtual unsigned long writeObject(const std::string& placement,
                                    const TClass* classDesc,
                                    const void* pObj) = 0;

  /// Destructor a given object of TClass.
  virtual void destructObject(TClass* classDesc, void* pObj) const = 0;
  
  ///@}

  /// open the file `fname` with open mode `mode`
  /// @returns the file descriptor or -1 if failure
  virtual IIoSvc::Fd open(const std::string& fname,
                          IIoSvc::IoType mode) = 0;

  /// Connect the file descriptor `fd` to the service.
  virtual StatusCode connect(IIoSvc::Fd fd) = 0;
  
  /// get the RootConnection associated with file descriptor `fd`
  /// @returns NULL if no such file is known to this service
  virtual Athena::RootConnection* connection(IIoSvc::Fd fd) = 0;

  /// Commit data and flush buffer.
  virtual StatusCode commitOutput() = 0;
  
  /// Disconnect the file from the service.
  virtual StatusCode disconnect(IIoSvc::Fd fd) = 0;
};

#endif /* !ATHENAROOTKERNEL_IROOTSVC_H */
