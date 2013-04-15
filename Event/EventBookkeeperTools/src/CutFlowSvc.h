///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header file for class CutFlowSvc
// Authors: Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>

/////////////////////////////////////////////////////////////////// 
#ifndef CUTFLOWSVC_H 
#define CUTFLOWSVC_H 

/**
 * @class CutFlowSvc
 * @brief This implementes the methods for ICutFlowSvc
 */

// for size_t
#include <cassert>

// STL includes
#include <string>
#include <vector>
#include <tr1/unordered_map>

// FrameWork includes
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentListener.h"
#include "SGTools/crc64.h"


#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGate.h"
#include "GeneratorObjects/McEventCollection.h"



// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"


class CutFlowSvc : 
  virtual public ICutFlowSvc, 
  virtual public IIncidentListener,
          public AthService 
{ 

public: 

/// Constructor
  CutFlowSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~CutFlowSvc();

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  StatusCode queryInterface( const InterfaceID& riid, void** ppvi );
  //@}


  /// Incident service handle listening for BeginFile and EndFile.
  void handle(const Incident& incident);

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

public:

  // Register TopFilter,filter,cut returning its adress in the EventBookkeeperCollection
  //EventBookkeeper* selfRegisterFilter(const std::string& name, const std::string& description);
  virtual CutIdentifier selfRegisterFilter( const std::string& name,
                                            const std::string& description );

  CutIdentifier registerCut( const std::string& name, 
                             const std::string& description, 
                             CutIdentifier parentCutID );

  void declareChildFilter( const std::string& name, 
                           CutIdentifier parentCutID );

  void setFilterDescription( CutIdentifier cutID, 
                             const std::string& descr );

  void declareTopFilter( const std::string& name, 
                         const std::string& logic, 
                         const std::string& outputStream );

  void addEvent( CutIdentifier cutID );

  void addEvent( CutIdentifier cutID, double weight );

  TTree* dumpCutFlowToTTree(const char* treeName="CutFlowTree");

  void loadCutFlowFromTTree(TTree*);

  void print();

private: 

  StatusCode recordCollection(EventBookkeeperCollection* coll, 
                              const std::string &collName);

  void updateCollFromInputStore(EventBookkeeperCollection* coll, 
                                const std::string &collName);

  CutIdentifier getCutIdentifier( const std::string& name );

  EventBookkeeper* getEventBookkeeper( const CutIdentifier cutID );

  void fillMapWithChildren( const std::vector<EventBookkeeper*>* childVecPtr );
                            


  //properties
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_outMetaDataStore;
  StoreGateSvc_t m_inMetaDataStore;
  StoreGateSvc_t m_EventStore;
  StringProperty  m_completeCollName;
  StringProperty  m_incompleteCollName;
  int m_skimmingCycle;
  StringProperty  m_inputStream;
  StringProperty m_printStream;
  StringProperty m_printVirtual;
  StringProperty m_writeTxtFileNamed;
  StringProperty m_writeRootFileNamed;

  //class data members
  EventBookkeeperCollection*  m_inputCompleteBookTmp;

  bool m_fileCurrentlyOpened;

  /// Declare a simple typedef for the internal map
  typedef std::tr1::unordered_map<CutIdentifier, EventBookkeeper*> CutIDMap_t;

  /// This internal map keeps the association between the instance identifier of each algorithm
  /// to the pointer of associated EventBookkeeper
  CutIDMap_t m_ebkMap;

public:

  static const InterfaceID& interfaceID();
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& CutFlowSvc::interfaceID() 
{ 
  return ICutFlowSvc::interfaceID(); 
}



inline CutIdentifier CutFlowSvc::getCutIdentifier( const std::string& name )
{
  // Create a 32-bit integer from the name that was given
  const uint64_t hash64 = SG::crc64( name );
  CutIdentifier tmpCutID = (CutIdentifier)(hash64 & 0xFFFFFFFF);
  return tmpCutID;
}



inline EventBookkeeper* CutFlowSvc::getEventBookkeeper( const CutIdentifier cutID )
{
  return m_ebkMap[cutID];
}


#endif //> !CUTFLOWSVC_H

