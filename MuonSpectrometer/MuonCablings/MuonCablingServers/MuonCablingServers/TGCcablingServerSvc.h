/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCcablingServerSvc_H
#define TGCcablingServerSvc_H

#ifndef LVL1_STANDALONE

#include "AthenaBaseComps/AthService.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

class TGCcablingServerSvc : public AthService, 
                            virtual public ITGCcablingServerSvc
{
   private:
    BooleanProperty m_atlas;

    bool m_forcedUse;
    bool m_useMuonTGC_CablingSvc;

    public:
    // Constructor and other Service methods
    TGCcablingServerSvc(const std::string& name, ISvcLocator* svc);
    virtual ~TGCcablingServerSvc()=default;
  
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    // Interface implementation
    virtual StatusCode giveCabling( const ITGCcablingSvc*&) const;
    virtual bool isAtlas(void) const;
};
 
#endif

#endif  //TGCcablingServerSvc_H
