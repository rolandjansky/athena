/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ICSC_DCSCONDITIONSTOOL_H 
#define MUONCONDINTERFACE_ICSC_DCSCONDITIONSTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the CSC DCS Info from COOL DB
//* retrieving of tables from DB virtual interface
//********************************************************* 

static const InterfaceID IID_ICSC_DCSConditionsTool("ICSC_DCSConditionsTool", 1, 0);

class ICSC_DCSConditionsTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_ICSC_DCSConditionsTool;};

   virtual std::string HVFolderName() const = 0; 
   virtual std::string ChamberFolder() const = 0; 
   
   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
   
   virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadchamber(IOVSVC_CALLBACK_ARGS) = 0;
   virtual const std::vector<Identifier>& deadStationsId()=0;
   virtual const std::vector<Identifier>& deadWireLayersId()=0;

   virtual const std::vector<std::string>& deadStations()=0;
   virtual const std::vector<std::string>& deadWireLayers()=0;
  
};




#endif  
