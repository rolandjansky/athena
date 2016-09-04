/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MCKLoader
#define TrigConf_MCKLoader

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfStorage/DBLoader.h"


namespace TrigConf {

    class MCKLoader : public DBLoader
    {
    public:
        MCKLoader(StorageMgr& sm);
        virtual ~MCKLoader();

        bool loadMCKlinkedToSMK(unsigned int smk, unsigned int & mck);        
        bool loadReleaseLinkedToMCK(unsigned int mck, std::string & mck_release);

    };

}
#endif
