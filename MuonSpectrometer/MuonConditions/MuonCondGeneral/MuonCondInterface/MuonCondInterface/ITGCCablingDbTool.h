/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H 
#define MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//class TgcIdHelper; 

#include <vector>
#include <string>

static const InterfaceID IID_ITGCCablingDbTool("ITGCCablingDbTool", 1, 0);

class ITGCCablingDbTool: virtual public IAlgTool, 
			 virtual public IAddressProvider {
 public:
  
  static const InterfaceID& interfaceID() {return IID_ITGCCablingDbTool;};
  
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
  
  virtual StatusCode loadASD2PP_DIFF_12(IOVSVC_CALLBACK_ARGS) = 0;

  virtual std::vector<std::string>* giveASD2PP_DIFF_12(void) = 0;

  virtual std::string getFolderName(void) const = 0;
};

#endif // MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H
