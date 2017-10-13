/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IOVDB test package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IOVDbTestAlg.h,v 1.17 2009-03-30 12:10:14 ivukotic Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IOVDBTESTALG_IOVDBTESTALG_H
# define IOVDBTESTALG_IOVDBTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "IOVDbTestConditions/IOVDbTestMDTEleMap.h"

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StoreGateSvc;
class EventInfo;
class IIOVRegistrationSvc;
class IAthenaOutputStreamTool;

/**
 ** Algorithm to test writing conditions data and reading them back.
 **/

class IOVDbTestAlg: public AthAlgorithm 
{
public:
    IOVDbTestAlg (const std::string& name, ISvcLocator* pSvcLocator);
    ~IOVDbTestAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    StatusCode createCondObjects();
    StatusCode printCondObjects();
    StatusCode streamOutCondObjects();
    StatusCode registerCondObjects();
    StatusCode readWithBeginRun();
    void       waitForSecond();
    StatusCode testCallBack(  IOVSVC_CALLBACK_ARGS  );
    StatusCode registerIOV(const CLID& clid);

    BooleanProperty           m_writeCondObjs;
    BooleanProperty           m_regIOV;
    BooleanProperty           m_readWriteCool;
    BooleanProperty           m_twoStepWriteReg;
    BooleanProperty           m_createExtraChans;
    BooleanProperty           m_nameChans;
    BooleanProperty           m_readInInit;
    BooleanProperty           m_writeOnlyCool;
    BooleanProperty           m_fancylist;
    BooleanProperty           m_printLB;
    BooleanProperty           m_writeNewTag;
    BooleanProperty           m_readNewTag;
    BooleanProperty           m_noStream;
    IntegerProperty           m_regTime;
    StringProperty            m_streamName;
	IntegerProperty           m_run;
    BooleanProperty           m_online;
    std::string               m_tagID;
    
    StoreGateSvc*             m_sgSvc;
    StoreGateSvc*             m_detStore;
    const EventInfo*          m_evt;
    IIOVRegistrationSvc*      m_regSvc;
    IAthenaOutputStreamTool*  m_streamer;
};


#endif // IOVDBTESTALG_IOVDBTESTALG_H
