/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfJobOptData/JobOption.h"
#include "TrigConfJobOptData/JobOptionTable.h"

#include "./JobOptionTableLoader.h"
#include "./DBHelper.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"

#include <set>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace TrigConf;

bool
TrigConf::JobOptionTableLoader::loadHLTMasterTable(int SuperMasterKey,
                                                   int& masterTableID,
                                                   int& triggerMenuID,
                                                   int& l2SetupID,
                                                   int& efSetupID ) {

   unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );

   // tables
   q->addToTableList ( "SUPER_MASTER_TABLE", "SM" );
   q->addToTableList ( "HLT_MASTER_TABLE", "HM" );

   // Bind variables
   coral::AttributeList bindList;
   bindList.extend<int>("smid");
   bindList[0].data<int>() = SuperMasterKey;

   // the select condition
   string theCondition = "SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID AND SM.SMT_ID = :smid";

   q->setCondition( theCondition, bindList );

   // output fields
   coral::AttributeList attList;
   attList.extend<int> ( "HM.HMT_ID" );
   attList.extend<int> ( "HM.HMT_TRIGGER_MENU_ID" );
   if(isRun1()) {
      attList.extend<int> ( "HM.HMT_L2_SETUP_ID" );
      attList.extend<int> ( "HM.HMT_EF_SETUP_ID" );
   } else {
      attList.extend<int> ( "HM.HMT_SETUP_ID" );
   }
   fillQuery(q.get(),attList);

   // query
   coral::ICursor& cursor = q->execute();

   // get the HLT master-table id
   if( ! cursor.next() )
      return false;

   const coral::AttributeList& row = cursor.currentRow();

   masterTableID = row["HM.HMT_ID"].data<int>();
   triggerMenuID = row["HM.HMT_TRIGGER_MENU_ID"].data<int>();
   if(isRun1()) {
      l2SetupID     = row["HM.HMT_L2_SETUP_ID"].data<int>();
      efSetupID     = row["HM.HMT_EF_SETUP_ID"].data<int>();
   } else {
      l2SetupID = efSetupID = row["HM.HMT_SETUP_ID"].data<int>();
   }
   return true;
}


bool
TrigConf::JobOptionTableLoader::loadComponentNamesAndParameters(TrigConf::JobOptionTable& JOTable, const std::set<int>& compIDsToLoad) {

   unsigned int batchSize = 500;
   set<int>::const_iterator current = compIDsToLoad.begin();
   vector< SplitParam > splitparams;


   while(current!=compIDsToLoad.end()) {
      loadComponentNamesAndParameters(JOTable, compIDsToLoad, splitparams, current, batchSize);
   }


   TRG_MSG_INFO("Split parameters => " << splitparams.size());
   TRG_MSG_INFO("Non-split parameters => " << JOTable.jobOptionVector().size());
   unsigned int countSplitParam = assembleSplitParameters2(JOTable, splitparams);
   TRG_MSG_INFO("Assembled split parameters => " << countSplitParam);

   return true;
}

