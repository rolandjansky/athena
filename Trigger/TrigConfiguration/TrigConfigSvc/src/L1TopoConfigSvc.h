/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFIGSVC_L1TOPOCONFIGSVC
#define TRIGCONFIGSVC_L1TOPOCONFIGSVC

#include "ConfigSvcBase.h"
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "CxxUtils/checker_macros.h"

#include <memory>

namespace TXC {
   class L1TopoMenu;
}


namespace TrigConf {
   class L1TopoConfigSvc : public extends1< ConfigSvcBase, IL1TopoConfigSvc>
   {
   public:
      L1TopoConfigSvc(const std::string& name, ISvcLocator* pSvcLocator);

      virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;

      virtual const TXC::L1TopoMenu* menu() const override { return m_menu.get(); }

   private:
   
      std::unique_ptr<TXC::L1TopoMenu> m_menu;

   };

}

#endif
