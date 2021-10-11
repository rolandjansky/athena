/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./TriggerThresholdValueLoader.h"
#include "./TriggerThresholdLoader.h"
#include "./DBHelper.h"
#include "TrigConfStorage/StorageMgr.h"

#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/MuonThresholdValue.h"
#include "TrigConfL1Data/NimThresholdValue.h"
#include "TrigConfL1Data/XsThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "boost/lexical_cast.hpp"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace std;

bool
TrigConf::TriggerThresholdLoader::load( TriggerThreshold& ttTarget ) {

   const unsigned int schema_version_with_zb_fields = 9;

   TRG_MSG_DEBUG("TriggerThresholdLoader loading threshold with ID = " 
                 << ttTarget.id() << " for MenuId = " 
                 << m_MenuId << ": ")
   unsigned int schema_version = triggerDBSchemaVersion();

   try {
      startSession();

      {
         unique_ptr<coral::IQuery> query(m_session.nominalSchema().tableHandle( "L1_TRIGGER_THRESHOLD").newQuery());
         query->setRowCacheSize( 5 );

         //Bind list
         coral::AttributeList bindList;
         bindList.extend<long>("ttId");
         std::string cond= "L1TT_ID = :ttId";
         bindList[0].data<long>() = ttTarget.id();
         query->setCondition( cond, bindList );

         //Output data and types
         coral::AttributeList attList;
         attList.extend<std::string>( "L1TT_NAME" );
         attList.extend<int>( "L1TT_VERSION" );
         attList.extend<std::string>( "L1TT_TYPE" );
         attList.extend<int>( "L1TT_ACTIVE" );
         attList.extend<int>( "L1TT_MAPPING" );
         if( (isRun1() && schema_version >= schema_version_with_zb_fields) || isRun2() ) {
            attList.extend<int>( "L1TT_BCDELAY" );
            attList.extend<std::string>( "L1TT_SEED" );
            attList.extend<int>( "L1TT_SEED_MULTI" );
         }
         fillQuery(query.get(),attList);

         coral::ICursor& cursor = query->execute();
        
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No TriggerThreshold exists with ID " << ttTarget.id());
            commitSession();
            throw std::runtime_error( "TriggerThresholdLoader >> TriggerThreshold not available" );
         }
        
         const coral::AttributeList& row = cursor.currentRow();

         std::string name    = row["L1TT_NAME"].data<std::string>();
         int         version = row["L1TT_VERSION"].data<int>();
         std::string type    = row["L1TT_TYPE"].data<std::string>();
         int         active  = row["L1TT_ACTIVE"].data<int>();
         int         mapping = row["L1TT_MAPPING"].data<int>();

         // zero bias related
         int         bcdelay(-1), seed_multi(-1);
         std::string seed("");
         if( (isRun1() && schema_version >= schema_version_with_zb_fields) || isRun2() ) {
            bcdelay    = row["L1TT_BCDELAY"].data<int>();
            seed       = row["L1TT_SEED"].data<std::string>();
            seed_multi = row["L1TT_SEED_MULTI"].data<int>();
         }

         TRG_MSG_DEBUG(name << " " << version << " " << type << " " << active << " " << mapping);
	
         // Fill the object with data
         ttTarget.setName   ( name );
         ttTarget.setVersion( version );
         ttTarget.setType   ( type );
         ttTarget.setInput  ( (type=="TOPO" || type=="ALFA") ? "ctpcore" : "ctpin" );
         ttTarget.setActive ( active );

         ttTarget.setZBSeedingThresholdName(seed);
         ttTarget.setZBSeedingThresholdMulti(seed_multi);
         ttTarget.setBCDelay(bcdelay);

         if(ttTarget.isInternal()) {
            string::size_type pos = name.find_first_of("0123456789");
            mapping = boost::lexical_cast<int,string>(name.substr(pos));
         }
         ttTarget.setMapping( mapping );
      }


      //==================================================
      // now get the cable info from TM_TT
      if(loadCableInfo()) {
          
         unique_ptr<coral::IQuery> query(m_session.nominalSchema().tableHandle( "L1_TM_TO_TT").newQuery());
         query->setRowCacheSize( 5 );
          
         //Bind list
         coral::AttributeList bindList;
         std::string cond = "L1TM2TT_TRIGGER_THRESHOLD_ID = :ttId";
         cond += " AND L1TM2TT_TRIGGER_MENU_ID = :menuId";
         bindList.extend<long>("ttId");
         bindList.extend<int>("menuId");
         bindList[0].data<long>() = ttTarget.id();
         bindList[1].data<int>() = m_MenuId;
         query->setCondition( cond, bindList );
     
         //Define the data types
         coral::AttributeList attList;
         attList.extend<std::string>( "L1TM2TT_CABLE_NAME" );
         attList.extend<std::string>( "L1TM2TT_CABLE_CTPIN" );
         attList.extend<std::string>( "L1TM2TT_CABLE_CONNECTOR" );
         attList.extend<int>( "L1TM2TT_CABLE_START" );
         attList.extend<int>( "L1TM2TT_CABLE_END" );
         fillQuery(query.get(),attList);

         coral::ICursor& cursor = query->execute();
          
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No such combination in L1_TM_TO_TT: TT ID "<< ttTarget.id() << ", menu ID " << m_MenuId);
            commitSession();
            throw std::runtime_error( "TriggerThresholdLoader >> "
                                      "TMTI combination not availbale in TM_TT" );
         }
          
         const coral::AttributeList& row = cursor.currentRow();
         std::string cable_name      = row["L1TM2TT_CABLE_NAME"].data<std::string>();
         std::string cable_ctpin     = row["L1TM2TT_CABLE_CTPIN"].data<std::string>();
         std::string cable_connector = row["L1TM2TT_CABLE_CONNECTOR"].data<std::string>();
         int cable_start             = row["L1TM2TT_CABLE_START"].data<int>();
         int cable_end               = row["L1TM2TT_CABLE_END"].data<int>();
	
         ttTarget.setCableName(cable_name);
         ttTarget.setCableCtpin(cable_ctpin);
         ttTarget.setCableConnector(cable_connector);
         ttTarget.setCableStart(cable_start);
         ttTarget.setCableEnd(cable_end);
          
      }

      if( ! ttTarget.isInternal() ) { // no trigger threshold values exist for internal triggers
          
         //=================================================================
         // now get the TTVs from TT_TTV:
         coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_TT_TO_TTV");
         coral::IQuery* query = table.newQuery();
         query->setRowCacheSize(5);

         //Bind list
         coral::AttributeList bindList;
         bindList.extend<long>("ttId");
         std::string cond = "L1TT2TTV_TRIGGER_THRESHOLD_ID = :ttId";
         bindList[0].data<long>() = ttTarget.id();
         query->setCondition( cond, bindList );
     
         //Output data and types
         coral::AttributeList attList;
         attList.extend<long>( "L1TT2TTV_TRIG_THRES_VALUE_ID" );
         query->defineOutput(attList);
         query->addToOutputList( "L1TT2TTV_TRIG_THRES_VALUE_ID" );
        
         coral::ICursor& cursor = query->execute();
        
         std::vector<long> vec_ttv_id;
         while( cursor.next() ) {
            const coral::AttributeList& row = cursor.currentRow();
            vec_ttv_id.push_back((long)row["L1TT2TTV_TRIG_THRES_VALUE_ID"].data<long>());
         }
        
         unsigned int numberofvalues = vec_ttv_id.size();
        
         TriggerThresholdValueLoader& ttvldr = dynamic_cast<TriggerThresholdValueLoader&>
            ((dynamic_cast<StorageMgr&>(m_storageMgr)).triggerThresholdValueLoader());
         ttvldr.setVerbose(verbose());
         for (unsigned int i=0; i<numberofvalues; ++i) {

            TriggerThresholdValue* ttv = 0;          
            if ( ttTarget.type() == L1DataDef::emType() || 
                 ttTarget.type() == L1DataDef::tauType() ) {
               ttv = new ClusterThresholdValue();
            } else if (ttTarget.type()==L1DataDef::jetType() || 
                       ttTarget.type()==L1DataDef::jbType() ||
                       ttTarget.type()==L1DataDef::jfType()) {
               ttv = new JetThresholdValue();
            } else if (ttTarget.type() ==L1DataDef::muonType()) {
               ttv = new MuonThresholdValue();
            } else if (ttTarget.type() == L1DataDef::jeType() || 
                       ttTarget.type() == L1DataDef::xeType() || 
                       ttTarget.type() == L1DataDef::teType()) {
               ttv = new EtThresholdValue();
            } else if (ttTarget.type() == L1DataDef::xsType()) {
               ttv = new XsThresholdValue();
            } else if (ttTarget.type() == L1DataDef::nimType() ||
                       ttTarget.type() == L1DataDef::mbtsType() ||
                       ttTarget.type() == L1DataDef::mbtssiType() ||
                       ttTarget.type() == L1DataDef::calreqType() ||
                       ttTarget.type() == L1DataDef::zdcType() ||
                       ttTarget.type() == L1DataDef::trtType() ||
                       ttTarget.type() == L1DataDef::bcmType() ||
                       ttTarget.type() == L1DataDef::bcmcmbType() ||
                       ttTarget.type() == L1DataDef::lucidType()) { 
               ttv = new NimThresholdValue();
            } else {
               msg() << "TriggerThresholdLoader: not supported type " 
                     << ttTarget.id() << " type is " << ttTarget.type() << std::endl;
               delete query;
               delete ttv;
               commitSession();
               throw std::runtime_error("TriggerThresholdLoader: not supported type" );
            }
            ttv->setId(vec_ttv_id[i]);
            if ( !ttvldr.load( *ttv ) ) {
               msg() << "TriggerThresholdLoader: Error loading TriggerThreshodValue " 
                     << ttv->id() << std::endl;
               delete ttv;
               ttv = 0;
               delete query;
               commitSession();
               throw std::runtime_error("TriggerThresholdLoader: Error loading TriggerThreshodValue " );
            } else {
               // fill the vector
               ttTarget.addThresholdValue(ttv);
            }
         }
      }

      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "TriggerThresholdLoader:           SchemaException: "           
            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "TriggerThresholdLoader >> Standard C++ exception: " << e.what() << std::endl;
        
      m_session.transaction().rollback();
      return false; 
   } catch( ... ) {
      msg() << "TriggerThresholdLoader >> unknown C++ exception" << std::endl;
        
      m_session.transaction().rollback();
      return false; 
   }
}
  
void
TrigConf::TriggerThresholdLoader::setMenuId(const int& id) {
   m_MenuId = id;
}  

int
TrigConf::TriggerThresholdLoader::menuId() {
   return m_MenuId;
}

