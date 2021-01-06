/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingServers/RPCcablingServerSvc.h"


RPCcablingServerSvc::RPCcablingServerSvc(const std::string& name, ISvcLocator* sl) : 
  AthService( name, sl ),
  m_tagsCompared(false),
  m_tagInfoMgr(0)
{
// these JO are not effective anymore, kept temporarely to avoid to change the config. pythons
    declareProperty( "Atlas", m_atlas=false ); 
    // Atlas true/false will imply data-like cabling/RPCcablingSim otherwise
    declareProperty( "forcedUse", m_forcedUse=true ); 
    // forced verso auto conf. (for data and digitization always force to Atlas + useMuonRPC_Cabling) 
    declareProperty( "useMuonRPC_CablingSvc", m_useMuonRPC_CablingSvc=true); 
    // if Atlas, it is still possible to set this to false and use RPCcabling
}

// queryInterface 
StatusCode 
RPCcablingServerSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if( IID_RPCcablingServerSvc.versionMatch(riid) ) 
    { 
        *ppvIF = dynamic_cast<IRPCcablingServerSvc*>(this); 
    } else { 
        ATH_MSG_DEBUG ( name() << " cannot found the interface!" );
        return AthService::queryInterface(riid, ppvIF); 
    }

    addRef();
    return StatusCode::SUCCESS;
} 

StatusCode
RPCcablingServerSvc::initialize() {

    ATH_MSG_DEBUG ( "initializing ..." );

    ATH_CHECK( AthService::initialize() );

    if(m_forcedUse) { 
	// Selected cabling is used without comparison 
        m_tagsCompared = true;
    } 
    else 
    { 
       // The cabling to be used and the cabling in tag info will be compared by compareTags method
       ATH_CHECK( service("TagInfoMgr", m_tagInfoMgr) );
       m_tagInfoMgr->addListener(this);
    }

    ATH_MSG_DEBUG ( "... done!" );
    return StatusCode::SUCCESS;
}

StatusCode 
RPCcablingServerSvc::giveCabling(const IRPCcablingSvc*& cabling) const {

    ATH_MSG_DEBUG ( "requesting instance of RPCcabling" );

    cabling = 0;
    
    if (!isConfigured()) {
        ATH_MSG_INFO ( "Tring to get an instance of the RPC cablingSvc with autoconfiguration requested but tags have not been compared yet - it may happen that inconsistencies are found later on ");
	ATH_MSG_INFO ( "setting of conf. flags is: Atlas = "<<m_atlas<<" forcedUse = "<<m_forcedUse<<" useMuonRPC_CablingSvc = "<<m_useMuonRPC_CablingSvc);
	//	sc = StatusCode::RECOVERABLE;
	//	return sc;
    }    

    
    //if(m_atlas) {
    //sc = service((m_useMuonRPC_CablingSvc ? "MuonRPC_CablingSvc" : "RPCcablingSvc"),cabling,true);
    ATH_CHECK( service("MuonRPC_CablingSvc",cabling,true) );

    //} else {
    //	sc = service("RPCcablingSimSvc",cabling,true);
    //	if ( sc != StatusCode::SUCCESS ) {
    //	    MsgStream log(msgSvc(), name());
    //	    log << MSG::ERROR << "Cannot retrieve the instance of RPCcablingSim"
    //	        << endmsg; 
    //       return sc; 
    //   }
    //}

    
    ATH_MSG_DEBUG ("Retrieved RPC cabling svc ");
    return StatusCode::SUCCESS;
}

bool
RPCcablingServerSvc::isAtlas() const {
    return m_atlas;
}


StatusCode 
RPCcablingServerSvc::compareTags()
{
    ATH_MSG_INFO ( "compareTags() callback triggered" );
    
    std::string cablingType = m_tagInfoMgr->findInputTag("RPC_CablingType");
    ATH_MSG_INFO( "RPC_CablingType from TagInfo: " << cablingType );
    
    if(cablingType=="") {
       // assume it is SIM in case the call-back is active
       //            cablingType="RPCcablingSim";
       cablingType="MuonRPC_Cabling";
    }
    // check cablingType
    if (cablingType != "MuonRPC_Cabling"){
       ATH_MSG_ERROR( "RPC_CablingType from TagInfo is "
                      << cablingType
                      << "  , incompatible with present cabling package MuonRPC_Cabling" );
       return StatusCode::FAILURE;
    }
    m_tagsCompared=true;
    return StatusCode::SUCCESS;
}
