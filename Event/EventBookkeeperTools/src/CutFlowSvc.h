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

// FrameWork includes
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "StoreGate/StoreGateSvc.h"

// EDM includes
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

// STL include(s):
#if __cplusplus < 201100
#   include <tr1/unordered_map>
namespace MAP_NS = std::tr1;
#else
#   include <unordered_map>
namespace MAP_NS = std;
#endif // C++

// forward declarations
class EventBookkeeperCollection;
class EventBookkeeper;



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
  virtual StatusCode initialize() override;
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvi ) override;
  //@}


  /// Incident service handle listening for BeginFile and EndFile.
  virtual void handle(const Incident& incident) override;

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

public:

  /// Register filter in the CutFlowSvc and returns the CutID of the
  /// corresponding CutBookkeeper.
  /// This method should be used by filters that register themselves.
  CutIdentifier registerFilter( const std::string& name,
                                const std::string& description ) override final;

  /// Register cut as child of a filter in the CutFlowSvc and returns the CutID
  /// of the corresponding CutBookkeeper. This method should be used by
  /// filters to register their internal cuts that are not the Algs themselves.
  CutIdentifier registerCut( const std::string& name,
                             const std::string& description,
                             CutIdentifier originCutID ) override final;

  /// Tells CutFlowSvc that a filter is used directly by an outputStream with
  /// a given logical context. The only foreseen client should the DecisionSvc,
  /// with its Accept/Require/Veto.
  CutIdentifier registerTopFilter( const std::string& name,
                                   const std::string& description,
                                   unsigned int logic,
                                   const std::string& outputStream ) override final;

  /// Tells CutFlowSvc that a filter should not be treated as as being used by
  /// another filter. This should be used by filters that use other filter Algs
  /// internally, e.g., like the LogicalFilterCombiner
  CutIdentifier declareUsedOtherFilter( const std::string& name,
                                        CutIdentifier originCutID ) override final;

  /// Set the description of an existing CutBookkeeper
  void setFilterDescription( CutIdentifier cutID,
                             const std::string& descr ) override final;

  /// Tells CutFlowSvc to update the event counter of a CutIdentifier cutID,
  /// using the CutIdentifier returned by selfRegisterFilter or registerCut
  /// Internally, the Monte Carlo event weight will be retrieved from the
  /// xAOD::EventInfo object as evtWeight = evtInfo->mcEventWeight();
  void addEvent( CutIdentifier cutID ) override final;

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter or registerCut
  void addEvent( CutIdentifier cutID, double weight ) override final;

  /// Get a CutBookkeeper given a CutID
  xAOD::CutBookkeeper* getCutBookkeeper( const CutIdentifier cutID );

  // TTree* dumpCutFlowToTTree(const char* treeName="CutFlowTree");
  // void loadCutFlowFromTTree(TTree*) override final;

  void print();

private:
  /// Helper function to determine the processing cycle number from the
  /// input meta-data store
  StatusCode determineCycleNumberFromInput( const std::string& collName );

  /// Determine the cycle number from the old-style EventBookkeeper
  StatusCode determineCycleNumberFromOldInput( const std::string& collName );

  /// Helper function to record the collection (and its aux store) to the
  /// output MetaData store
  StatusCode recordCollection( xAOD::CutBookkeeperContainer* coll,
                               const std::string &collName );

  /// Update an existing (possibly empty) xAOD::CutBookkeeperContainer with
  /// all the information from the container(s) form the input file
  StatusCode updateCollFromInputStore( xAOD::CutBookkeeperContainer* coll,
                                       const std::string &collName );

  /// Helper class to update a container with information from another one
  StatusCode updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                              const xAOD::CutBookkeeperContainer* otherCont );

  /// Helper class to update a container with information from another one from the old EDM
  StatusCode updateContainerFromOldEDM( xAOD::CutBookkeeperContainer* contToUpdate,
                                        const EventBookkeeperCollection* otherContOldEDM );

  /// Helper class to update a container with information from another one from the old EDM
  StatusCode updateContainerFromOldEDM( xAOD::CutBookkeeperContainer* contToUpdate,
                                        xAOD::CutBookkeeper* newEBK,
                                        const EventBookkeeper* oldEBK,
                                        const xAOD::CutBookkeeper* parentEBK=0 );

  // bool inputStoreHasFlatTTree() const;
  // void fillIncompleteCollectionFromInputStore(xAOD::CutBookkeeperContainer *coll);
  // void fillCompleteCollectionFromInputStore(xAOD::CutBookkeeperContainer *coll);
  // void fillCollectionFromInputStore(xAOD::CutBookkeeperContainer *coll, int wantIsComplete);



  /// Create a typedef
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

  /// The output meta-data store
  StoreGateSvc_t m_outMetaDataStore;

  /// The input meta-data store
  StoreGateSvc_t m_inMetaDataStore;

  /// The event store
  StoreGateSvc_t m_eventStore;

  /// The name of the completed, i.e., fully processed, CutBookkeeperContainer
  StringProperty m_completeCollName;

  /// The name of the incomplete, i.e., not fully processed (e.g. failed job), CutBookkeeperContainer
  StringProperty m_incompleteCollName;

  /// The current skimming cycle, i.e., how many processing stages we already had
  int m_skimmingCycle;

  /// The name of the currently used input file stream
  StringProperty m_inputStream;

  /// Temporary container for frequent use
  xAOD::CutBookkeeperContainer*  m_inputCompleteBookTmp;

  /// Temporary auxiliary container for frequent use
  xAOD::CutBookkeeperAuxContainer* m_inputCompleteBookAuxTmp;

  /// A flag to say if the input file is currently open or not
  bool m_fileCurrentlyOpened;

  /// Declare a simple typedef for the internal map
  typedef MAP_NS::unordered_map<CutIdentifier, xAOD::CutBookkeeper*> CutIDMap_t;

  /// This internal map keeps the association between the instance identifier of each algorithm
  /// to the pointer of associated CutBookkeeper
  CutIDMap_t m_ebkMap;

  /// Internal flag to track if the old-style EventBookkeepers from the input were already processed
  bool m_alreadyCopiedEventBookkeepers;

  /// Internal flag to track if we have already determined the cycle number from the first input file
  //bool m_alreadyDeterminedCycleNumber;

public:

  /// Publish the interface for this service
  static const InterfaceID& interfaceID();

};





///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline const InterfaceID& CutFlowSvc::interfaceID() {
  return ICutFlowSvc::interfaceID();
}


inline
xAOD::CutBookkeeper*
CutFlowSvc::getCutBookkeeper( const CutIdentifier cutID ) {
  return m_ebkMap[cutID];
}


#endif //> !CUTFLOWSVC_H
