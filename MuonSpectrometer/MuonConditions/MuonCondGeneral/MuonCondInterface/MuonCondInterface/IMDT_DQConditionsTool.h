/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDT_DQCONDITIONSTOOL_H 
#define MUONCONDINTERFACE_IMDT_DQCONDITIONSTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT Info from COOL DB provided by DQ
//* retrieving of tables from DB virtual interface
//********************************************************* 

static const InterfaceID IID_IMDT_DQConditionsTool("IMDT_DQConditionsTool", 1, 0);

class IMDT_DQConditionsTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IMDT_DQConditionsTool;};

   virtual std::string NoisyFolderName() const = 0; 
   virtual std::string DeadFolderName() const = 0; 
   

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

   
   virtual StatusCode loadDeadChamber(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadNoisyChamber(IOVSVC_CALLBACK_ARGS) = 0;

   //   virtual const std::vector<std::string>& deadTubes()=0;     
   virtual const std::vector<Identifier>& deadTubesId()=0;  
   virtual const std::vector<Identifier>& deadMultiLayersId()=0;  
   virtual const std::vector<Identifier>& deadLayersId()=0;  
   virtual const std::vector<Identifier>& List_Chambers_with_deadTube()=0;
};




#endif  
