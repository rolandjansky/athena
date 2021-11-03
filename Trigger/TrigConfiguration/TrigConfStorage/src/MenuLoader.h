/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MenuLoader
#define TrigConf_MenuLoader

#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "TrigConfStorage/IMenuLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <string>

namespace TrigConf {
  
   class LogicExpression;

   /**@brief TriggerDB loader of the LVL1 trigger menu configuration*/
   class MenuLoader : virtual public IMenuLoader, public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       * @param session reference to the database session
       */
      MenuLoader( StorageMgr& sm, coral::ISessionProxy& session ) :
         IMenuLoader(), DBLoader("MenuLoader", sm, session) {}

      /**@brief destructor*/       
      virtual ~MenuLoader(){};
    
      virtual bool load( Menu& data);
    
      virtual void setEnv(ENV env) { m_env = env;  }
   private:
      ENV m_env{CTP};

      struct ThrInfo {
         int thrId;
         unsigned int thrPos;
         short thrMult;
         TriggerThreshold* thr;
      };

      void loadMenuAttributes(TrigConf::Menu& menu);
      void loadCaloInfo(TrigConf::Menu& menu);
      void loadThresholds(TrigConf::Menu& menu);
      void loadPIT(TrigConf::Menu& menu);
      void loadItems(TrigConf::Menu& menu);
      void loadMonitoring(TrigConf::Menu& menu);
      void createTipFromDirectThresholds(TrigConf::Menu& menu);

      /**@brief New versions for item definition in string*/
      TriggerItemNode* constructTree(const std::string& def, const std::vector<ThrInfo>&);

      TriggerItemNode* constructTree(const LogicExpression& def, const std::vector<ThrInfo>&);

   };
}

#endif
