/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDT_DEADTUBECONDITIONSTOOL_H 
#define MUONCONDINTERFACE_IMDT_DEADTUBECONDITIONSTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT Dead Tube Info from COOL DB
//* retrieving of tables from DB virtual interface
//********************************************************* 

static const InterfaceID IID_IMDT_DeadTubeConditionsTool("IMDT_DeadTubeConditionsTool", 1, 0);

class IMDT_DeadTubeConditionsTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IMDT_DeadTubeConditionsTool;};

   virtual std::string DeadTubeFolderName() const = 0; 
   

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

   
   virtual StatusCode loadDeadTube(IOVSVC_CALLBACK_ARGS) = 0;
   virtual const std::vector<std::string>& deadTubes()=0;     
   virtual const std::vector<Identifier>& deadTubesId()=0;  
   virtual const std::vector<Identifier>& List_Chambers_with_deadTube()=0;
};




#endif  
