/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARHVCORRTOOL_H
#define LARELECCALIB_ILARHVCORRTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "AthenaKernel/IOVSvcDefs.h"



static const InterfaceID IID_ILArHVCorrTool("ILArHVCorrTool", 1 , 0);

class ILArHVCorrTool: virtual public IAlgTool {
  /**
   * AlgoTool to compute HV correction for Cell energy
   *
   * @author G.Unal
   * @version 0-0-1, 09/11/2006
   *
   */

 public: 

  virtual ~ILArHVCorrTool() {};
  
  virtual float Scale(const HWIdentifier& id)  const = 0 ;
  
  virtual float Scale(const Identifier& id)  const = 0 ;

  static const InterfaceID& interfaceID() { return IID_ILArHVCorrTool; }

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;
 
  virtual StatusCode record() = 0;

  virtual bool updateOnLastCallback() const = 0 ;
  
};

#endif 
