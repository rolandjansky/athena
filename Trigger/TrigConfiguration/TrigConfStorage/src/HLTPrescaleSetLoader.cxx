/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./HLTPrescaleSetLoader.h"

#include "./DBHelper.h"

#include "TrigConfHLTData/HLTPrescaleSet.h"

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
#include <memory>

#include "boost/lexical_cast.hpp"

#include <sys/time.h>

using namespace std;

namespace {
   //Need to convert any tildas to empty strings
  template <class T>
  bool from_string(T& t,const std::string& s,std::ios_base& (*f)(std::ios_base&))
  {
    std::istringstream iss(s);
    return !(iss >> f >> t).fail();
  }
  bool convert(const string & prescale_s, float& prescale_f){
    return from_string<float>(prescale_f, std::string(prescale_s), std::dec);
  } 
  std::string CheckTilda(const std::string& input){
    return (input=="~")?"":input;
  }
}


bool
TrigConf::HLTPrescaleSetLoader::load( HLTPrescaleSet& hltpss ) {

   using std::string;
   
   TRG_MSG_INFO("Started loading data with HLT PSK: " << hltpss.id());

   //Find if old or new schema is being used:
   try {

      triggerDBSchemaVersion();

      startSession();

      unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );

      //Set the tables that are used
      q->addToTableList ( "HLT_PRESCALE_SET", "PS" );
      q->addToTableList ( "HLT_PRESCALE",     "PR" );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("psid");
      bindList[0].data<int>() = (int)hltpss.id();

      // selection condition
      string theCondition = " PS.HPS_ID = :psid";
      theCondition += " AND PS.HPS_ID = PR.HPR_PRESCALE_SET_ID";
      q->setCondition( theCondition, bindList );

      // output data
      coral::AttributeList attList;
      attList.extend<string>( "PS.HPS_NAME"              );
      attList.extend<int>   ( "PS.HPS_VERSION"           );
      if(isRun1()) { 
         attList.extend<string>( "PR.HPR_L2_OR_EF"       ); 
         attList.extend<string>( "PR.HPR_PASS_THROUGH_RATE" ); 
         attList.extend<string>( "PR.HPR_PRESCALE"          ); 
      } else { 
         attList.extend<float>   ( "PR.HPR_VALUE" ); 
         attList.extend<string>  ( "PR.HPR_TYPE" ); 
      } 
      attList.extend<int>   ( "PR.HPR_CHAIN_COUNTER" ); 

      fillQuery(q.get(),attList);

      // the ordering
      string theOrder = ""; 
      if(isRun1()) { 
         theOrder = " PR.HPR_L2_OR_EF DESC, PR.HPR_CHAIN_COUNTER ASC"; 
      } else { 
         theOrder += "PR.HPR_CHAIN_COUNTER ASC"; 
      } 
      q->addToOrderList( theOrder );


      q->setRowCacheSize(1000);
      q->setDistinct();

      // process the query
      coral::ICursor& cursor = q->execute();

      bool pssnameset = false;
      

      // fill chain info list
      while (cursor.next()) {
        
         const coral::AttributeList& row = cursor.currentRow();

         if(!pssnameset) {
            string pssname = CheckTilda(row["PS.HPS_NAME"].data<string>());
            int    pssversion = row["PS.HPS_VERSION"].data<int>();
            hltpss.setName(pssname);
            hltpss.setVersion(pssversion);
            pssnameset=true;
         }

         string level = isRun1() ? row["PR.HPR_L2_OR_EF"].data<string>() : "HLT";  
         int counter = row["PR.HPR_CHAIN_COUNTER"].data<int>(); 

         // this is faster than boost::lexical_cast string->float:
         float  ps(-999);
         float  pt(-999); 
         if(isRun1()) {
            std::string prescaleStr = row["PR.HPR_PRESCALE"].data<string>(); 
            if(prescaleStr=="-0" || prescaleStr=="na" ) prescaleStr = "-1"; 
            if (!convert(prescaleStr, ps)) { 
               msg() << "Could not convert prescale string '" << prescaleStr << "' of chain " 
                     << counter << " to float" << std::endl; 
            } 
	 
            std::string passThroughStr = row["PR.HPR_PASS_THROUGH_RATE"].data<string>(); 
            if (!convert(passThroughStr, pt)) { 
               msg() << "Could not convert passthrough string '" << passThroughStr << "' of chain " 
                     << counter << " to float" << std::endl; 
            } 
         } else {
            // RUN 2 
            float value = row["PR.HPR_VALUE"].data<float>(); 
            string pstype = row["PR.HPR_TYPE"].data<string>(); 
            if(pstype=="Prescale") { 
               ps = value; 
            } else if (pstype=="Pass_Through") { 
               pt = value; 
            } else if (pstype=="Rerun") { 
               //float psrr = value; // TODO 
            }          
         }

         if(level=="L2" || level=="l2" || level=="EF" || level=="ef" || level=="hlt" || level=="HLT" || level=="") {
            hltpss.thePrescale( counter, str2lvl(level) )
               .setPrescale(ps)
               .setPassThrough(pt);
         } else {
            std::string streamnametype(level);
            // sanity check: stream name and type are separated by ':'
            if(streamnametype.rfind(':')==std::string::npos)
               streamnametype += ":" + streamnametype;
         }
      }

      TRG_MSG_INFO("Loaded " << hltpss.size(L2) << " L2 prescales and " << hltpss.size(EF) << " EF prescales.");
      commitSession();
   }
   catch (const coral::SchemaException& e) {
      TRG_MSG_ERROR("HLTPrescaleSetLoader >> IRelationalException: " << e.what()); 
      m_session.transaction().rollback();
      return false;
   }
   catch (const std::exception& e) {
      TRG_MSG_ERROR("HLTPrescaleSetLoader >> Standard C++ exception: " << e.what()); 
      m_session.transaction().rollback();
      return false;
   }
   catch (...) {
      TRG_MSG_ERROR("HLTPrescaleSetLoader >> unknown C++ exception");
      m_session.transaction().rollback();
      return false;
   }

   return true;

}

