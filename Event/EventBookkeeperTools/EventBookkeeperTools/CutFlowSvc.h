/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Header file for class CutFlowSvc
// Authors:
//  Tadej Novak <tadej@cern.ch>
//  Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>

///////////////////////////////////////////////////////////////////
#ifndef EVENT_BOOKKEEPER_TOOLS__CUT_FLOW_SVC_H
#define EVENT_BOOKKEEPER_TOOLS__CUT_FLOW_SVC_H

// Not dual use at the moment
#ifndef XAOD_STANDALONE

/**
 * @class CutFlowSvc
 * @brief This implementes the methods for ICutFlowSvc
 */

// STL includes
#include <mutex>
#include <string>
#include <unordered_map>

// FrameWork includes
#include <GaudiKernel/ISvcLocator.h>
#include <GaudiKernel/IIncidentListener.h>
#include <GaudiKernel/ServiceHandle.h>

// Athena includes
#include <AthenaBaseComps/AthService.h>
#include <AthenaKernel/ICutFlowSvc.h>
#include <CxxUtils/checker_macros.h>
#include <StoreGate/StoreGateSvc.h>

// EDM includes
#include <xAODCutFlow/CutBookkeeper.h>
#include <EventBookkeeperTools/CutBookkeepersLocalCache.h>


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
                                       const std::string& description,
                                       bool nominalOnly) override final;

  /// Tells CutFlowSvc that a filter is used directly by an outputStream with
  /// a given logical context. The only foreseen client should the DecisionSvc,
  /// with its Accept/Require/Veto.
  virtual CutIdentifier registerTopFilter ATLAS_NOT_THREAD_SAFE (const std::string& name,
                                          const std::string& description,
                                          unsigned int logic,
                                          const std::string& outputStream,
                                          bool nominalOnly) override final;

  /// Register cut as child of a filter in the CutFlowSvc and returns the CutID
  /// of the corresponding EventBookkeeper. This method should be used by
  /// filters to register their internal cuts that are not the Algs themselves.
  virtual CutIdentifier registerCut(const std::string& name,
                                    const std::string& description,
                                    CutIdentifier parentCutID,
                                    bool nominalOnly) override final;

  /// Set the description of an existing CutBookkeeper
  virtual void setFilterDescription(CutIdentifier cutID,
                                    const std::string& descr) override final;

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter
  virtual void addEvent(CutIdentifier cutID,
                        const std::vector<float>& weights) override final;

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter
  virtual void addEvent(CutIdentifier cutID,
                        double weight) override final;

  /// Get number of accepted events for a cut
  virtual uint64_t getNAcceptedEvents(const CutIdentifier cutID) const override final;

  /// Get CutBookkeepers cache
  const CutBookkeepersLocalCache &getCutBookkeepers() const;

  /// Set number of weight variations
  StatusCode setNumberOfWeightVariations(size_t count);


private:
  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID
  /// for a specific index in the cache
  void addEvent(CutIdentifier cutID,
                size_t index,
                double weight);

  /// Helper function to determine the processing cycle number from the
  /// input meta-data store
  StatusCode determineCycleNumberFromInput(const std::string& collName);

  /// Helper function to create an empty containers (and its aux store)
  StatusCode createContainers(size_t count);

  /// Get a CutBookkeeper given a CutID
  xAOD::CutBookkeeper* getCutBookkeeper(const CutIdentifier cutID,
                                        size_t index) const;

  /// CutFlow service should be explicitly configured
  Gaudi::Property<bool> m_configured{this, "Configured", true, "configuration check"};

  /// The input meta-data store
  ServiceHandle<StoreGateSvc> m_inMetaDataStore{this, "InputMetaDataStore", "StoreGateSvc/InputMetaDataStore", ""};

  /// The name of the completed, i.e., fully processed, CutBookkeeperContainer
  Gaudi::Property<std::string> m_completeCollName{this, "OutputCollName", "CutBookkeepers", ""};

  /// Local CutBookkeeperContainers
  CutBookkeepersLocalCache m_containers;

  /// The current skimming cycle, i.e., how many processing stages we already had
  int m_skimmingCycle{};

  /// The name of the currently used input file stream
  Gaudi::Property<std::string> m_inputStream{this, "InputStream", "N/A", "The name of the input file stream"};

  /// Mutex to protect adding an event
  mutable std::recursive_mutex m_addEventMutex;

  /// List of nominal-only filters
  std::unordered_set<CutIdentifier> m_nominalOnlyCuts;
};



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline const InterfaceID& CutFlowSvc::interfaceID() {
  return ICutFlowSvc::interfaceID();
}

#endif // dual use

#endif // EVENT_BOOKKEEPER_TOOLS__CUT_FLOW_SVC_H
