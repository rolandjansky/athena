///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CondInputLoader.h 
// Header file for class CondInputLoader
/////////////////////////////////////////////////////////////////// 
#ifndef IOVSVC_CONDINPUTLOADER_H
#define IOVSVC_CONDINPUTLOADER_H 1

// STL includes
#include <string>


#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IIOVSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"

#include <string>
#include <map>


class CondInputLoader
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CondInputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CondInputLoader(); 

  // Assignment operator: 
  //CondInputLoader &operator=(const CondInputLoader &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  start();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  //  CondInputLoader();

  // need to override the ExtraInputs/Outputs property handler
  // from AthAlgorithm
  void extraDeps_update_handler(Property&);

  //  void loader(Property&);

  /// Containers
  DataObjIDColl m_load, m_handlesToCreate;
  std::vector< SG::VarHandleKey > m_vhk;

  Gaudi::Property<bool> m_dumpEvt{ this, "DumpCondStore", false, 
      "dump the ConditionStore at the end execute"};
  Gaudi::Property<bool> m_abort {this, "AbortIfInitFails", true, 
      "Abort execution if unable to create the CondCont<T> in first event"};


  bool m_first { true };

  ServiceHandle<StoreGateSvc> m_condStore;
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<IIOVSvc> m_IOVSvc;
  ServiceHandle<IClassIDSvc> m_clidSvc;
  
  std::map<std::string,std::string> m_keyFolderMap;
}; 


#endif //> !SGCOMPS_SGINPUTLOADER_H
