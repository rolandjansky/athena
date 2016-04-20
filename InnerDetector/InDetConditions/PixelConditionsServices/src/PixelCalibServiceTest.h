/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PixelCalibServiceTest.cxx
// Algorithm to create Pixel CalibDb in the Conditions Database
// Author Weiming Yao <wmyao@lbl.gov>
/////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBSERVICETEST_H
#define PIXELCALIBSERVICETEST_H

#include <vector>
#include <string>
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TFile.h"
#include "TH1.h"


class Identifier;
//class StoreGateSvc;
class PixelID; 

namespace InDetDD{ 
  class PixelDetectorManager; 
}

class IPixelCalibSvc;

/** @class PixelCalibSerciceTest 
    The PixelCalibServiceTest is an algorithm to illustrate the usage of the PixelCalibSvc.
    The algorithm can also allow to dump the pixel calibration data into a text file.

    @author  Weiming Yao <WMYAO@LBL.GOV>
*/

class PixelCalibServiceTest:public AthAlgorithm {
 public:
  PixelCalibServiceTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelCalibServiceTest(void);

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);
  
 private:
  // MsgStream m_log;
  //  StoreGateSvc* m_sgSvc;
  // StoreGateSvc* m_detStore;

  const ServiceHandle<IPixelCalibSvc> m_calibsvc;
  const InDetDD::PixelDetectorManager* m_pixman;
  const PixelID* m_pixid;
  bool m_setup;                            //true for first event

  // algorithm parameters to be set in the jobOptions
  std::string par_rfile;   //<! text file to read calibration data from
  bool m_dummy;
  
  TFile* par_histf;
  TH1F*  _myhf[14];

}; 

#endif // PIXELCALIBSERVICETEST_H 





