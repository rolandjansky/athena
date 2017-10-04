/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPANALYSISEXAMPLES_TEST__EGAMMACALIBRATIONANDSMEARINGTOOL_H
#define CPANALYSISEXAMPLES_TEST__EGAMMACALIBRATIONANDSMEARINGTOOL_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"

#include "TH2D.h"
#include "TH1D.h"

namespace Test {

class EgammaCalibrationAndSmearingTool: public ::AthAnalysisAlgorithm { 
 public: 
  EgammaCalibrationAndSmearingTool( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~EgammaCalibrationAndSmearingTool(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 
   int i=0; //count events processed 
   ToolHandle<CP::IEgammaCalibrationAndSmearingTool> myTool;

   TH2D* m_el_pt_new_vs_old = 0;
   TH1D* m_el_pt = 0;
}; 
} //> end namespace Test
#endif //> !CPANALYSISEXAMPLES_TEST__EGAMMACALIBRATIONANDSMEARINGTOOL_H
