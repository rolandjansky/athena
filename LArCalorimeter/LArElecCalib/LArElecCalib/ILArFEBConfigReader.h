/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARELECCALIB_ILARFEBCONFIGREADERTOOL_H
#define LARELECCALIB_ILARFEBCONFIGREADERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/HWIdentifier.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "LArElecCalib/LArCalibErrorCode.h"


static const InterfaceID IID_ILArFEBConfigReader("ILArFEBConfigReader", 1 , 0);

class ILArFEBConfigReader: virtual public IAlgTool {
 public: 

  virtual ~ILArFEBConfigReader() {};
  
  
  virtual short lowerGainThreshold(const HWIdentifier& id) const = 0 ; 
  virtual short upperGainThreshold(const HWIdentifier& id) const = 0 ;
  
  static const InterfaceID& interfaceID() { return IID_ILArFEBConfigReader; }

  virtual StatusCode loadData(IOVSVC_CALLBACK_ARGS) =0;
   

  enum {ERRORCODE = LArElecCalib::ERRORCODE};
};

#endif 
