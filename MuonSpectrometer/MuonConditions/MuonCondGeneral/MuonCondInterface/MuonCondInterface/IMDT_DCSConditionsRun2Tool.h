/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDT_DCSCONDITIONSRUN2TOOL_H 
#define MUONCONDINTERFACE_IMDT_DCSCONDITIONSRUN2TOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT DCS Info from COOL DB
//* retrieving of tables from DB virtual interface for run2
//********************************************************* 

static const InterfaceID IID_IMDT_DCSConditionsRun2Tool("IMDT_DCSConditionsRun2Tool", 1, 0);

class IMDT_DCSConditionsRun2Tool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IMDT_DCSConditionsRun2Tool;};


   virtual std::string HVFolderName() const = 0;
   virtual std::string LVFolderName() const = 0;
   //   virtual std::string JTAGFolderName() const = 0;

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

   

   virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadLV(IOVSVC_CALLBACK_ARGS) = 0;
   // virtual StatusCode loadJTAG(IOVSVC_CALLBACK_ARGS) = 0;
    virtual const std::vector<std::string>& deadStations()=0;
   virtual const std::vector<Identifier>& deadStationsId()=0;
   //virtual const std::vector<std::string>& deadMultiLayer()=0;
   virtual const std::vector<Identifier>& deadMultiLayersId()=0;
  
};




#endif  
