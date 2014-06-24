/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ITEMPERATUREDBTOOL_H 
#define MUONCONDINTERFACE_ITEMPERATUREDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

 
//class MdtIdHelper;
 

static const InterfaceID IID_ITemperatureDbTool("ITemperatureDbTool", 1, 0);

class ITemperatureDbTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_ITemperatureDbTool;};

   virtual StatusCode loadTemperature(IOVSVC_CALLBACK_ARGS) = 0;

   virtual std::string TemperatureFolderName() const = 0;
 
};




#endif  
