/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./ThresholdConfigLoader.h"
#include "./TriggerThresholdLoader.h"
#include "./CaloJetInputLoader.h"
#include "./CaloSinCosLoader.h"
#include "./CaloInfoLoader.h"
#include "./DBHelper.h"
#include "TrigConfStorage/StorageMgr.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/CaloJetInput.h"
#include "TrigConfL1Data/CaloSinCos.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/HelperFunctions.h"


#include "TrigConfL1Data/NimThresholdValue.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include "boost/lexical_cast.hpp" // for loading ptcut, etc

using namespace std;

bool TrigConf::ThresholdConfigLoader::load( ThresholdConfig& thrConfig ) {

   const unsigned int schema_version_with_zb_fields = 9;

   TRG_MSG_DEBUG("Loading ThresholdConfig object attached to Lvl1 master ID " << thrConfig.lvl1MasterTableId());
   TRG_MSG_DEBUG("Current number of thresholds: " << thrConfig.size());

   long caloinfoid = 0;

   try {
      startSession();
      // get menu id and caloinfo id from thrConfig.m_MasterTableId and the table L1_MASTER_TABLE
      int menuid = 0;

      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE").newQuery());
      query0->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("mtId");
      std::string cond0 = "L1MT_ID = :mtId";
      bindList0[0].data<int>() = thrConfig.lvl1MasterTableId();
      query0->setCondition( cond0, bindList0 );

      //Output and types
      coral::AttributeList attList0;
      attList0.extend<int>( "L1MT_TRIGGER_MENU_ID" );
      attList0.extend<long>( "L1MT_CALO_INFO_ID" );
      query0->defineOutput(attList0);
      query0->addToOutputList( "L1MT_TRIGGER_MENU_ID" );
      query0->addToOutputList( "L1MT_CALO_INFO_ID" );
      coral::ICursor& cursor0 = query0->execute();

      if ( ! cursor0.next() ) {
         TRG_MSG_ERROR("ThresholdConfigLoader >> No such Master_Table exists " << thrConfig.lvl1MasterTableId());
         commitSession();
         throw std::runtime_error( "ThresholdConfigLoader >> ThresholdConfig not available" );
      }
	
      const coral::AttributeList& row0 = cursor0.currentRow();
      menuid = row0["L1MT_TRIGGER_MENU_ID"].data<int>();
      caloinfoid = row0["L1MT_CALO_INFO_ID"].data<long>();

      if ( cursor0.next() ) {

         TRG_MSG_ERROR("ThresholdConfigLoader >> More than one Master_Table exists " 
                       << thrConfig.lvl1MasterTableId());
         commitSession();
         throw std::runtime_error( "ThresholdConfigLoader >>  Master_Table not available" );
      }

      //=====================================================
      // get Thresholds WITH trigger threshold values

      coral::AttributeList emptyBindVariableList;
	
      unique_ptr<coral::IQuery> query(m_session.nominalSchema().newQuery());

      // Set the tables that are used
      query->addToTableList ( "L1_TM_TO_TT",                "TM2TT"  );
      query->addToTableList ( "L1_TRIGGER_THRESHOLD",       "TT"     );
      query->addToTableList ( "L1_TT_TO_TTV",               "TT2TTV" );
      query->addToTableList ( "L1_TRIGGER_THRESHOLD_VALUE", "TTV"    );

      // Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("menuId");
      bindList[0].data<int>() = menuid;
      std::string theCondition = "";
      theCondition += std::string( " TM2TT.L1TM2TT_TRIGGER_MENU_ID = :menuId"                );
      theCondition += std::string( " AND TM2TT.L1TM2TT_TRIGGER_THRESHOLD_ID = TT.L1TT_ID"    );
      theCondition += std::string( " AND TT2TTV.L1TT2TTV_TRIGGER_THRESHOLD_ID = TT.L1TT_ID"  );
      theCondition += std::string( " AND TT2TTV.L1TT2TTV_TRIG_THRES_VALUE_ID = TTV.L1TTV_ID" );
      query->setCondition( theCondition, bindList );

      // Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "TM2TT.L1TM2TT_CABLE_NAME"      );
      attList.extend<std::string>( "TM2TT.L1TM2TT_CABLE_CTPIN"     );
      attList.extend<std::string>( "TM2TT.L1TM2TT_CABLE_CONNECTOR" );
      attList.extend<int>        ( "TM2TT.L1TM2TT_CABLE_START"     );
      attList.extend<int>        ( "TM2TT.L1TM2TT_CABLE_END"       );
      if(isRun2()) {
          attList.extend<int>        ( "TM2TT.L1TM2TT_CABLE_CLOCK"     );
      }
      attList.extend<int>        ( "TT.L1TT_ID"                    );
      attList.extend<std::string>( "TT.L1TT_NAME"                  );
      attList.extend<int>        ( "TT.L1TT_VERSION"               );
      attList.extend<std::string>( "TT.L1TT_TYPE"                  );
      attList.extend<int>        ( "TT.L1TT_ACTIVE"                );
      attList.extend<int>        ( "TT.L1TT_MAPPING"               );
      attList.extend<int>        ( "TT.L1TT_BITNUM"                );
      attList.extend<int>        ( "TTV.L1TTV_ID"                  );
      attList.extend<std::string>( "TTV.L1TTV_NAME"                );
      attList.extend<int>        ( "TTV.L1TTV_VERSION"             );
      attList.extend<std::string>( "TTV.L1TTV_TYPE"                );
      attList.extend<std::string>( "TTV.L1TTV_PT_CUT"              );
      attList.extend<int>        ( "TTV.L1TTV_ETA_MIN"             );
      attList.extend<int>        ( "TTV.L1TTV_ETA_MAX"             );
      attList.extend<int>        ( "TTV.L1TTV_PHI_MIN"             );
      attList.extend<int>        ( "TTV.L1TTV_PHI_MAX"             );
      attList.extend<std::string>( "TTV.L1TTV_EM_ISOLATION"        );
      attList.extend<std::string>( "TTV.L1TTV_HAD_ISOLATION"       );
      attList.extend<std::string>( "TTV.L1TTV_HAD_VETO"            );
      attList.extend<int>        ( "TTV.L1TTV_WINDOW"              );
      attList.extend<std::string>( "TTV.L1TTV_PRIORITY"            );
      fillQuery(query.get(), attList);

      // the ordering
      std::string theOrder = "";
      theOrder += "  TT.L1TT_MAPPING ASC";
      theOrder += ", TT.L1TT_ID ASC";
      theOrder += ", TTV.L1TTV_PRIORITY ASC";
      query->addToOrderList( theOrder );

      //query->setRowCacheSize(1000);
      query->setRowCacheSize(500);

      coral::ICursor& cursor = query->execute();

      unsigned int schema_version = triggerDBSchemaVersion();
      unsigned int numberofvalues = 0;
      TriggerThreshold*       tt = 0;
      int nRowsLoop1 = -2009;
      int nRowsLoop2 = -2010;

      while (cursor.next()) {

         const coral::AttributeList& row = cursor.currentRow();

         nRowsLoop1 = row["TT.L1TT_ID"].data<int>();
         if (nRowsLoop1 != nRowsLoop2) {

            if (tt != 0) {
               // trigger thresholds sorted by type
               thrConfig.addTriggerThreshold(tt);
               tt = new TriggerThreshold();
               numberofvalues = 0;
            } else {
               tt = new TriggerThreshold();
               numberofvalues = 0;
            }

            //==================================================
            // now get the cable info from TM_TT

            string cableName = row["TM2TT.L1TM2TT_CABLE_NAME"].data<std::string>();
            tt->setCableName     (cableName);
            tt->setCableCtpin    (row["TM2TT.L1TM2TT_CABLE_CTPIN"].data<std::string>());
            tt->setCableConnector(row["TM2TT.L1TM2TT_CABLE_CONNECTOR"].data<std::string>());
            tt->setCableStart    (row["TM2TT.L1TM2TT_CABLE_START"].data<int>());
            tt->setCableEnd      (row["TM2TT.L1TM2TT_CABLE_END"].data<int>());
            
            int clock = 0;
            if(isRun2()) {
                clock = row["TM2TT.L1TM2TT_CABLE_CLOCK"].data<int>();
            }
            if(clock >= 0) {
               tt->setClock(clock);
            }
            tt->setId     (row["TT.L1TT_ID"].data<int>());
            tt->setName   (row["TT.L1TT_NAME"].data<std::string>());
            tt->setVersion(row["TT.L1TT_VERSION"].data<int>());
            string thrtype(row["TT.L1TT_TYPE"].data<std::string>());
            tt->setType   (thrtype);
            tt->setActive (row["TT.L1TT_ACTIVE"].data<int>());
            tt->setMapping(row["TT.L1TT_MAPPING"].data<int>());
            tt->setBitnum(row["TT.L1TT_BITNUM"].data<int>());
            tt->setInput  ( (thrtype=="TOPO" || thrtype=="ALFA") ? "ctpcore" : "ctpin" );

            TRG_MSG_VERBOSE("ThresholdConfigLoader loading threshold with ID = " << tt->id() << " for MenuId = " << menuid << ": ");
         }
  
         if(tt->type() == L1DataDef::rndmType() ||
            tt->type() == L1DataDef::pclkType() ||
            tt->type() == L1DataDef::bgrpType() ) {
         } else {

            TriggerThresholdValue* ttv = tt->createThresholdValue(row["TT.L1TT_TYPE"].data<std::string>()); 

            ttv->setId       (row["TTV.L1TTV_ID"].data<int>());

            // Fill the value common for all threshold_value
            ttv->setName     (row["TTV.L1TTV_NAME"].data<std::string>());
            ttv->setVersion  (row["TTV.L1TTV_VERSION"].data<int>());
            ttv->setType     (row["TTV.L1TTV_TYPE"].data<std::string>());
            float ptcut = boost::lexical_cast<float,std::string>(row["TTV.L1TTV_PT_CUT"].data<std::string>());
            string emisolation = row["TTV.L1TTV_EM_ISOLATION"].data<std::string>();
            string hadisolation = row["TTV.L1TTV_HAD_ISOLATION"].data<std::string>();
            string hadveto = row["TTV.L1TTV_HAD_VETO"].data<std::string>();
            float priority = boost::lexical_cast<float,std::string>(row["TTV.L1TTV_PRIORITY"].data<std::string>());
            ttv->setPtcut    (ptcut);
            ttv->setPriority (priority);

            //is it a cluster_threshold_value?
            try {
               ClusterThresholdValue& ctv = dynamic_cast<ClusterThresholdValue&>(*ttv);
               if (ttv->type() != L1DataDef::emType() &&
                   ttv->type() != L1DataDef::tauType()) {
                  msg() << "ThresholdConfigLoader >> No type match for ttv_id = "
                        << ctv.id() << " " << ttv->type() << std::endl;
                  throw std::runtime_error( "ThresholdConfigLoader >> ClusterThresholdValue not available" );
               }

               if(hadveto=="USEISOBITS" || boost::lexical_cast<int,std::string>(hadveto)==99 ) {
                  ctv.setEmIsolation( 63 );
                  ctv.setHadIsolation( 63 );
                  ctv.setHadVeto( 99 );
                  ctv.setIsolationMask( TrigConf::bin2uint(emisolation) );
                  ctv.setUseIsolationMask();
               } else {
                  ctv.setEmIsolation( boost::lexical_cast<float,std::string>(emisolation) );
                  ctv.setHadIsolation( boost::lexical_cast<float,std::string>(hadisolation) );
                  ctv.setHadVeto( boost::lexical_cast<float,std::string>(hadveto) );
                  ctv.setUseIsolationMask( false );
               }

            } catch (std::bad_cast& ex) { }

            //is it a jet_threshold_value?
            try {
               JetThresholdValue& jtv = dynamic_cast<JetThresholdValue&>(*ttv);
               if (ttv->type() != L1DataDef::jetType() &&
                   ttv->type() != L1DataDef::jbType() &&
                   ttv->type() != L1DataDef::jfType()) {
                  TRG_MSG_ERROR("No type match for ttv_id = " << jtv.id() << ttv->type());
                  throw std::runtime_error( "ThresholdConfigLoader >> TriggerThresholdValue not available" );
               }
               //             jtv.setPhiMin(row["TTV.L1TTV_PHI_MIN"].data<int>());
               //             jtv.setPhiMax(row["TTV.L1TTV_PHI_MAX"].data<int>());
               //             jtv.setEtaMin(row["TTV.L1TTV_ETA_MIN"].data<int>());
               //             jtv.setEtaMax(row["TTV.L1TTV_ETA_MAX"].data<int>());
               //             jtv.setWindow(row["TTV.L1TTV_WINDOW"].data<int>());
            } catch (std::bad_cast& ex) { }

            ttv->setPhiMin(row["TTV.L1TTV_PHI_MIN"].data<int>());
            ttv->setPhiMax(row["TTV.L1TTV_PHI_MAX"].data<int>());
            ttv->setEtaMin(row["TTV.L1TTV_ETA_MIN"].data<int>());
            ttv->setEtaMax(row["TTV.L1TTV_ETA_MAX"].data<int>());
            ttv->setWindow(row["TTV.L1TTV_WINDOW"].data<int>());
            tt->addThresholdValue(ttv);
            ++numberofvalues;
            TRG_MSG_DEBUG("ThresholdConfigLoader loading thresholdvalue with ID = " << ttv->id() << ":  " << ttv->name());
         }

         nRowsLoop2 = row["TT.L1TT_ID"].data<int>();

      }

      // trigger thresholds sorted by type
      thrConfig.addTriggerThreshold(tt);

      //===========================================
      // now get the Thresholds WITHOUT trigger threshold values
      // we are aware that some code is duplicated below
      // it is done this way because most Thresholds are WITH trigger threshold values       
 
      if (!m_session.transaction().isActive()) m_session.transaction().start();
      unique_ptr<coral::IQuery> query1(m_session.nominalSchema().newQuery());

      // Set the tables that are used
      query1->addToTableList ( "L1_TM_TO_TT",                "TM2TT"  );
      query1->addToTableList ( "L1_TRIGGER_THRESHOLD",       "TT"     );

      // Bind list
      coral::AttributeList bindList1;
      bindList1.extend<int>("menuId");
      bindList1[0].data<int>() = menuid;
      std::string theCondition1 = "";
      theCondition1 += std::string( " TM2TT.L1TM2TT_TRIGGER_MENU_ID = :menuId"                );
      theCondition1 += std::string( " AND TM2TT.L1TM2TT_TRIGGER_THRESHOLD_ID = TT.L1TT_ID"    );
      query1->setCondition( theCondition1, bindList1 );

      // Output data and types
      coral::AttributeList attList1;
      attList1.extend<std::string>( "TM2TT.L1TM2TT_CABLE_NAME"      );
      attList1.extend<std::string>( "TM2TT.L1TM2TT_CABLE_CTPIN"     );
      attList1.extend<std::string>( "TM2TT.L1TM2TT_CABLE_CONNECTOR" );
      attList1.extend<int>        ( "TM2TT.L1TM2TT_CABLE_START"     );
      attList1.extend<int>        ( "TM2TT.L1TM2TT_CABLE_END"       );
      if(isRun2()) {
          attList1.extend<int>        ( "TM2TT.L1TM2TT_CABLE_CLOCK"     );
      }
      attList1.extend<int>        ( "TT.L1TT_ID"                    );
      attList1.extend<std::string>( "TT.L1TT_NAME"                  );
      attList1.extend<int>        ( "TT.L1TT_VERSION"               );
      attList1.extend<std::string>( "TT.L1TT_TYPE"                  );
      attList1.extend<int>        ( "TT.L1TT_ACTIVE"                );
      attList1.extend<int>        ( "TT.L1TT_MAPPING"               );
      if( isRun2() || (schema_version >= schema_version_with_zb_fields) ) {
         attList1.extend<int>        ( "TT.L1TT_BCDELAY" );
         attList1.extend<std::string>( "TT.L1TT_SEED" );
         attList1.extend<int>        ( "TT.L1TT_SEED_MULTI" );
      }
      fillQuery(query1.get(), attList1);

      // the ordering
      std::string theOrder1 = "";
      theOrder1 += "  TT.L1TT_ID ASC";
      query1->addToOrderList( theOrder1 );

      //query->setRowCacheSize(1000);
      query1->setRowCacheSize(500);

      coral::ICursor& cursor1 = query1->execute();

      numberofvalues = 0;
      tt = 0;
      nRowsLoop1 = -2009;
      nRowsLoop2 = -2010;

      while (cursor1.next()) {

         const coral::AttributeList& row = cursor1.currentRow();

         nRowsLoop1 = row["TT.L1TT_ID"].data<int>();
         tt = thrConfig.findTriggerThreshold(nRowsLoop1);
         if (tt) continue;
         if (nRowsLoop1 != nRowsLoop2) {

            if (tt != 0) {
               //tt->setNumberofValues(numberofvalues);
               // trigger thresholds sorted by type
               thrConfig.addTriggerThreshold(tt);
               tt = new TriggerThreshold();
               numberofvalues = 0;
            } else {
               tt = new TriggerThreshold();
               numberofvalues = 0;
            }

            //==================================================
            // now get the cable info from TM_TT

            // ttldr.setLoadCableInfo(true); will be moved from tt to thrcfg loader

            string cableName = row["TM2TT.L1TM2TT_CABLE_NAME"].data<std::string>();
            tt->setCableName(cableName);
            tt->setCableCtpin    (row["TM2TT.L1TM2TT_CABLE_CTPIN"].data<std::string>());
            tt->setCableConnector(row["TM2TT.L1TM2TT_CABLE_CONNECTOR"].data<std::string>());
            tt->setCableStart    (row["TM2TT.L1TM2TT_CABLE_START"].data<int>());
            tt->setCableEnd      (row["TM2TT.L1TM2TT_CABLE_END"].data<int>());

            int clock = 0;
            if(isRun2()) {
                clock = row["TM2TT.L1TM2TT_CABLE_CLOCK"].data<int>();
            }
            if(clock >= 0) {
               tt->setClock(clock);
            }
            tt->setId     (row["TT.L1TT_ID"].data<int>());
            tt->setName   (row["TT.L1TT_NAME"].data<std::string>());
            tt->setVersion(row["TT.L1TT_VERSION"].data<int>());
            string thrtype(row["TT.L1TT_TYPE"].data<std::string>());
            tt->setType   (thrtype);
            tt->setActive (row["TT.L1TT_ACTIVE"].data<int>());
            tt->setMapping(row["TT.L1TT_MAPPING"].data<int>());
            tt->setInput  ( (thrtype=="TOPO" || thrtype=="ALFA") ? "ctpcore" : "ctpin" );


            // zero bias related
            int   bcdelay(-1), seed_multi(-1);
            std::string seed("");
            if( isRun2() || (schema_version >= schema_version_with_zb_fields) ) {
               bcdelay    = row["TT.L1TT_BCDELAY"].data<int>();
               seed       = row["TT.L1TT_SEED"].data<std::string>();
               seed_multi = row["TT.L1TT_SEED_MULTI"].data<int>();
            }
            tt->setBCDelay                (bcdelay);
            tt->setZBSeedingThresholdName (seed);
            tt->setZBSeedingThresholdMulti(seed_multi);

            TRG_MSG_VERBOSE("ThresholdConfigLoader loading threshold with ID = " << tt->id() << " for MenuId = " << menuid << ": ");
            //tt->setNumberofValues(numberofvalues);
            // trigger thresholds sorted by type
            thrConfig.addTriggerThreshold(tt);
            tt = nullptr;
         }

         nRowsLoop2 = row["TT.L1TT_ID"].data<int>();

      }
      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Caught coral exception: " << e.what() );
      throw; 
   }
   catch( const std::exception& e ) {
      TRG_MSG_ERROR("Caught standard exception: " << e.what() );
      throw; 
   }

   loadCaloInfo(thrConfig, caloinfoid);

   // create TIPs from TOPO and ALFA TriggerThresholds

   return true;    

}



void
TrigConf::ThresholdConfigLoader::loadCaloInfo( ThresholdConfig& thrConfig, long caloInfoId ) {
   //============================================
   // get CaloInfo
   //============================================
   try {
      startSession();
      CaloInfoLoader& cildr = dynamic_cast<CaloInfoLoader&>
         ((dynamic_cast<StorageMgr&>(m_storageMgr)).caloInfoLoader());
      cildr.setLevel(outputLevel());
      CaloInfo ci;
      ci.setId(caloInfoId);
      if ( !cildr.load( ci )) {
         TRG_MSG_ERROR("loading CaloInfo " << ci.id());
         throw runtime_error( "ThresholdConfigLoader: Error loading CaloInfo" );
      }
      thrConfig.setCaloInfo(ci);
      TriggerThresholdValue::setCaloInfo(ci);
      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Caught coral exception: " << e.what() );
      throw; 
   }
   catch( const std::exception& e ) {
      TRG_MSG_ERROR("Caught standard exception: " << e.what() );
      throw; 
   }
}


