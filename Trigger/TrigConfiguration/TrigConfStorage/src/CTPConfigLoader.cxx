/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./CTPConfigLoader.h"
#include "./MenuLoader.h"
#include "./PrescaleSetLoader.h"
#include "./BunchGroupSetLoader.h"
#include "./RandomLoader.h"
#include "./PrescaledClockLoader.h"
#include "./DeadTimeLoader.h"
#include "./CTPFilesLoader.h"
#include "./PrioritySetLoader.h"
#include "./DBHelper.h"

#include "TrigConfL1Data/CTPConfig.h"

//#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace std;

bool TrigConf::CTPConfigLoader::load(CTPConfig& ctpcTarget) {
   TRG_MSG_INFO("Loading data for L1 master key " << ctpcTarget.lvl1MasterTableId() 
                << " L1 PSS key " << ctpcTarget.prescaleSetId()  
                << " L1 BGS key " << ctpcTarget.bunchGroupSetId() );

   try {

      unsigned int schema = triggerDBSchemaVersion();
      TRG_MSG_INFO("TriggerDB schema: " << schema);

      startSession();

      long prescalesetid = ctpcTarget.prescaleSetId();
      int menuid = 0;
      long bunchgroupsetid = 0;
      long prescaledclockid = 0;
      long randomid = 0;
      long deadtimeid = 0;

      {
         unique_ptr<coral::IQuery> q( m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE").newQuery() );
         q->setRowCacheSize( 5 );
        
         //Bind list
         coral::AttributeList bindList;
         bindList.extend<int>("mtId");
         bindList[0].data<int>() = ctpcTarget.lvl1MasterTableId();
         std::string cond = "L1MT_ID = :mtId";
         q->setCondition( cond, bindList );

         //Output data and types
         coral::AttributeList attList;
         attList.extend<int>( "L1MT_TRIGGER_MENU_ID" );
         attList.extend<long>( "L1MT_PRESCALED_CLOCK_ID" );
         attList.extend<long>( "L1MT_RANDOM_ID" );
         if(isRun2()) {
            attList.extend<long>( "L1MT_CTPVERSION" );
            attList.extend<long>( "L1MT_L1VERSION" );
         }
         if( isRun1() ) attList.extend<long>( "L1MT_DEAD_TIME_ID" );

         fillQuery(q.get(),attList);

         coral::ICursor& cursor0 = q->execute();
        
         if ( ! cursor0.next() ) {
            TRG_MSG_ERROR("No such Master_Table exists " << ctpcTarget.lvl1MasterTableId() );
            throw std::runtime_error( "CTPConfigLoader:                  CTPConfig not available" );
         }
        
         const coral::AttributeList& row0 = cursor0.currentRow();
         
         menuid = row0["L1MT_TRIGGER_MENU_ID"].data<int>();
         bunchgroupsetid = ctpcTarget.bunchGroupSetId();
         prescaledclockid = row0["L1MT_PRESCALED_CLOCK_ID"].data<long>();
         randomid = row0["L1MT_RANDOM_ID"].data<long>();
         if( isRun1() ) deadtimeid = row0["L1MT_DEAD_TIME_ID"].data<long>();

         unsigned int ctpVersion;
         unsigned int l1Version;
         if(isRun1()) {
            ctpVersion = 3;
            l1Version = 0;
         } else {
            ctpVersion = row0["L1MT_CTPVERSION"].data<long>();
            l1Version = row0["L1MT_L1VERSION"].data<long>();
         }
         ctpcTarget.setCTPVersion(ctpVersion);
         ctpcTarget.setL1Version(l1Version);

         TRG_MSG_INFO("CTP version " << ctpcTarget.ctpVersion());
         TRG_MSG_INFO("L1  version " << ctpcTarget.l1Version());

         L1DataDef::setMaxThresholdsFromL1Version( ctpcTarget.l1Version() );
         //L1DataDef::printMaxThresholds();


      }
      
      if( ctpcTarget.loadCtpFiles() ) {

         TRG_MSG_INFO("Loading CTPFiles");

         // get the lutCam id
         unique_ptr<coral::IQuery> q( m_session.nominalSchema().tableHandle( "L1_TRIGGER_MENU").newQuery() );
         q->setRowCacheSize( 1 );

         //Bind list
         coral::AttributeList bindList;
         bindList.extend<int>("menuId");
         std::string cond = "L1TM_ID = :menuId";
         bindList[0].data<int>() = menuid;
         q->setCondition( cond, bindList );

         //Output data and types
         coral::AttributeList attList1;
         attList1.extend<long>( "L1TM_CTP_FILES_ID" );
         attList1.extend<long>( "L1TM_CTP_SMX_ID" );
         fillQuery(q.get(),attList1);

         coral::ICursor& cursor = q->execute();

         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No trigger menu exists with ID " << menuid);
            commitSession();
            throw std::runtime_error( "CTPConfigLoader >> CTPConfig not available" );
         }

         const coral::AttributeList& row = cursor.currentRow();
         long filesid = row["L1TM_CTP_FILES_ID"].data<long>();
         long smxid = row["L1TM_CTP_SMX_ID"].data<long>();

         if(filesid > 0 && smxid > 0) {

            CTPFiles* files = new CTPFiles();
            files->setId( filesid );
            files->setSmxId( smxid );
            files->setLoadCtpcoreFiles( ctpcTarget.loadCtpcoreFiles() );
            files->setLoadCtpinFiles( ctpcTarget.loadCtpinFiles() );
            files->setLoadCtpmonFiles( ctpcTarget.loadCtpmonFiles() );
            try {
               CTPFilesLoader& ctpfilesldr =
                  dynamic_cast<CTPFilesLoader&>(m_storageMgr.ctpFilesLoader());
               ctpfilesldr.setLevel(outputLevel());
               if ( !ctpfilesldr.load( *files ) ) {
                  TRG_MSG_ERROR("Error loading CTP files id " << files->id() << " and smx id " << smxid );
                  delete files;
                  commitSession();
                  throw std::runtime_error( "CTPConfigLoader: Error loading CTP files" );
               }
               ctpcTarget.setCTPFiles( files );
            } 
            catch (std::bad_cast& ex) {
               TRG_MSG_ERROR("Caught exception in CTPConfigLoader : " << ex.what());
               commitSession();
               throw std::runtime_error( "CTPConfigLoader: Error casting CTPFilesLoader" );
            }
         } else {
            TRG_MSG_ERROR("Error loading CTP files, no files pointed to by the menu. CTP files id = " << filesid << ", SMX files id = " << smxid);
         }
 
         CTPFiles * files = ctpcTarget.ctpfiles();
         if( files != nullptr ) {
            TRG_MSG_INFO("Loaded CTPfiles. CAM size=" << files->ctpcoreCAM().size() << ", LUT size=" << files->ctpcoreCAM().size());
         }

         //load the priorityset
         if(isRun1()) {
            try {
               PrioritySetLoader& prsldr = dynamic_cast<PrioritySetLoader&>(m_storageMgr.prioritySetLoader());
               PrioritySet prs;
               prs.setLvl1MasterTableId(ctpcTarget.lvl1MasterTableId());
               if ( !prsldr.load( prs ) ) {
                  TRG_MSG_ERROR("Can't load PrioritySet " << prs.lvl1MasterTableId());
                  commitSession();
                  throw std::runtime_error( "CTPConfigLoader: Error loading PrioritySet" );
               }
               ctpcTarget.setPrioritySet(prs);
            } catch (std::bad_cast& ex) {
               TRG_MSG_ERROR("Caught exception in CTPConfigLoader : " << ex.what() );
               commitSession();
               throw std::runtime_error( "CTPConfigLoader: Error casting PrioritySetLoader" );
            }
         }
      } else {
         TRG_MSG_INFO("No loading of CTPFiles");
      }



      // now load the data
      {
         MenuLoader& mldr = dynamic_cast<MenuLoader&>(m_storageMgr.menuLoader());
         mldr.setLevel(outputLevel());
         Menu& m = ctpcTarget.menu();
         m.setId(menuid);
         m.setSuperMasterTableId(ctpcTarget.superMasterTableId());
         if ( !mldr.load( m ) ) {
            TRG_MSG_ERROR("failed loading Menu " << m.id());
            throw std::runtime_error( "CTPConfigLoader:                  ERROR loading Menu" );
         }
         if(isRun1()) {
            ctpcTarget.menu().thresholdConfig().attributeThresholdNumbers();
         }
      }


      {
         PrescaleSetLoader& psldr = dynamic_cast<PrescaleSetLoader&>(m_storageMgr.prescaleSetLoader());
         psldr.setLevel(outputLevel());
         PrescaleSet pss = ctpcTarget.prescaleSet();
         if( prescalesetid > 0 && (schema > 6 || isRun2()) ) { //defined in old schema
            TRG_MSG_INFO("loding LVL1 prescales.");
            pss.setId(prescalesetid);
            if ( !psldr.load( ctpcTarget.ctpVersion(), pss ) ) {
               TRG_MSG_ERROR("CTPConfigLoader: Error loading PrescaleSet " << pss.id());
               throw std::runtime_error( "CTPConfigLoader: Error loading PrescaleSet" );
            }
            ctpcTarget.setPrescaleSet(pss);
         } else {
            TRG_MSG_INFO("Not loading LVL1 prescales. PSK = " << prescalesetid);
         }
      }

         
      {
         BunchGroupSetLoader& bgsldr = dynamic_cast<BunchGroupSetLoader&>(m_storageMgr.bunchGroupSetLoader());
         bgsldr.setLevel(outputLevel());
         BunchGroupSet bgs = ctpcTarget.bunchGroupSet();
         if( bunchgroupsetid > 0 && (schema > 6 || isRun2())) { //proby won't work in old schema
            bgs.setId(bunchgroupsetid);
            if ( !bgsldr.load( bgs ) ) {
               TRG_MSG_ERROR("loading BunchGroupSet " << bgs.id());
               throw std::runtime_error( "CTPConfigLoader: Error loading BunchGroupSet" );
            }
            ctpcTarget.setBunchGroupSet(bgs);
         }
      }

            
      {
         PrescaledClockLoader& pscldr = dynamic_cast<PrescaledClockLoader&>(m_storageMgr.prescaledClockLoader());
         pscldr.setLevel(outputLevel());
         PrescaledClock psc;
         psc.setId(prescaledclockid);
         if ( !pscldr.load( psc ) ) {
            TRG_MSG_ERROR("Can't load PrescaledClock " << psc.id() );
            throw std::runtime_error( "CTPConfigLoader: Error loading PrescaledClock" );
         }
         ctpcTarget.setPrescaledClock(psc);
      }


      if(isRun1() ) {
         DeadTimeLoader& dtldr = dynamic_cast<DeadTimeLoader&>(m_storageMgr.deadTimeLoader());
         dtldr.setLevel(outputLevel());
         DeadTime dt;
         dt.setId(deadtimeid);
         if ( !dtldr.load( dt ) ) {
            TRG_MSG_ERROR("CTPConfigLoader: Error loading DeadTime " << dt.id() );
            throw std::runtime_error( "CTPConfigLoader: Error loading DeadTime" );
         }
         ctpcTarget.setDeadTime(dt);
      }


      {
         RandomLoader& rldr = dynamic_cast<RandomLoader&>(m_storageMgr.randomLoader());
         rldr.setLevel(outputLevel());
         Random r;
         r.setId(randomid);
         if ( !rldr.load( r ) ) {
            TRG_MSG_ERROR("while loading Random " << r.id());
            throw std::runtime_error( "CTPConfigLoader: Error loading Random" );
         }
         ctpcTarget.setRandom(r);
      }

   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      m_session.transaction().rollback();
      throw;
   }
   catch (std::bad_cast& ex) {
      TRG_MSG_ERROR("Cast exception: " << ex.what());
      throw;
   }

   return true;
}

