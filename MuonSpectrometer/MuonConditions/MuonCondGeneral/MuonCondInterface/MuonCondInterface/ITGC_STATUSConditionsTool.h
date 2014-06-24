/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ITGC_STATUSCONDITIONSTOOL_H 
#define MUONCONDINTERFACE_ITGC_STATUSCONDITIONSTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT DCS Info from COOL DB
//* retrieving of tables from DB virtual interface
//********************************************************* 

static const InterfaceID IID_ITGC_STATUSConditionsTool("ITGC_STATUSConditionsTool", 1, 0);

class ITGC_STATUSConditionsTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_ITGC_STATUSConditionsTool;};

   virtual std::string FolderName() const = 0; 
   

   virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS) = 0;

   
   virtual StatusCode loadTgcDqStatus(IOVSVC_CALLBACK_ARGS) = 0;
   


   virtual const std::vector<Identifier>& deadStationsId()=0;

  
};




#endif  
