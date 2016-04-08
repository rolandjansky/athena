/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCcablingServerSvc_H
#define RPCcablingServerSvc_H

#ifndef LVL1_STANDALONE

#include "AthenaBaseComps/AthService.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonCablingServers/ICallBackRPCcablingServerSvc.h"

class StoreGateSvc;
class ITagInfoMgr;



class RPCcablingServerSvc : public AthService, 
                            virtual public IRPCcablingServerSvc, virtual public ICallBackRPCcablingServerSvc 
{
    private:
    mutable BooleanProperty m_atlas;
    mutable bool  m_tagsCompared;
    bool m_forcedUse;
    BooleanProperty m_useMuonRPC_CablingSvc;

    StoreGateSvc* m_pDetStore;        // The Transient Detector Store Service
    ITagInfoMgr*  m_tagInfoMgr;                   // Tag Info Manager
    

    public:
    // Constructor and other Service methods
    RPCcablingServerSvc(const std::string& name, ISvcLocator* svc);
    virtual ~RPCcablingServerSvc() {}
  
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

    // Interface implementation
    virtual StatusCode giveCabling(const IRPCcablingSvc*&) const;
    virtual bool isAtlas(void) const;
    virtual bool isConfigured(void) const {return m_tagsCompared;}

    virtual StatusCode compareTags(IOVSVC_CALLBACK_ARGS);

};
 
#endif

#endif  //RPCcablingServerSvc_H
