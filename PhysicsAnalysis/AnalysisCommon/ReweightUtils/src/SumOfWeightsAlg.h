///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Calls IWeightTool objects for each event and stores SumOfWeights
// for each of these computations
//
// Author: Danilo Ferreira de Lima <dferreir@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef REWEIGHTUTILS_SUMOFWEIGHTSALG_H
#define REWEIGHTUTILS_SUMOFWEIGHTSALG_H

// Include the base class
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <string>

// EDM includes

#include "xAODCutFlow/CutBookkeeper.h"

class IWeightTool;

class SumOfWeightsAlg : public ::AthFilterAlgorithm { 
   
   public: 
   
     /// Constructor with parameters: 
     SumOfWeightsAlg(const std::string& name, ISvcLocator* pSvcLocator);
   
     /// Destructor: 
     ~SumOfWeightsAlg(); 
   
     /// Athena algorithm's Hooks
     StatusCode  initialize();
     StatusCode  execute();
     StatusCode  finalize();
   
   private: 
   
     /// Default constructor: 
     SumOfWeightsAlg();

     /// Athena configured tools
     ToolHandleArray<IWeightTool> m_weightTools;

     /// number of events processed
     unsigned long m_eventsProcessed;

     /// cut IDs
     std::vector<CutIdentifier> m_cutIDs;

}; 

#endif //> !REWEIGHTUTILS_SUMOFWEIGHTSALG_H
