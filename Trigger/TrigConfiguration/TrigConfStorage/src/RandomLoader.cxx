/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./RandomLoader.h"
#include "./DBHelper.h"
#include "TrigConfL1Data/Random.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include <boost/lexical_cast.hpp>

using namespace std;

bool TrigConf::RandomLoader::load( Random& rdTarget ) {

   TRG_MSG_INFO("Start loading data with ID = " << rdTarget.id());
      
   try {
      startSession();
      
      unique_ptr<coral::IQuery> q(m_session.nominalSchema().tableHandle( "L1_RANDOM").newQuery());
      q->setRowCacheSize( 5 );
	
      coral::AttributeList bindList;
      bindList.extend<long>("rdId");
      bindList[0].data<long>() = rdTarget.id();
      
      q->setCondition( "L1R_ID = :rdId", bindList );

      //Output data and types
      coral::AttributeList attList;
      if(isRun1()) {
         attList.extend<std::string>( "L1R_NAME" ); 
         attList.extend<int>( "L1R_VERSION" ); 
         attList.extend<long>( "L1R_RATE1" ); 
         attList.extend<long>( "L1R_RATE2" ); 
      } else {
         attList.extend<string>( "L1R_CUT0" );
         attList.extend<string>( "L1R_CUT1" );
         attList.extend<string>( "L1R_CUT2" );
         attList.extend<string>( "L1R_CUT3" );
      }
      fillQuery(q.get(), attList);

      coral::ICursor& cursor = q->execute();
      
      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("No such random exists " << rdTarget.id());
         commitSession();
         throw std::runtime_error( "RandomLoader >> Random not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      if(isRun1()) {
         std::string name = row["L1R_NAME"].data<std::string>();
         int version      = row["L1R_VERSION"].data<int>();
         long rate1       = row["L1R_RATE1"].data<long>();
         long rate2       = row["L1R_RATE2"].data<long>();
         // Fill the object with data
         rdTarget.setName( name );
         rdTarget.setVersion( version );
         rdTarget.setRate1( rate1 );
         rdTarget.setRate2( rate2 );
      } else {
         string cut0        = row["L1R_CUT0"].data<string>();
         string cut1        = row["L1R_CUT1"].data<string>();
         string cut2        = row["L1R_CUT2"].data<string>();
         string cut3        = row["L1R_CUT3"].data<string>();
         // Fill the object with data
         rdTarget.setCut( 0, boost::lexical_cast<uint32_t,string>( cut0 ) );
         rdTarget.setCut( 1, boost::lexical_cast<uint32_t,string>( cut1 ) );
         rdTarget.setCut( 2, boost::lexical_cast<uint32_t,string>( cut2 ) );
         rdTarget.setCut( 3, boost::lexical_cast<uint32_t,string>( cut3 ) );
      }

      if ( cursor.next() ) {
         TRG_MSG_ERROR("More than one Random exists " << rdTarget.id());
         commitSession();
         throw std::runtime_error( "RandomLoader >>  Random not available" );
      }

      commitSession();
      return true;
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      m_session.transaction().rollback();
      throw;
   }
}
