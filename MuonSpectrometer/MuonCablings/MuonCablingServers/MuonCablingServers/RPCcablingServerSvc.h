/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCcablingServerSvc_H
#define RPCcablingServerSvc_H

#ifndef LVL1_STANDALONE

#include "AthenaBaseComps/AthService.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include <atomic>


class RPCcablingServerSvc : public AthService, 
                            virtual public IRPCcablingServerSvc,
                            virtual public ITagInfoMgr::Listener
{
    private:
    BooleanProperty m_atlas;
    mutable std::atomic_bool m_tagsCompared;
    bool m_forcedUse;
    BooleanProperty m_useMuonRPC_CablingSvc;

    ITagInfoMgr*  m_tagInfoMgr;                   // Tag Info Manager
    

    public:
    // Constructor and other Service methods
    RPCcablingServerSvc(const std::string& name, ISvcLocator* svc);
    virtual ~RPCcablingServerSvc()=default;
  
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF) override;
    virtual StatusCode initialize(void) override;

    // Interface implementation
    virtual StatusCode giveCabling(const IRPCcablingSvc*&) const override;
    virtual bool isAtlas(void) const override;
    // ITagInfoMgr callback
    virtual void tagInfoUpdated() override final { compareTags().ignore(); }

    bool isConfigured(void) const {return m_tagsCompared;}
    StatusCode compareTags();

};
 
#endif

#endif  //RPCcablingServerSvc_H
