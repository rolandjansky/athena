///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisAlgorithm.h 
// Header file for class AthAnalysisAlgorithm
// Inherits from AthHistogramAlgorithm
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H
#define ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H 1

/** @class AthAnalysisAlgorithm AthAnalysisAlgorithm.h AthAnalysisBaseComps/AthAnalysisAlgorithm.h
 *
 *  Same as AthAlgorithm but adds a beginInputFile method and handle method for incident listening
 *  Also adds a retrieveMetadata method for easy metadata retrieval from InputMetaDataStore (method not IOV Safe though!)
 *  Update Feb 2016: Made inherit from AthHistogramAlgorithm, since that has nice histogram booking features

 *  @author Will Buttinger
 *  @date   2015
 */ 

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools!
#include "GaudiKernel/IIncidentSvc.h"


#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "TFile.h"

class AthAnalysisAlgorithm : public ::AthHistogramAlgorithm, virtual public IIncidentListener {
 public:

  /// Constructor taking just a name
  AthAnalysisAlgorithm(const std::string& name);

  /// Constructor with parameters: 
  AthAnalysisAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~AthAnalysisAlgorithm() override;

  /// @name Functions providing access to the input/output metadata
  /// @{

  /// Accessor for the input metadata store
  ServiceHandle< StoreGateSvc >& inputMetaStore() const;
  /// Accessor for the output metadata store
  ServiceHandle< StoreGateSvc >& outputMetaStore() const;

  /// @}

  /// Function initialising the tool in the correct way in Athena
  virtual StatusCode sysInitialize() override;
  /// override to do firstEvent method
  virtual StatusCode sysExecute(const EventContext&) override;

    /// Helper function to access IOVMetaDataContainer information helped in the MetaDataStore
    template<typename T> StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out) {
        try {
            return AthAnalysisHelper::retrieveMetadata(folder,key,out,inputMetaStore());
        } catch(std::exception& e) {
            ATH_MSG_WARNING(e.what());
            return StatusCode::FAILURE;
        }
    }

    /// Helper function to access IOVMetaDataContainer information held in InputMetaDataStore, but will check IOVTime. Also can give a channel (leave as -1 to take first available)
    /// IOVTime(runNumber, eventNumber) is a valid constructor for example
    template<typename T> StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out, IOVTime time, int channel=-1) {
        try {
            return AthAnalysisHelper::retrieveMetadata(folder,key,out,inputMetaStore(),time,channel);
        } catch(std::exception& e) {
            ATH_MSG_WARNING(e.what());
            return StatusCode::FAILURE;
        }
    }

protected:
  void updateEvtStore(Gaudi::Details::PropertyBase& prop);


    /// Function receiving incidents from IncidentSvc/TEvent
    /// Experts can override but they should ensure they add
    ///   AthAnalysisAlgorithm::handle();
    /// to the end of their own implementation
    virtual void handle( const Incident& inc ) override;

    /// Function called when a new input file is opened
    /// user can read input metadata from inputMetaStore()
    virtual StatusCode beginInputFile();

    /// Function called as an input file is being closed
    virtual StatusCode endInputFile();

    /// Function called before finalize
    /// user can read output metadata from outputMetaStore()
    virtual StatusCode metaDataStop();

    /// Function called when first execute is encountered
    /// user can read event information with evtStore()
    virtual StatusCode firstExecute();

    /// Function returning the TFile pointer of the currently open file of the
    /// given EventSelector (in athena jobs this defaults to "EventSelector")
    virtual TFile* currentFile(const char* evtSelName="EventSelector") final;

private:
  /// Object accessing the input metadata store
  mutable ServiceHandle< StoreGateSvc > m_inputMetaStore;
  /// Object accessing the output metadata store
  mutable ServiceHandle< StoreGateSvc > m_outputMetaStore;

    TFile* m_currentFile = 0; //used to cache the current file

  bool m_doneFirstEvent{false};
};

#endif //> !ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H
