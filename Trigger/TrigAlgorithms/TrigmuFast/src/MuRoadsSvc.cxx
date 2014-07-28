/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TrigmuFast/MuRoadsSvc.h"

using namespace std;

MuRoadsSvc::MuRoadsSvc(const std::string& name,ISvcLocator* sl) :
    Service(name,sl),m_scaleBarrelInner(0),m_scaleBarrelMiddle(0),
    m_scaleBarrelOuter(0)
{
    declareProperty("ScaleBarrelInnerRoads",m_scaleBarrelInner);
    declareProperty("ScaleBarrelMiddleRoads",m_scaleBarrelMiddle);
    declareProperty("ScaleBarrelOuterRoads",m_scaleBarrelOuter);
}

// queryInterface 
StatusCode MuRoadsSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if(IID_IMuRoadsSvc.versionMatch(riid) ) 
    { 
        *ppvIF = (MuRoadsSvc*)this; 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode MuRoadsSvc::initialize()
{
    StatusCode sc;

    MsgStream log(messageService(), name());
    log << MSG::DEBUG << name() << ": Start of run initialisation" << endreq;

    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;

    m_muon_roads = 0;
    m_muon_roads = new MuRoads(m_scaleBarrelInner,
                               m_scaleBarrelMiddle,
			       m_scaleBarrelOuter);
    
    return StatusCode::SUCCESS;
}

StatusCode MuRoadsSvc::finalize() 
{
    if (m_muon_roads) delete m_muon_roads;
    return StatusCode::SUCCESS;
} 

// Implement the specifics ...

const MuRoads*
MuRoadsSvc::muRoads() const
{
    return m_muon_roads;
} 
