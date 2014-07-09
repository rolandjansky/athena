/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ICaloJetInputLoader
#define TrigConf_ICaloJetInputLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfL1Data/CaloJetInput.h"

namespace TrigConf {

   class ICaloJetInputLoader : virtual public ILoader {
   public:
      virtual ~ICaloJetInputLoader() {};
      virtual bool load( CaloJetInput& data ) = 0;
   };

}

#endif