bool
JobOptionTableLoader::loadComponentNamesAndParameters(JobOptionTable& JOTable, 
                                                      const set<int>& compIDsToLoad,
                                                      vector<SplitParam>& splitparams,
                                                      set<int>::const_iterator & current,
                                                      unsigned int batchSize) {

   unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   q->setRowCacheSize( 500 );

   q->addToTableList ( "HLT_COMPONENT", "CP" );
   q->addToTableList ( "HLT_CP_TO_PA",  "CP2PA" );
   q->addToTableList ( "HLT_PARAMETER", "PA" );
   
   
   // bind
   coral::AttributeList bind;
   
   std::string cond( buildCond_IN_("CP.HCP_ID", compIDsToLoad, current, batchSize) );
   cond += " AND CP2PA.HCP2PA_COMPONENT_ID = CP.HCP_ID";
   cond += " AND CP2PA.HCP2PA_PARAMETER_ID = PA.HPA_ID";
   q->setCondition( cond , bind );

   // output and their types
   coral::AttributeList attList;
   attList.extend<std::string>( "CP.HCP_ALIAS" );
   attList.extend<std::string>( "CP.HCP_TYPE" );
   attList.extend<std::string>( "PA.HPA_NAME" );
   attList.extend<std::string>( "PA.HPA_VALUE" );
   fillQuery(q.get(),attList);
   
   coral::ICursor& cursor = q->execute();
   
   while( cursor.next() ) {
      
      const coral::AttributeList& row = cursor.currentRow();
      
      string alias = row["CP.HCP_ALIAS"].data<std::string>();
      string type  = "type"; //row["CP.HCP_TYPE"].data<std::string>();
      string name  = row["PA.HPA_NAME"].data<std::string>();
      string value = row["PA.HPA_VALUE"].data<std::string>();
      
      if( value == "~" ) value = "";
      
      //if the parameter contains __ipc__ then its split - need to merge (after finding what to merge it with!)
      if(name.find("__IPC__",0) != string::npos){
         splitparams.push_back( SplitParam(alias, name, type, "set", value) );
      } else {
         JOTable.addJobOption( JobOption( alias, name, type, "set", value) );
      }      
   }

   return true;
}


std::set<int>
TrigConf::JobOptionTableLoader::loadComponentIDsFromSetup(const int setup_id) {

   std::set<int> compIDs;

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   q->setRowCacheSize( 1000 );

   // tables
   q->addToTableList ( "HLT_ST_TO_CP",  "ST2CP" );

   // bind setup id
   coral::AttributeList bind;
   bind.extend<int>("setupid");
   bind[0].data<int>() = setup_id;
   
   std::string cond("ST2CP.HST2CP_SETUP_ID = :setupid");
   q->setCondition( cond , bind );

   // output and their types
   coral::AttributeList attList;
   attList.extend<int>( "ST2CP.HST2CP_COMPONENT_ID" );
   fillQuery(q.get(),attList);

   // execute
   coral::ICursor& cursor = q->execute();
   while( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      int id = row["ST2CP.HST2CP_COMPONENT_ID"].data<int>();
      compIDs.insert(id);
   }

   TRG_MSG_INFO("Loaded I-Components => " << compIDs.size());
   return compIDs;
}



std::set<int>
TrigConf::JobOptionTableLoader::loadComponentIDsFromMenu(const int menu_id, int triggerLevel) {

   std::set<int> compIDs;

   std::unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   q->setRowCacheSize( 1000 );

   q->addToTableList( "HLT_TM_TO_TC",      "TM2TC" );
   q->addToTableList( "HLT_TRIGGER_CHAIN", "TC"    );
   q->addToTableList( "HLT_TC_TO_TS",      "TC2TS" );
   q->addToTableList( "HLT_TS_TO_TE",      "TS2TE" );
   q->addToTableList( "HLT_TE_TO_CP",      "TE2CP" );

   // bind menu ID
   coral::AttributeList bind;
   bind.extend<int>("menuid");
   bind[0].data<int>() = menu_id;


   // condition
   std::string cond("TM2TC.HTM2TC_TRIGGER_MENU_ID = :menuid");
   cond += " AND TM2TC.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID";
   cond += " AND TM2TC.HTM2TC_TRIGGER_CHAIN_ID = TC2TS.HTC2TS_TRIGGER_CHAIN_ID";
   cond += " AND TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID = TS2TE.HTS2TE_TRIGGER_SIGNATURE_ID";
   cond += " AND TS2TE.HTS2TE_TRIGGER_ELEMENT_ID = TE2CP.HTE2CP_TRIGGER_ELEMENT_ID";

   // triggerLevel
   if(triggerLevel!=2) { // 2 means combined
      cond += " AND TC.HTC_L2_OR_EF = ";
      cond += ( triggerLevel==0 ? "'L2'" : "'EF'" );
   }

   q->setCondition( cond , bind );

   // output
   coral::AttributeList attList;
   attList.extend<int>( "TE2CP.HTE2CP_COMPONENT_ID" );
   fillQuery(q.get(),attList);

   // execute
   coral::ICursor& cursor = q->execute();
   while( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      int id = row["TE2CP.HTE2CP_COMPONENT_ID"].data<int>();
      compIDs.insert(id);
   }

   TRG_MSG_INFO("Loaded M-Components => " << compIDs.size());

   return compIDs;
}


