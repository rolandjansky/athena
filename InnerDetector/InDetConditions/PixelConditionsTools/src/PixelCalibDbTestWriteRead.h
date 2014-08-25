/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PixelCalibDbTestWriteRead.cxx
// Algorithm to create Pixel CalibDb in the Conditions Database
// Author Weiming Yao <wmyao@lbl.gov>
/////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBTOOL_PIXELCALIBDBTESTWRITEREAD_H
#define PIXELCALIBTOOL_PIXELCALIBDBTESTWRITEREAD_H

#include <vector>
#include <string>
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/DataHandle.h"

//class StoreGateSvc;


class IPixelCalibDbTool; 

/** @class PixelCalibDbTestWriteRead

    The PixelCalibDbTestWriteRead provides an algorithm how to read/write the calibration data in a text file 
    from/to the COOL database. 

    @author  Weiming Yao <WMYAO@LBL.GOV>
*/


class PixelCalibDbTestWriteRead:public AthAlgorithm {
 public:
  PixelCalibDbTestWriteRead(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelCalibDbTestWriteRead(void);

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);
  
 private:
  //MsgStream m_log;
  //StoreGateSvc* m_sgSvc;
  //StoreGateSvc* m_detStore;

  const ToolHandle<IPixelCalibDbTool> m_calibdbtool;
  bool m_setup;                            //true for first event

  // algorithm parameters to be set in the jobOptions
  bool par_read;           // read DB to file
  bool par_write;          // stream structures to output 
  std::string par_rfile;   // text file to read calibration data from
  std::string par_wfile;     // text file to write

}; 

#endif // PIXELCALIBTOOL_PIXELCALIBDBTESTWRITE_H 





