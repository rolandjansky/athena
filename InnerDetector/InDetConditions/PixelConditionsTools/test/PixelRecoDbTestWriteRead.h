/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PixelRecoDbTestWriteRead.cxx
// Algorithm to create PixelRecoCalibDb in the Conditions Database
// Author Tommaso Lari <tommaso.lari@mi.infn.it>
/////////////////////////////////////////////////////////////////

#ifndef PIXELRECOTOOL_PIXELRECODBTESTWRITEREAD_H
#define PIXELRECOTOOL_PIXELRECODBTESTWRITEREAD_H

#include "PixelConditionsTools/IPixelRecoDbTool.h"
#include <vector>
#include <string>
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/DataHandle.h"

class Identifier;


/** @class PixelRecoDbTestWriteRead

    The PixelRecoDbTestWriteRead provides an algorithm how 
    to read/write the offline calibration data 
    in a text file from/to the COOL database. 

    @author  Tommaso Lari <tommaso.lari@mi.infn.it>
*/


class PixelRecoDbTestWriteRead:public AthAlgorithm {
 public:
  PixelRecoDbTestWriteRead(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelRecoDbTestWriteRead(void);

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);
  
 private:
  //MsgStream m_log;

  //const PixelRecoDbTool* m_calibdbtool;
  PublicToolHandle< IPixelRecoDbTool >  m_calibdbtool
     {this,"PixelRecoDbTool","PixelRecoDbTool",""};
  bool m_setup;                            //true for first event

  // algorithm parameters to be set in the jobOptions
  bool m_par_read;           // read DB to file
  bool m_par_write;          // stream structures to output 
  std::string m_par_rfile;   // text file to read calibration data from
  std::string m_par_wfile;     // text file to write

}; 

#endif // PIXELRECOTOOL_PIXELRECODBTESTWRITE_H 





