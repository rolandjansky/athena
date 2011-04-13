/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "MuonCablingServers/TGCcablingServerSvc.h"
#include "GaudiKernel/MsgStream.h"

TGCcablingServerSvc::TGCcablingServerSvc(const std::string& name, ISvcLocator* sl) : 
Service( name, sl ),
m_pDetStore(0), 
m_tagInfoMgr(0), 
m_tagsCompared(false)
{
    declareProperty( "Atlas", m_atlas=true );
    declareProperty( "forcedUse", m_forcedUse=false );
    declareProperty( "useMuonTGC_CablingSvc", m_useMuonTGC_CablingSvc=true );
}

// queryInterface 
StatusCode 
TGCcablingServerSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if( IID_TGCcablingServerSvc.versionMatch(riid) ) 
    { 
        *ppvIF = dynamic_cast<ITGCcablingServerSvc*>(this); 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cast to Service Interface" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }
    
    addRef();
    return StatusCode::SUCCESS;
} 

StatusCode
TGCcablingServerSvc::initialize() 
{
    
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
    

    if(m_forcedUse) { // Selected cabling is used without comparison 
        m_tagsCompared = true;
    } else { // The cabling to be used and the cabling in tag info will be compared by compareTags method
        if(m_pDetStore->regFcn(&ITGCcablingServerSvc::compareTags,
                               dynamic_cast<ITGCcablingServerSvc*>(this),
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
TGCcablingServerSvc::finalize() {
    return StatusCode::SUCCESS;
}

StatusCode
TGCcablingServerSvc::giveCabling(const ITGCcablingSvc*& cabling) const {
    StatusCode sc;
    MsgStream log(messageService(), name());
    log << MSG::DEBUG << "requesting instance of TGCcabling" << endreq;
    
    if (!this->isConfigured()) {
        log<<MSG::ERROR<<"The tagsCompared callback has not yet happened! Taking default configuration ("<< (m_atlas ? "12-fold cabling." : "8-fold cabling")
        <<" Move this call to execute() / beginRun() to get rid of this WARNING message (or set the forcedUse property to True)"<<endreq;
    }
    
    cabling = 0;
    
    if(m_atlas) {
        sc = service((m_useMuonTGC_CablingSvc ? "MuonTGC_CablingSvc" : "TGCcabling12Svc"),cabling,true);
        MsgStream log(messageService(), name());
        if ( sc != StatusCode::SUCCESS ) {
            MsgStream log(messageService(), name());
            log << MSG::ERROR << "Cannot retrieve the instance of TGCcabling12"
	        << endreq;
            return sc; 
        }
    } else {
        sc = service("TGCcablingSvc",cabling,true);
        if ( sc != StatusCode::SUCCESS ) {
            MsgStream log(messageService(), name());
            log << MSG::ERROR << "Cannot retrieve the instance of TGCcabling"
	        << endreq; 
            return sc; 
        }
    }

    return StatusCode::SUCCESS;
}

bool
TGCcablingServerSvc::isAtlas() const {
    MsgStream log(messageService(), name());
    
    if (!this->isConfigured()) {
        log<<MSG::ERROR<<"The tagsCompared callback has not yet happened! Taking default configuration ("<< (m_atlas ? "12-fold cabling." : "8-fold cabling")
        <<" Move this call to execute() / beginRun() to get rid of this WARNING message (or set the forcedUse property to True)"<<endreq;
    }
    return m_atlas;
}

bool
TGCcablingServerSvc::isConfigured() const {
    return m_tagsCompared;
}

StatusCode 
TGCcablingServerSvc::compareTags(IOVSVC_CALLBACK_ARGS)
{
    MsgStream log(msgSvc(), name());
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
        tagInfo->findInputTag("TGC_CablingType", cablingType);
        
        log << MSG::INFO 
        << "TGC_CablingType from TagInfo: " << cablingType << endreq;
	
	if(cablingType=="") {
            // assume it is 8-fold in case the call-back is active
            cablingType="TGCcabling8Svc";
        }
        
	// check cablingType
        if (m_atlas) {
            tagMatch = (cablingType != "TGCcabling8Svc");
        } else {
            tagMatch = (cablingType != "TGCcabling12Svc");
        } 
    }
    
    if (!tagMatch) {
        std::string cablingName;
        std::string flagAtlas;
        if (m_atlas) {
            cablingName = "TGCcabling12 (12-fold)";
            flagAtlas  =  "True";
            m_atlas = false;
        } else {
            cablingName = "TGCcabling (8-fold)";
            flagAtlas  =  "False";
            m_atlas = true;
        }
        
        log << MSG::INFO 
        << "TGC_CablingType : " << cablingType << " is mismatched "
        << "with TGCcablingServerSvc configuration of "<< cablingName << ". "
        << "m_atlas flag is flipped to " << (m_atlas ? "true" : "false") << "." 
        << endreq; 
        log << MSG::INFO 
        << "If you definitely want to use " << cablingName << ", " 
        << "please add the following lines in your jobOptions \n\n" 
        << "  from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc \n"
        << "  ServiceMgr += TGCcablingServerSvc() \n"
        << "  theApp.CreateSvc += [ \"TGCcablingServerSvc\" ] \n"
        << "  ServiceMgr.TGCcablingServerSvc.Atlas=" << (!m_atlas ? "True" : "False") << " \n" 
        << "  ServiceMgr.TGCcablingServerSvc.forcedUse=True \n"
        << "  ServiceMgr.TGCcablingServerSvc.useMuonTGC_CablingSvc=True \n"
        << "  from TGC_CondCabling.TGC_CondCablingConf import TGCCablingDbTool \n"
        << "  ToolSvc += TGCCablingDbTool() \n"
        << "  from IOVDbSvc.CondDB import conddb \n"
        << "  conddb.addFolderSplitMC('TGC','/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA') \n"
        << endreq;
    }
    
    log << MSG::INFO << "compareTags() callback determined that the cabling is "<< cablingType << endreq;

    m_tagsCompared=true;
    return StatusCode::SUCCESS;
}

