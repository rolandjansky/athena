/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARAUTOCORRTOTALTOOL_H
#define LARELECCALIB_ILARAUTOCORRTOTALTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <vector> 

static const InterfaceID IID_ILArAutoCorrTotalTool("ILArAutoCorrTotalTool", 1 , 0);

class ILArAutoCorrTotalTool: virtual public IAlgTool {
  /*
   * AlgoTool to compute the total (electronic + pile-up parts) 
      autocorrelation function
   *
   */

 public: 

  virtual ~ILArAutoCorrTotalTool() {}
   
  virtual const std::vector<double> autoCorrTotal(const HWIdentifier& id, 
						  int gain,
						  float Nminbias=-1) const = 0;
  
  virtual const std::vector<double> samplRMS(const HWIdentifier& id,
                                             int gain,
                                             float Nminbias=-1) const = 0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;  

 static const InterfaceID& interfaceID() { return IID_ILArAutoCorrTotalTool; }

};

#endif 
