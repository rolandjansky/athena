/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./HLTChainLoader.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"


#include "./DBHelper.h"

using namespace std;

bool
TrigConf::HLTChainLoader::load( HLTFrame& frame ) {
   m_smk=frame.smk();

   m_schemaversion = triggerDBSchemaVersion();
   TRG_MSG_INFO("Loading HLT chains");

   HLTChainList& chains = frame.theHLTChainList();

   try {
      startSession ();
      loadChains( chains );
      loadGroups( chains );
      loadTypes( chains );
      loadStreams( chains );
      loadSignatures( chains );
      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      m_session.transaction().rollback();
      throw;
   }

   chains.setEFLowerChainCounter();

   return true;
}


void
TrigConf::HLTChainLoader::loadChains( HLTChainList& chainlist ) {

   TRG_MSG_INFO("Loading chains with SMK " << m_smk);

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );

   //Set the tables that are used
   q->addToTableList ( "SUPER_MASTER_TABLE",    "SM"    );
   q->addToTableList ( "HLT_MASTER_TABLE",      "HM"    );
   q->addToTableList ( "HLT_TRIGGER_MENU",      "TM"    );
   q->addToTableList ( "HLT_TM_TO_TC",          "TM2TC" );
   q->addToTableList ( "HLT_TRIGGER_CHAIN",     "TC"    );

   //Bind list
   coral::AttributeList bindList;
   bindList.extend<int>("smid");
   bindList[0].data<int>() = (int)m_smk;

   string theCondition = "";
   theCondition += string( " SM.SMT_ID     = :smid");
   theCondition += string( " AND HM.HMT_ID = SM.SMT_HLT_MASTER_TABLE_ID"        );
   theCondition += string( " AND TM.HTM_ID = HM.HMT_TRIGGER_MENU_ID"            );
   theCondition += string( " AND TM.HTM_ID = TM2TC.HTM2TC_TRIGGER_MENU_ID "     );
   theCondition += string( " AND TC.HTC_ID = TM2TC.HTM2TC_TRIGGER_CHAIN_ID"     );

   q->setCondition( theCondition, bindList );

   //Output data and types
   coral::AttributeList attList;
   attList.extend<long>  ( "TC.HTC_ID"               );
   attList.extend<string>( "TC.HTC_NAME"             );
   attList.extend<string>( "TC.HTC_LOWER_CHAIN_NAME" );
   if(isRun1()) {
      attList.extend<string>( "TC.HTC_L2_OR_EF"         );
      attList.extend<string>( "TC.HTC_RERUN_PRESCALE"   );
   }
   attList.extend<int>   ( "TC.HTC_CHAIN_COUNTER"    );
   attList.extend<int>   ( "TC.HTC_VERSION"          );
   fillQuery(q.get(),attList);

   // the ordering
   string theOrder = "";
   if(isRun1()) {
      theOrder += "  TC.HTC_L2_OR_EF DESC, ";
   }
   theOrder += " TC.HTC_CHAIN_COUNTER ASC";
   q->addToOrderList( theOrder );

   // process the query
   q->setDistinct();
   coral::ICursor& cursor = q->execute();

   while ( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      
      long chainId                         = row["TC.HTC_ID"].data<long>();
      int counter                          = row["TC.HTC_CHAIN_COUNTER"].data<int>();  
      int version                          = row["TC.HTC_VERSION"].data<int>();  
      string name                          = rmtilde(row["TC.HTC_NAME"].data<string>());
      string lower_chain_name              = rmtilde(row["TC.HTC_LOWER_CHAIN_NAME"].data<string>());

      // level
      string level = "HLT";
      if(isRun1()) {
         level = rmtilde(row["TC.HTC_L2_OR_EF"].data<string>());
         if(level=="HL") level="HLT";
      }

      HLTChain* ch = new HLTChain( name, counter, version, level, lower_chain_name, 0, std::vector<HLTSignature*>() );

      // rerun ps from the chain table only in run 1
      if(isRun1()) {
         float rerunps = 0;
         string rerunps_s = rmtilde(row["TC.HTC_RERUN_PRESCALE"].data<string>());
         rerunps = boost::lexical_cast<float,string>(rerunps_s);
         if(rerunps>=0)
            ch->set_rerun_prescale(rerunps);
      }

      ch->setId(chainId);
      chainlist.addHLTChain(ch);
   }
   cursor.close();

   TRG_MSG_INFO("Loaded " << chainlist.size() << " chains");

}


