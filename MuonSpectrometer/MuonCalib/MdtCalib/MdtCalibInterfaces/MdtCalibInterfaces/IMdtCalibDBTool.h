/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBDB_IMDTCALIBDBTOOL_H 
#define MDTCALIBDB_IMDTCALIBDBTOOL_H 

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"


namespace MuonCalib 
{      


class IMdtCalibDBTool: virtual public IAlgTool, 
                       virtual public IAddressProvider {
  
public:

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode loadTube(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode loadRt(IOVSVC_CALLBACK_ARGS) = 0;
};
}

#endif  
