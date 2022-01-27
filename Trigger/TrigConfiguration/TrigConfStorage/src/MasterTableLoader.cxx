/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/CTPConfig.h"

#include "./DBHelper.h"

#include "./MasterTableLoader.h"
#include "./ThresholdConfigLoader.h"
#include "./CTPConfigLoader.h"
#include "./CTPConfigOnlineLoader.h"
#include "./MuctpiLoader.h"
#include "L1TopoMenuLoader.h"

using namespace std;

bool
TrigConf::MasterTableLoader::loadMasterKeys(int SuperMasterKey, int& Lvl1MasterKey, std::string & menuName) {
   try {

      startSession();
	
      coral::ITable& table = m_session.nominalSchema().tableHandle( "SUPER_MASTER_TABLE");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );
   
      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("smtid");
      bindList[0].data<int>() = SuperMasterKey;
      query->setCondition( "SMT_ID = :smtid" , bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<int>( "SMT_L1_MASTER_TABLE_ID" );
      attList.extend<std::string>( "SMT_NAME" );
      query->defineOutput(attList);
      query->addToOutputList( "SMT_L1_MASTER_TABLE_ID" );
      query->addToOutputList( "SMT_NAME" );

      coral::ICursor& cursor = query->execute();

      if ( ! cursor.next() ) {
         msg() << "MasterTableLoader >> No such SuperMaster key exists " << SuperMasterKey << std::endl; 
         delete query;
         commitSession();
         throw std::runtime_error( "MasterTableLoader >> SuperMasterKey not available" );
      }
	
      const coral::AttributeList& row = cursor.currentRow();
      Lvl1MasterKey = row["SMT_L1_MASTER_TABLE_ID"].data<int>();
      menuName = row["SMT_NAME"].data<std::string>();

      delete query;
      commitSession();
   }
   catch( const coral::SchemaException& e ) {
      msg() << "BunchGroupLoader >> SchemaException: " << e.what() << std::endl;
      m_session.transaction().rollback();
      throw;
   } catch( const std::exception& e ) {
      msg() << "BunchGroupLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      throw;
   } catch( ... ) {
      msg() << "BunchGroupLoader >> Unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      throw;
   }
   return true; 
}

bool
TrigConf::MasterTableLoader::load(ThresholdConfig& thrcfg) {

   if ( !thrcfg.lvl1MasterTableId() ) {
      int SuperMasterKey = thrcfg.superMasterTableId();
      if(SuperMasterKey==0) {
         msg() << "MasterTableLoader: Error loading ThresholdConfig" << std::endl;
         msg() << ">>no super- or lvl1- master key is specified" << std::endl;
         return false;
      }
      int Lvl1MasterKey(0);
      std::string menuName{""};
      loadMasterKeys(SuperMasterKey, Lvl1MasterKey, menuName);
      thrcfg.setLvl1MasterTableId(Lvl1MasterKey);
   }
   try {
      ThresholdConfigLoader& thrLoader = 
         dynamic_cast<ThresholdConfigLoader&>(m_storageMgr.thresholdConfigLoader());
      thrLoader.setVerbose(DBLoader::verbose());
      return thrLoader.load(thrcfg);
   } catch (std::bad_cast& ex) {
      msg() << "Caught exception in MasterTableLoader : "
            << ex.what() << std::endl;
      return false;
   }
}

bool
TrigConf::MasterTableLoader::load(CTPConfig& ctpc) {

   if ( !ctpc.lvl1MasterTableId() ) {
      int SuperMasterKey = ctpc.superMasterTableId();
      if(SuperMasterKey==0) {
         msg() << "MasterTableLoader: Error loading CTPConfig" << std::endl;
         msg() << ">>no super- or lvl1- master key is specified" << std::endl;
         return false;
      }
      int Lvl1MasterKey(0);

      msg() << "Load L1 master key" << std::endl;
      std::string menuName{""};
      loadMasterKeys(SuperMasterKey, Lvl1MasterKey, menuName);

      msg() << "Loaded L1 master key" << Lvl1MasterKey << std::endl;

      ctpc.setLvl1MasterTableId(Lvl1MasterKey);
      ctpc.setName(menuName);
   }
   try {
      CTPConfigLoader& ctpLoader = 
         dynamic_cast<CTPConfigLoader&>(m_storageMgr.ctpConfigLoader());
      ctpLoader.setLevel(outputLevel());
      try {
         return ctpLoader.load(ctpc);
      } catch ( const std::exception& e ) {
         msg() << "MasterTableLoader >> Failed to load CTPConfig: " 
               << e.what() << std::endl;
         return false;
      }
   } catch (std::bad_cast& ex) {
      msg() << "Caught exception in MasterTableLoader : "
            << ex.what() << std::endl;
      return false;
   }
}

