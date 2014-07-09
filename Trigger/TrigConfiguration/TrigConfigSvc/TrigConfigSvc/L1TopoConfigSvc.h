/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFIGSVC_L1TOPOCONFIGSVC
#define TRIGCONFIGSVC_L1TOPOCONFIGSVC

#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "TrigConfigSvc/ConfigSvcBase.h"

#include <memory>

namespace TXC {
   class L1TopoMenu;
}


namespace TrigConf {
   class L1TopoConfigSvc : public extends1< ConfigSvcBase, IL1TopoConfigSvc>
   {
   public:
      L1TopoConfigSvc(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~L1TopoConfigSvc();

      virtual StatusCode initialize();
      virtual StatusCode start();
      virtual StatusCode finalize();
   
      virtual const TXC::L1TopoMenu* menu() const { return m_menu.get(); }

   private:
   
      std::unique_ptr<TXC::L1TopoMenu> m_menu;

   };

}

#endif
