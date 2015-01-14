/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     DeadTimeLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_DeadTimeLoader
#define TrigConf_DeadTimeLoader 

#include "TrigConfStorage/IDeadTimeLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
   /**@brief TriggerDB loader of the LVL1 deadtime configuration*/
   class DeadTimeLoader : virtual public IDeadTimeLoader,public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      DeadTimeLoader( StorageMgr& sm,  coral::ISessionProxy& session)
         : ILoader(), DBLoader(sm, session) {}

      /**@brief destructor*/       
      virtual ~DeadTimeLoader(){};

      virtual bool load( DeadTime& data);
   };
}

#endif
