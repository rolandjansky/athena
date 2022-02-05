/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./HLTPrescaleSetLoader.h"
#include "./HLTPrescaleSetCollectionLoader.h"
#include "./DBHelper.h"
#include "TrigConfStorage/StorageMgr.h"

#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTPrescaleSetCollection.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "boost/lexical_cast.hpp"

using namespace std;

bool
TrigConf::HLTPrescaleSetCollectionLoader::load( HLTPrescaleSetCollection& pscoll,
                                                unsigned int requestcounter,
                                                const std::string& partition ) {
   TRG_MSG_INFO("started loading data");

   vector< pair<unsigned int, unsigned int> > lb_psk;

   //Find if old or new schema is being used:
   try {
      startSession();

      unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );

      //Set the tables that are used
      q->addToTableList ( "HLT_PRESCALE_SET_COLL", "PSC" );

      //Bind list
      coral::AttributeList bindings;
      bindings.extend<uint>("req");
      bindings.extend<string>("part");
      bindings[0].data<uint>() = requestcounter;
      bindings[1].data<string>() = partition;

      string theCondition = "HPSC_PARTITION=:part and :req>=0";

      q->setCondition( theCondition, bindings );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<int>( "PSC.HPSC_LB"  );
      attList.extend<int>( "PSC.HPSC_HLTPSK" );
      fillQuery(q.get(), attList);
      
      // the ordering
      q->addToOrderList( "PSC.HPSC_LB" );

      // process the query
      // query->setRowCacheSize(20);
      q->setDistinct();

      coral::ICursor& cursor = q->execute();

      // fill chain info list
      while (cursor.next()) {
         const coral::AttributeList& row = cursor.currentRow();
         int lb  = row["PSC.HPSC_LB"].data<int>();
         int psk = row["PSC.HPSC_HLTPSK"].data<int>();
         lb_psk.push_back( make_pair(lb, psk) );
      }

      TRG_MSG_INFO("LB_PSK map for partition " << partition);
      for(uint i=0; i< lb_psk.size(); i++) {
         TRG_MSG_INFO("For LB " << lb_psk[i].first << " onwards load psk " << lb_psk[i].second);
      }
      commitSession();
   }
   catch (const std::exception& e) {
      TRG_MSG_ERROR("HLTPrescaleSetCollectionLoader:  caught exception: " << e.what());
      throw;
   }

   pscoll.set_prescale_keys_to_load( lb_psk );

   return load(pscoll);
}



bool
TrigConf::HLTPrescaleSetCollectionLoader::load( HLTPrescaleSetCollection& psc ) {

   IHLTPrescaleSetLoader& pssloader = m_storageMgr.hltPrescaleSetLoader();
   pssloader.setLevel(outputLevel());

   bool loadsuccess = true;

   for(HLTPrescaleSetCollection::cont& psinfo: psc.sets()) {

      // already loaded?
      if(psinfo.pss!=0) continue;

      // create new prescale set and load it
      psinfo.pss = new HLTPrescaleSet();
      psinfo.pss->setId(psinfo.psk);
      bool loadpss = pssloader.load(*psinfo.pss);

      if(!loadpss) {
         TRG_MSG_ERROR("HLTPrescaleSetCollectionLoader::load(): Could not load HLT Prescales for key " << psinfo.psk);
         loadsuccess = false;
      }
   }

   return loadsuccess;
}

