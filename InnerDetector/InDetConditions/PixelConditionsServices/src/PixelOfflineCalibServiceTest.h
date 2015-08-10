/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PixelOfflineCalibServiceTest.cxx
// Algorithm to create Pixel CalibDb in the Conditions Database
// Author Weiming Yao <wmyao@lbl.gov>
/////////////////////////////////////////////////////////////////

#ifndef PIXELOFFLINECALIBSERVICETEST_H
#define PIXELOFFLINECALIBSERVICETEST_H

#include <vector>
#include <string>
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ServiceHandle.h"

//#include "TFile.h"
//#include "TH1.h"


class IPixelOfflineCalibSvc;

class IPixelRecoDbTool;


/** @class PixelOfflineCalibSerciceTest 
    The PixelOfflineCalibServiceTest is an algorithm to illustrate the usage of the PixelOfflineCalibSvc.
    The algorithm can also allow to dump the pixel calibration data into a text file.

    @author  Weiming Yao <WMYAO@LBL.GOV>
*/

class PixelOfflineCalibServiceTest:public AthAlgorithm {
 public:
  PixelOfflineCalibServiceTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelOfflineCalibServiceTest(void);

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);
  
 private:
  // MsgStream m_log;
  //  StoreGateSvc* m_sgSvc;
  // StoreGateSvc* m_detStore;

  const ServiceHandle<IPixelOfflineCalibSvc> m_calibsvc;
  ToolHandle<IPixelRecoDbTool >  m_dbTool;
  mutable PixelCalib::PixelOfflineCalibData* m_pat;

  bool m_setup;                            //true for first event

  // algorithm parameters to be set in the jobOptions
  std::string par_rfile1;   //<! text file to read calibration data from
  std::string par_rfile2;   //<! text file to read calibration data from
  std::string par_rfile3;   //<! text file to read calibration data from

}; 

#endif // PIXELCALIBSERVICETEST_H 





