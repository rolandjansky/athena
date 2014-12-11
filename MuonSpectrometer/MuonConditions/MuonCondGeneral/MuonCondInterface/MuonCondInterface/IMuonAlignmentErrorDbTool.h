/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMUONALIGNMENTERRORDBTOOL_H 
#define MUONCONDINTERFACE_IMUONALIGNMENTERRORDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the AlignmentError Info from COOL DB provided by DQ
//* retrieving of tables from DB virtual interface
//********************************************************* 

static const InterfaceID IID_IMuonAlignmentErrorDbTool("IMuonAlignmentErrorDbTool", 1, 0);

class IMuonAlignmentErrorDbTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IMuonAlignmentErrorDbTool;};

   virtual std::string ErrorFolderName() const = 0; 

   

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

   
   virtual StatusCode loadAlignmentError(IOVSVC_CALLBACK_ARGS) = 0;

   virtual const std::string& errorValue()=0;
};




#endif  
