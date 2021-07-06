/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H 
#define MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"
#include "CxxUtils/checker_macros.h"

//class TgcIdHelper; 

#include <vector>
#include <string>

static const InterfaceID IID_ITGCCablingDbTool("ITGCCablingDbTool", 1, 0);

class ATLAS_CHECK_THREAD_SAFETY
      ITGCCablingDbTool: virtual public IAlgTool, 
			 virtual public IAddressProvider {
 public:
  
  static const InterfaceID& interfaceID() {return IID_ITGCCablingDbTool;};
  
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
  
  virtual StatusCode loadASD2PP_DIFF_12(IOVSVC_CALLBACK_ARGS) = 0;

  virtual StatusCode readASD2PP_DIFF_12FromText() = 0;

  virtual std::vector<std::string>* giveASD2PP_DIFF_12(void) = 0;

  virtual std::string getFolderName(void) const = 0;
};

#endif // MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H
