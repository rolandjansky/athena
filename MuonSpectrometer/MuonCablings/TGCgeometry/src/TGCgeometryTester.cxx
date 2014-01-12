/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCgeometry/TGCgeometryTester.h"
#include "TGCgeometry/TGCgeometrySvc.h"

TGCgeometryTester::TGCgeometryTester(const std::string& name, ISvcLocator* pSvcLocator) :
Algorithm(name, pSvcLocator),
m_pTGCgeometrySvc(NULL)
{
}

StatusCode TGCgeometryTester::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;

    // initialize TGCgeometrySvc
    StatusCode sc = serviceLocator()->service("TGCgeometrySvc", m_pTGCgeometrySvc);
    if (sc.isFailure() || m_pTGCgeometrySvc == NULL)
    {
        log << MSG::ERROR << "Cannot retrieve service TGCgeometrySvc" << endreq;
        return sc;
    }

    return StatusCode::SUCCESS;
}

StatusCode TGCgeometryTester::execute()
{
    MsgStream log(msgSvc(), name());
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG << "Executing " << name() << endreq;

    return StatusCode::SUCCESS;
}

StatusCode TGCgeometryTester::finalize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Finalizing " << name() << " - package version " << PACKAGE_VERSION << endreq;

    return StatusCode::SUCCESS;
}

