// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     MuctpiLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//UPDATED:  8 Dec 2008 Paul Bell for sqlite access 
//          (use of defineOutput method to set data types)
//
//////////////////////////////////////////////////////////////////////

#include "./MuctpiLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITableDescription.h"

#include "TrigConfL1Data/Muctpi.h"

#include <sstream>
#include <stdexcept>
#include <typeinfo>

bool 
TrigConf::MuctpiLoader::load( Muctpi& mTarget ) {

  if(verbose())
    msg() << "MuctpiLoader:                     Loading MuCTPi with ID = " << mTarget.id() << " and Lvl1 masterId = " << mTarget.lvl1MasterTableId() << std::endl;
  try {
    startSession();
    if ( mTarget.lvl1MasterTableId() ) {
      // get the muctpi id for this mastertable
      long muctpi_id = 0;

      std::unique_ptr<coral::IQuery> query0(m_session.nominalSchema().tableHandle("L1_MASTER_TABLE").newQuery());
      query0->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<long>("mtId");
      std::string cond = "L1MT_ID = :mtId";
      bindList[0].data<long>() = mTarget.lvl1MasterTableId();
      query0->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<long>( "L1MT_MUCTPI_INFO_ID" );
      query0->defineOutput(attList);
      query0->addToOutputList( "L1MT_MUCTPI_INFO_ID" );

      coral::ICursor& cursor0 = query0->execute();

      if ( ! cursor0.next() ) {
        msg() << "MuctpiLoader >> No such Master_Table exists " << mTarget.lvl1MasterTableId() << std::endl;
        commitSession();
        throw std::runtime_error( "MuctpiLoader >> Muctpi_Info not available" );
      }
	
      const coral::AttributeList& row0 = cursor0.currentRow();
      muctpi_id = row0["L1MT_MUCTPI_INFO_ID"].data<long>();
	
      if ( cursor0.next() ) {
        msg() << "MuctpiLoader >> More than one Muctpi_Info exists for this master_table id" 
              << mTarget.lvl1MasterTableId() << std::endl;
        commitSession();
        throw std::runtime_error( "MuctpiLoader >>   Muctpi_Info not available" );
      }
      mTarget.setId(muctpi_id);
    }
      
    std::unique_ptr<coral::IQuery> query(m_session.nominalSchema().tableHandle( "L1_MUCTPI_INFO").newQuery());
    query->setRowCacheSize( 5 );

    //Bind list
    coral::AttributeList bindList2;
    bindList2.extend<long>("miId");
    std::string cond2 = "L1MI_ID = :miId";
    bindList2[0].data<long>() = mTarget.id();
    query->setCondition( cond2, bindList2 );

    //Output data and types
    coral::AttributeList attList;
    attList.extend<int>( "L1MI_VERSION" );
    attList.extend<std::string>( "L1MI_NAME" );
    attList.extend<int>( "L1MI_LOW_PT" );
    attList.extend<int>( "L1MI_HIGH_PT" );
    attList.extend<int>( "L1MI_MAX_CAND" );
    attList.extend<long>( "L1MI_ID" );
    query->defineOutput(attList);

    query->addToOutputList( "L1MI_VERSION" );
    query->addToOutputList( "L1MI_NAME" );
    query->addToOutputList( "L1MI_LOW_PT" );
    query->addToOutputList( "L1MI_HIGH_PT" );
    query->addToOutputList( "L1MI_MAX_CAND" );
    query->addToOutputList( "L1MI_ID" );
    query->addToOrderList( "L1MI_ID" );

    coral::ICursor& cursor = query->execute();
    if ( ! cursor.next() ) {
      msg() << "MuctpiLoader >> No such Muctpi_info exists " << mTarget.id() << std::endl;
      commitSession();
      throw std::runtime_error( "MuctpiLoader >> Muctpi not available" );
    }

    const coral::AttributeList& row = cursor.currentRow();
    std::string name = row["L1MI_NAME"].data<std::string>();
    int version = row["L1MI_VERSION"].data<int>();
    int low_pt = row["L1MI_LOW_PT"].data<int>();
    int high_pt = row["L1MI_HIGH_PT"].data<int>();
    int max_cand = row["L1MI_MAX_CAND"].data<int>();

    if ( cursor.next() ) {
      msg() << "MuctpiLoader >> More than one Muctpi exists "   << mTarget.id() << std::endl;
      commitSession();
      throw std::runtime_error( "MuctpiLoader >>  Muctpi not available" );
    }


    // Fill the object with data
    mTarget.setName( name );
    mTarget.setVersion( version );
    mTarget.setLowptThreshold( low_pt );
    mTarget.setHighptThreshold( high_pt );
    mTarget.setMaxCand( max_cand );
    commitSession();
    return true;
  } catch( const coral::SchemaException& e ) {
    msg() << "MuctpiLoader >> SchemaException: " << e.what() << std::endl;
    m_session.transaction().rollback();
    return false;
  } catch( const std::exception& e ) {
    msg() << "MuctpiLoader >> Standard C++ exception: " << e.what() << std::endl;
    m_session.transaction().rollback();
    return false; 

  } catch( ... ) {
    msg() << "MuctpiLoader >> unknown C++ exception" << std::endl;
    m_session.transaction().rollback();
    return false; 
  }
}
