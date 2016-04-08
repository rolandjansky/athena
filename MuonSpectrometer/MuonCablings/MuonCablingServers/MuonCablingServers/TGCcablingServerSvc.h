/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCcablingServerSvc_H
#define TGCcablingServerSvc_H

#ifndef LVL1_STANDALONE

#include "AthenaBaseComps/AthService.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

class ITagInfoMgr;


class TGCcablingServerSvc : public AthService, 
                            virtual public ITGCcablingServerSvc
{
   private:
  //    mutable bool m_atlas;
    mutable BooleanProperty m_atlas;
    StoreGateSvc* m_pDetStore;        // The Transient Detector Store Service
    ITagInfoMgr*  m_tagInfoMgr;                   // Tag Info Manager
    mutable bool  m_tagsCompared;

    bool m_forcedUse;
    bool m_useMuonTGC_CablingSvc;

    public:
    // Constructor and other Service methods
    TGCcablingServerSvc(const std::string& name, ISvcLocator* svc);
    virtual ~TGCcablingServerSvc() {}
  
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

    // Interface implementation
    virtual StatusCode giveCabling( const ITGCcablingSvc*&) const;
    virtual bool isAtlas(void) const;
    virtual bool isConfigured(void) const;

    virtual StatusCode compareTags(IOVSVC_CALLBACK_ARGS);
};
 
#endif

#endif  //TGCcablingServerSvc_H
