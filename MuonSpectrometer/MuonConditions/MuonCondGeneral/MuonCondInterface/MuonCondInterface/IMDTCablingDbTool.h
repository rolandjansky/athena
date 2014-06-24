/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDTCABLINGDBTOOL_H 
#define MUONCONDINTERFACE_IMDTCABLINGDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

 
class MdtIdHelper;
 

static const InterfaceID IID_IMDTCablingDbTool("IMDTCablingDbTool", 1, 0);

class IMDTCablingDbTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IMDTCablingDbTool;};

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadMezzanine(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadMDTMap(IOVSVC_CALLBACK_ARGS) = 0;
   
   virtual std::string mezzanineFolderName() const = 0;
   virtual std::string mapFolderName() const = 0;
 
};




#endif  
