/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTFrameLoader
#define TrigConf_HLTFrameLoader 

#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/DBLoader.h"

#include <set>

namespace TrigConf {

   /**@brief TriggerDB loader of the HLT menu*/
   class HLTFrameLoader : public IHLTFrameLoader, public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      HLTFrameLoader( StorageMgr& sm,  coral::ISessionProxy& session) :
         IHLTFrameLoader(), DBLoader("HLTFrameLoader", sm, session) {}

      /**@brief destructor*/       
      virtual ~HLTFrameLoader(){};

      virtual bool load( HLTFrame& data );

   private:

      void loadSMT(HLTFrame& frame);

      std::set<unsigned int> GetConnectedPSKs(unsigned int smk);

   };
}

#endif
