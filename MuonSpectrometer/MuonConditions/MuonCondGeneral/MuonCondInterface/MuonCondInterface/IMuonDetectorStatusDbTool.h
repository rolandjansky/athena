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
 
 
class IMuonDetectorStatusDbTool
  : virtual public extend_interfaces<IAlgTool, IAddressProvider>
{
   
 public:
  DeclareInterfaceID(IMuonDetectorStatusDbTool, 1, 0);
 
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode loadTubeStatus(IOVSVC_CALLBACK_ARGS) = 0;
};
//}
 
#endif  
