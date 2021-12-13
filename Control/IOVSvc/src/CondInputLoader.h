///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CondInputLoader.h 
// Header file for class CondInputLoader
/////////////////////////////////////////////////////////////////// 
#ifndef IOVSVC_CONDINPUTLOADER_H
#define IOVSVC_CONDINPUTLOADER_H 1

#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"

#include <string>
#include <map>


class CondInputLoader
  : public ::AthAlgorithm
{ 

 public:

  /// Constructor with parameters:
  CondInputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  start() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;


 private:

  // need to override the ExtraInputs/Outputs property handler
  // from AthAlgorithm
  void extraDeps_update_handler(Gaudi::Details::PropertyBase&);

  RootType loadDict (CLID clid);
  RootType loadDict (const std::string& name);

  /// Containers
  Gaudi::Property<DataObjIDColl> m_load{this,"Load",{},
                                        "List of objects to be loaded","OrderedSet<std::vector<std::string> >"};
  DataObjIDColl  m_handlesToCreate;
  std::vector< SG::VarHandleKey > m_vhk;

  Gaudi::Property<bool> m_dumpCondStore{ this, "DumpCondStore", false,
      "dump the ConditionStore at the end execute"};
  Gaudi::Property<bool> m_dumpCondSvc{ this, "DumpCondSvc", false,
      "dump the CondSvc at the end execute"};
  Gaudi::Property<bool> m_abort {this, "AbortIfInitFails", true, 
      "Abort execution if unable to create the CondCont<T> in first event"};


  ServiceHandle<StoreGateSvc> m_condStore;
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<IIOVSvc> m_IOVSvc;
  ServiceHandle<IIOVDbSvc> m_IOVDbSvc;
  ServiceHandle<IClassIDSvc> m_clidSvc;
  ServiceHandle<Athena::IRCUSvc> m_rcuSvc;
  ServiceHandle<IDictLoaderSvc> m_dictLoader
    { this, "DictLoaderSvc", "AthDictLoaderSvc", "" };
   ServiceHandle<ITPCnvSvc>      m_tpCnvSvc
    { this, "TPCnvSvc", "AthTPCnvSvc", "" };
  
  std::map<std::string,std::string> m_keyFolderMap;
}; 


#endif

