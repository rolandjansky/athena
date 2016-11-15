/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ITGCTRIGGERDBTOOL_H 
#define MUONCONDINTERFACE_ITGCTRIGGERDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

//class TgcIdHelper; 

#include <vector>
#include <string>

static const InterfaceID IID_ITGCTriggerDbTool("ITGCTriggerDbTool", 1, 0);

class ITGCTriggerDbTool: virtual public IAlgTool, 
			 virtual public IAddressProvider {
 public:

  enum {CW_BW=0, CW_EIFI=1, CW_TILE=2, CW_NUM=3}; 

  virtual std::string getFolderName(int cwtype) const = 0;
 
  virtual std::string getData(int cwtype, std::string file) const = 0;
  virtual std::string getData(int cwtype, int channel) const = 0;
  virtual std::string getFile(int cwtype, int channel) const = 0;
  virtual std::string getVersion(int cwtype, int channel = 0) const = 0;
  virtual std::string getType(int cwtype, int channel = 0) const = 0;
  virtual bool isActive(int cwtype, int channel = 0) const = 0;

  static const InterfaceID& interfaceID() {return IID_ITGCTriggerDbTool;};
  
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
  
};

#endif // MUONCONDINTERFACE_ITGCCABLINGDBTOOL_H
