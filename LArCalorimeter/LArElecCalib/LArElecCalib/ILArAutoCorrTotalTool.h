/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARAUTOCORRTOTALTOOL_H
#define LARELECCALIB_ILARAUTOCORRTOTALTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "CxxUtils/checker_macros.h"
#include <vector> 

static const InterfaceID IID_ILArAutoCorrTotalTool("ILArAutoCorrTotalTool", 1 , 0);

class ATLAS_NOT_THREAD_SAFE ILArAutoCorrTotalTool: virtual public IAlgTool {
  /*
   * AlgoTool to compute the total (electronic + pile-up parts) 
      autocorrelation function
   *
   */

 public: 

  virtual ~ILArAutoCorrTotalTool() {}
   
  virtual const std::vector<double> autoCorrTotal ATLAS_NOT_THREAD_SAFE
                                                 (const HWIdentifier& id, 
						  int gain,
						  float Nminbias=-1) const = 0;
  
  virtual const std::vector<double> samplRMS ATLAS_NOT_THREAD_SAFE
                                            (const HWIdentifier& id,
                                             int gain,
                                             float Nminbias=-1) const = 0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;  

 static const InterfaceID& interfaceID() { return IID_ILArAutoCorrTotalTool; }

};

#endif 
