/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     PrescaleSetLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  25. Jan. 2006   
//                                                     
//PURPOSE:
//UPDATED:  8 Dec 2008 Paul Bell for sqlite access 
//          (use of defineOutput method to set data type)
//
//////////////////////////////////////////////////////////////////////


#include "CTPfragment/CTPdataformatVersion.h"
#include "TrigConfL1Data/L1PSNumber.h"
#include "./PrescaleSetLoader.h"
#include "./DBHelper.h"

#include "boost/lexical_cast.hpp"

using namespace std;

bool
TrigConf::PrescaleSetLoader::load( unsigned int ctpVersion, PrescaleSet& prescaleSet ) {

   TRG_MSG_INFO("loading L1 prescale set with PSK " << prescaleSet.id());

   CTPdataformatVersion ctpformat(ctpVersion);

   prescaleSet.resize(ctpformat.getMaxTrigItems());

   try {
      startSession();
      unique_ptr< coral::IQuery > q( m_session.nominalSchema().tableHandle( "L1_PRESCALE_SET").newQuery() );
      q->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("psId");
      std::string cond = "L1PS_ID = :psId";
      bindList[0].data<int>() = prescaleSet.id();
      q->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1PS_NAME" );
      attList.extend<int>( "L1PS_VERSION" );
      attList.extend<std::string>( "L1PS_COMMENT" );
      for (unsigned int ctpid = 0; ctpid < ctpformat.getMaxTrigItems(); ++ctpid) {
         attList.extend<int64_t>( "L1PS_VAL" + boost::lexical_cast<string,int>(ctpid+1) );
      }
      fillQuery(q.get(),attList);

      coral::ICursor& cursor = q->execute();
      
      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("No such prescaleset exists " << prescaleSet.id());
         throw std::runtime_error( "PrescaleSetLoader >> PrescaleSet not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name = row["L1PS_NAME"].data<std::string>();
      int version = row["L1PS_VERSION"].data<int>();
      std::string comment = row["L1PS_COMMENT"].data<std::string>();

      // fill the object with data
      prescaleSet.setName( name );
      prescaleSet.setVersion( version );
      prescaleSet.setComment( comment );
      for (unsigned int ctpid=0; ctpid < ctpformat.getMaxTrigItems(); ++ctpid) {
         int64_t val = row["L1PS_VAL" + boost::lexical_cast<string,unsigned int>(ctpid+1)].data<int64_t>();
         if(isRun1()) {
            float prescale = L1PSNumber(val).getFloatPrescale();
            prescaleSet.setPrescale( ctpid, prescale ); 
         } else {
            prescaleSet.setCut(ctpid,val);
         }
      }

      commitSession();
      return true;
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      throw;
   }

   TRG_MSG_DEBUG("loaded L1 prescale set '" << prescaleSet.name() << "' (psk " << prescaleSet.id() << ")" );

   return true;
}