/***********
 *
 * turns field and a set<int>=(x1,...,x2531) into a string
 *
 * (field IN (x1,x2,...,x1000) OR field IN (x1001,x1002,...,x2000) OR field IN (x2001,x2002,...,x2531))
 *
 * this is needed since ORACLE has a limit of 1000 entries within IN(...)
 ***/
std::string
TrigConf::JobOptionTableLoader::buildCond_IN_(const std::string& field, const std::set<int>& IDs, std::set<int>::const_iterator & current, unsigned int batchSize) {

   std::stringstream ss;
   ss << "(" << field << " IN (";

   unsigned int count(0), totalCount(0);
   set<int>::iterator last = IDs.end(); --last;
   for(; current!=last && totalCount!=batchSize-1; ++current) {
      totalCount++;
      ss << *current;
      if(++count==1000) {
         ss << ") OR " << field << " IN (";
         count=0;
      } else {
         ss << ",";
      }
   }
   ss << *current++ << "))";

   return ss.str();
}


namespace {
   vector<int> createSortedVector(const set<int>& compIDs) {
      vector<int> sortedVector(compIDs.size());
      copy(compIDs.begin(), compIDs.end(), sortedVector.begin());
      sort(sortedVector.begin(),sortedVector.end());
      return sortedVector;
   }

   vector<string> concatAndSplit(const vector<int>& v, uint maxStringSize) {

      // create a single long string of comma-separated numbers
      vector<string> split;

      string s("");s.reserve(8000);
      uint count(0);

      for(int i:v) {
         
         s+=to_string(i) + ",";
         count++;

         if(count<1000  &&  s.size()<maxStringSize )  // oracle allows max 1000 items in (list)
            continue;

         s.erase(s.end()-1); // erase last comma

         split.push_back( s );
         
         s="";
         count=0;
      }

      // the last entries in the loop
      if(s!="") {
         s.erase(s.end()-1); // erase last comma
         split.push_back( s );
      }
      
      return split;
   }

}

std::set<int>
TrigConf::JobOptionTableLoader::getChildCompIDs(const std::set<int>& compIDs) {

   std::set<int> childCompIDs; // the children IDs

   TRG_MSG_DEBUG("Number of parents " << compIDs.size());

   vector<int> sortedParentIDs = createSortedVector(compIDs);

   vector<string> splitParentIDs = concatAndSplit(sortedParentIDs, 7500);

   for(const string& idList : splitParentIDs) {

      // condition
      string cond = "HCP2CP_PARENT_COMP_ID IN (" + idList + ")";

      coral::ITable& table = m_session.nominalSchema().tableHandle("HLT_CP_TO_CP");
      std::unique_ptr< coral::IQuery > q( table.newQuery() );

      coral::AttributeList bind;
      q->setCondition( cond, bind );

      // output
      coral::AttributeList attList;
      attList.extend<int>( "HCP2CP_CHILD_COMP_ID" );
      fillQuery(q.get(),attList);

      // distinct
      q->setDistinct();

      // execute
      coral::ICursor& cursor = q->execute();

      // fill output
      while( cursor.next() ) {
         const coral::AttributeList& row = cursor.currentRow();
         int id = row["HCP2CP_CHILD_COMP_ID"].data<int>();
         childCompIDs.insert(id);
      }

   }

   // no more children, can stop recursion
   if(childCompIDs.size()==0)
      return childCompIDs;

   // get all children recursively
   set<int> allSubChildrenIDs = getChildCompIDs(childCompIDs);

   // add to set of all children
   childCompIDs.insert(allSubChildrenIDs.begin(),allSubChildrenIDs.end());

   return childCompIDs;
}


