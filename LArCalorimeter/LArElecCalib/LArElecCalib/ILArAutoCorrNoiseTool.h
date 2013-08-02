/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARAUTOCORRNOISETOOL_H
#define LARELECCALIB_ILARAUTOCORRNOISETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <vector> 

static const InterfaceID IID_ILArAutoCorrNoiseTool("ILArAutoCorrNoiseTool", 1 , 0);

class ILArAutoCorrNoiseTool: virtual public IAlgTool {

 public: 

  virtual ~ILArAutoCorrNoiseTool() {};
  
  virtual const std::vector<float>& autoCorrSqrt(const HWIdentifier& id, 
						  int gain,
						  int Nsampl=-1) = 0;
  
  virtual const std::vector<float>& autoCorrSqrt(const Identifier& id, 
						  int gain,
						  int Nsampl=-1) = 0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;  

  static const InterfaceID& interfaceID() { return IID_ILArAutoCorrNoiseTool; }
};

#endif 
