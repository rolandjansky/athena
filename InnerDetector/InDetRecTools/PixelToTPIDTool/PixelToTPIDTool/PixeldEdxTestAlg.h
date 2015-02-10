/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTESTALG_PIXELDEDXTESTALG_H
#define IOVDBTESTALG_PIXELDEDXTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#define NCLASS 5
#define NPAR   9

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StoreGateSvc;
class EventInfo;
class IIOVRegistrationSvc;
class IAthenaOutputStreamTool;

/**
 ** Algorithm to test writing conditions data and reading them back.
 **/

class PixeldEdxTestAlg: public AthAlgorithm 
{
public:
    PixeldEdxTestAlg (const std::string& name, ISvcLocator* pSvcLocator);
    ~PixeldEdxTestAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    StatusCode createCondObjects();
    StatusCode printCondObjects();
    StatusCode streamOutCondObjects();
    StatusCode registerCondObjects();
    StatusCode readWithBeginRun();
	StatusCode testCallBack(  IOVSVC_CALLBACK_ARGS  );

    std::string               m_tagID;    
  ServiceHandle<IIOVRegistrationSvc>   m_regSvc;
  ToolHandle<IAthenaOutputStreamTool>  m_streamer;
  std::string m_filename;
};


#endif 
