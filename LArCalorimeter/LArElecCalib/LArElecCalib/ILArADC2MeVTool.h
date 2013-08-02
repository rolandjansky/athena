/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARADC2MEVTOOL_H
#define LARELECCALIB_ILARADC2MEVTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <vector> 

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_ILArADC2MeVTool("ILArADC2MeVTool", 1 , 0);

class ILArADC2MeVTool: virtual public IAlgTool {
  /**
   * AlgoTool to compute ADC2MEV factor from the 3 subfactors ADC2DAC, DAC2UA, UA2MEV
   *
   * @author S. Laplace
   * @version 0-0-1, 16/01/2004
   *
   * History:
   *  - 08/02/2004, S. Laplace: new online ID
   *
   */

 public: 

  virtual ~ILArADC2MeVTool() {};
 
  virtual const std::vector<float>& ADC2MEV(const HWIdentifier& id, int gain )  const = 0 ;
  
  virtual const std::vector<float>& ADC2MEV(const Identifier& id, int gain )  const = 0 ;
  
  enum {ERRORCODE = -999};

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;

  static const InterfaceID& interfaceID() { return IID_ILArADC2MeVTool; }
};

#endif 
