/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MasterTableLoader
#define TrigConf_MasterTableLoader

#include "TrigConfStorage/IMasterTableLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <string>

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 trigger configuration*/
   class MasterTableLoader : virtual public IMasterTableLoader, public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      MasterTableLoader(StorageMgr& sm, coral::ISessionProxy& session) :
         IMasterTableLoader(), DBLoader(sm, session) {}

      /**@brief destructor*/       
      virtual ~MasterTableLoader(){}


      bool load(ThresholdConfig& thrcfg);
      bool load(CTPConfig& ctpc);
      bool load(Muctpi& muctpi);
      bool load(TXC::L1TopoMenu& l1topo);
   private:
      bool loadMasterKeys(int SuperMasterKey, int& Lvl1MasterKey, std::string & menuName); 

      /**@brief next run configuration key (depreciated)*/
      int mt_id_for_next_run();

   };
}

#endif 

