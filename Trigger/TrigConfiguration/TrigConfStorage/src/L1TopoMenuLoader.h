/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_L1TopoMenuLoader
#define TrigConf_L1TopoMenuLoader

#include "TrigConfStorage/IL1TopoMenuLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <set>

namespace TrigConf {

   /**@brief TriggerDB loader of the HLT menu*/
   class L1TopoMenuLoader : public IL1TopoMenuLoader, public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       * @param session reference to the database session
       */
      L1TopoMenuLoader( StorageMgr& sm,  coral::ISession& session) :
         IL1TopoMenuLoader(), DBLoader(sm, session) {}
      
      /**@brief destructor*/       
      virtual ~L1TopoMenuLoader(){};

      virtual bool load( TXC::L1TopoMenu& );

   };
}

#endif
