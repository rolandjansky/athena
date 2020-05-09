/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header file for class CutFlowSvc
// Authors:
//  Tadej Novak <tadej@cern.ch>
//  Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>

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
  virtual StatusCode initialize() override final;
  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvi) override final;

  /// Publish the interface for this service
  static const InterfaceID& interfaceID();
  //@}


  /// Incident service handle listening for BeginInputFile, EndInputFile and MetaDataStop
  virtual void handle(const Incident& incident) override final;

public:

  /// Register filter in the CutFlowSvc and returns the CutID of the
  /// corresponding CutBookkeeper.
  /// This method should be used by filters that register themselves.
  virtual CutIdentifier registerFilter(const std::string& name,
                                       const std::string& description)  override final;

  /// Tells CutFlowSvc that a filter is used directly by an outputStream with
  /// a given logical context. The only foreseen client should the DecisionSvc,
  /// with its Accept/Require/Veto.
  virtual CutIdentifier registerTopFilter(const std::string& name,
                                          const std::string& description,
                                          unsigned int logic,
                                          const std::string& outputStream) override final;

  /// Set the description of an existing CutBookkeeper
  virtual void setFilterDescription(CutIdentifier cutID,
                                    const std::string& descr) override final;

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter
  virtual void addEvent(CutIdentifier cutID,
                        double weight) override final;

  /// Get current container
  const xAOD::CutBookkeeperContainer *getCutBookkeepers() const;

  /// Get number of accepted events for a cut
  virtual uint64_t getNAcceptedEvents(const CutIdentifier cutID) const override final;

private:
  /// Helper function to determine the processing cycle number from the
  /// input meta-data store
  StatusCode determineCycleNumberFromInput(const std::string& collName);

  /// Helper function to create an empty container (and its aux store)
  StatusCode createContainer();

  /// Get a CutBookkeeper given a CutID
  xAOD::CutBookkeeper* getCutBookkeeper(const CutIdentifier cutID) const;

  /// The input meta-data store
  ServiceHandle<StoreGateSvc> m_inMetaDataStore{this, "InputMetaDataStore", "StoreGateSvc/InputMetaDataStore", ""};

  /// The name of the completed, i.e., fully processed, CutBookkeeperContainer
  Gaudi::Property<std::string> m_completeCollName{this, "OutputCollName", "CutBookkeepers", ""};

  /// Primary cutflow container (and its aux store)
  std::unique_ptr<xAOD::CutBookkeeperContainer> m_container{};
  std::unique_ptr<xAOD::CutBookkeeperAuxContainer> m_containerAux{};

  /// The current skimming cycle, i.e., how many processing stages we already had
  int m_skimmingCycle{};

  /// The name of the currently used input file stream
  Gaudi::Property<std::string> m_inputStream{this, "InputStream", "N/A", "The name of the input file stream"};

  /// Declare a simple typedef for the internal map
  typedef std::unordered_map<CutIdentifier, xAOD::CutBookkeeper*> CutIDMap_t;

  /// This internal map keeps the association between the instance identifier of each algorithm
  /// to the pointer of associated CutBookkeeper
  CutIDMap_t m_ebkMap;

  mutable std::recursive_mutex m_addEventMutex;
};



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline const InterfaceID& CutFlowSvc::interfaceID() {
  return ICutFlowSvc::interfaceID();
}

#endif //> !CUTFLOWSVC_H
