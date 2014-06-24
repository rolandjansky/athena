/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDT_MAPCONVERSION_H 
#define MUONCONDINTERFACE_IMDT_MAPCONVERSION_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to convert the online Id in Offline Id for the MDT
//* 
//********************************************************* 

static const InterfaceID IID_IMDT_MapConversion("IMDT_MapConversion", 1, 0);

class IMDT_MapConversion: virtual public IAlgTool{ 

   
 public:

   static const InterfaceID& interfaceID() {return IID_IMDT_MapConversion;};

 
   virtual const Identifier& ConvertToOffline(const std::string &OnlineId) const =0;
  
};




#endif  
