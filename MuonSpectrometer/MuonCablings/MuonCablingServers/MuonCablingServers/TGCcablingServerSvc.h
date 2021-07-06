/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    BooleanProperty m_atlas{this, "Atlas", true, "Controls whether using ATLAS cabling, or from testbeams etc"};

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
