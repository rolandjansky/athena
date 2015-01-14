/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloInfoLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  26. June. 2006                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_CaloInfoLoader
#define TrigConf_CaloInfoLoader 

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <vector>

namespace TrigConf {
  
   /**@brief TriggerDB loader of the LVL1 calorimeter trigger configuration*/
   class CaloInfoLoader : virtual public ICaloInfoLoader, public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      CaloInfoLoader( StorageMgr& sm,  coral::ISessionProxy& session)
         : ILoader(), DBLoader("CaloInfoLoader", sm, session) {}    

      /**@brief destructor*/       
      virtual ~CaloInfoLoader(){};

      virtual bool load( CaloInfo& data);

   private:

      void loadMinTobInfo( CaloInfo& data, const std::vector<int> & mintobIDs );

      void loadIsolationInfo( CaloInfo& data, const std::vector<int> & isoparIDs );

   };
}

#endif
