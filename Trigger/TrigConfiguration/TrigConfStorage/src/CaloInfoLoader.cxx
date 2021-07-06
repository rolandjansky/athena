/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloInfoLoader.cpp 
//PACKAGE:  TrigConfStorage
//
//////////////////////////////////////////////////////////////////////

#include "./CaloInfoLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/CaloInfo.h"

#include "./DBHelper.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <iterator>
#include <map>

#include "boost/lexical_cast.hpp"

using namespace std;

namespace {

   string join(const vector<int> & v) {
      string joined("");
      bool first=true;
      for(int id : v) {
         if(first) { first = false; } else { joined += ","; }
         joined += boost::lexical_cast<string,int>(id);
      }
      return joined;
   }

}


bool
TrigConf::CaloInfoLoader::load( CaloInfo& data ) {
   
   if(data.smk()!=0) {
      TRG_MSG_INFO("Loading CaloInfo for SMK " << data.smk());
   } else if( data.id()!=0 ) {
      TRG_MSG_INFO("Loading CaloInfo with ID = " << data.id());
   } else {
      TRG_MSG_ERROR("Can not load CaloInfo which has no id or SMK specified");
      throw runtime_error("Can not load CaloInfo, no SMK or Id specified");
   }



   try {
      unsigned int schema = triggerDBSchemaVersion();

      startSession();

      unique_ptr<coral::IQuery> q(m_session.nominalSchema().newQuery());
      q->addToTableList( "L1_CALO_INFO", "L1CI" );
      q->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("Id");
      std::string cond("");
      if(data.smk()!=0) {
         q->addToTableList( "SUPER_MASTER_TABLE", "SMT" );
         q->addToTableList( "L1_MASTER_TABLE", "L1M" );
         cond  = "SMT.SMT_ID = :Id";
         cond += " AND SMT.SMT_L1_MASTER_TABLE_ID = L1M.L1MT_ID";
         cond += " AND L1CI.L1CI_ID = L1M.L1MT_CALO_INFO_ID";
         bindList[0].data<int>() = data.smk();
      } else {
         cond = "L1CI.L1CI_ID = :Id";
         bindList[0].data<int>() = data.id();
      }
      q->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1CI.L1CI_NAME" ); 
      attList.extend<int>( "L1CI.L1CI_VERSION" );
      if(isRun1()) {
         attList.extend<std::string>( "L1CI.L1CI_GLOBAL_SCALE" ); 
      } else {
         attList.extend<float>( "L1CI.L1CI_GLOBAL_EM_SCALE" ); 
         attList.extend<float>( "L1CI.L1CI_GLOBAL_JET_SCALE" ); 
      }
      if(isRun2() || (isRun1() && schema>=10)) {
         attList.extend<int>( "L1CI.L1CI_XS_SIGMA_SCALE" ); 
         attList.extend<int>( "L1CI.L1CI_XS_SIGMA_OFFSET" );
         attList.extend<int>( "L1CI.L1CI_XS_XE_MIN" );
         attList.extend<int>( "L1CI.L1CI_XS_XE_MAX" );
         attList.extend<int>( "L1CI.L1CI_XS_TESQRT_MIN" );
         attList.extend<int>( "L1CI.L1CI_XS_TESQRT_MAX" );
      }
      if(isRun1()) {
         for(unsigned int index = 1; index <= 12 ;index++) {
            std::stringstream helpstring;
            helpstring << "L1CI.L1CI_JET_WEIGHT" << index;
            attList.extend<int>( helpstring.str() ); 
         }
      }
      if(isRun2()) {
         attList.extend<int>( "L1CI.L1CI_MIN_TOB_EM" ); 
         attList.extend<int>( "L1CI.L1CI_MIN_TOB_TAU" ); 
         attList.extend<int>( "L1CI.L1CI_MIN_TOB_JETS" ); 
         attList.extend<int>( "L1CI.L1CI_MIN_TOB_JETL" ); 
         attList.extend<int>( "L1CI.L1CI_ISO_HA_EM" ); 
         attList.extend<int>( "L1CI.L1CI_ISO_EM_EM" ); 
         attList.extend<int>( "L1CI.L1CI_ISO_EM_TAU" ); 
      }


      fillQuery(q.get(),attList);

      coral::ICursor& cursor = q->execute();

      if(!cursor.next()) {
         if(data.smk()!=0) {
            TRG_MSG_ERROR("No CaloInfo exists for SMK " << data.smk());
         } else {
            TRG_MSG_ERROR("No CaloInfo exists with ID " << data.id());
         }
         commitSession();
         throw std::runtime_error( "CaloInfoLoader >> CaloInfo not available" );
      }
        
      // fill the object with data
      const coral::AttributeList& row = cursor.currentRow();
      data.setName       ( row["L1CI.L1CI_NAME"].data<std::string>()  );
      data.setVersion    ( row["L1CI.L1CI_VERSION"].data<int>()      );
      if(isRun1()) {
         if(schema <= 6)
            data.setGlobalScale( row["L1CI.L1CI_GLOBAL_SCALE"].data<float>());        
         else
            data.setGlobalScale( boost::lexical_cast<float,std::string>(row["L1CI.L1CI_GLOBAL_SCALE"].data<std::string>()));
      } else {
            data.setGlobalEmScale( row["L1CI.L1CI_GLOBAL_EM_SCALE"].data<float>() );
            data.setGlobalJetScale( row["L1CI.L1CI_GLOBAL_JET_SCALE"].data<float>() );
      }
      if(isRun1()) {
         for(unsigned int index = 1; index <= 12 ;index++) {
            std::stringstream helpstring;
            helpstring << "L1CI.L1CI_JET_WEIGHT" << index;
            data.addJetWeight( static_cast<int>(row[helpstring.str()].data<int>()) );
         }
      }
      if(isRun2() || (isRun1() && schema>=10)) {
         int XSSigmaScale  = row["L1CI.L1CI_XS_SIGMA_SCALE"].data<int>();
         int XSSigmaOffset = row["L1CI.L1CI_XS_SIGMA_OFFSET"].data<int>();
         int XEmin         = row["L1CI.L1CI_XS_XE_MIN"].data<int>();
         int XEmax         = row["L1CI.L1CI_XS_XE_MAX"].data<int>();
         int TESqrtMin     = row["L1CI.L1CI_XS_TESQRT_MIN"].data<int>();
         int TESqrtMax     = row["L1CI.L1CI_XS_TESQRT_MAX"].data<int>();
         data.metSigParam().setValues( XSSigmaScale, XSSigmaOffset,
                                       XEmin, XEmax, TESqrtMin, TESqrtMax);
      }

      vector<int> mintobIDs;
      vector<int> isoparIDs;
      if(isRun2()) {
         mintobIDs.push_back(row["L1CI.L1CI_MIN_TOB_EM"].data<int>());
         mintobIDs.push_back(row["L1CI.L1CI_MIN_TOB_TAU"].data<int>());
         mintobIDs.push_back(row["L1CI.L1CI_MIN_TOB_JETS"].data<int>());
         mintobIDs.push_back(row["L1CI.L1CI_MIN_TOB_JETL"].data<int>());
         isoparIDs.push_back(row["L1CI.L1CI_ISO_HA_EM"].data<int>());
         isoparIDs.push_back(row["L1CI.L1CI_ISO_EM_EM"].data<int>());
         isoparIDs.push_back(row["L1CI.L1CI_ISO_EM_TAU"].data<int>());
      }

      // check for uniqness of CaloInfo object
      if ( cursor.next() ) {
         TRG_MSG_ERROR("More than one CaloInfo exists " );
         throw std::runtime_error( "Too many CaloInfo objects" );
      }
        
      if(isRun2()) {
         loadMinTobInfo(data, mintobIDs);
         loadIsolationInfo(data, isoparIDs);
      }      

      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      m_session.transaction().rollback();
      throw;
   }
   return true;
}


