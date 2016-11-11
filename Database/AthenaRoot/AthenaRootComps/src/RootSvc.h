///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootSvc.h
// Header file for class Athena::RootSvc
// Author: Peter van Gemmeren <gemmeren@anl.gov>
///////////////////////////////////////////////////////////////////
#ifndef ATHENAROOTCOMPS_ROOTSVC_H
#define ATHENAROOTCOMPS_ROOTSVC_H 1

/** @file RootSvc.h
 *  @brief This file contains the class definition for the RootSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// AthenaRootKernel includes
#include "AthenaRootKernel/IRootSvc.h"

// fwk includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "PersistentDataModel/Guid.h"

#include <map>

// fwd declares
class IDictLoaderSvc;
class Placement;
namespace pool { class IFileCatalog; }
namespace Athena { class RootConnection; }

namespace Athena {
/** @class RootSvc
 *  @brief This class provides the interface to the ROOT software.
 **/
class RootSvc : virtual public ::IRootSvc, public ::AthService {
  friend class SvcFactory<Athena::RootSvc>;

public:
  /// Standard Service Constructor
  RootSvc(const std::string& name, ISvcLocator* pSvcLocator);

  /// Gaudi Service Interface method implementations:
  StatusCode initialize();
  StatusCode finalize();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return ::IRootSvc::interfaceID();
  }

  ///@{ RootType-base interface
  /// Load the type (dictionary) from Root.
  RootType getType(const std::type_info& type) const;

  /// Read object from Root.
  void* readObject(const Token& token, void*& pObj);

  /// Write object of a given class to Root.
  const Token* writeObject(const Placement& placement, const RootType& type, const void* pObj);

  /// Create an object of a given `RootType`.
  void* createObject(const RootType& type) const;

  /// Destruct a given object of type `RootType`.
  void destructObject(const RootType& type, void* pObj) const;
  ///@}

  /// Open the file `fname` with open mode `mode`
  StatusCode open(const std::string& fname, const std::string& mode);

  /// Connect the file `fname` to the service.
  StatusCode connect(const std::string& fname);

  /// Commit data and flush buffer.
  StatusCode commitOutput();

  /// Disconnect the file `fname` from the service.
  StatusCode disconnect(const std::string& fname);

  /// Get the RootConnection associated with file `fname`
  /// @returns NULL if no such file is known to this service
  Athena::RootConnection* connection(const std::string& fname);

protected:
  /// Destructor
  virtual ~RootSvc();

private:
  /// Default constructor:
  RootSvc(); //< not implemented
  RootSvc(const RootSvc&); //< not implemented
  RootSvc& operator=(const RootSvc&); //< not implemented

private:
  pool::IFileCatalog* m_catalog;

  typedef std::map<Guid, Athena::RootConnection*> ConnMap_t;
  /// Map of file name keys and connection values.
  ConnMap_t m_conns;
  Athena::RootConnection* m_wconn;

  /// ServiceHandle to the dictionary service
  ServiceHandle< ::IDictLoaderSvc> m_dictSvc;
};

}//> namespace Athena

#endif /* !ATHENAROOTCOMPS_ROOTSVC_H */
