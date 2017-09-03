///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MetaInputLoader.h 
// Header file for class MetaInputLoader
/////////////////////////////////////////////////////////////////// 
#ifndef IOVSVC_METAINPUTLOADER_H
#define IOVSVC_METAINPUTLOADER_H 1

// STL includes
#include <string>


#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ClassID.h"

#include "AthenaKernel/MetaCont.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"

#include <string>
#include <map>


class MetaInputLoader
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MetaInputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~MetaInputLoader(); 

  // Assignment operator: 
  //MetaInputLoader &operator=(const MetaInputLoader &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
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
  //  MetaInputLoader();

  //  void loader(Property&);
  StatusCode createMetaObj(MetaContBase* mcb, 
                           const CLID& clid, 
                           const std::string key,
                           MetaContBase::SourceID);

  /// Containers
  DataObjIDColl m_load;

  bool m_dump;
  bool m_first { true };

  ServiceHandle<StoreGateSvc> m_metaStore;
  ServiceHandle<StoreGateSvc> m_inputStore;
  
  std::map<std::string,std::string> m_keyFolderMap;
}; 


#endif //> !SGCOMPS_SGINPUTLOADER_H