void
TrigConf::CaloInfoLoader::loadMinTobInfo( CaloInfo& data, const vector<int> & mintobIDs ) {

   unique_ptr<coral::IQuery> q(m_session.nominalSchema().tableHandle( "L1_CALO_MIN_TOB" ).newQuery());
   q->setRowCacheSize( 4 );

   string cond("L1CMT_ID IN (");
   bool first=true;
   for(int id : mintobIDs) {
      if(first) { first = false; } else { cond += ","; }
      cond += boost::lexical_cast<string,int>(id);
   }
   cond += ")";
   q->setCondition( cond, coral::AttributeList() );

   coral::AttributeList attList;
   attList.extend<int>( "L1CMT_ID" ); 
   attList.extend<string>( "L1CMT_THR_TYPE" ); 
   attList.extend<int>( "L1CMT_WINDOW" ); 
   attList.extend<int>( "L1CMT_PT_MIN" ); 
   attList.extend<int>( "L1CMT_ETA_MIN" ); 
   attList.extend<int>( "L1CMT_ETA_MAX" ); 
   attList.extend<int>( "L1CMT_PRIORITY" ); 
   fillQuery(q.get(),attList);

   coral::ICursor& cursor = q->execute();      

   while ( cursor.next() ) {

      const coral::AttributeList& row = cursor.currentRow();

      string thrtype = row["L1CMT_THR_TYPE"].data<string>();
      int window = row["L1CMT_WINDOW"].data<int>();
      int ptmin = row["L1CMT_PT_MIN"].data<int>();
      int etamin = row["L1CMT_ETA_MIN"].data<int>();
      int etamax = row["L1CMT_ETA_MAX"].data<int>();
      int priority = row["L1CMT_PRIORITY"].data<int>();

      if(ptmin<0) {
         TRG_MSG_ERROR("MinTOBPt " << thrtype << " with pt " << ptmin << " which is less than 0");
         throw runtime_error("PTMin of MinTOBPt found to be less than 0");
      }
      if(priority<0) {
         TRG_MSG_ERROR("MinTOBPt " << thrtype << " with priority " << priority << " which is less than 0");
         throw runtime_error("Priority of MinTOBPt found to be less than 0");
      }

      MinTOBPt mintob((unsigned int)ptmin, etamin, etamax, (unsigned int) priority);

      if(thrtype=="EM") {
         data.setMinTobEM(mintob);
      } else if(thrtype=="TAU") {
         data.setMinTobTau(mintob);
      } else if(thrtype=="JETS") {
         data.setMinTobJetSmall(mintob);
         data.setJetWindowSizeSmall(window);
      } else if(thrtype=="JETL") {
         data.setMinTobJetLarge(mintob);
         data.setJetWindowSizeLarge(window);
      } else {
         TRG_MSG_ERROR("Unknown threshold type " << thrtype);
         throw runtime_error("MinTOBPt with unknown threshold type");
      }

   }
   
}



