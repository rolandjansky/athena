/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// PixelDCSTestReadWrite.h
// 
// An example algorithm to test PixelDCSTool class
//
// 09-13-2007
//
// author Georges AAD <aad@cppm.in2p3.fr>
//
//****************************************************************************



#ifndef PIXEL_DCS_TEST_READ_WRITE_H
#define PIXEL_DCS_TEST_READ_WRITE_H

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "PixelConditionsData/PixelDCSData.h"
//#include "GaudiKernel/MsgStream.h"
#include "PixelDCSTool.h"

//class StoreGateSvc;
class IIOVRegistrationSvc;
class Identifier;
class PixelID; 
class IAthenaOutputStreamTool;
class IToolSvc;


class PixelDCSTestReadWrite: public AthAlgorithm{

 public: 
  
  PixelDCSTestReadWrite(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelDCSTestReadWrite();  

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


 private:

  //mutable MsgStream m_log;

  // ServiceHandle< IToolSvc >  m_toolsvc; 
  //IToolSvc*                  m_toolsvc; 

  //PixelDCSTool*     m_pixelDCSTool;
  ToolHandle<IPixelDCSTool >  m_pixelDCSTool;

  bool m_write;

};

#endif // PIXEL_DCS_TEST_READ_WRITE_H
