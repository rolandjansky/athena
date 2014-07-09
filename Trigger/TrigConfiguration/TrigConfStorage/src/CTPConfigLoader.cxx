/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/CTPConfig.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool TrigConf::CTPConfigLoader::load(CTPConfig& ctpcTarget) {
   TRG_MSG_INFO("Loading data for L1 master key " << ctpcTarget.lvl1MasterTableId() 
                << " L1 PSS key " << ctpcTarget.prescaleSetId()  
                << " L1 BGS key " << ctpcTarget.bunchGroupSetId() );

   try {
      unsigned int schema = triggerDBSchemaVersion();
      TRG_MSG_DEBUG("TriggerDB schema: " << schema);

      startSession();

      long prescalesetid = ctpcTarget.prescaleSetId();
      // get menu id from ctpcTarget.m_MasterTableId and the table L1_MASTER_TABLE
      int menuid = 0;
      long bunchgroupsetid = 0;
      long prescaledclockid = 0;
      long deadtimeid = 0;
      long randomid = 0;
        
      coral::ITable& table0 = m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE");
      coral::IQuery* query0 = table0.newQuery();
      query0->setRowCacheSize( 5 );
        
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("mtId");
      std::string cond0 = "L1MT_ID = :mtId";
      bindList0[0].data<int>() = ctpcTarget.lvl1MasterTableId();
      query0->setCondition( cond0, bindList0 );

      //Output data and types
      coral::AttributeList attList0;
      attList0.extend<int>( "L1MT_TRIGGER_MENU_ID" );
      if(schema <= 6) attList0.extend<long>( "L1MT_BUNCH_GROUP_SET_ID" );
      attList0.extend<long>( "L1MT_PRESCALED_CLOCK_ID" );
      attList0.extend<long>( "L1MT_DEAD_TIME_ID" );
      attList0.extend<long>( "L1MT_RANDOM_ID" );
      query0->defineOutput(attList0);

      query0->addToOutputList( "L1MT_TRIGGER_MENU_ID" );
      if(schema <= 6) query0->addToOutputList( "L1MT_BUNCH_GROUP_SET_ID" );
      query0->addToOutputList( "L1MT_PRESCALED_CLOCK_ID" );
      query0->addToOutputList( "L1MT_DEAD_TIME_ID" );
      query0->addToOutputList( "L1MT_RANDOM_ID" );
        
      coral::ICursor& cursor0 = query0->execute();
        
      if ( ! cursor0.next() ) {
         TRG_MSG_ERROR("No such Master_Table exists " << ctpcTarget.lvl1MasterTableId() );
         delete query0;
         throw std::runtime_error( "CTPConfigLoader:                  CTPConfig not available" );
      }
        
      const coral::AttributeList& row0 = cursor0.currentRow();
      menuid           = row0["L1MT_TRIGGER_MENU_ID"].data<int>();
      if(schema <= 6) {
         bunchgroupsetid = row0["L1MT_BUNCH_GROUP_SET_ID"].data<long>();
      }else{
         bunchgroupsetid = ctpcTarget.bunchGroupSetId();
      }
      prescaledclockid = row0["L1MT_PRESCALED_CLOCK_ID"].data<long>();
      deadtimeid       = row0["L1MT_DEAD_TIME_ID"].data<long>();
      randomid         = row0["L1MT_RANDOM_ID"].data<long>();
        
      if ( cursor0.next() ) {
         TRG_MSG_ERROR( "More than one Master_Table exists " << ctpcTarget.lvl1MasterTableId() );
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Unique MasterTable not available" );
      }

      TRG_MSG_INFO("Skip loading CTPFiles and PrioritySet!");
      TRG_MSG_VERBOSE("To load CTPFiles and PrioritySet (for online running or debugging), add this line to your source code: ");
      TRG_MSG_VERBOSE("TrigConf::DBLoader::setEnv(TrigConf::DBLoader::CTPOnl);");

      if (DBLoader::getEnv() == DBLoader::ALL ||
          DBLoader::getEnv() == DBLoader::CTPOnl) {

         // get the lutCam id
         coral::ITable& table1 = m_session.nominalSchema().tableHandle( "L1_TRIGGER_MENU");
         coral::IQuery* query1 = table1.newQuery();
         query1->setRowCacheSize( 5 );

         //Bind list
         coral::AttributeList bindList;
         bindList.extend<int>("menuId");
         std::string cond = "L1TM_ID = :menuId";
         bindList[0].data<int>() = menuid;
         query1->setCondition( cond, bindList );

         //Output data and types
         coral::AttributeList attList1;
         attList1.extend<long>( "L1TM_CTP_FILES_ID" );
         attList1.extend<long>( "L1TM_CTP_SMX_ID" );
         query1->defineOutput(attList1);
         query1->addToOutputList( "L1TM_CTP_FILES_ID" );
         query1->addToOutputList( "L1TM_CTP_SMX_ID" );

         coral::ICursor& cursor1 = query1->execute();

         if ( ! cursor1.next() ) {
            msg() << "CTPConfigLoader >> No such Trigger_Menu exists " << menuid << std::endl;
            delete query0;
            delete query1;
            commitSession();
            throw std::runtime_error( "CTPConfigLoader >> CTPConfig not available" );
         }
       
         const coral::AttributeList& row1 = cursor1.currentRow();
         long filesid = row1["L1TM_CTP_FILES_ID"].data<long>();
         long smxid = row1["L1TM_CTP_SMX_ID"].data<long>();

         if(filesid != 0) {
            if( ctpcTarget.loadCtpFiles() ) {
               try {
                  CTPFilesLoader& ctpfilesldr =
                     dynamic_cast<CTPFilesLoader&>(m_storageMgr.ctpFilesLoader());
                  CTPFiles* files = new CTPFiles();
                  files->setId( filesid );
                  files->setSmxId( smxid );
                  files->setLoadCtpcoreFiles(ctpcTarget.loadCtpcoreFiles());
                  files->setLoadCtpinFiles(ctpcTarget.loadCtpinFiles());
                  files->setLoadCtpmonFiles(ctpcTarget.loadCtpmonFiles());

                  if ( !ctpfilesldr.load( *files ) ) {
                     msg() << "CTPConfigLoader: Error loading CTP files "
                           << files->id() << std::endl;
                     delete query0;
                     delete query1;
                     delete files;
                     commitSession();
                     throw std::runtime_error( "CTPConfigLoader: Error loading CTP files" );
                  }
                  ctpcTarget.setCTPFiles(files);
               } catch (std::bad_cast& ex) {
                  msg() << "Caught exception in CTPConfigLoader : "
                        << ex.what() << std::endl;
                  delete query0;
                  delete query1;
                  commitSession();
                  throw std::runtime_error( "CTPConfigLoader: Error casting CTPFilesLoader" );
               }
            } else {
               msg() << "CTPConfigLoader: Skip loading CTP files." << std::endl;
               delete query0;
               delete query1;
            }
         } else {
            msg() << "CTPConfigLoader: Error loading CTP files "
                  << filesid << " : no files stored in database." << std::endl;
            delete query0;
            delete query1;
         } 

         //load the priorityset
         try {
            PrioritySetLoader& prsldr = dynamic_cast<PrioritySetLoader&>(m_storageMgr.prioritySetLoader());
            PrioritySet prs;
            prs.setLvl1MasterTableId(ctpcTarget.lvl1MasterTableId());
            if ( !prsldr.load( prs ) ) {
               msg() << "CTPConfigLoader: Error loading PrioritySet "
                     << prs.lvl1MasterTableId() << std::endl;
               delete query0;
               delete query1;
               commitSession();
               throw std::runtime_error( "CTPConfigLoader: Error loading PrioritySet" );
            }
            ctpcTarget.setPrioritySet(prs);
         } catch (std::bad_cast& ex) {
            msg() << "Caught exception in CTPConfigLoader : "
                  << ex.what() << std::endl;
            delete query0;
            delete query1;
            commitSession();
            throw std::runtime_error( "CTPConfigLoader: Error casting PrioritySetLoader" );
         }
         delete query1;
      } // ENV

      // now load the data
      try{
         MenuLoader& mldr = dynamic_cast<MenuLoader&>(m_storageMgr.menuLoader());
         mldr.setVerbose(verbose());
         Menu& m = ctpcTarget.menu();
         m.setId(menuid);
         m.setSuperMasterTableId(ctpcTarget.superMasterTableId());
         if ( !mldr.load( m ) ) {
            msg() << "CTPConfigLoader:                  ERROR loading Menu " 
                  << m.id() << std::endl;
            delete query0;
            throw std::runtime_error( "CTPConfigLoader:                  ERROR loading Menu" );
         }
      } catch (std::bad_cast& ex) {
         msg() << "Caught exception in CTPConfigLoader : "
               << ex.what() << std::endl;
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Error casting MenuLoader" );
      }
        
      try {
         //PJB 21.01.09
         PrescaleSetLoader& psldr = dynamic_cast<PrescaleSetLoader&>(m_storageMgr.prescaleSetLoader());
         psldr.setVerbose(verbose());
         PrescaleSet pss = ctpcTarget.prescaleSet();
         if( prescalesetid > 0 && schema > 6) { //defined in old schema
            pss.setId(prescalesetid);
            if ( !psldr.load( pss ) ) {
               msg() << "CTPConfigLoader: Error loading PrescaleSet " 
                     << pss.id() << std::endl;
               delete query0;
               throw std::runtime_error( "CTPConfigLoader: Error loading PrescaleSet" );
            }
            ctpcTarget.setPrescaleSet(pss);
            if(verbose())
               msg() << "CTPConfigLoader:                  got prescales." << std::endl;
         } else {
            if(verbose())
               msg() << "CTPConfigLoader:                  dont load LVL1 prescales." << std::endl;
         }
      } catch (std::bad_cast& ex) {
         msg() << "Caught exception in CTPConfigLoader : "
               << ex.what() << std::endl;
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Error casting PrescaleSetLoader" );
      }
         
      try {
         BunchGroupSetLoader& bgsldr = dynamic_cast<BunchGroupSetLoader&>(m_storageMgr.bunchGroupSetLoader());
         bgsldr.setVerbose(verbose());
         BunchGroupSet bgs = ctpcTarget.bunchGroupSet();
         if( bunchgroupsetid > 0 && schema > 6) { //proby won't work in old schema
            bgs.setId(bunchgroupsetid);
            if ( !bgsldr.load( bgs ) ) {
               msg() << "CTPConfigLoader: Error loading BunchGroupSet " 
                     << bgs.id() << std::endl;
               throw std::runtime_error( "CTPConfigLoader: Error loading BunchGroupSet" );
            }
            ctpcTarget.setBunchGroupSet(bgs);
         }
      } catch (std::bad_cast& ex) {
         msg() << "Caught exception in CTPConfigLoader : "
               << ex.what() << std::endl;
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Error casting BunchGroupSetLoader" );
      }
            
      try {
         PrescaledClockLoader& pscldr = dynamic_cast<PrescaledClockLoader&>(m_storageMgr.prescaledClockLoader());
         pscldr.setVerbose(verbose());
         PrescaledClock psc;
         psc.setId(prescaledclockid);
         if ( !pscldr.load( psc ) ) {
            msg() << "CTPConfigLoader: Error loading PrescaledClock " 
                  << psc.id() << std::endl;
            delete query0;
            throw std::runtime_error( "CTPConfigLoader: Error loading PrescaledClock" );
         }
         ctpcTarget.setPrescaledClock(psc);
      } catch (std::bad_cast& ex) {
         msg() << "Caught exception in CTPConfigLoader : "
               << ex.what() << std::endl;
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Error casting PrescaledClockLoader" );
      }
      try {
         DeadTimeLoader& dtldr = dynamic_cast<DeadTimeLoader&>(m_storageMgr.deadTimeLoader());
         dtldr.setVerbose(verbose());
         DeadTime dt;
         dt.setId(deadtimeid);
         if ( !dtldr.load( dt ) ) {
            msg() << "CTPConfigLoader: Error loading DeadTime " 
                  << dt.id() << std::endl;
            delete query0;
            throw std::runtime_error( "CTPConfigLoader: Error loading DeadTime" );
         }
         ctpcTarget.setDeadTime(dt);
      }
      catch (std::bad_cast& ex) {
         TRG_MSG_ERROR( "Caught exception in CTPConfigLoader : " << ex.what() );
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Error casting DeadTimeLoader" );
      }
      try {
         RandomLoader& rldr = dynamic_cast<RandomLoader&>(m_storageMgr.randomLoader());
         rldr.setVerbose(verbose());
         Random r;
         r.setId(randomid);
         if ( !rldr.load( r ) ) {
            msg() << "CTPConfigLoader: Error loading Random " 
                  << r.id() << std::endl;
            delete query0;
            throw std::runtime_error( "CTPConfigLoader: Error loading Random" );
         }
         ctpcTarget.setRandom(r);
      } catch (std::bad_cast& ex) {
         msg() << "Caught exception in CTPConfigLoader : "
               << ex.what() << std::endl;
         delete query0;
         throw std::runtime_error( "CTPConfigLoader >>  Error casting RandomLoader" );
      }
        
      delete query0;
      return true;
   }
   catch( const coral::SchemaException& e ) {
      msg() << "CTPConfigLoader  >> SchemaException: " 
            << e.what() << std::endl;
      m_session.transaction().rollback();
      throw;
      return false;
   }
   catch( const std::exception& e ) {
      msg() << "CTPConfigLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      throw;
      return false; 
   }
   catch( ... ) {
      msg() << "CTPConfigLoader >> unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      throw;
      return false; 
   }
}

