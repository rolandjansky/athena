/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     JobOptionTableLoader.h
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  06. Feb. 2006                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_JobOptionTableLoader
#define TrigConf_JobOptionTableLoader

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfStorage/IJobOptionTableLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <set>

namespace coral {
   class IQuery;
   class AttributeList;
}

namespace TrigConf {

   class JobOptionTable;
  
   /**@brief TriggerDB loader of the HLT job options*/
   class JobOptionTableLoader : public DBLoader,
                                virtual public IJobOptionTableLoader
   {
   public:

      using TrigConfMessaging::msg;

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      JobOptionTableLoader( StorageMgr& sm,  coral::ISessionProxy& session)
         : DBLoader("JobOptionTableLoader", sm, session)
      {}

      /**@brief destructor*/       
      virtual ~JobOptionTableLoader(){};

      virtual bool load( TrigConfData& data);

      void defineQueries();

   private:

      class SplitParam {
      public:
         SplitParam(const std::string& alias, const std::string& name,
                    const std::string& type, const std::string& op, const std::string& value) :
            alias(alias),
            name(name),
            type(type),
            op(op),
            value(value),
            actualname(name)
         {
            actualname = actualname.erase(actualname.find("__IPC__"));
         }
         std::string alias;
         std::string name;
         std::string type;
         std::string op;
         std::string value;
         std::string actualname;
      };


      bool load( JobOptionTable& jot );

      bool loadHLTMasterTable(int SuperMasterKey,
                              int& masterTableID,
                              int& triggerMenuID,
                              int& l2SetupID,
                              int& efSetupID );

      bool loadComponentNamesAndParameters(TrigConf::JobOptionTable& JOTable, const std::set<int>& compIDsToLoad);

      bool loadComponentNamesAndParameters(TrigConf::JobOptionTable& JOTable, const std::set<int>& compIDsToLoad, std::vector<SplitParam>& splitparams,
                                           std::set<int>::const_iterator & current, unsigned int batchSize);

      std::set<int> loadComponentIDsFromSetup(const int setup_id);

      std::set<int> loadComponentIDsFromMenu(const int menu_id, int triggerLevel);

      std::set<int> getChildCompIDs(const std::set<int>& compIDs);

      std::string buildCond_IN_(const std::string& field, const std::set<int>& IDs, std::set<int>::const_iterator & current, unsigned int batchSize);

      unsigned int assembleSplitParameters( JobOptionTable& jot, const std::vector< SplitParam >& splitparams );
      unsigned int assembleSplitParameters2( JobOptionTable& jot, const std::vector< SplitParam >& splitparams );

   };
}

#endif
