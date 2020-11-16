/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILAROFCTOOL_H
#define LARELECCALIB_ILAROFCTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "LArElecCalib/LArVectorProxy.h"
#include "CxxUtils/checker_macros.h"

class Identifier;
class HWIdentifier;

#include <vector> 

static const InterfaceID IID_ILArOFCTool("ILArOFCTool", 1 , 0);

class ATLAS_NOT_THREAD_SAFE ILArOFCTool: virtual public IAlgTool {
  /**
   * AlgoTool to compute OFC on fly
   *
   */

 public: 
  
  typedef LArVectorProxy OFCRef_t;

  virtual ~ILArOFCTool() {};

  virtual OFCRef_t OFC_a ATLAS_NOT_THREAD_SAFE
                        (const HWIdentifier& id, 
                         int gain,
                         float Nminbias=-1) const = 0;
  virtual OFCRef_t OFC_b ATLAS_NOT_THREAD_SAFE
                        (const HWIdentifier& id, 
                         int gain,
                         float Nminbias=-1) const = 0;
  virtual OFCRef_t OFC_a ATLAS_NOT_THREAD_SAFE
                        (const Identifier& id, 
                         int gain,
                         float Nminbias=-1) const = 0;
  virtual OFCRef_t OFC_b ATLAS_NOT_THREAD_SAFE
                        (const Identifier& id, 
                         int gain,
                         float Nminbias=-1) const = 0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;

  static const InterfaceID& interfaceID() { return IID_ILArOFCTool; }

};

#endif 