namespace {
   void
   defineChainSubQuery( coral::IQuery* q, std::string& theCondition, coral::AttributeList& bindings, int smk, coral::AttributeList& output) {

      q->addToTableList ( "SUPER_MASTER_TABLE",    "SM"    );
      q->addToTableList ( "HLT_MASTER_TABLE",      "HM"    );
      q->addToTableList ( "HLT_TM_TO_TC",          "TM2TC" );
      q->addToTableList ( "HLT_TRIGGER_CHAIN",     "TC"    );

      theCondition += string( " SM.SMT_ID = :smid" );
      theCondition += string( " AND SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID" );
      theCondition += string( " AND HM.HMT_TRIGGER_MENU_ID = TM2TC.HTM2TC_TRIGGER_MENU_ID " );
      theCondition += string( " AND TC.HTC_ID = TM2TC.HTM2TC_TRIGGER_CHAIN_ID" );

      bindings.extend<int>("smid");
      bindings[0].data<int>() = smk;

      output.extend<string>( "TC.HTC_NAME" );
   }
}



void
TrigConf::HLTChainLoader::loadGroups( HLTChainList& chainlist ) {

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   string theCondition("");
   coral::AttributeList bindings, output;

   defineChainSubQuery( q.get(), theCondition, bindings, m_smk, output);

   q->addToTableList ( "HLT_TRIGGER_GROUP",     "GR"    );
   
   theCondition += string( " AND GR.HTG_TRIGGER_CHAIN_ID = TM2TC.HTM2TC_TRIGGER_CHAIN_ID" );

   q->setCondition( theCondition, bindings );

   output.extend<string>( "GR.HTG_NAME"   );

   fillQuery(q.get(), output);
   
   q->setDistinct();
   coral::ICursor& cursor = q->execute();

   uint count(0);
   while ( cursor.next() ) {
      ++count;
      const coral::AttributeList& row = cursor.currentRow();
      string name                          = rmtilde(row["TC.HTC_NAME"].data<string>());
      string grname                        = rmtilde(row["GR.HTG_NAME"].data<string>());
      chainlist.chain(name)->addGroup(grname);
   }
   cursor.close();

   TRG_MSG_INFO("Loaded " << count << " groups");
   
}




void
TrigConf::HLTChainLoader::loadTypes( HLTChainList& chainlist ) {

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   string theCondition("");
   coral::AttributeList bindings, output;

   defineChainSubQuery( q.get(), theCondition, bindings, m_smk, output);

   q->addToTableList ( "HLT_TRIGGER_TYPE",   "TT" );
   
   theCondition += string( " AND TT.HTT_TRIGGER_CHAIN_ID = TM2TC.HTM2TC_TRIGGER_CHAIN_ID" );

   q->setCondition( theCondition, bindings );

   output.extend<int>( "TT.HTT_TYPEBIT" );

   fillQuery(q.get(), output);
   
   q->setDistinct();
   coral::ICursor& cursor = q->execute();

   uint count(0);
   while ( cursor.next() ) {
      ++count;
      const coral::AttributeList& row = cursor.currentRow();
      string name                          = rmtilde(row["TC.HTC_NAME"].data<string>());
      int    triggertype                   = row["TT.HTT_TYPEBIT"].data<int>();
      chainlist.chain(name)->triggerTypeList().push_back(new HLTTriggerType(triggertype));
   }
   cursor.close();

   TRG_MSG_INFO("Loaded " << count << " trigger types");
}


void
TrigConf::HLTChainLoader::loadStreams( HLTChainList& chainlist ) {

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   string theCondition("");
   coral::AttributeList bindings, output;

   defineChainSubQuery( q.get(), theCondition, bindings, m_smk, output);

   q->addToTableList ( "HLT_TC_TO_TR",          "TC2TR" );
   q->addToTableList ( "HLT_TRIGGER_STREAM",    "TR"    );
   
   theCondition += string( " AND TC2TR.HTC2TR_TRIGGER_CHAIN_ID = TC.HTC_ID" );
   theCondition += string( " AND TC2TR.HTC2TR_TRIGGER_STREAM_ID = TR.HTR_ID" );

   output.extend<string>( "TC2TR.HTC2TR_TRIGGER_STREAM_PRESCALE");
   output.extend<string>( "TR.HTR_NAME" );
   output.extend<string>( "TR.HTR_TYPE" );
   output.extend<int>( "TR.HTR_OBEYLB" );

   fillQuery(q.get(), output);

   q->setCondition( theCondition, bindings );

   q->setDistinct();
   coral::ICursor& cursor = q->execute();

   uint count(0);
   while ( cursor.next() ) {
      ++count;
      const coral::AttributeList& row = cursor.currentRow();
      string chainname    = rmtilde(row["TC.HTC_NAME"].data<string>());
      string prescale_str = rmtilde(row["TC2TR.HTC2TR_TRIGGER_STREAM_PRESCALE"].data<string>());
      string streamname   = rmtilde(row["TR.HTR_NAME"].data<string>());
      string type         = rmtilde(row["TR.HTR_TYPE"].data<string>());
      bool obeyLB         = row["TR.HTR_OBEYLB"].data<int>();
      int prescale = 1;
      try {
         prescale = boost::lexical_cast<int,string>(prescale_str);
      }
      catch(boost::bad_lexical_cast & e) {}
      chainlist.chain(chainname)->addStream( new HLTStreamTag(streamname, type, obeyLB, prescale) );
   }
   cursor.close();

   TRG_MSG_INFO("Loaded " << count << " streams");
}