bool
TrigConf::JobOptionTableLoader::load( TrigConfData& data ) {

   JobOptionTable& jot = dynamic_cast<JobOptionTable&>(data);
   try {
      return load(jot);
   }
   catch( const std::exception& e ) {
      TRG_MSG_FATAL("std::exception: " << e.what());
      throw;
   }
   return false;
}


/*************
 *
 * Reassemble the split parameters - this isn't a very pretty way, probably should be rewritten
 *
 *************/
unsigned int
TrigConf::JobOptionTableLoader::assembleSplitParameters( JobOptionTable& jot, const std::vector< SplitParam >& splitparams ) {

   unsigned int paramCount = 0;

   // Loop over the vectors and match the alias and parameter names (excluding the __IPC__)
   std::string mergedvalue ="";
   std::string aliasname   ="";
   std::string paraname    ="";
   
   
   //ignore those we have already matched
   vector< SplitParam > alreadymatched_v;
   
   for (unsigned int sit=0; sit<splitparams.size(); sit++) {
      
      const SplitParam & splitpar = splitparams.at(sit);

      std::string actualname = splitpar.actualname;
      // std::cout << splitpar.alias << "    " << splitpar.name << std::endl;
         
      // see if already matched - paraname and the alias!
      bool matched = false;
      for (unsigned int mit=0; mit<alreadymatched_v.size(); mit++){ 
         SplitParam matchtemp = alreadymatched_v.at(mit);
         matched = ( matchtemp.actualname==actualname  &&  matchtemp.alias == splitpar.alias );
         if(matched) break;
      }
	

      if(!matched) {
         //have we gone on to the next name - if so, we have a merged parameter now
         if( paraname!=actualname || aliasname!=splitpar.alias ){
               
            if(aliasname!=""){
               //MAKE THE NEW JOBOPTION!
               JobOption jo( aliasname, paraname, "type", "set", mergedvalue); 
               jot.addJobOption(jo);
               paramCount++;
            }
            aliasname = splitpar.alias;
            paraname = actualname;
            mergedvalue="";
         }
            
         alreadymatched_v.push_back( splitpar );
         mergedvalue = splitpar.value;
            
         //now look for a match here
         for (unsigned int sit2=sit+1; sit2<splitparams.size(); sit2++){    
            const SplitParam& newtemp2 = splitparams.at(sit2);
            std::string actualname2 = newtemp2.actualname;
	    
            if(actualname2==actualname && splitpar.alias==newtemp2.alias ){
               mergedvalue+= newtemp2.value;
               TRG_MSG_VERBOSE("Found split parameters " << splitpar.alias << " : " <<  splitpar.name << " - " << newtemp2.name);
               TRG_MSG_VERBOSE(splitpar.value << " - " << newtemp2.value);
            }
         }
      }
   }

   if(aliasname!=""){
      //have to take care of the last one!
      //MAKE THE NEW JOBOPTION!
      JobOption jo( aliasname, paraname, "type", "set", mergedvalue);	
      jot.addJobOption(jo);
      paramCount++;
   }
   return paramCount;

}


namespace {
   class AssembledPar {
   public:
      AssembledPar(const std::string& compalias, const std::string& parname) :
         comp_alias(compalias),
         par_name(parname)
      {}
      std::string comp_alias;
      std::string par_name;
   };

   struct APlessthan {
      bool operator()(const AssembledPar& p1, const AssembledPar& p2) const {
         int alias_cmp = p1.comp_alias.compare(p2.comp_alias);
         if(alias_cmp>0) return false;
         if(alias_cmp<0) return true;
         return p1.par_name.compare(p2.par_name)<0;
      }
   };
}

