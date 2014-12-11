/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IRPC_DCSCONDITIONSTOOL_H 
#define MUONCONDINTERFACE_IRPC_DCSCONDITIONSTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the RPC DCS Info from COOL DB
//* retrieving of tables from DB virtual interface
//********************************************************* 

static const InterfaceID IID_IRPC_DCSConditionsTool("IRPC_DCSConditionsTool", 1, 0);

class IRPC_DCSConditionsTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IRPC_DCSConditionsTool;};

   virtual std::string PanelOffFolderName() const = 0; 
   virtual std::string PanelDeadFolderName() const = 0;
   

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
   
   
   virtual StatusCode loadPanelOff(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadPanelDead(IOVSVC_CALLBACK_ARGS) = 0;
 
   
  
   virtual const std::vector<Identifier>& deadPanelId()=0;
   virtual const std::vector<Identifier>& offPanelId()=0;
    
  
};




#endif  
