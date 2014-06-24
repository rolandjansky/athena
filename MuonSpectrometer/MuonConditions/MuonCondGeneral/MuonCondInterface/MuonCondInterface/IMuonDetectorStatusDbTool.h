/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMUONDETECTORSTATUSDBTOOL_H 
#define MUONCONDINTERFACE_IMUONDETECTORSTATUSDBTOOL_H 
 
 // Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"
 
 
//namespace MuonCalib 
// {      
 
 
class IMuonDetectorStatusDbTool: virtual public IAlgTool, 
                        virtual public IAddressProvider {
   
 public:
 
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode loadTubeStatus(IOVSVC_CALLBACK_ARGS) = 0;
};
//}
 
#endif  
