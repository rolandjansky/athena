/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonContainerManager/MuonRdoContainerManager.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

#include "StoreGate/ActiveStoreSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"


StatusCode 
Muon::MuonRdoContainerAccess::record(RpcPadContainer* cont, std::string key, 
                                      ISvcLocator* svcloc, MsgStream& log, StoreGateSvc* storeGate)
{
    //create the class instance if needed
    if(MuonRdoContainerManager::s_instance==0) 
    {
        try {
            MuonRdoContainerManager::s_instance = 
	                               new MuonRdoContainerManager(svcloc, storeGate);
        } catch (...) {
	    log << MSG::FATAL << "Cannot instanciate MuonRdoContainerManager"
	        << endmsg;
            return StatusCode::FAILURE;
	}
    }
    
    MuonRdoContainerManager* instance = MuonRdoContainerManager::s_instance;
    
    if (storeGate && instance->m_evtstore!=storeGate){
        // EJWM. This seems pretty inelegant. Why bother caching the sg pointer this can happen (e.g. with overlay)
        log << MSG::VERBOSE<<"Changing cached store from "<<instance->m_evtstore->name()<<" to "<<storeGate->name()<<endmsg;
        instance->m_evtstore=storeGate;
    }
    
    instance->m_activeStore->setStore( instance->m_evtstore );

    //exit if the manager already hold a RpcPadContainer
    if(instance->m_pad_container.first!=0) return StatusCode::SUCCESS;
    
    
    //cleanup the container if needed
    if (cont->numberOfCollections()!=0) cont->cleanup();

    
    //register the container in StoreGate with addRef
    cont->addRef();
    if (instance->m_evtstore->record(cont, key).isFailure()) 
    {
        log << MSG::FATAL << "Unable to record RPC PAD Container." << endmsg;
        return StatusCode::FAILURE;
    }
    
    
    //keep the pointer and the key in the manager
    instance->m_pad_container.first  = cont;
    instance->m_pad_container.second = key;
    
    log << MSG::DEBUG << "Container " << key << " recorded." << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode 
Muon::MuonRdoContainerAccess::record(RpcSectorLogicContainer* cont, std::string key, 
                                      ISvcLocator* svcloc, MsgStream& log, StoreGateSvc* storeGate)
{
    //create the class instance if needed
    if(MuonRdoContainerManager::s_instance==0) 
    {
        try {
            MuonRdoContainerManager::s_instance = 
	                               new MuonRdoContainerManager(svcloc, storeGate);
        } catch (...) {
	    log << MSG::FATAL << "Cannot instanciate MuonRdoContainerManager"
	        << endmsg;
            return StatusCode::FAILURE;
	}
    }
    
    MuonRdoContainerManager* instance = MuonRdoContainerManager::s_instance;

    if (storeGate && instance->m_evtstore!=storeGate){
        // EJWM. This seems pretty inelegant. Why bother caching the sg pointer this can happen (e.g. with overlay)
        log << MSG::VERBOSE<<"Changing cached store from "<<instance->m_evtstore->name()<<" to "<<storeGate->name()<<endmsg;
        instance->m_evtstore=storeGate;
    }

    instance->m_activeStore->setStore( instance->m_evtstore );
    
    //exit if the manager already hold a RpcPadContainer
    if(instance->m_sec_container.first!=0) return StatusCode::SUCCESS;
    
    
    //cleanup the container if needed
    //if (cont->numberOfCollections()!=0) cont->cleanup();

    
    //register the container in StoreGate with addRef
    //cont->addRef();
    if (instance->m_evtstore->record(cont, key).isFailure()) 
    {
        log << MSG::FATAL << "Unable to record Sector Logic Container." << endmsg;
        return StatusCode::FAILURE;
    }
    
    
    //keep the pointer and the key in the manager
    instance->m_sec_container.first  = cont;
    instance->m_sec_container.second = key;
    
    log << MSG::DEBUG << "Container " << key << " recorded." << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode 
Muon::MuonRdoContainerAccess::record(MdtCsmContainer* cont,std::string key,
                                      ISvcLocator* svcloc, MsgStream& log, StoreGateSvc* storeGate)
{    
    //create the class instance if needed
    if(MuonRdoContainerManager::s_instance==0) 
    {
        try {
            MuonRdoContainerManager::s_instance = 
	                               new MuonRdoContainerManager(svcloc, storeGate);
        } catch (...) {
	    log << MSG::FATAL << "Cannot instanciate MuonRdoContainerManager"
	        << endmsg;
            return StatusCode::FAILURE;
	}
    }
    
    MuonRdoContainerManager* instance = MuonRdoContainerManager::s_instance;

    if (storeGate && instance->m_evtstore!=storeGate){
        // EJWM. This seems pretty inelegant. Why bother caching the sg pointer this can happen (e.g. with overlay)
        log << MSG::VERBOSE<<"Changing cached store from "<<instance->m_evtstore->name()<<" to "<<storeGate->name()<<endmsg;
        instance->m_evtstore=storeGate;
    }

    instance->m_activeStore->setStore( instance->m_evtstore );
    
    //exit if the manager already hold a MdtCsmContainer
    if(instance->m_csm_container.first!=0) return StatusCode::SUCCESS;
    
    
    //cleanup the container if needed
    if (cont->numberOfCollections()!=0) cont->cleanup();

    
    //register the container in StoreGate with addRef
    cont->addRef();
    if (instance->m_evtstore->record(cont, key).isFailure()) 
    {
        log << MSG::FATAL << "Unable to record MDT CSM Container." << endmsg;
        return StatusCode::FAILURE;
    }
    
    
    //keep the pointer and the key in the manager
    instance->m_csm_container.first  = cont;
    instance->m_csm_container.second = key;
    
    log << MSG::DEBUG << "Container " << key << " recorded." << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode 
Muon::MuonRdoContainerAccess::record(TgcRdoContainer* cont, std::string key,
				      ISvcLocator* svcloc, MsgStream& log, StoreGateSvc* storeGate)
{
    //create the class instance if needed
    if(MuonRdoContainerManager::s_instance==0) 
    {
        try {
            MuonRdoContainerManager::s_instance = 
	                               new MuonRdoContainerManager(svcloc, storeGate);
        } catch (...) {
	    log << MSG::FATAL << "Cannot instanciate MuonRdoContainerManager"
	        << endmsg;
            return StatusCode::FAILURE;
	}
    }
    
    MuonRdoContainerManager* instance = MuonRdoContainerManager::s_instance;

    if (storeGate && instance->m_evtstore!=storeGate){
        // EJWM. This seems pretty inelegant. Why bother caching the sg pointer this can happen (e.g. with overlay)
        log << MSG::VERBOSE<<"Changing cached store from "<<instance->m_evtstore->name()<<" to "<<storeGate->name()<<endmsg;
        instance->m_evtstore=storeGate;
    }

    instance->m_activeStore->setStore( instance->m_evtstore );
    
    //exit if the manager already hold a TgcRdoContainer
    if(instance->m_tgc_container.first!=0) return StatusCode::SUCCESS;
    
    
    //cleanup the container if needed
    if (cont->numberOfCollections()!=0) cont->cleanup();

    
    //register the container in StoreGate with addRef
    cont->addRef();
    if (instance->m_evtstore->record(cont, key).isFailure()) 
    {
        log << MSG::FATAL << "Unable to record TGC RDO Container." << endmsg;
        return StatusCode::FAILURE;
    }
    
    
    //keep the pointer and the key in the manager
    instance->m_tgc_container.first  = cont;
    instance->m_tgc_container.second = key;
    
    log << MSG::DEBUG << "Container " << key << " recorded." << endmsg;
    
    return StatusCode::SUCCESS;

}

StatusCode 
Muon::MuonRdoContainerAccess::record(CscRawDataContainer* cont, std::string key,
				      ISvcLocator* svcloc, MsgStream& log, StoreGateSvc* storeGate)
{
    //create the class instance if needed
    if(MuonRdoContainerManager::s_instance==0) 
    {
        try {
            MuonRdoContainerManager::s_instance = 
	                               new MuonRdoContainerManager(svcloc, storeGate);
        } catch (...) {
	    log << MSG::FATAL << "Cannot instanciate MuonRdoContainerManager"
	        << endmsg;
            return StatusCode::FAILURE;
	}
    }
    
    MuonRdoContainerManager* instance = MuonRdoContainerManager::s_instance;

    if (storeGate && instance->m_evtstore!=storeGate){
        // EJWM. This seems pretty inelegant. Why bother caching the sg pointer this can happen (e.g. with overlay)
        log << MSG::VERBOSE<<"Changing cached store from "<<instance->m_evtstore->name()<<" to "<<storeGate->name()<<endmsg;
        instance->m_evtstore=storeGate;
    }
    instance->m_activeStore->setStore( instance->m_evtstore );
    
    //exit if the manager already hold a CscRawDataContainer
    if(instance->m_csc_container.first!=0) return StatusCode::SUCCESS;
    
    
    //cleanup the container if needed
    if (cont->numberOfCollections()!=0) cont->cleanup();

    
    //register the container in StoreGate with addRef
    cont->addRef();
    if (instance->m_evtstore->record(cont, key).isFailure()) 
    {
        log << MSG::FATAL << "Unable to record CSC RAW DATA Container." << endmsg;
        return StatusCode::FAILURE;
    }
    
    
    //keep the pointer and the key in the container
    instance->m_csc_container.first  = cont;
    instance->m_csc_container.second = key;
    
    log << MSG::DEBUG << "Container " << key << " recorded." << endmsg;
    
    return StatusCode::SUCCESS;

}

RpcPadContainer*
Muon::MuonRdoContainerAccess::retrieveRpcPad(std::string key)
{
    if(MuonRdoContainerManager::s_instance)
    {
        if(MuonRdoContainerManager::s_instance->m_pad_container.second == key) 
	    return MuonRdoContainerManager::s_instance->m_pad_container.first;
    }
    
    return 0;
}

RpcSectorLogicContainer*
Muon::MuonRdoContainerAccess::retrieveRpcSec(std::string key)
{
    if(MuonRdoContainerManager::s_instance)
    {
        if(MuonRdoContainerManager::s_instance->m_sec_container.second == key) 
	    return MuonRdoContainerManager::s_instance->m_sec_container.first;
    }
    
    return 0;
}


MdtCsmContainer*
Muon::MuonRdoContainerAccess::retrieveMdtCsm(std::string key)
{
    if(MuonRdoContainerManager::s_instance)
    {
        if(MuonRdoContainerManager::s_instance->m_csm_container.second == key) 
	    return MuonRdoContainerManager::s_instance->m_csm_container.first;
    }
    
    return 0;

}

TgcRdoContainer*
Muon::MuonRdoContainerAccess::retrieveTgcRdo(std::string key)
{
    if(MuonRdoContainerManager::s_instance)
    {
        if(MuonRdoContainerManager::s_instance->m_tgc_container.second == key) 
	    return MuonRdoContainerManager::s_instance->m_tgc_container.first;
    }
    
    return 0;

}

CscRawDataContainer*
Muon::MuonRdoContainerAccess::retrieveCscRaw(std::string key)
{
    if(MuonRdoContainerManager::s_instance)
    {
        if(MuonRdoContainerManager::s_instance->m_csc_container.second == key) 
	    return MuonRdoContainerManager::s_instance->m_csc_container.first;
    }
    
    return 0;
}
