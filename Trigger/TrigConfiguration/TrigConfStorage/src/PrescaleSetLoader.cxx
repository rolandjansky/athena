/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "./PrescaleSetLoader.h"

#include "./DBHelper.h"

using namespace std;

bool
TrigConf::PrescaleSetLoader::load( PrescaleSet& psTarget ) {

   if(verbose())
      msg() << "PrescaleSetLoader:                started loading data via ID. ID = " << psTarget.id() << std::endl;

   psTarget.resize(256); // TODO: adapt to new style and new CTP


   try {
      startSession();
      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_PRESCALE_SET");
      unique_ptr< coral::IQuery > query( table.newQuery() );
      query->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("psId");
      std::string cond = "L1PS_ID = :psId";
      bindList[0].data<int>() = psTarget.id();
      query->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1PS_NAME" );
      attList.extend<int>( "L1PS_VERSION" );
      attList.extend<std::string>( "L1PS_COMMENT" );
      query->addToOutputList( "L1PS_NAME" );
      query->addToOutputList( "L1PS_VERSION" );
      query->addToOutputList( "L1PS_COMMENT" );
      //char buffer0[50];
      for (unsigned int ij = 0; ij < PrescaleSet::N_PRESCALES; ++ij) {
         std::ostringstream formatedoutput;
         formatedoutput << "L1PS_VAL" << ij+1;

         //             std::string os= "L1PS_VAL";
         //             sprintf (buffer0, "%d", ij+1);
         //             os+=buffer0;
         //            attList.extend<int>( os );
         // TODO: check coral documentation. Tiago.
         // AttributeList.extend<Type>("NAME")
         attList.extend<int64_t>( formatedoutput.str() );
         query->addToOutputList( formatedoutput.str() );
      }
      query->defineOutput(attList);


      coral::ICursor& cursor = query->execute();
      

      if ( ! cursor.next() ) {
         msg() << "PrescaleSetLoader >> No such prescaleset exists " << psTarget.id() << std::endl;
         throw std::runtime_error( "PrescaleSetLoader >> PrescaleSet not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name = row["L1PS_NAME"].data<std::string>();
      int version = row["L1PS_VERSION"].data<int>();
      std::string comment = row["L1PS_COMMENT"].data<std::string>();

      // fill the object with data
      psTarget.setName( name );
      psTarget.setVersion( version );
      psTarget.setComment( comment );
      char buffer [50];
      for (unsigned int ij=0; ij < PrescaleSet::N_PRESCALES; ++ij) {
         std::string os= "L1PS_VAL";
         sprintf (buffer, "%d", ij+1);
         os+=buffer;
         psTarget.setPrescale(ij,row[os].data<int64_t>());
      }

      if ( cursor.next() ) {
         msg() << "PrescaleSetLoader >> More than one PrescaleSet exists " 
               << psTarget.id() << std::endl;
         throw std::runtime_error( "PrescaleSetLoader >>  PrescaleSet not available" );
      }

      commitSession();
      return true;
   } catch( const coral::Exception& e ) {
      msg() << "PrescaleSetLoader >> SchemaException: " 

            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;

   } catch( const std::exception& e ) {
      msg() << "PrescaleSetLoader >> Standard C++ exception: " << e.what() << std::endl;

      m_session.transaction().rollback();
      return false; 

   } catch( ... ) {
      msg() << "PrescaleSetTimeLoader >> Unknown C++ exception" << std::endl;

      m_session.transaction().rollback();
      return false; 
   }
}