bool
TrigConf::MasterTableLoader::load(Muctpi& m) {
    
   if ( !m.lvl1MasterTableId() ) {
      int SuperMasterKey = m.superMasterTableId();
      if(SuperMasterKey==0) {
         msg() << "MasterTableLoader: Error loading Muctpi since no L1MasterKey nor a SMK is specified" << std::endl;
         return false;
      }
      int Lvl1MasterKey(0);
      std::string menuName{""};
      loadMasterKeys(SuperMasterKey, Lvl1MasterKey, menuName);
      m.setLvl1MasterTableId(Lvl1MasterKey);
   }
   try {
      MuctpiLoader& muctpiLoader = dynamic_cast<MuctpiLoader&>( m_storageMgr.muctpiLoader() );
      muctpiLoader.setLevel(outputLevel());
      return muctpiLoader.load(m);
   } catch (std::bad_cast& ex) {
      msg() << "Caught exception in MasterTableLoader : "
            << ex.what() << std::endl;
      return false;
   }
}

bool TrigConf::MasterTableLoader::load(TXC::L1TopoMenu& l1topo){
   try {
      L1TopoMenuLoader& l1topoLoader = dynamic_cast<L1TopoMenuLoader&>(m_storageMgr.l1topoMenuLoader());
      l1topoLoader.setLevel(outputLevel());
      return l1topoLoader.load(l1topo);
   } catch (std::exception& ex) {
      msg() << "Caught exception in MasterTableLoader : "
            << ex.what() << std::endl;
      return false;
   }
}

  
int
TrigConf::MasterTableLoader::mt_id_for_next_run() {
    
   msg() << "MasterTableLoader loading mt_id for next run " << std::endl;
    
   try {
     
      if ( ! m_session.transaction().isActive() )
         m_session.transaction().start(true);
        
      coral::AttributeList emptyBindVariableList0;
      coral::ITable& table0 = m_session.nominalSchema().tableHandle( "TRIGGER_NEXT_RUN");
      coral::IQuery* query0 = table0.newQuery();
      query0->setRowCacheSize( 5 );
        
      std::string whereclause0= "TNR_ID = 1";
        
      query0->setCondition( whereclause0, emptyBindVariableList0 );
        
      query0->addToOutputList( "TNR_SUPER_MASTER_TABLE_ID" );
        
      coral::ICursor& cursor0 = query0->execute();
        
      if ( ! cursor0.next() ) {
         msg() << "MasterTableLoader >> No Entry in NEXT_RUN Table " << std::endl;
         delete query0;
          
         throw std::runtime_error( "MasterTableLoader >> no entry in NEXT_RUN table" );
      }
        
      const coral::AttributeList& row0 = cursor0.currentRow();
      int supermastertableid = 0;
      supermastertableid = row0["TNR_SUPER_MASTER_TABLE_ID"].data<int>();
        
      if ( cursor0.next() ) {
         msg() << "MasterTableLoader >> More than one entry in NEXT_RUNtable" << std::endl;
         delete query0;
          
         throw std::runtime_error( "MasterTableLoader >> More than one entry in NEXT_RUNtable" );
      }
        
      // now get the master_table_id from the super_master_table
        
      msg() << "MasterTableLoader>> getting data for super_mt_id = " << supermastertableid << std::endl;
        
      coral::ITable& table = m_session.nominalSchema().tableHandle( "SUPER_MASTER_TABLE");
        
      unique_ptr< coral::IQuery > query( table.newQuery() );
      query->setRowCacheSize( 5 );
      coral::AttributeList emptyBindVariableList;
        
      std::string os= "SMT_ID = ";
      std::ostringstream lStringOS;
      lStringOS <<  supermastertableid;
      os += lStringOS.str();
        
      query->setCondition( os , emptyBindVariableList );
        
      query->addToOutputList( "SMT_L1_MASTER_TABLE_ID" );	
        
      coral::ICursor& cursor = query->execute();
        
      if ( ! cursor.next() ) {
         msg() << "MasterTableLoader >> No such Super_Master_table exists " << supermastertableid << std::endl;
         delete query0;
          
         throw std::runtime_error( "MasterTableLoader >> No such Super_Master_table exists " );
      }
        
      const coral::AttributeList& row = cursor.currentRow();
      int mt_id = 0;
      mt_id = row["SMT_L1_MASTER_TABLE_ID"].data<int>();
        
      if ( cursor.next() ) {
         msg() << "MasterTableLoader >> More than one Super_Master_table exists " 
               << supermastertableid << std::endl;
         throw std::runtime_error( "MasterTableLoader >> More than one Super_Master_table exists " );
      }
      delete query0;
      return mt_id;
   }	
   catch( const coral::SchemaException& e )
      {
         msg() << "CTPConfigLoader >> SchemaException: " 
               << e.what() << std::endl;
         m_session.transaction().rollback();
         return false;
      }
   catch( const std::exception& e )
      {
         msg() << "CTPConfigLoader >> Standard C++ exception: " << e.what() << std::endl;
         m_session.transaction().rollback();
         return false; 
      }
   catch( ... )
      {
         msg() << "CTPConfigLoader >> unknown C++ exception" << std::endl;
         m_session.transaction().rollback();
         return false; 
      }
    
   return true;    
}

