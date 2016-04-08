///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisAlgorithm.h 
// Header file for class AthAnalysisAlgorithm
// Inherits from AthAlgorithm but adds beginInputFile incident listener
// and includes a retrieveMetadata method for easy metadata access
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H
#define ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H 1


/** @class AthAnalysisAlgorithm AthAnalysisAlgorithm.h AthAnalysisBaseComps/AthAnalysisAlgorithm.h
 *
 *  Same as AthAlgorithm but adds a beginInputFile method and handle method for incident listening
 *  Also adds a retrieveMetadata method for easy metadata retrieval from InputMetaDataStore (method not IOV Safe though!)
 *  
 *  @author Will Buttinger
 *  @date   2015
 */ 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools!
#include "GaudiKernel/IIncidentSvc.h"


#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

class AthAnalysisAlgorithm : public ::AthAlgorithm, virtual public IIncidentListener {
 public: 

  /// Constructor with parameters: 
  AthAnalysisAlgorithm(const std::string& name, 
               ISvcLocator* pSvcLocator,
               const std::string& version=PACKAGE_VERSION);


      virtual ~AthAnalysisAlgorithm();

      /// @name Functions providing access to the input/output metadata
      /// @{

      /// Accessor for the input metadata store
      ServiceHandle< StoreGateSvc >& inputMetaStore() const;
      /// Accessor for the output metadata store
      ServiceHandle< StoreGateSvc >& outputMetaStore() const;

      /// @}

      /// Function initialising the tool in the correct way in Athena
      virtual StatusCode sysInitialize();

      /// Helper function to access IOVMetaDataContainer information helped in the MetaDataStore
      template<typename T> StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out) { return AthAnalysisHelper::retrieveMetadata(folder,key,out,inputMetaStore()); }

   protected:
      /// @name Callback functions helping in metadata reading
      /// @{

      /// Function receiving incidents from IncidentSvc/TEvent
      virtual void handle( const Incident& inc );

      /// Function called when a new input file is opened
      virtual StatusCode beginInputFile();

 
 private:
      /// Object accessing the input metadata store
      mutable ServiceHandle< StoreGateSvc > m_inputMetaStore;
      /// Object accessing the output metadata store
      mutable ServiceHandle< StoreGateSvc > m_outputMetaStore;

  
}; 




#endif //> !ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H
