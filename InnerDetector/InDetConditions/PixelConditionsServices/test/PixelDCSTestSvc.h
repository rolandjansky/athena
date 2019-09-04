/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// PixelDCSTestSvc.h
// 
// An example algorithm to test PixelDCSSvc class
//
// 09-13-2007
//
// author Georges AAD <aad@cppm.in2p3.fr>
//
//****************************************************************************



#ifndef PIXEL_DCS_TEST_SVC_H
#define PIXEL_DCS_TEST_SVC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class IPixelDCSSvc;
class PixelConditionsSummaryTool;
class PixelID;

class PixelDCSTestSvc: public AthAlgorithm{

 public: 
  
  PixelDCSTestSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelDCSTestSvc();  

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


 private:

  const PixelID* m_pixid; 

  ServiceHandle< IPixelDCSSvc > m_pixelDCSSvc;
  ToolHandle<PixelConditionsSummaryTool> m_pixelSvc;

};

#endif // PIXEL_DCS_TEST_SVC_H
