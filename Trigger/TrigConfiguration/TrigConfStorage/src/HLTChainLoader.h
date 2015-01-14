/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTChainLoader
#define TrigConf_HLTChainLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {

   class HLTFrame;
   class HLTChainList;
  
   /**@brief TriggerDB loader of the HLT signatures*/
   class HLTChainLoader : public DBLoader, virtual public ILoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       * @param session reference to the database session
       */
      HLTChainLoader( StorageMgr& sm,  coral::ISessionProxy& session) :
         DBLoader("HLTChainLoader", sm, session) {}

      /**@brief destructor*/       
      virtual ~HLTChainLoader(){};

      void loadChains( HLTChainList& chainlist );
      void loadGroups( HLTChainList& chainlist );
      void loadTypes( HLTChainList& chainlist );
      void loadStreams( HLTChainList& chainlist );
      void loadSignatures( HLTChainList& chainlist );

      virtual bool load( HLTFrame& frame );

   private:
      unsigned int m_smk;
      unsigned int m_schemaversion;
   };
}

#endif
