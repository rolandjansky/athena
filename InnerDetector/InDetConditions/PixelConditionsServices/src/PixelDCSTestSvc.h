/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class IPixelDCSSvc;
class PixelConditionsSummarySvc;
class PixelID;
namespace InDetDD{ 
  class PixelDetectorManager; 
}

class PixelDCSTestSvc: public AthAlgorithm{

 public: 
  
  PixelDCSTestSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelDCSTestSvc();  

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


 private:

  const PixelID* m_pixid; 
  const InDetDD::PixelDetectorManager* m_pixman; 


  ServiceHandle< IPixelDCSSvc > m_pixelDCSSvc;
  ServiceHandle< PixelConditionsSummarySvc > m_pixelSvc;

};

#endif // PIXEL_DCS_TEST_SVC_H
