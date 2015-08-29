/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_L1TopoMenuLoader
#define TrigConf_L1TopoMenuLoader

#include "TrigConfStorage/IL1TopoMenuLoader.h"
#include "TrigConfStorage/DBLoader.h"
#include "L1TopoConfig/L1TopoMenu.h"

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
      L1TopoMenuLoader( StorageMgr& sm,  coral::ISessionProxy& session) :
         IL1TopoMenuLoader(), DBLoader("L1TopoMenuLoader", sm, session) {}
      
      /**@brief destructor*/       
      virtual ~L1TopoMenuLoader(){};

      virtual bool load( TXC::L1TopoMenu& );

      bool loadTopoAlgos( TXC::L1TopoMenu& tcaTarget);
//       bool loadAlgInput( TXC::L1TopoConfigAlg& tcaTarget, const int& );
//       bool loadAlgOutput( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id, const int& );
//       bool loadAlgRegister( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id );
//       bool loadAlgFixed( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id );
      bool loadOutputList( TXC::L1TopoMenu& tcaTarget, const int& ctplinkid);
      bool loadTopoConfig( TXC::L1TopoMenu& tcaTarget);


      bool loadAllAlgsInput( std::map<uint, TXC::L1TopoConfigAlg> & idToAlgMap );
      bool loadAllAlgsOutput( std::map<uint, TXC::L1TopoConfigAlg> & idToAlgMap );
      bool loadAllAlgsRegister( std::map<uint, TXC::L1TopoConfigAlg> & idToAlgMap );
      bool loadAllAlgsFixed( std::map<uint, TXC::L1TopoConfigAlg> & idToAlgMap );

   };
}

#endif