void
TrigConf::CaloInfoLoader::loadIsolationInfo( CaloInfo& data, const vector<int> & isoparIDs ) {

   // first get a map of threshold type to isolation parameterizations

   map<string,vector<int>> m;

   {
      unique_ptr<coral::IQuery> q(m_session.nominalSchema().tableHandle( "L1_CALO_ISOLATION" ).newQuery());
      q->setRowCacheSize( 3 );

      string cond( "L1CIS_ID IN (" + join(isoparIDs) + ")" );
      q->setCondition( cond, coral::AttributeList() );

      coral::AttributeList attList;
      attList.extend<string>( "L1CIS_THR_TYPE" ); 
      attList.extend<int>( "L1CIS_PAR1_ID" ); 
      attList.extend<int>( "L1CIS_PAR2_ID" ); 
      attList.extend<int>( "L1CIS_PAR3_ID" ); 
      attList.extend<int>( "L1CIS_PAR4_ID" ); 
      attList.extend<int>( "L1CIS_PAR5_ID" ); 
      fillQuery(q.get(),attList);

      coral::ICursor& cursor = q->execute();      

      while ( cursor.next() ) {

         const coral::AttributeList& row = cursor.currentRow();
     
         string thrtype = row["L1CIS_THR_TYPE"].data<string>();

         vector<int> & idbytype = m[thrtype];

         idbytype.push_back( row["L1CIS_PAR1_ID"].data<int>() );
         idbytype.push_back( row["L1CIS_PAR2_ID"].data<int>() );
         idbytype.push_back( row["L1CIS_PAR3_ID"].data<int>() );
         idbytype.push_back( row["L1CIS_PAR4_ID"].data<int>() );
         idbytype.push_back( row["L1CIS_PAR5_ID"].data<int>() );

      }

   }

   // second get the individual parameterizations

   {
      for( const auto & isolation : m ) {

         unique_ptr<coral::IQuery> q(m_session.nominalSchema().tableHandle( "L1_CALO_ISOPARAM" ).newQuery());
         q->setRowCacheSize( 5 );

         const string & thrtype = isolation.first;
         const vector<int> & isoparIds = isolation.second;

         string cond( "L1CIP_ID IN (" + join(isoparIds) + ")" );
         q->setCondition( cond, coral::AttributeList() );

         coral::AttributeList attList;
         attList.extend<int>( "L1CIP_ID" );
         attList.extend<int>( "L1CIP_ISO_BIT" );
         attList.extend<int>( "L1CIP_OFFSET" );
         attList.extend<int>( "L1CIP_SLOPE" );
         attList.extend<int>( "L1CIP_MIN_CUT" );
         attList.extend<int>( "L1CIP_UPPER_LIMIT" );
         attList.extend<int>( "L1CIP_ETA_MIN" );
         attList.extend<int>( "L1CIP_ETA_MAX" );
         attList.extend<int>( "L1CIP_PRIORITY" );
         fillQuery(q.get(),attList);

         coral::ICursor& cursor = q->execute();      

         while ( cursor.next() ) {

            const coral::AttributeList& row = cursor.currentRow();

            unsigned int id = (unsigned int)row["L1CIP_ID"].data<int>();
            unsigned int pos=1; // the position of this paramerization
            for(unsigned int parId : isoparIds) {
               if(id==parId) break;
               ++pos;
            }

            int isobit     = row["L1CIP_ISO_BIT"].data<int>();
            int offset     = row["L1CIP_OFFSET"].data<int>();
            int slope      = row["L1CIP_SLOPE"].data<int>();
            int mincut     = row["L1CIP_MIN_CUT"].data<int>();
            int upperlimit = row["L1CIP_UPPER_LIMIT"].data<int>();
            int etamin     = row["L1CIP_ETA_MIN"].data<int>();
            int etamax     = row["L1CIP_ETA_MAX"].data<int>();
            int priority   = row["L1CIP_PRIORITY"].data<int>();


            if(isobit>0) {
               if(pos!=(unsigned int)isobit) {
                  TRG_MSG_ERROR(thrtype << " isolation bit " << isobit << " does not match the position it should have " << pos);
                  throw runtime_error("Isolation bit not matching");
               }
               data.setIsolation(thrtype, pos, IsolationParam( thrtype, isobit, offset, slope, mincut, upperlimit,  etamin, etamax, priority ));
            } else {
               data.setIsolation(thrtype, pos, IsolationParam());
            } 
            

         }

         cursor.close();
      }
   }
}
