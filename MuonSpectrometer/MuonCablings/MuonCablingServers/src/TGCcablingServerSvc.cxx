/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonCablingServers/TGCcablingServerSvc.h"

TGCcablingServerSvc::TGCcablingServerSvc(const std::string& name, ISvcLocator* sl) : 
AthService( name, sl )
{
}

// queryInterface 
StatusCode 
TGCcablingServerSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if( IID_TGCcablingServerSvc.versionMatch(riid) ) 
    { 
        *ppvIF = dynamic_cast<ITGCcablingServerSvc*>(this); 
    } else { 
        ATH_MSG_DEBUG ( name() << " cast to Service Interface" );
        return AthService::queryInterface(riid, ppvIF); 
    }
    
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode
TGCcablingServerSvc::giveCabling(const ITGCcablingSvc*& cabling) const {
    ATH_MSG_DEBUG ( "requesting instance of TGCcabling" );
    
    cabling = 0;
    
    if(m_atlas) {
        ATH_CHECK( service( "MuonTGC_CablingSvc",cabling,true) );
    } else {
        ATH_CHECK(  service("TGCcablingSvc",cabling,true) );
    }

    return StatusCode::SUCCESS;
}

bool
TGCcablingServerSvc::isAtlas() const {
    return m_atlas;
}