unsigned int
TrigConf::JobOptionTableLoader::assembleSplitParameters2( JobOptionTable& jot, const std::vector< SplitParam >& splitparams ) {

   std::map<AssembledPar, std::vector<std::string>, APlessthan > assembled_params;

   boost::regex pname_exp("(.*)__IPC__(\\d+)");
   boost::cmatch matches;

   //   for(unsigned int i=0; i<splitparams.size(); ++i) {
   for(const SplitParam & splitpar: splitparams ) {
      //const SplitParam & splitpar = splitparams[i];

      if(boost::regex_match(splitpar.name.c_str(), matches, pname_exp)) {

         const std::string& comp_alias = splitpar.alias;
         std::string par_name(matches[1].first, matches[1].second);
         unsigned int pos = boost::lexical_cast<unsigned int,std::string>(string(matches[2].first, matches[2].second)) - 1;  // start with __IPC__01 

         std::vector<std::string>& values = assembled_params[ AssembledPar(comp_alias, par_name) ];

         if(values.size()<=pos) values.resize(pos+1);
         values[pos]= splitpar.value;

      } else {
         std::cout << "ERROR, can't parse " << splitpar.name << std::endl;
      }

   }

   for(auto par: assembled_params) {

      const string& comp_alias = par.first.comp_alias;
      const string& par_name = par.first.par_name;
      vector<string>& values = par.second;
      stringstream ss;
      copy(values.begin(), values.end(), std::ostream_iterator<std::string>(ss));
      jot.addJobOption( JobOption( comp_alias, par_name, "type", "set", ss.str()) );
   }

   return assembled_params.size();
}


bool
TrigConf::JobOptionTableLoader::load( JobOptionTable& jot ) {

   if(jot.smk()==0) {
      TRG_MSG_ERROR("requested SMK is 0");
      return false;
   }

   triggerDBSchemaVersion();

   TRG_MSG_INFO("Start loading job options with smk " << jot.smk());

   startSession();
   
   int masterTableID(0);
   int triggerMenuID(0);
   int l2SetupID(0);
   int efSetupID(0);
   if( ! loadHLTMasterTable( jot.smk(), 
                             masterTableID, 
                             triggerMenuID, 
                             l2SetupID, 
                             efSetupID ) )
      {
         TRG_MSG_ERROR("Could not load menu and setup IDs for SMK " << jot.smk());
         return false;
      }
   
   jot.setHltMasterTableId( masterTableID );
   TRG_MSG_INFO("HLT masterkey: " << jot.hltMasterTableId());
   TRG_MSG_INFO("Menu ID      : " << triggerMenuID);
   if(l2SetupID==efSetupID) {
      TRG_MSG_INFO("Setup ID     : " << l2SetupID);
   } else {
      TRG_MSG_INFO("L2 Setup ID  : " << l2SetupID);
      TRG_MSG_INFO("EF Setup ID  : " << efSetupID);
   }

   int level = jot.triggerLevel()==0; // triggerLevel: 0 - L2, 1 - EF, 2 - HLT(combined L2+EF)
   if( l2SetupID == efSetupID || 
       (l2SetupID != 0 && efSetupID==0) )
      level=2; // combined

   int setupId = (level==0 || level==2) ? l2SetupID : efSetupID;

   std::set<int> compIDsToLoad = loadComponentIDsFromSetup(setupId);


   TRG_MSG_INFO("Direct components => " << compIDsToLoad.size());

   std::set<int> childCompIDs = getChildCompIDs( compIDsToLoad );
   
   TRG_MSG_INFO("All children => " << childCompIDs.size());

   compIDsToLoad.insert(childCompIDs.begin(), childCompIDs.end());

   TRG_MSG_INFO("All components => " << compIDsToLoad.size());

   loadComponentNamesAndParameters(jot, compIDsToLoad);

   jot.sort();

   commitSession();

   // some stats:
   TRG_MSG_INFO("JobOptionTableLoader:             Total parameters  : " << jot.jobOptionVector().size());
   
   return true;        
}


