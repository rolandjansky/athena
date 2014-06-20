///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootSvc.h
// Header file for class Athena::RootSvc
// Author: Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef ATHENAROOTCOMPS_ROOTSVC_H
#define ATHENAROOTCOMPS_ROOTSVC_H 1

/** @file RootSvc.h
 *  @brief This file contains the class definition for the RootSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// AthenaRootKernel includes
#include "AthenaRootKernel/IRootSvc.h"
#include "AthenaRootKernel/IIoSvc.h"

// STL includes
#include <string>
#include "CxxUtils/unordered_map.h" //> migrate to STL

// fwk includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaBaseComps/AthService.h"

// fwd declares
namespace Athena { class RootConnection; }
class TClass;

namespace Athena {
/** @class RootSvc
 *  @brief This class provides the interface to the ROOT software.
 **/
class RootSvc
  : virtual public ::IRootSvc,
            public ::AthService
{
  friend class SvcFactory<Athena::RootSvc>;

public:
  /// Standard Service Constructor
  RootSvc(const std::string& name, ISvcLocator* pSvcLocator);

  /// Gaudi Service Interface method implementations:
  StatusCode initialize();
  StatusCode finalize();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// Retrieve interface ID
  static const InterfaceID& interfaceID()
  {
    return ::IRootSvc::interfaceID();
  }

  ///@{ RootType-base interface

  /// Load the type (dictionary) from Root.
  RootType getType(const std::type_info& type) const;

  /// Create an object of a given `RootType`.
  void* createObject(const RootType type) const;

  /// Write object of a given class to Root.
  unsigned long writeObject(const std::string& placement,
                            const RootType type,
                            const void* pObj);

  /// Destruct a given object of type `RootType`.
  void destructObject(RootType type, void* pObj) const;

  ///@}

  ///@{ CINT-base interface
  /// Load the class (dictionary) from Root.
  TClass* getClass(const std::type_info& type) const;

  /// Create an object of a given TClass.
  void* createObject(const TClass* classDesc) const;

  /// Write object of a given class to Root.
  unsigned long writeObject(const std::string& placement,
                            const TClass* classDesc,
                            const void* pObj);

  /// Destructor a given object of TClass.
  void destructObject(TClass* classDesc, void* pObj) const;

  ///@}

  /// open the file `fname` with open mode `mode`
  /// @returns the file descriptor or -1 if failure
  IIoSvc::Fd open(const std::string& fname,
                  IIoSvc::IoType mode);

  /// Connect the file descriptor `fd` to the service.
  StatusCode connect(IIoSvc::Fd fd);

  /// get the RootConnection associated with file descriptor `fd`
  /// @returns NULL if no such file is known to this service
  Athena::RootConnection* connection(IIoSvc::Fd fd);

  /// get the connection (or create it) for file descriptor `fd`
  Athena::RootConnection* new_connection(IIoSvc::Fd fd);

  /// Commit data and flush buffer.
  StatusCode commitOutput();

  /// Disconnect the file from the service.
  StatusCode disconnect(IIoSvc::Fd fd);

protected:
   /// Destructor
   virtual ~RootSvc();

private:
   /// Default constructor:
   RootSvc(); //< not implemented
   RootSvc(const RootSvc&); //< not implemented
   RootSvc& operator=(const RootSvc&); //< not implemented

private:
  typedef SG::unordered_map<IIoSvc::Fd,
                            Athena::RootConnection*> ConnMap_t;
  /// Map of file name keys and connection values.
  ConnMap_t m_conns;

  Athena::RootConnection* m_wconn;

  /// dict loader service
  mutable ServiceHandle< ::IDictLoaderSvc> m_dictsvc;

  /// iosvc
  ServiceHandle< ::IIoSvc> m_iosvc;
};

}//> namespace Athena

#endif /* !ATHENAROOTCOMPS_ROOTSVC_H */
