/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_PIXEL_DCS_TOOL_H
#define I_PIXEL_DCS_TOOL_H

// Includes for Gaudi 
#include "GaudiKernel/IAlgTool.h" 
#include "AthenaKernel/IOVSvcDefs.h" 
#include "AthenaKernel/IAddressProvider.h" 

static const InterfaceID IID_IPixelDCSTool("IPixelDCSTool", 1, 0);

class IPixelDCSTool: virtual public IAlgTool{

 public:

  virtual ~IPixelDCSTool()
    { }

  static const InterfaceID& interfaceID(){return IID_IPixelDCSTool;};
  virtual StatusCode initialize()=0;
  virtual StatusCode finalize()=0;
  
  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS)=0; 

  virtual StatusCode createDCSData() =0;
  virtual StatusCode recordDCSData() const=0;
  virtual StatusCode writeDataToDB() const=0;

  virtual StatusCode printData() const=0;

}; 


#endif // I_PIXEL_DCS_TOOL_H
