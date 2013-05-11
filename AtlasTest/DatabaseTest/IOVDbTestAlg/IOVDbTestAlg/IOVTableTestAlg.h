/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IOVDB test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: IOVTableTestAlg.h,v 1.3 2007-02-08 06:50:06 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IOVDBTESTALG_IOVTABLETESTALG_H
# define IOVDBTESTALG_IOVTABLETESTALG_H


#include "GaudiKernel/Algorithm.h"
#include "AthenaKernel/IOVSvcDefs.h" /*IOVSVC_CALLBACK_ARGS*/
#include "GaudiKernel/IIncidentListener.h"

class StoreGateSvc;
class EventInfo;
class IIOVRegistrationSvc;
class GenericDbTable;

/**
 ** Algorithm to test writing conditions data and reading them back.
 **/

class IOVTableTestAlg: public Algorithm,
		       virtual public IIncidentListener {
public:
    IOVTableTestAlg (const std::string& name, ISvcLocator* pSvcLocator);
    ~IOVTableTestAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    void printTable(const GenericDbTable *table);

    /// incident service handle for EndEvent
    void handle(const Incident& incident);

private:

    StatusCode createCondObjects();
    StatusCode printCondObjects();
    
    StatusCode registerIOV(const CLID& clid);

    BooleanProperty       m_regIOV;
    BooleanProperty       m_createObjs;
    
    StoreGateSvc*         m_sgSvc;
    StoreGateSvc*         m_detStore;
    const EventInfo*      m_evt;
    IIOVRegistrationSvc*  m_regSvc;
};


#endif // IOVDBTESTALG_IOVTABLETESTALG_H
