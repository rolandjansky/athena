///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Calls IWeightTool objects for each event and stores SumOfWeights
// for each of these computations
// Stores the total SumOfWeights as well as the sumOfWeights in bins
// of averageIntPerXing
// Dumps the final SumOfWeights to a text file
//
// Author: Olivier Arnaez <olivier.arnaez@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef REWEIGHTUTILS_SUMOFWEIGHTSTOOL_H
#define REWEIGHTUTILS_SUMOFWEIGHTSTOOL_H

// Include the base class
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

// EDM includes

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"


class IWeightTool;

class SumOfWeightsTool : public AthAnalysisAlgorithm { 
   
   public: 
   
     /// Constructor with parameters: 
     SumOfWeightsTool(const std::string& name, ISvcLocator* pSvcLocator);
   
     /// Destructor: 
     ~SumOfWeightsTool(); 
   
     /// Athena algorithm's Hooks
     StatusCode  initialize();
     StatusCode  execute();
     StatusCode  endRun();
   
   private: 
   
     /// Default constructor: 
     SumOfWeightsTool();

     StatusCode dumpIntoTxtFile(); //dumps the results into a text file
   
     /// Athena configured tools
     ToolHandleArray<IWeightTool> m_weightTools;

     std::string m_ofname; //output text file containing the sumOfWeights

     bool m_dumpTxtFile; //if true the sumOfWeights are dumped into a text file
     unsigned int m_maxMuBins; //number of bins agains which we'll store the averageIntPerXing-dependent sumOfWeights 

     std::string m_CBKcontainerName;//Name of the output collection of CutBookKeepers
     std::string m_CBKnamePrefix;//Prefix of the CutBookKeeper inside the container

     std::vector<float> m_TotalSumOfWeights; //Stores the total sumOfWeights for each IWeightTool computation
     std::vector<float> m_TotalSumOfSquaredWeights; //Stores the total sumOfWeights for each IWeightTool computation
     std::vector< std::vector<float> > m_SumOfWeightsPerMu; //Stores the sumOfWeights in bins of averageIntPerXing for each IWeightTool computation
     std::vector< std::vector<float> > m_SumOfSquaredWeightsPerMu; //Stores the sumOfWeights in bins of averageIntPerXing for each IWeightTool computation

     std::vector<xAOD::CutBookkeeper*> m_ebcList;
     std::vector< std::vector<xAOD::CutBookkeeper*> > m_ebcListPerMu;
   
}; 

#endif //> !REWEIGHTUTILS_SUMOFWEIGHTSTOOL_H
