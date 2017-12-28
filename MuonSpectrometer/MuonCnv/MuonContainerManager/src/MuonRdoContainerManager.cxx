/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonContainerManager/MuonRdoContainerManager.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/ActiveStoreSvc.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"


//initilize the static instance of the class
Muon::MuonRdoContainerManager* Muon::MuonRdoContainerManager::s_instance = 0;

Muon::MuonRdoContainerManager::MuonRdoContainerManager( ISvcLocator* svcloc, StoreGateSvc* storeGate ) :
    m_svcloc(svcloc),m_refCount(0)
{
    if (svcloc->service("IncidentSvc",m_incSvc).isFailure())
        throw GaudiException("Cannot find IncidentSvc",
	                     "MuonRdoContainerManager",StatusCode::FAILURE);
    
    if (svcloc->service("ActiveStoreSvc", m_activeStore).isFailure() ) {
       throw GaudiException("Cannot find ActiveStoreSvc",
                             "MuonRdoContainerManager",StatusCode::FAILURE);
    }

    if ( storeGate ) m_evtstore=storeGate; 
    else {
       if (svcloc->service("StoreGateSvc",m_evtstore).isFailure())
          throw GaudiException("Cannot find StoreGateSvc",
	                     "MuonRdoContainerManager",StatusCode::FAILURE);
    }

    m_incSvc->addListener(this, IncidentType::BeginEvent) ;
    m_incSvc->addListener(this, IncidentType::EndRun) ;
    
    m_pad_container.first=0;   m_pad_container.second = "";
    m_csm_container.first=0;   m_csm_container.second = "";
    m_tgc_container.first=0;   m_tgc_container.second = "";
    m_csc_container.first=0;   m_csc_container.second = "";
    m_sec_container.first=0;   m_sec_container.second = "";
}
   
   
Muon::MuonRdoContainerManager::~MuonRdoContainerManager()
{
    m_incSvc->removeListener(this , IncidentType::BeginEvent);
    m_incSvc->removeListener(this , IncidentType::EndRun);
}
 

StatusCode 
Muon::MuonRdoContainerManager::queryInterface(const InterfaceID& riid, 
                                              void** ppvInterface)
{
    if ( IIncidentListener::interfaceID() == riid )
    {
        *ppvInterface = (IIncidentListener*)this;
    }
    else  
    {
        *ppvInterface = NULL;
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}


void 
Muon::MuonRdoContainerManager::handle(const Incident& incident)
{
    /** this should be done before any StioreGate->record() or StoreGate->retrieve() */
    m_activeStore->setStore( m_evtstore );

    if (incident.type() == IncidentType::EndRun)
    {
        if(m_pad_container.first) m_pad_container.first->cleanup();
	if(m_csm_container.first) m_csm_container.first->cleanup();
	if(m_tgc_container.first) m_tgc_container.first->cleanup();
	if(m_csc_container.first) m_csc_container.first->cleanup();
    }
    
    
    // Check if BeginEvent is fired
    if (incident.type() != IncidentType::BeginEvent) return;
    

    // cleanup and record the RpcPadContainer
    RpcPadContainer* pad = m_pad_container.first;
    if(pad!=0)
    {
        if(pad->numberOfCollections()!=0) pad->cleanup();
	if(!m_evtstore->transientContains<RpcPadContainer>(m_pad_container.second))
	{
	    if(m_evtstore->record(pad,m_pad_container.second).isFailure())
	    {
	        throw GaudiException("Cannot record RpcPadContainer",
	              "MuonRdoContainerManager::handle",StatusCode::FAILURE);
	    }
	} //else pad->addRef();}
    } 
       
    // record the RpcSectorLogicContainer
    RpcSectorLogicContainer* sec = m_sec_container.first;
    if(sec!=0)
    {
        //if(sec->numberOfCollections()!=0) sec->cleanup();
	if(!m_evtstore->transientContains<RpcSectorLogicContainer>(m_sec_container.second))
	{
	    
	    try{
	        sec = new RpcSectorLogicContainer();
            } catch(const std::bad_alloc&) {
	        sec = 0;
	    }
	    
	    m_sec_container.first = sec;
	    
	    if(sec && m_evtstore->record(sec,m_sec_container.second).isFailure())
	    {
	        throw GaudiException("Cannot record RpcSectorLogicContainer",
	              "MuonRdoContainerManager::handle",StatusCode::FAILURE);
	    }
	} //else pad->addRef();}
    } 
    
    // cleanup and record the MdtCsmContainer
    MdtCsmContainer* csm = m_csm_container.first;
    if(csm!=0)
    {
        if(csm->numberOfCollections()!=0) csm->cleanup();
	if(!m_evtstore->transientContains<MdtCsmContainer>(m_csm_container.second))
	{
	    if(m_evtstore->record(csm,m_csm_container.second).isFailure())
	    {
	        throw GaudiException("Cannot record MdtCsmContainer",
	              "MuonRdoContainerManager::handle",StatusCode::FAILURE);
	    }
	} //else csm->addRef();
    }
    
    // cleanup and record the TgcRdoContainer
    TgcRdoContainer* tgc = m_tgc_container.first;
    if(tgc!=0)
    {
        if(tgc->numberOfCollections()!=0) tgc->cleanup();
	if(!m_evtstore->transientContains<TgcRdoContainer>(m_tgc_container.second))
	{
	    if(m_evtstore->record(tgc,m_tgc_container.second).isFailure())
	    {
	        throw GaudiException("Cannot record TgcRdoContainer",
	              "MuonRdoContainerManager::handle",StatusCode::FAILURE);
	    }
	} //else tgc->addRef();
    }
    
    // cleanup and record the TgcRdoContainer
    CscRawDataContainer* csc = m_csc_container.first;
    if(csc!=0)
    {
        if(csc->numberOfCollections()!=0) csc->cleanup();
	if(!m_evtstore->transientContains<CscRawDataContainer>(m_csc_container.second))
	{
	    if(m_evtstore->record(csc,m_csc_container.second).isFailure())
	    {
	        throw GaudiException("Cannot record CscRawDataContainer",
	              "MuonRdoContainerManager::handle",StatusCode::FAILURE);
	    }
	} //else csc->addRef();
    }
}


unsigned long 
Muon::MuonRdoContainerManager::addRef()   
{
    m_refCount++;
    return m_refCount;
}


unsigned long 
Muon::MuonRdoContainerManager::release()
{
    unsigned long count = --m_refCount;
    if( count == 0) {
        delete this;
    }
    return count;
}