void
TrigConf::HLTChainLoader::loadSignatures( HLTChainList& chainlist ) {

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   string theCondition("");
   coral::AttributeList bindings, output;

   defineChainSubQuery( q.get(), theCondition, bindings, m_smk, output);

   q->addToTableList ( "HLT_TC_TO_TS",          "TC2TS" );
   q->addToTableList ( "HLT_TRIGGER_SIGNATURE", "TS"    );
   q->addToTableList ( "HLT_TS_TO_TE",          "TS2TE" );
   q->addToTableList ( "HLT_TRIGGER_ELEMENT",   "TE"    );


   theCondition += string( " AND TC2TS.HTC2TS_TRIGGER_CHAIN_ID = TC.HTC_ID" );
   theCondition += string( " AND TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID = TS.HTS_ID" );
   theCondition += string( " AND TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID = TS2TE.HTS2TE_TRIGGER_SIGNATURE_ID" );
   theCondition += string( " AND TE.HTE_ID = TS2TE.HTS2TE_TRIGGER_ELEMENT_ID" );
   theCondition += string( " AND TS2TE.HTS2TE_ELEMENT_COUNTER>=0" ); // counter<0 are for TE's that are only input TEs for a menu

   output.extend<int>( "TC2TS.HTC2TS_SIGNATURE_COUNTER" );
   output.extend<int>( "TS2TE.HTS2TE_ELEMENT_COUNTER" );
   output.extend<int>( "TS.HTS_LOGIC" );
   output.extend<int>( "TE.HTE_ID" );
   output.extend<string>( "TE.HTE_NAME" );
   fillQuery(q.get(), output);

   q->setCondition( theCondition, bindings );

   q->setDistinct();

   q->addToOrderList( "TC.HTC_NAME" );
   q->addToOrderList( "TC2TS.HTC2TS_SIGNATURE_COUNTER" );
   q->addToOrderList( "TS2TE.HTS2TE_ELEMENT_COUNTER" );

   coral::ICursor& cursor = q->execute();

   uint count(0);
   while ( cursor.next() ) {
      ++count;
      
      const coral::AttributeList& row = cursor.currentRow();
      
      string chainname    = rmtilde(row["TC.HTC_NAME"].data<string>());
      vector<HLTSignature*>& sig_list = chainlist.chain(chainname)->signatureList();
      
      unsigned int sig_counter = (unsigned int)row["TC2TS.HTC2TS_SIGNATURE_COUNTER"].data<int>();
      if( sig_list.size() < sig_counter+1 )
         sig_list.resize( sig_counter+1, 0 );

      if( sig_list[sig_counter] == 0) {
         int logic = row["TS.HTS_LOGIC"].data<int>();
         sig_list[sig_counter] = new HLTSignature(sig_counter, logic, std::vector<HLTTriggerElement*>());
         sig_list[sig_counter]->set_label( chainname + "_" + boost::lexical_cast<string,int>(sig_counter) );
      }

      HLTSignature* sig = sig_list[sig_counter];
      vector<HLTTriggerElement*>& te_list = sig->outputTEs();

      unsigned int te_counter  = (unsigned int)row["TS2TE.HTS2TE_ELEMENT_COUNTER"].data<int>();
      if( te_list.size() < te_counter+1 )
         te_list.resize( te_counter+1, 0 );

      if( te_list[te_counter] == 0) {
         int te_id = row["TE.HTE_ID"].data<int>();
         string te_name = row["TE.HTE_NAME"].data<string>(); 
         te_list[te_counter] = new HLTTriggerElement(te_id, te_name);
      }

   }
   cursor.close();

   TRG_MSG_INFO("Loaded " << count << " signatures");

   // remove 0 pointers from signature list of each chain to keep old
   // behavior, would be nicer if 0's were kept and step could be
   // accessed by index (to be seen)
   for(HLTChain* ch : chainlist) {
      vector<HLTSignature*>& s = ch->signatureList();
      s.erase(remove(s.begin(), s.end(), (HLTSignature*)0), s.end());
   }

}
