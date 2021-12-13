/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./HLTFrameLoader.h"
#include "./HLTChainLoader.h"
#include "./HLTSequenceLoader.h"
#include "./HLTPrescaleSetCollectionLoader.h"
#include "./DBHelper.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTUtils.h"


#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

bool
TrigConf::HLTFrameLoader::load( HLTFrame& frame ) {

   unsigned int smk = frame.smk();
   if(smk==0) return false;

   TRG_MSG_INFO("Loading HLT menu for SMK " << smk);

   typedef pair<unsigned int, unsigned int> lbnpsk_t;
   vector<lbnpsk_t> reqested_prescale_keys = frame.getPrescaleSetCollection().prescale_keys();
   
   // always start a clean HLTFrame
   frame.clear();

   // set the keys
   frame.setSMK(smk);

   try {
      set<unsigned int> connected_psk = GetConnectedPSKs(smk);

      loadSMT(frame);

      HLTSequenceLoader seqldr(m_storageMgr, m_session);
      seqldr.setLevel(outputLevel());
      seqldr.load(frame);

      HLTChainLoader chldr(m_storageMgr, m_session);
      chldr.setLevel(outputLevel());
      chldr.load(frame);

      // prescale set collection
      bool all_connected(true);
      TRG_MSG_INFO("Requested to load prescale sets: ");
      for( lbnpsk_t lbnpsk: reqested_prescale_keys) {
         unsigned int psk = lbnpsk.second;
         bool is_connected = psk!=0 && connected_psk.find(psk)!=connected_psk.end();
         TRG_MSG_INFO("LB " << setw(3) << right << lbnpsk.first << " : " << lbnpsk.second);
         if(!is_connected) {
            TRG_MSG_ERROR("prescale key " << lbnpsk.second << " is not connected to SMK");
            all_connected = false;
         }
      }

      if(all_connected) {
         frame.thePrescaleSetCollection().set_prescale_keys_to_load( reqested_prescale_keys );

         HLTPrescaleSetCollectionLoader pssldr(m_storageMgr, m_session);
         pssldr.setLevel(outputLevel());
         //pssldr.setLevel(MSGTC::DEBUG);
         pssldr.load(frame.thePrescaleSetCollection());

         

         // by default assign the single prescale set
         if(frame.getPrescaleSetCollection().size()==1) {
            HLTPrescaleSet* singlePrescaleSet(frame.getPrescaleSetCollection().sets().begin()->pss);
            // for backward compatibility we first copy the unconditional chain rerun prescale to the prescale set
            for(HLTChain* ch: frame.theHLTChainList()) {
               pair<bool, float> rrps = ch->rerun_prescale("");
               if(rrps.first) {
                  singlePrescaleSet->thePrescale(ch->chain_counter(), ch->level_enum()).setRerunPrescale("",rrps.second);
               }
            }
            frame.theHLTChainList().applyPrescaleSet(singlePrescaleSet);
         }

      } else {
         TRG_MSG_ERROR("Not all requested HLT prescale keys are connected to the SMK");
         stringstream s;
         for(int psk: connected_psk) {
            s << psk << ", ";
         }
         TRG_MSG_INFO("these HLT PSK are connected to SMK " << smk << ": " << s.str());
         throw runtime_error("HLTFrameLoader: not all requested HLT prescale keys are connected to the SMK");
      }

      //merge chainlist
      if( frame.mergedHLT() ) {
        bool needMerging(false);
        for( const TrigConf::HLTChain* chain : frame.getHLTChainList() ) {
          if(chain->level_enum() != TrigConf::HLT)
            needMerging = true;
        }
        
        if(needMerging) {
          TRG_MSG_DEBUG("Call merging of " << frame.getHLTChainList().size() << " L2 and EF chains");
          HLTTEUtils::mergeHLTChainList2(frame);
        }
      }
      
      //frame.print();
   }
   catch (const std::exception& e) {
      TRG_MSG_ERROR("exception " << e.what());
      throw;
   }



   return true;
}


// load SM name, version, and description for SMK
void
TrigConf::HLTFrameLoader::loadSMT(HLTFrame& frame) {
   startSession();
   unique_ptr< coral::IQuery > q( m_session.nominalSchema().tableHandle( "SUPER_MASTER_TABLE").newQuery() );
      
   std::string cond = "SMT_ID = :smId";
   coral::AttributeList bindings;
   bindings.extend<int>("smId");
   bindings[0].data<int>() = frame.smk();
   q->setCondition( cond, bindings );

   coral::AttributeList attList;
   attList.extend<std::string>( "SMT_NAME" );
   attList.extend<int>        ( "SMT_VERSION" );
   attList.extend<std::string>( "SMT_COMMENT" );
   fillQuery(q.get(), attList);


   coral::ICursor& cursor = q->execute();
   while (cursor.next()) {
      const coral::AttributeList& row = cursor.currentRow();
      string name    = row["SMT_NAME"   ].data<std::string>();
      int    version = row["SMT_VERSION"].data<int>();
      string comment = row["SMT_COMMENT"].data<std::string>();
      
      frame.setName( name );
      frame.setComment( comment );
      frame.setVersion( version );
   }
   commitSession();
}


set<unsigned int>
TrigConf::HLTFrameLoader::GetConnectedPSKs(unsigned int smk) {
   set<unsigned int> connected_psks;

   startSession();
   unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );

   q->addToTableList( "SUPER_MASTER_TABLE",    "SMT"    );
   q->addToTableList( "HLT_MASTER_TABLE",      "HMT"    );
   q->addToTableList( "HLT_TRIGGER_MENU",      "HTM"    );
   q->addToTableList( "HLT_TM_TO_PS",          "HM2PS"  );
      
   coral::AttributeList bindings;
   bindings.extend<int>("smid");
   bindings[0].data<int>() = smk;

   string theCondition("");
   theCondition += string( " SMT.SMT_ID     = :smid");
   theCondition += string( " AND SMT.SMT_HLT_MASTER_TABLE_ID = HMT.HMT_ID"       );
   theCondition += string( " AND HMT.HMT_TRIGGER_MENU_ID = HTM.HTM_ID"           );
   theCondition += string( " AND HTM.HTM_ID = HM2PS.HTM2PS_TRIGGER_MENU_ID"      );


   q->setCondition( theCondition, bindings );

   //Don't really want anything back, just want to see there is one entry
   coral::AttributeList attList;
   attList.extend<int>( "HM2PS.HTM2PS_PRESCALE_SET_ID" );
   fillQuery(q.get(),attList);

   coral::ICursor& cursor = q->execute();

   while (cursor.next()) {
      const coral::AttributeList& row = cursor.currentRow();
      int psk = row["HM2PS.HTM2PS_PRESCALE_SET_ID"].data<int>();
      connected_psks.insert(psk);
   }

   commitSession();

   stringstream ss;
   for(int psk : connected_psks)
       ss << psk << ", ";

   TRG_MSG_INFO( "To SMK " << smk << " connected HLT prescale keys are " << ss.str() );

   return connected_psks;
}

