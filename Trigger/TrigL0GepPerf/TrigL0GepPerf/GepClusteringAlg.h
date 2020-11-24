/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_GEPCLUSTERINGALG_H
#define TRIGL0GEPPERF_GEPCLUSTERINGALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "TrigL0GepPerf/CustomCaloCell.h"
#include "TrigL0GepPerf/CustomTopoCluster.h"
#include "TrigL0GepPerf/CaloCellsHandler.h"

#include <string>


class GepClusteringAlg: public ::AthAnalysisAlgorithm { 
 public: 
  GepClusteringAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GepClusteringAlg(); 

                                        //IS EXECUTED:
  virtual StatusCode  initialize();     //once, before any input is loaded
  //virtual StatusCode  beginInputFile(); //start of each input file, only metadata loaded
  //virtual StatusCode  firstExecute();   //once, after first eventdata is loaded (not per file)
  virtual StatusCode  execute();        //per event
  //virtual StatusCode  endInputFile();   //end of each input file
  //virtual StatusCode  metaDataStop();   //when outputMetaStore is populated by MetaDataTools
  virtual StatusCode  finalize();       //once, after all events processed
  

 private: 

  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
  std::string m_topoclAlg;

  CaloCellsHandler m_cch;
  bool m_cellsFilledOnce = false;
  const CaloCell_ID* m_ccIdHelper;


}; 

#endif //> !TRIGL0GEPPERF_GEPCLUSTERINGALG_H




