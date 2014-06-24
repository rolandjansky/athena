/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IRPCCABLINGDBTOOL_H 
#define MUONCONDINTERFACE_IRPCCABLINGDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

 

class RpcIdHelper; 

static const InterfaceID IID_IRPCCablingDbTool("IRPCCablingDbTool", 1, 0);

class IRPCCablingDbTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IRPCCablingDbTool;};

   virtual std::string mapConfFolderName() const = 0; 
   virtual std::string mapCorrFolderName() const = 0;
   virtual const std::string* GetStringRPCCorrMap() const = 0;
   virtual const std::string* GetStringRPCConfMap() const = 0;
    
   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadRPCMap(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadRPCCorr(IOVSVC_CALLBACK_ARGS) = 0;

};




#endif  
