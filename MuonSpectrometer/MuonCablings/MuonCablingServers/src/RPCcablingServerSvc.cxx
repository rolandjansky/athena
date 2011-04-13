/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingServers/RPCcablingServerSvc.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"



RPCcablingServerSvc::RPCcablingServerSvc(const std::string& name, ISvcLocator* sl) : 
  Service( name, sl ),
  m_tagsCompared(false)
{
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
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }

    addRef();
    return StatusCode::SUCCESS;
} 

StatusCode
RPCcablingServerSvc::initialize() {

    StatusCode sc;
    MsgStream log(messageService(), name());
    log << MSG::DEBUG << "initializing ..." << endreq;

    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;

    // get DetectorStore
    sc = service("DetectorStore", m_pDetStore );  
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found!" << endreq;
        return sc;
    } 



    if(m_forcedUse) { 
	// Selected cabling is used without comparison 
        m_tagsCompared = true;
    } 
    else 
    { 
	// The cabling to be used and the cabling in tag info will be compared by compareTags method
	// **** **** **** TagInfo **** **** ****
	const DataHandle<TagInfo> tagInfoH;
	std::string tagInfoKey = "";
	// get the key
	sc = service("TagInfoMgr", m_tagInfoMgr);
	if(sc.isFailure() || m_tagInfoMgr==0) {
	    log << MSG::WARNING << " Unable to locate TagInfoMgr service" << endreq; 
	} else {
	    tagInfoKey = m_tagInfoMgr->tagInfoKey();
	}

        if(m_pDetStore->regFcn(&ICallBackRPCcablingServerSvc::compareTags,
                               dynamic_cast<ICallBackRPCcablingServerSvc*>(this),
                               tagInfoH, 
                               tagInfoKey) 
           != StatusCode::SUCCESS) {
            log << MSG::WARNING 
            << "Cannot register compareTags function for key "  << tagInfoKey << endreq;
        } else {
            log << MSG::DEBUG 
            << "Registered compareTags callback for key: " << tagInfoKey << endreq;
        }
    }


    log << MSG::DEBUG << "... done!" << endreq;

    return sc;
}

StatusCode
RPCcablingServerSvc::finalize() {
    return StatusCode::SUCCESS;
}

StatusCode 
RPCcablingServerSvc::giveCabling(const IRPCcablingSvc*& cabling) const {

    StatusCode sc;
    MsgStream log(messageService(), name());
    log << MSG::DEBUG << "requesting instance of RPCcabling" << endreq;

    cabling = 0;
    
    if (!isConfigured()) {
	log << MSG::INFO << "Tring to get an instance of the RPC cablingSvc with autoconfiguration requested but tags have not been compared yet - it may happen that inconsistencies are found later on "<<endreq;
	log << MSG::INFO << "setting of conf. flags is: Atlas = "<<m_atlas<<" forcedUse = "<<m_forcedUse<<" useMuonRPC_CablingSvc = "<<m_useMuonRPC_CablingSvc<<endreq;
	//	sc = StatusCode::RECOVERABLE;
	//	return sc;
    }    

    if(m_atlas) {
        sc = service((m_useMuonRPC_CablingSvc ? "MuonRPC_CablingSvc" : "RPCcablingSvc"),cabling,true);
	if ( sc != StatusCode::SUCCESS ) {
	    MsgStream log(messageService(), name());
	    log << MSG::ERROR << "Cannot retrieve the instance of RPCcabling"
	        << endreq;
            return sc; 
        }
    } else {
	sc = service("RPCcablingSimSvc",cabling,true);
	if ( sc != StatusCode::SUCCESS ) {
	    MsgStream log(messageService(), name());
	    log << MSG::ERROR << "Cannot retrieve the instance of RPCcablingSim"
	        << endreq; 
            return sc; 
        }
    }
    
    log << MSG::DEBUG <<"Retrieved RPC cabling svc "<<endreq;
    return StatusCode::SUCCESS;
}

bool
RPCcablingServerSvc::isAtlas() const {
    return m_atlas;
}


StatusCode 
RPCcablingServerSvc::compareTags(IOVSVC_CALLBACK_ARGS)
{
    MsgStream log(messageService(), name());
    StatusCode sc;
    bool tagMatch = true;  
    
    log << MSG::INFO << "compareTags() callback triggered" << endreq;
    
    // Get TagInfo and retrieve tags
    const TagInfo* tagInfo = 0;
    sc= m_pDetStore->retrieve(tagInfo);
    
    std::string cablingType;
    
    if (sc.isFailure() || tagInfo==0) {
        log << MSG::INFO
        << "No TagInfo in DetectorStore while attempting to compare tags" 
        << endreq;
    } else {
        tagInfo->findInputTag("RPC_CablingType", cablingType);
        
        log << MSG::INFO 
        << "RPC_CablingType from TagInfo: " << cablingType << endreq;
	
	if(cablingType=="") {
            // assume it is SIM in case the call-back is active
            cablingType="RPCcablingSim";
        }
        
	// check cablingType
        if (m_atlas) {
            tagMatch = (cablingType != "RPCcablingSim") ;
	    if (!tagMatch) 
	    {
	      // expecting to run on real-atlas cabling maps and roads but input RDO file is produced with RPCcablingSim
	      m_atlas = false;
	      log << MSG::WARNING<<"Autoconfiguration of the RPC cabling Svc: Requested data-like cabling but tag info determine that cablingType must be "<<cablingType<<"; hence, resetting default"<<endreq;
	    }
	    else 
	    {
		log << MSG::INFO<<"Autoconfiguration of the RPC cabling Svc: Requested data-like cabling matches tag where cablingType is "<<cablingType<<"; hence, cablingType will be set to ";
		if (m_useMuonRPC_CablingSvc) log<<"MuonRPC_Cabling"<<endreq;
		else log<<"RPCcabling"<<endreq;	 
	    }
        } else {
            tagMatch = (cablingType == "RPCcablingSim");
	    if (!tagMatch) 
	    {
	     // expecting to run on NON REAL-atlas cabling maps and roads but input RDO file is produced with MuonRPC_Cabling
	      m_atlas = true;
	      log << MSG::WARNING<<"Autoconfiguration of the RPC cabling Svc: Requested SIM-like cabling but tag info determine that cablingType must be "<<cablingType<<"; hence, resetting default"<<endreq;
	    }
	    else
	    {
		log << MSG::INFO<<"Autoconfiguration of the RPC cabling Svc: Requested SIM-like cabling matches tag where cablingType is "<<cablingType<<endreq;
	    }
        } 
    }
    
    //log << MSG::INFO << "compareTags() callback determined that the cabling is "<< cablingType << endreq;
    log << MSG::INFO << "compareTags() callback determined that the cabling should be "<< cablingType << endreq;
    if (!tagMatch) {
	log << MSG::ERROR <<"Since an incompatible cabling gas been retrieved, stop here "<< endreq;
	log << MSG::ERROR <<"In order to process this data set, please, use the options: from MuonCnvExample.MuonCnvFlags import muonCnvFlags; muonCnvFlags.RpcCablingMode=";
	if (cablingType=="" || cablingType=="RPCcablingSim") log<<"\"sim\""<<endreq;
	if (cablingType=="RPCcabling") log<<"\"old\""<<endreq;
	if (cablingType=="MuonRPC_Cabling") log<<"\"new\""<<endreq;	
	return StatusCode::FAILURE;
    }
    
    m_tagsCompared=true;
    return StatusCode::SUCCESS;
}
