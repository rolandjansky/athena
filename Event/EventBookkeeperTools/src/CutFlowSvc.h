///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// STL includes
#include <mutex>
#include <string>
#include <unordered_map>
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


class CutFlowSvc :
  virtual public ICutFlowSvc,
  virtual public IIncidentListener,
          public AthService
{

public:
  /// Constructor
  CutFlowSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvi ) override;

  /// Publish the interface for this service
  static const InterfaceID& interfaceID();
  //@}


  /// Incident service handle listening for BeginInputFile, EndInputFile and MetaDataStop
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

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter or registerCut
  void addEvent( CutIdentifier cutID, double weight ) override final;

  const std::string SGKey() override final;

  /// Get a CutBookkeeper given a CutID
  xAOD::CutBookkeeper* getCutBookkeeper( const CutIdentifier cutID ) const;

  void print();

private:
  /// Helper function to determine the processing cycle number from the
  /// input meta-data store
  StatusCode determineCycleNumberFromInput( const std::string& collName );

  /// Helper function to record the collection (and its aux store) to the
  /// output MetaData store
  StatusCode recordCollection( xAOD::CutBookkeeperContainer* coll,
                               const std::string &collName );

  /// The output meta-data store
  ServiceHandle<StoreGateSvc> m_outMetaDataStore{this, "OutputMetaDataStore", "StoreGateSvc/MetaDataStore", ""};

  /// The input meta-data store
  ServiceHandle<StoreGateSvc> m_inMetaDataStore{this, "InputMetaDataStore", "StoreGateSvc/InputMetaDataStore", ""};

  /// The name of the completed, i.e., fully processed, CutBookkeeperContainer
  Gaudi::Property<std::string> m_completeCollName{this, "OutputCollName", "CutBookkeepers", ""};

  /// The name of the container in storegate with cutflow values for a file. 
  std::string m_fileCollName{"CutBookkeepersFile"};

  /// The current skimming cycle, i.e., how many processing stages we already had
  int m_skimmingCycle{};

  /// The name of the currently used input file stream
  Gaudi::Property<std::string> m_inputStream{this, "InputStream", "N/A", "The name of the input file stream"};

  /// Declare a simple typedef for the internal map
  typedef std::unordered_map<CutIdentifier, xAOD::CutBookkeeper*> CutIDMap_t;

  /// This internal map keeps the association between the instance identifier of each algorithm
  /// to the pointer of associated CutBookkeeper
  CutIDMap_t m_ebkMap;

  mutable std::recursive_mutex m_addeventMutex;
};



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline const InterfaceID& CutFlowSvc::interfaceID() {
  return ICutFlowSvc::interfaceID();
}

inline const std::string CutFlowSvc::SGKey() {
  return m_fileCollName;
}

#endif //> !CUTFLOWSVC_H
