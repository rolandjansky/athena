/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTSequenceLoader
#define TrigConf_HLTSequenceLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {

   class HLTFrame;
   class HLTSequenceList;
  
   /**@brief TriggerDB loader of the HLT signatures*/
   class HLTSequenceLoader : public DBLoader, virtual public ILoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       * @param session reference to the database session
       */
      HLTSequenceLoader( StorageMgr& sm,  coral::ISessionProxy& session) :
         DBLoader("HLTSequenceLoader", sm, session) {}

      /**@brief destructor*/       
      virtual ~HLTSequenceLoader() override = default;

      void loadSequences( HLTSequenceList& seqlist );

      virtual bool load( HLTFrame& frame );

   private:
      unsigned int m_smk{0};
      unsigned int m_schemaversion{0};
   };
}

#endif
