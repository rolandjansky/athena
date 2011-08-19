/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOCONTAINERMANAGER_H
#define MUONRDOCONTAINERMANAGER_H


#include "GaudiKernel/IIncidentListener.h"

#include <string>
#include <utility>

class MsgStream;
class ISvcLocator;
class IIncidentSvc;
class StoreGateSvc;
class ActiveStoreSvc;

class RpcPadContainer;
class MdtCsmContainer;
class TgcRdoContainer;
class CscRawDataContainer;
class RpcSectorLogicContainer;



namespace Muon
{

class MuonRdoContainerAccess;


class MuonRdoContainerManager : public IIncidentListener
{
    
    friend class MuonRdoContainerAccess;
    
    public:
 
    // Reimplements from queryInterface   
    StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    // Reimplements from IIncidentListener
    void handle(const Incident& incident);
   
    // Increment the reference count of Interface instance
    unsigned long addRef();
 
    // Release Interface instance
    unsigned long release();


    
    private:
    static MuonRdoContainerManager* s_instance;
    
    // Constructor and destructor
    MuonRdoContainerManager( ISvcLocator* svcloc, StoreGateSvc* storeGate=0 );
    ~MuonRdoContainerManager();
    
    
    ISvcLocator*  m_svcloc;
    IIncidentSvc* m_incSvc;
    StoreGateSvc* m_evtstore;
    ActiveStoreSvc* m_activeStore;
 
    unsigned long m_refCount;

    // so far simple pairs, but could become a list of pairs
    // to allow different conversion type of the same data 
    std::pair<RpcPadContainer*,std::string>          m_pad_container;
    std::pair<MdtCsmContainer*,std::string>          m_csm_container;
    std::pair<TgcRdoContainer*,std::string>          m_tgc_container;
    std::pair<CscRawDataContainer*,std::string>      m_csc_container;
    std::pair<RpcSectorLogicContainer*,std::string>  m_sec_container;
};

}

#endif //MUONRDOCONTAINERMANAGER_H
