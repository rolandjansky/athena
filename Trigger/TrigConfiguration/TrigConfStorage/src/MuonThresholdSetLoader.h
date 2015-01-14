/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     MuonThresholdSetLoader.h
//PACKAGE:  TrigConfStorage
//
//AUTHOR:   D.Berge (CERN)	berge@cern.ch 
//CREATED:  04. Aug. 2008
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_MuonThresholdSetLoader
#define TrigConf_MuonThresholdSetLoader

#include "TrigConfStorage/IMuonThresholdSetLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <vector>

namespace TrigConf {
   
   /**@brief TriggerDB loader of the LVL1 muon h/w configuration for online use */
   class MuonThresholdSetLoader : virtual public IMuonThresholdSetLoader, public DBLoader {
   public:
    
      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      MuonThresholdSetLoader( StorageMgr& sm,  coral::ISessionProxy& session)
         : ILoader(), DBLoader(sm, session) {}
      
      /**@brief destructor*/           
      virtual ~MuonThresholdSetLoader(){};

      virtual bool load( MuonThresholdSet& data);

      // retrieve all muon theshold sets for the current entries in the
      // trigger_alias table
      std::vector<TrigConf::MuonThresholdSet> loadAll();

   };
}

#endif
