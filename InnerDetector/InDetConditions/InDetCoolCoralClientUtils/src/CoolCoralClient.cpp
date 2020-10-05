/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////
///
/// CoolCoralclient.cpp
///
/// C++ class for creating the TRT DB schema
/// fill the DB, read it back
///
/// This class manages CORAL only type of access
/// as well as COOL+CORAL type of access.
///
/// This class does not use CORACOOL package
///
/// Created by Ruslan Mashinistov in 2006
/// Modified by Ruslan and Beniamino in 2007
///
///////////////////////////////////////////////////

// GENERIC C++/C headers
#include <stddef.h>                     // for size_t
#include <string.h>                     // for strlen
#include <sys/time.h>                   // for timeval, gettimeofday
#include <time.h>                       // for ctime, time_t
#include <cstdio>                       // for NULL, printf, sscanf
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <set>                          // for set, etc
#include <stdexcept>                    // for runtime_error
#include <string>                       // for string, operator<<, etc
#include <vector>                       // for vector, etc

// CORAL API
#include "CoralKernel/Context.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Date.h"

#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITransaction.h"

#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDescription.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IBulkOperation.h"

//#include "AttributeList/AttributeList.h"

// COOL API: database service bootstrap
#include "CoolApplication/Application.h"
#include "CoolApplication/DatabaseSvcFactory.h"

// COOL API include files (CoolKernel)
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/FolderSpecification.h"

//#include "CoolKernel/ExtendedAttributeListSpecification.h"
//#include "CoolKernel/AttributeListSpecification.h"
#include "InDetCoolCoralClientUtils/CORAL-utils.h"
//#include "TRT_defs.h"

//#include "RCDConnectionService.h"
#include "InDetCoolCoralClientUtils/ConfDBif.h"

#include "InDetCoolCoralClientUtils/CoolCoralClient.h"

using namespace cool;
using namespace std;

COOLCORALClient::COOLCORALClient(const std::string& id1) : m_connString(id1), m_session(0)
{ 
  m_verbose = true;

  // Create the RCDConnectionService
  if(m_verbose) std::cout << "Creating Connection Service"<<std::endl;
  // get all IConnectionService's already loaded

  // Instantiate connection service
  coral::Context* context = &coral::Context::instance();

  // Load CORAL connection service
  coral::IHandle<coral::IConnectionService> lookSvcH = context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
      context->loadComponent( "CORAL/Services/ConnectionService" );
      lookSvcH = context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
      throw std::runtime_error( "Could not locate the connection service" );
  }
  /// connection to CORAL
  m_session = lookSvcH->connect( id1, coral::Update );

  //-   RCDConnectionService *theConnectionService=RCDConnectionService::instance();

  //Configure the Connection Service

  //-   theConnectionService->configuration()->setConnectionTimeOut(10);

  //UNCOMMENT/COMMENT THE FOLLOWING LINE TO TURN ON/OFF DEBUG MESSAGES
  // theConnectionService->setOutputLevel( seal::Msg::Debug);

  //-   m_session = theConnectionService->connect( m_connString, coral::Update );
   
}

COOLCORALClient::COOLCORALClient(const std::string& id1, bool verbose) : m_connString(id1), m_session(0)
{ 
  m_verbose = verbose;

  // Create the RCDConnectionService
  if(m_verbose) std::cout << "Creating Connection Service"<<std::endl;


  // Instantiate connection service
  coral::Context* context = &coral::Context::instance();

  // Load CORAL connection service
  coral::IHandle<coral::IConnectionService> lookSvcH = context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
      context->loadComponent( "CORAL/Services/ConnectionService" );
      lookSvcH = context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
      throw std::runtime_error( "Could not locate the connection service" );
  }
  /// connection to CORAL
  m_session = lookSvcH->connect( id1, coral::Update );

  //-   RCDConnectionService *theConnectionService=RCDConnectionService::instance();

  //Configure the Connection Service

  //-   theConnectionService->configuration()->setConnectionTimeOut(10);

  //UNCOMMENT/COMMENT THE FOLLOWING LINE TO TURN ON/OFF DEBUG MESSAGES
  // theConnectionService->setOutputLevel( seal::Msg::Debug);

  //-   m_session = theConnectionService->connect( m_connString, coral::Update );
   
}

/// Alternate constructor to connect both COOL and CORAL
COOLCORALClient::COOLCORALClient(const std::string& id1, const std::string& id2, cool::IDatabaseSvc &dbSvc): m_connString(id1), m_session(0), m_dbId(id2)
{
  m_verbose = true;

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Connecting to database '" << m_connString << "'" << std::endl;
  if(m_verbose) std::cout << "\nCOOLCORAL Client: Starting COOL client '" << m_dbId << "'" << std::endl;
    

  // Instantiate connection service
  coral::Context* context = &coral::Context::instance();

  // Load CORAL connection service
  coral::IHandle<coral::IConnectionService> lookSvcH = context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
      context->loadComponent( "CORAL/Services/ConnectionService" );
      lookSvcH = context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
      throw std::runtime_error( "Could not locate the connection service" );
  }
  /// connection to CORAL
  m_session = lookSvcH->connect( id1, coral::Update );

    
  /// connection to COOL
  try {
    m_db = dbSvc.openDatabase( m_dbId,false );
  }
  catch ( DatabaseDoesNotExist  & e ) {
    m_db = dbSvc.createDatabase( m_dbId );
  }
    
}

COOLCORALClient::COOLCORALClient(const std::string& id1, const std::string& id2, cool::IDatabaseSvc &dbSvc, bool verbose): m_connString(id1), m_session(0), m_dbId(id2)
{
  m_verbose = verbose;

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Connecting to database '" << m_connString << "'" << std::endl;
  if(m_verbose) std::cout << "\nCOOLCORAL Client: Starting COOL client '" << m_dbId << "'" << std::endl;
    
  // Instantiate connection service
  coral::Context* context = &coral::Context::instance();

  // Load CORAL connection service
  coral::IHandle<coral::IConnectionService> lookSvcH = context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
      context->loadComponent( "CORAL/Services/ConnectionService" );
      lookSvcH = context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
      throw std::runtime_error( "Could not locate the connection service" );
  }
  /// connection to CORAL
  m_session = lookSvcH->connect( id1, coral::Update );

  /// connection to COOL
  try {
    m_db = dbSvc.openDatabase( m_dbId,false );
  }
  catch ( DatabaseDoesNotExist  & e ) {
    m_db = dbSvc.createDatabase( m_dbId ); 
  }
    
}

//---------------------------------------------------------------------------
/// Destructor.
/// Close the connection
COOLCORALClient::~COOLCORALClient() {
  if (m_session){
    if(m_verbose) std::cout << "\nCOOLCORAL Client: Disconnecting from database '" << m_connString << "'" << std::endl; 
    delete m_session;
  }
}
  
//---------------------------------------------------------------------------
/// Get the list of tables on the database and print it on stdout.
void COOLCORALClient::printTables()
{
  if(m_verbose) std::cout << "\nCOOLCORAL Client: list of tables" << std::endl;

  m_session->transaction().start();
  std::set<std::string> tables = m_session->nominalSchema().listTables();
  m_session->transaction().commit();

  for ( std::set<std::string>::const_iterator tName = tables.begin(); tName != tables.end(); ++tName ){
    if(m_verbose) std::cout << "\t" << *tName << std::endl;
  }
  //    if(m_verbose) std::cout << std::endl;
}



/// Get the content of a table identified by an input string on the database and print it on stdout.
void COOLCORALClient::printTableContent(const std::string& tableName){
    
  //    std::string tableName = "BDTMROC";
  //    std::string colName = "Thresh0_Low_FE";
  if(m_verbose) std::cout << "\nCOOLCORAL Client:" << tableName <<" Table content" << std::endl;
  //    short T;
   
  m_session->transaction().start();
    
  coral::IQuery* query = m_session->nominalSchema().tableHandle(tableName).newQuery();
  //query->addToOutputList( colName );

  coral::ICursor& cursor = query->execute();
  int nRows = 0;
  while ( cursor.next() ) {
    //    cursor.currentRow().toOutputStream( std::cout ) << std::endl;
    //    if(m_verbose) std::cout<<std::endl;
    /*
      coral::AttributeList sel = cursor.currentRow();
      if(m_verbose) std::cout<< "Column   "<< colName << "  contain : ";
      T = sel[colName].data<short>();
      printf("%d \n", T);
    */
    ++nRows;
  }
  delete query;

  printf("Total  %d    records\n", nRows);
  m_session->transaction().commit();

  //if(m_verbose) std::cout<< "Column   "<< colName << "  contain : ";
  //printf("%d",thresh[1]);
    

}
//------------------------------------------------------------------------------
/// Create CORAL only tables for a partition, identified by an input string
void COOLCORALClient::createTables(const std::string& part_trt){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Create tables" << std::endl;
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  m_session->transaction().start();

  
  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string DTMROC_TABLE;


  if (part_trt=="barrel"){
    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
  }
  else if (part_trt=="endcap") {
    TTC_TABLE = "ECTTC";
    TTCGR_TABLE = "ECTTCGR";
    ROD_TABLE = "ECROD";
    DTMROC_TABLE = "ECDTMROC";
  }

  if(m_verbose) std::cout << "Deleting the old barrel tables: " << std::endl;
  if(m_verbose) std::cout <<  TTC_TABLE << std::endl;
  if(m_verbose) std::cout <<  TTCGR_TABLE << std::endl;
  if(m_verbose) std::cout <<  ROD_TABLE << std::endl;
  if(m_verbose) std::cout <<  DTMROC_TABLE << std::endl;


  m_session->nominalSchema().dropIfExistsTable(TTC_TABLE);
  m_session->nominalSchema().dropIfExistsTable(TTCGR_TABLE);
  m_session->nominalSchema().dropIfExistsTable(ROD_TABLE);
  m_session->nominalSchema().dropIfExistsTable(DTMROC_TABLE);

  //-------------------TRT TTC Table

  if(m_verbose) std::cout << "Creating table: " << TTC_TABLE << std::endl;

       
  // Create the table
  coral::TableDescription ttc_columns( "SchemaDefinition_TTC" );
  ttc_columns.setName( TTC_TABLE );
  ttc_columns.insertColumn( "ttc_UID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "ttc_iovfk",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttc_columns.insertColumn( "ByteSwapping",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  ttc_columns.insertColumn( "PhysicalAddress",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "DetID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "VMESlot",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "Delay",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "ArShaping",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  ttc_columns.insertColumn( "SetSendID",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  ttc_columns.insertColumn( "Name",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "ClockSource",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "InputFragmentType",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "Comment",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "ttc_tag",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  std::vector <std::string> tmp_strings;

  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  /*
    tmp_strings.clear();
    tmp_strings.push_back ("ttc_iovfk");
    // Define primary key
    ttc_columns.setPrimaryKey( tmp_string );
    ttc_columns->setNotNullConstraint ("ttc_iovfk");
  */
  // put an index on const
  if (part_trt=="barrel"){
    tmp_strings.clear();
    tmp_strings.push_back ("ttc_UID");
    ttc_columns.createIndex ("bttc_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttc_iovfk");
    // Define primary key
    //	      ttc_columns.setPrimaryKey( tmp_strings );
    // ttc_columns.setNotNullConstraint ("ttc_iovfk");
    // Define index
    ttc_columns.createIndex ("bttcio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttc_tag");
    ttc_columns.createIndex ("btag_idx", tmp_strings);
  }
  else if (part_trt=="endcap"){
    tmp_strings.clear();
    tmp_strings.push_back ("ttc_UID");
    ttc_columns.createIndex ("ecttc_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttc_iovfk");
    // Define primary key
    //	      ttc_columns.setPrimaryKey( tmp_strings );
    //  ttc_columns.setNotNullConstraint ("ttc_iovfk");
    // Define index
    ttc_columns.createIndex ("ettcio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttc_tag");
    ttc_columns.createIndex ("ectag_idx", tmp_strings);
  }
  ttc_columns.setNotNullConstraint ("ttc_UID");

	      
  // Create the actual table
  m_session->nominalSchema().createTable( ttc_columns );

  //-------------------TRT TTCGR Table

  if(m_verbose) std::cout << "Creating table: " << TTCGR_TABLE << std::endl;

       
  // Create the table
  coral::TableDescription ttcgr_columns( "SchemaDefinition_TTCGR" );
  ttcgr_columns.setName( TTCGR_TABLE );
  ttcgr_columns.insertColumn( "ttcgr_UID",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgr_columns.insertColumn( "ttcgr_iovfk",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttcgr_columns.insertColumn( "Group",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgr_columns.insertColumn( "DutyCycle",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgr_columns.insertColumn( "EdgeSelect",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "finebx",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "finedx",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "TDMdelay",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "TTCGRNAME",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "ttcgr_tag",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );


  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  /*
    tmp_strings.clear();
    tmp_strings.push_back ("ttc_iovfk");
    // Define primary key
    ttc_columns.setPrimaryKey( tmp_string );
    ttc_columns->setNotNullConstraint ("ttc_iovfk");
  */
  // put an index on const
  if (part_trt=="barrel"){
    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_UID");
    ttcgr_columns.createIndex ("bttcgr_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_iovfk");
    ttcgr_columns.createIndex ("bttcgrio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_tag");
    ttcgr_columns.createIndex ("bgrtag_idx", tmp_strings);
  }
  else if (part_trt=="endcap"){
    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_UID");
    ttcgr_columns.createIndex ("ecttcgr_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_iovfk");
    ttcgr_columns.createIndex ("ettcgrio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_tag");
    ttcgr_columns.createIndex ("ecgrtag_idx", tmp_strings);
  }
  ttcgr_columns.setNotNullConstraint ("ttcgr_UID");
  // Create the actual table
  m_session->nominalSchema().createTable( ttcgr_columns );

  //-------------------TRT ROD Table

  if(m_verbose) std::cout << "Creating table: " << ROD_TABLE << std::endl;


  // Create the table
  coral::TableDescription rod_columns( "SchemaDefinition_ROD" );
  rod_columns.setName( ROD_TABLE );
  rod_columns.insertColumn( "rod_UID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "rod_iovfk",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rod_columns.insertColumn( "RODByteSwapping",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  rod_columns.insertColumn( "RODPhysicalAddress",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODDetID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODVMESlot",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODHalf",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect0",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect1",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect2",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect3",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay0",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay1",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay2",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay3",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODStatus",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  rod_columns.insertColumn( "RODInputFragmentType",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  rod_columns.insertColumn( "RODName",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  rod_columns.insertColumn( "Comment",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  rod_columns.insertColumn( "rod_tag",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK
  /*
    tmp_strings.clear();
    tmp_strings.push_back ("rod_iovfk");
    rod_columns.setPrimaryKey (tmp_strings);
    rod_columns.setNotNullConstraint ("rod_iovfk");
  */
  // put an index on const
  if (part_trt=="barrel"){
    tmp_strings.clear();
    tmp_strings.push_back ("rod_UID");
    rod_columns.createIndex ("brod_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("rod_iovfk");
    rod_columns.createIndex ("brodio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("rod_tag");
    rod_columns.createIndex ("brtag_idx", tmp_strings);
  }
  else if (part_trt=="endcap"){
    tmp_strings.clear();
    tmp_strings.push_back ("rod_UID");
    rod_columns.createIndex ("ecrod_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("rod_iovfk");
    rod_columns.createIndex ("erodio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("rod_tag");
    rod_columns.createIndex ("ecrtag_idx", tmp_strings);
  }
  rod_columns.setNotNullConstraint ("rod_UID");



  // Create the actual table
  m_session->nominalSchema().createTable( rod_columns );

  //-------------------TRT DTMROC Table

  if(m_verbose) std::cout << "Creating table: " << DTMROC_TABLE << std::endl;


  // Create the table
  coral::TableDescription dtmroc_columns( "SchemaDefinition_DTMROC" );
  dtmroc_columns.setName( DTMROC_TABLE );
  dtmroc_columns.insertColumn( "dtmroc_UID",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "dtmroc_iovfk",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  dtmroc_columns.insertColumn( "DTMROCByteSwapping",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "DTMROCPhysicalAddress",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "ChipID",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "ChipValid",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "RODGroup",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "RODInput",
			       coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  dtmroc_columns.insertColumn( "HW_Addr_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "TTC_Line_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh0_Low_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh0_High_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh1_Low_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh1_High_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "VT_DAC0_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "VT_DAC1_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Mask_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "Pipe_Latency_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "DTMROCName",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "DTMROCInputFragmentType",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "Clock_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "Comment",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "roc_tag",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );


  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK
  /*
    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_iovfk");
    dtmroc_columns.setPrimaryKey (tmp_strings);
    dtmroc_columns.setNotNullConstraint ("dtmroc_iovfk");
  */
  // put an index on const
  if (part_trt=="barrel"){
    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_UID");
    dtmroc_columns.createIndex ("bdtmroc_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_iovfk");
    dtmroc_columns.createIndex ("bdtmrocio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("roc_tag");
    dtmroc_columns.createIndex ("broctag_idx", tmp_strings);
  }
  else if (part_trt=="endcap"){
    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_UID");
    dtmroc_columns.createIndex ("ecdtmroc_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_iovfk");
    dtmroc_columns.createIndex ("edtmrocio_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("roc_tag");
    dtmroc_columns.createIndex ("ecroctag_idx", tmp_strings);
  }
    
  dtmroc_columns.setNotNullConstraint ("dtmroc_UID");
    
    

  // Create the actual table
  m_session->nominalSchema().createTable( dtmroc_columns );





  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

 
  m_session->transaction().commit();

}
//------------------------------------------------------------------------------  
/// Drop CORAL only tables fon a given partition, specified by an input string
void COOLCORALClient::dropTables(const std::string& part_trt){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Drop tables" << std::endl;

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);


  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string DTMROC_TABLE;
 

  m_session->transaction().start();
  if (part_trt=="barrel"){
    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
  }
  else if (part_trt=="endcap") {
    TTC_TABLE = "ECTTC";
    TTCGR_TABLE = "ECTTCGR";
    ROD_TABLE = "ECROD";
    DTMROC_TABLE = "ECDTMROC";
  }

  if(m_verbose) std::cout << "Deleting the old tables: " << std::endl;
  if(m_verbose) std::cout <<  TTC_TABLE << std::endl;
  if(m_verbose) std::cout <<  TTCGR_TABLE << std::endl;
  if(m_verbose) std::cout <<  ROD_TABLE << std::endl;
  if(m_verbose) std::cout <<  DTMROC_TABLE << std::endl;

  m_session->nominalSchema().dropIfExistsTable(TTC_TABLE);
  m_session->nominalSchema().dropIfExistsTable(TTCGR_TABLE);
  m_session->nominalSchema().dropIfExistsTable(ROD_TABLE);
  m_session->nominalSchema().dropIfExistsTable(DTMROC_TABLE);
  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;


  m_session->transaction().commit();

}



//------------------------------------------------------------------------------
/// Fill CORAL only tables for a partition, identified by an input string
void COOLCORALClient::fillTables(const std::string& part_trt){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Fill tables" << std::endl;

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  m_session->transaction().start();



  struct timeval nunc_time;
  unsigned long long nunc_usecs;
    
  gettimeofday(&nunc_time, NULL);

  // iovfk is time in nanoseconds
  nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);
  //nunc_usecs = (nunc_time.tv_usec);
  //    iovfkttc = (unsigned long long) nunc_usecs/10;

  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string DTMROC_TABLE;

    
  if (part_trt=="barrel"){
    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
  }
  else if (part_trt=="endcap") {
    TTC_TABLE = "ECTTC";
    TTCGR_TABLE = "ECTTCGR";
    ROD_TABLE = "ECROD";
    DTMROC_TABLE = "ECDTMROC";
  }


  if(m_verbose) std::cout << "Filling table " << TTC_TABLE  << std::endl;
  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);
  // ttc_row.toOutputStream( std::cout ) << std::endl;
  //--------
  // TRT-TTC quantities

  unsigned int tuid;
  unsigned long long iovfkttc=0;
  short tbyteswap;
  unsigned int tphysaddr;
  unsigned int detid; // hex
  unsigned int vmeslot; //hex
  unsigned int delay;
  short shaping;
  short sendid;
  std::string ttcname;
  std::string clocksource;
  std::string tfragtype;
  std::string tcomment;
	
  //--------//

	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,32);

  int run=0;	
	
  const char* filename="ttc.dat";
  if (part_trt=="barrel"){
    filename = "RCDTTC.dat";
    //filename = "TTC06.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDTTC.dat";
  }
  ifstream itAfile(filename);

  std::vector<char> buffer(999);
  if (itAfile.is_open()){
    while (itAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> tuid >> iovfkttc
	   >> tbyteswap >> tphysaddr
	   >> detid >> vmeslot
	   >> delay
	   >> shaping >> sendid 
	   >>  clocksource >> ttcname 
	   >>  tfragtype >> tcomment;
	    
      //	    if(m_verbose) std::cout << "Read TTC data done..." << std::endl;
	

      ttc_row["ttc_UID"].setValue <int> (tuid);
      ttc_row["ttc_iovfk"].setValue <long long> (iovfkttc);
      ttc_row["ByteSwapping"].setValue <short> (tbyteswap);
      ttc_row["PhysicalAddress"].setValue <int> (tphysaddr);
      ttc_row["DetID"].setValue <int> (detid);
      ttc_row["VMESlot"].setValue <int> (vmeslot);
      ttc_row["Delay"].setValue <int> (delay);
      ttc_row["ArShaping"].setValue <short> (shaping);
      ttc_row["SetSendID"].setValue <short> (sendid);
      ttc_row["Name"].setValue <std::string> (ttcname);
      ttc_row["ClockSource"].setValue <std::string> (clocksource);
      ttc_row["InputFragmentType"].setValue <std::string> (tfragtype);
      ttc_row["Comment"].setValue <std::string> (tcomment);
	    

      run++;
	    
      //ttc_editor.insertRow(ttc_row);
      ttc_bulk->processNextIteration();
    }
  } 


  itAfile.close();


  ttc_bulk->flush();
  delete ttc_bulk;
	
  printf("Total: %d  rows\n",run);

  //----  TTCGROUP ------//

  if(m_verbose) std::cout << "Filling table"<< TTCGR_TABLE  << std::endl;
  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
  // ttc_row.toOutputStream( std::cout ) << std::endl;
  //--------
  // TRT-TTC quantities

  unsigned int tgruid;
  unsigned int group;
  unsigned int duty;
  std::string edgeselect; // vector of 10 elements
  std::string bx; // vector of 10 elements
  std::string dout; // vector of 10 elements
  std::string tdmdelay; // vector of 10 elements
  std::string tgrname;

  std::vector <int> my_vector;
	

  //--------//

	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,16);

  unsigned int bxsz;
  std::vector <unsigned int> bxv(10);
	
  unsigned int doutsz;
  std::vector <unsigned int> doutv(10);
	
  unsigned int edgesz;
  std::vector <unsigned int> edgev(10);
	
  unsigned int tdmdelaysz;
  std::vector <unsigned int> tdmdelayv(10);
	
  run=0;	
	
  //		ifstream itGRfile ("RCDTTCGROUP.dat");
  //		ifstream itGRfile ("TTCGROUPTEMP.dat");

  if (part_trt=="barrel"){
    filename = "RCDTTCGROUP.dat";
    // filename = "TTCGROUPTEMP.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDTTCGROUP.dat";
  }

  ifstream itGRfile (filename);
  //	ifstream itGRfile ("ECRCDTTCGROUP.dat");
  //	std::vector<char> buffer(999);
  if (itGRfile.is_open()){
    while (itGRfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> tgruid >> tgrname >> group 
	   >> duty
	   >> edgesz 
	   >> edgev[0] >> edgev[1] >> edgev[2] >> edgev[3] >> edgev[4]
	   >> edgev[5] >> edgev[6] >> edgev[7] >> edgev[8] >> edgev[9] 
	   >> bxsz
	   >> bxv[0] >> bxv[1] >> bxv[2] >> bxv[3] >> bxv[4] >> bxv[5] 
	   >> bxv[6]  >> bxv[7] >> bxv[8] >> bxv[9]  
	   >>  doutsz 
	   >> doutv[0] >> doutv[1] >> doutv[2] >> doutv[3] >> doutv[4] 
	   >> doutv[5] >> doutv[6]  >> doutv[7] >> doutv[8] >> doutv[9] 
	   >> tdmdelaysz
	   >> tdmdelayv[0] >> tdmdelayv[1] >> tdmdelayv[2] 
	   >> tdmdelayv[3] >> tdmdelayv[4] >> tdmdelayv[5] 
	   >> tdmdelayv[6]  >> tdmdelayv[7] >> tdmdelayv[8] 
	   >> tdmdelayv[9];
	    
	    
      VECTOR2STRING (bxv, bx);
      VECTOR2STRING (doutv, dout);
      VECTOR2STRING (edgev, edgeselect);
      VECTOR2STRING (tdmdelayv, tdmdelay);
	    
	
      ttcgr_row["ttcgr_UID"].setValue <int> (tgruid);
      ttcgr_row["ttcgr_iovfk"].setValue <long long> (iovfkttc);
      ttcgr_row["Group"].setValue <int> (group);
      ttcgr_row["DutyCycle"].setValue <int> (duty);
      ttcgr_row["EdgeSelect"].setValue <std::string> (edgeselect);
      ttcgr_row["finebx"].setValue <std::string> (bx);
      ttcgr_row["finedx"].setValue <std::string> (dout);
      ttcgr_row["TDMdelay"].setValue <std::string> (tdmdelay);
      ttcgr_row["TTCGRNAME"].setValue <std::string> (tgrname);
	    

      run++;
	    
      //ttc_editor.insertRow(ttcgr_row);
      ttcgr_bulk->processNextIteration();
    }
  } 

	    

  itGRfile.close();

  ttcgr_bulk->flush();
  delete ttcgr_bulk;
	
  printf("Total: %d  rows\n",run);





  //-----------    ROD      ----------------------------------------------------
  if(m_verbose) std::cout << "Filling table"<< ROD_TABLE  << std::endl;
  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);
  //  rod_row.toOutputStream( std::cout ) << std::endl;
  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);


  unsigned int rodid;
  short rodbyteswap;
  unsigned int rodphysaddr;
  unsigned int roddetid;    // hex
  unsigned int rodvmeslot;  // hex
  unsigned int rodhalf; 
  unsigned int rodES0;  // one per each group
  unsigned int rodES1;
  unsigned int rodES2;
  unsigned int rodES3;
  unsigned int rodGol0;  // one per each group
  unsigned int rodGol1;
  unsigned int rodGol2;
  unsigned int rodGol3;
  short rodstatus;
	
  std::string rodfragtype;
  std::string rodname;
  std::string rcomment;
	
	
  // Barrel A
  run=0;
  if (part_trt=="barrel"){
    filename = "RCDROD.dat";
    //filename = "ROD06.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDROD.dat";
  }

  ifstream irAfile (filename);


  if (irAfile.is_open()){
    while (irAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> rodid 
	   >> rodbyteswap >> rodphysaddr
	   >> roddetid >> rodvmeslot
	   >> rodhalf
	   >> std::hex >> rodES0 >> rodES1
	   >> rodES2 >> rodES3 >> std::dec
	   >> rodGol0 >> rodGol1
	   >> rodGol2 >> rodGol3
	   >> rodstatus >> rodfragtype
	   >> rodname >> rcomment;

      rod_row["rod_UID"].setValue <int> (rodid);
      rod_row["rod_iovfk"].setValue <long long> (iovfkttc);
      rod_row["RODByteSwapping"].setValue <short> (rodbyteswap);
      rod_row["RODPhysicalAddress"].setValue <int> (rodphysaddr);
      rod_row["RODDetID"].setValue <int> (roddetid);
      rod_row["RODVMESlot"].setValue <int> (rodvmeslot);
      rod_row["RODHalf"].setValue <int> (rodhalf);
      rod_row["EdgeSelect0"].setValue <int> (rodES0);
      rod_row["EdgeSelect1"].setValue <int> (rodES1);
      rod_row["EdgeSelect2"].setValue <int> (rodES2);
      rod_row["EdgeSelect3"].setValue <int> (rodES3);
      rod_row["GolDelay0"].setValue <int> (rodGol0);
      rod_row["GolDelay1"].setValue <int> (rodGol1);
      rod_row["GolDelay2"].setValue <int> (rodGol2);
      rod_row["GolDelay3"].setValue <int> (rodGol3);
      rod_row["RODStatus"].setValue <short> (rodstatus);
      rod_row["RODInputFragmentType"].setValue <std::string> (rodfragtype);
      rod_row["RODName"].setValue <std::string> (rodname);
      rod_row["Comment"].setValue <std::string> (rcomment);
	    

  

 
      rod_bulk->processNextIteration();
      run++;
    }
  }
  irAfile.close();



  rod_bulk->flush();
  delete rod_bulk;

  printf("Total: %d  rows\n",run);
  // DTMROC
	
  if(m_verbose) std::cout << "Filling table"<< DTMROC_TABLE  << std::endl;
  coral::ITableDataEditor& dtmroc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList dtmroc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_editor.rowBuffer(dtmroc_row);
  //    dtmroc_row.toOutputStream( std::cout ) << std::endl;
  coral::IBulkOperation* dtmroc_bulk= dtmroc_editor.bulkInsert(dtmroc_row,32);
	
  unsigned int uid;
  short chipvalid;
  short byteswap;
  unsigned int physaddr;
  short rodgroup, rodinput, hwaddr, ttcline;
  short thresh0Low, thresh0High;
  short thresh1Low, thresh1High;
  short vtdac0, vtdac1;
  unsigned int mask; // hex
  short pipelatency;
	
  std::string fragtype;
  std::string clock;
  std::string comment;
  std::string dtname;
	
  int num_size=0,num_phi=0,num_chip=0,chipID=0;
  char size;
  // Barrel A
  run=0;
  if (part_trt=="barrel"){
    filename = "RCDDTMROC.dat";
    //filename = "DTMROC06.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDDTMROC.dat";
  }
  ifstream idAfile (filename);

  if (idAfile.is_open()){
    while (idAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> byteswap >> physaddr
	   >> fragtype >> dtname
	   >> uid >> chipvalid
	   >> rodgroup >> rodinput
	   >> hwaddr >> ttcline
	   >> thresh0Low >> thresh0High
	   >> thresh1Low >> thresh1High
	   >> vtdac0 >> vtdac1
	   >> mask >> pipelatency
	   >> clock
	   >> comment;

	    
      char boardch;
      int boardn;
	    
      if (part_trt=="barrel"){
	sscanf(dtname.c_str(),"roc-%d%c_%d",&num_phi,&size,&num_chip);
	if(size=='A')num_size=31;
	else if(size=='C')num_size=32;
	chipID=num_size*100000+num_phi*1000+num_chip;
      }
      else if (part_trt=="endcap"){
	sscanf(dtname.c_str(),"roc-%d%c_%c%d_%d",&num_phi,&size,&boardch,&boardn,&num_chip);
	if(size=='A')num_size=33;
	else if(size=='C')num_size=34;
	chipID=num_size*100000+num_phi*1000+num_chip;
      }


      dtmroc_row["dtmroc_UID"].setValue <int> (chipID);
      //	    dtmroc_row["dtmroc_iovfk"].setValue <long long> (iovfkttc);
      dtmroc_row["dtmroc_iovfk"].setValue <long long> (nunc_usecs/10);
      dtmroc_row["DTMROCByteSwapping"].setValue <short> (byteswap);
      dtmroc_row["DTMROCPhysicalAddress"].setValue <int> 
	(physaddr);
      dtmroc_row["ChipID"].setValue <int> (uid);
      dtmroc_row["ChipValid"].setValue <short> (chipvalid);
      dtmroc_row["RODGroup"].setValue <short> (rodgroup);
      dtmroc_row["RODInput"].setValue <int> (rodinput);
      dtmroc_row["HW_Addr_FE"].setValue <short> (hwaddr);
      dtmroc_row["TTC_Line_FE"].setValue <short> (ttcline);
      dtmroc_row["Thresh0_Low_FE"].setValue <short> (thresh0Low);
      dtmroc_row["Thresh0_High_FE"].setValue <short> (thresh0High);
      dtmroc_row["Thresh1_Low_FE"].setValue <short> (thresh1Low);
      dtmroc_row["Thresh1_High_FE"].setValue <short> (thresh1High);
      dtmroc_row["VT_DAC0_FE"].setValue <short> (vtdac0);
      dtmroc_row["VT_DAC1_FE"].setValue <short> (vtdac1);
      dtmroc_row["Mask_FE"].setValue <int> (mask);
      dtmroc_row["Pipe_Latency_FE"].setValue <short> (pipelatency);
      dtmroc_row["DTMROCName"].setValue <std::string> (dtname);
      dtmroc_row["DTMROCInputFragmentType"].setValue <std::string>
	(fragtype);
      dtmroc_row["Clock_FE"].setValue <std::string> (clock);
      dtmroc_row["Comment"].setValue <std::string> (comment);
	    


      dtmroc_bulk->processNextIteration();
      run++;
    }
  }
  idAfile.close();



  dtmroc_bulk->flush();
  delete dtmroc_bulk;


  printf("Total: %d  rows\n",run);
  //if(m_verbose) std::cout<<std::endl;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;







  m_session->transaction().commit();

}




//------------------------------------------------------------------------------
/// Create and fill CORAL only Connection table for a partition, identified by an input string
void COOLCORALClient::createConnect(const std::string& part_trt){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Create tables" << std::endl;
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);
  std::string BCONNECT_TABLE;



  if (part_trt=="barrel"){
    BCONNECT_TABLE = "BCONNECT";
  }
  else if (part_trt=="endcap") {
    BCONNECT_TABLE = "ECCONNECT";
  }


  m_session->transaction().start();


  if(m_verbose) std::cout << "Deleting the old tables: " << std::endl;
  if(m_verbose) std::cout <<  BCONNECT_TABLE << std::endl;

  m_session->nominalSchema().dropIfExistsTable(BCONNECT_TABLE);
  //-------------------TRT BARREL CONNECT Table

  if(m_verbose) std::cout << "Creating table: " << BCONNECT_TABLE << std::endl;

       
  // Create the table
  coral::TableDescription bconn_columns( "SchemaDefinition_BCONNECT" );
  bconn_columns.setName( BCONNECT_TABLE );
  bconn_columns.insertColumn( "Partition",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "Crate",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "Slot",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "phi1",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "phi2",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  if (part_trt=="barrel"){
    bconn_columns.insertColumn( "phi3",
				coral::AttributeSpecification::typeNameForId( typeid(int) ) );
    bconn_columns.insertColumn( "phi4",
				coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  }
    
  std::vector <std::string> tmp_strings;
    
  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  /*
    tmp_strings.clear();
    tmp_strings.push_back ("ttc_iovfk");
    // Define primary key
    bconn_columns.setPrimaryKey( tmp_string );
    bconn_columns->setNotNullConstraint ("ttc_iovfk");
  */
  // put an index on const
  if (part_trt=="barrel"){
    tmp_strings.clear();
    tmp_strings.push_back ("Partition");
    bconn_columns.createIndex ("bpart_idx", tmp_strings);
      
    tmp_strings.clear();
    tmp_strings.push_back ("Crate");
    bconn_columns.createIndex ("bcrate_idx", tmp_strings);
      
    tmp_strings.clear();
    tmp_strings.push_back ("Slot");
    bconn_columns.createIndex ("bslot_idx", tmp_strings);
  }
  else if (part_trt=="endcap"){
    tmp_strings.clear();
    tmp_strings.push_back ("Partition");
    bconn_columns.createIndex ("ecpart_idx", tmp_strings);
      
    tmp_strings.clear();
    tmp_strings.push_back ("Crate");
    bconn_columns.createIndex ("eccrate_idx", tmp_strings);
      
    tmp_strings.clear();
    tmp_strings.push_back ("Slot");
    bconn_columns.createIndex ("ecslot_idx", tmp_strings);
      
  }

  bconn_columns.setNotNullConstraint ("Partition");
  bconn_columns.setNotNullConstraint ("Crate");
  bconn_columns.setNotNullConstraint ("Slot");

  // Create the actual table
  m_session->nominalSchema().createTable( bconn_columns );

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;



  if(m_verbose) std::cout << "Filling table " << BCONNECT_TABLE  << std::endl;
  coral::ITableDataEditor& bconn_editor = m_session->nominalSchema().tableHandle(BCONNECT_TABLE ).dataEditor();
  coral::AttributeList bconn_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  bconn_editor.rowBuffer(bconn_row);


  //--------
  // TRT-BCONNECT quantities

  int part;
  int crate;
  int slot;
  int phi[4];
  //--------//

	
  coral::IBulkOperation* bconn_bulk= bconn_editor.bulkInsert(bconn_row,16);

  int run=0;	
  const char* filename = "connect.dat";
  if (part_trt=="barrel"){
    filename = "RCDConnectTable.dat";
  }
  else if (part_trt=="endcap"){
    filename = "EC-ConnectTable.dat";
  }

  ifstream itAfile (filename);

	
  //ifstream itAfile ("ConnectTable.dat");
  std::vector<char> buffer(999);
  if (itAfile.is_open()){
    while (itAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      if (part_trt=="barrel"){
	std::istringstream sbuf(&buffer[0]);
	sbuf >> part >> crate >> slot 
	     >> phi[0] >> phi[1] 
	     >> phi[2]
	     >> phi[3];
	bconn_row["Partition"].setValue <int> (part);
	bconn_row["Crate"].setValue <int> (crate);
	bconn_row["Slot"].setValue <int> (slot);
	bconn_row["phi1"].setValue <int> (phi[0]);
	bconn_row["phi2"].setValue <int> (phi[1]);
	bconn_row["phi3"].setValue <int> (phi[2]);
	bconn_row["phi4"].setValue <int> (phi[3]);
      }	    
      else if (part_trt=="endcap"){
	std::istringstream sbuf(&buffer[0]);
	sbuf >> part >> crate >> slot 
	     >> phi[0] >> phi[1];
	bconn_row["Partition"].setValue <int> (part);
	bconn_row["Crate"].setValue <int> (crate);
	bconn_row["Slot"].setValue <int> (slot);
	bconn_row["phi1"].setValue <int> (phi[0]);
	bconn_row["phi2"].setValue <int> (phi[1]);
      }
	    
      //	    bconn_row["phi3"].setValue <int> (0);
      //  bconn_row["phi4"].setValue <int> (0);

      run++;
	    
      //ttc_editor.insertRow(ttc_row);
      bconn_bulk->processNextIteration();
    }
  } 
  if(m_verbose) std::cout << "Read CONNECT data done..." << std::endl;

  itAfile.close();

  //	ttc_fld->flushStorageBuffer();	    

  bconn_bulk->flush();
  delete bconn_bulk;
	
  printf("Total: %d  rows\n",run);



 
  m_session->transaction().commit();



}


//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
int COOLCORALClient::GetTTCdummy(int ttc_id){
  if(m_verbose) std::cout << "\nGet TTC Queries and SubQueries"  << std::endl;


  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);
  TTCobj_t *ttc_param;
  std::vector<TTCobj_t *> ttc_params;
  TTCGroupobj_t ttcgr_param;
  DTMROCobj_t  dtmroc_param;

  if(m_verbose) std::cout << "TTC Identifier = " << ttc_id << std::endl;
  int part_nr = ttc_id/10000 - 4  ;
  if(m_verbose) std::cout << "part_nr = " << part_nr << std::endl;
  int crate_id = ttc_id/100 - (part_nr+4)*100;
  if(m_verbose) std::cout << "crate_id = " << crate_id << std::endl;
  int slot_id = ttc_id - (part_nr+4)*10000 - crate_id*100;
  if(m_verbose) std::cout << "slot_id = " << slot_id << std::endl;

  //

  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string  DTMROC_TABLE;
  std::string   BCONNECT_TABLE;
 
    
  m_session->transaction().start(true);

  if (part_nr <33){
    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
    BCONNECT_TABLE = "BCONNECT";
  }
  else if (part_nr>=33) {
    TTC_TABLE = "ECTTC";
    TTCGR_TABLE = "ECTTCGR";
    ROD_TABLE = "ECROD";
    DTMROC_TABLE = "ECDTMROC";
    BCONNECT_TABLE = "ECCONNECT";
  }


  std::vector<int> rod_id;
  int rod_master = part_nr * 100;
 
  std::vector<int> phi;



  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(BCONNECT_TABLE).newQuery();

  query00->addToOutputList("phi1" ); 
  query00->addToOutputList("phi2" ); 

  if (part_nr <33){
    query00->addToOutputList("phi3" ); 
    query00->addToOutputList("phi4" ); 
  }

  std::string conn_cond = "BCONNECT.Partition = :part_nr AND BCONNECT.Crate = :crate_id AND BCONNECT.Slot = :slot_id";
  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<int>( "part_nr" );
  conn_condData.extend<int>( "crate_id" );
  conn_condData.extend<int>( "slot_id" );
    
  query00->setCondition( conn_cond, conn_condData);
        

    
  conn_condData[0].data<int>() = part_nr;
  conn_condData[1].data<int>() = crate_id;
  conn_condData[2].data<int>() = slot_id;

  coral::ICursor& cursor00 = query00->execute();
  int nRowsM = 0;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();

      
    phi.push_back(row00[0].data<int>());
    phi.push_back(row00[1].data<int>());
    if (part_nr<33){
      phi.push_back(row00[2].data<int>());
      phi.push_back(row00[3].data<int>());
    }
    nRowsM++;
  }
  delete query00;
  printf("Total  %d   Mapping  records\n", nRowsM);

  if (phi.size()==0) {
    if(m_verbose) std::cout << "Provide phi values, please!" << std::endl;
    return 1;
  }

  size_t ik;

  for (ik = 0; ik < phi.size(); ik++) {
    rod_id.push_back(rod_master + phi[ik]);
  }

    
  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Comment" ); 

  std::string ttc_cond = "BTTC.ttc_UID = :ttcnr";
  coral::AttributeList ttc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_condData.extend<int>( "ttcnr" );
    
  query0->setCondition( ttc_cond, ttc_condData);
        

  //ttc_id = (part_nr+4)*10000+crate_id*100+slot_id;    
  ttc_condData[0].data<int>() = ttc_id;

  if(m_verbose) std::cout << "TTC ID = " << ttc_id << std::endl;

  // query on TTCGROUP

  coral::IQuery* query1 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();

  query1->addToOutputList("Group" );
  query1->addToOutputList("DutyCycle" );
  query1->addToOutputList("EdgeSelect");
  query1->addToOutputList("finebx" );
  query1->addToOutputList("finedx" );
  query1->addToOutputList("TDMdelay");

  std::string ttcgr_cond = "BTTCGR.ttcgr_UID = :ttcgr0 OR BTTCGR.ttcgr_UID = :ttcgr1";
  coral::AttributeList ttcgr_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_condData.extend<int>( "ttcgr0" );
  ttcgr_condData.extend<int>( "ttcgr1" );
    
    
  query1->setCondition( ttcgr_cond, ttcgr_condData);
    
  // query on dtmroc

  coral::IQuery* query2 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  query2->addToOutputList( "ChipID" ); 
  query2->addToOutputList( "ChipValid" ); 
  query2->addToOutputList( "RODGroup" ); 
  query2->addToOutputList( "RODInput" ); 
  query2->addToOutputList( "HW_Addr_FE" );
  query2->addToOutputList( "TTC_Line_FE" );
  query2->addToOutputList( "Thresh0_Low_FE" );
  query2->addToOutputList( "Thresh0_High_FE" );
  query2->addToOutputList( "Thresh1_Low_FE" );
  query2->addToOutputList( "Thresh1_High_FE" );
  query2->addToOutputList( "VT_DAC0_FE" );
  query2->addToOutputList( "VT_DAC1_FE" ); 
  query2->addToOutputList( "Mask_FE" ); 
  query2->addToOutputList( "Pipe_Latency_FE" ); 
  query2->addToOutputList( "Clock_FE" ); 

  std::string dtmroc_cond = "BDTMROC.dtmroc_UID >= :roc1 AND BDTMROC.dtmroc_UID <= :roc104";
  coral::AttributeList dtmroc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_condData.extend<int>( "roc1" );
  dtmroc_condData.extend<int>( "roc104" );

  query2->setCondition( dtmroc_cond, dtmroc_condData);
    


  coral::ICursor& cursor0 = query0->execute();
  int nRows = 0;
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    ttc_param = new TTCobj_t;

    ttc_param->VMEslot = row0[1].data<int>();
    ttc_param->Delay = row0[2].data<int>();
    ttc_param->ArShaping = row0[3].data<short>();
    ttc_param->SetSendID = row0[4].data<short>();
    ttc_param->DetID = row0[0].data<int>();
    ttc_param->Comment =  row0[5].data<std::string>();


    


    ttcgr_condData[0].data<int>() = row0[0].data<int>()*10;
    ttcgr_condData[1].data<int>() = row0[0].data<int>()*10 + 1;
    if(m_verbose) std::cout << "TTCGROUP = " << ttcgr_condData[0].data<int>() << std::endl;
    if(m_verbose) std::cout << "TTCGROUP = " << ttcgr_condData[1].data<int>() << std::endl;
    int nRows1 = 0;
    if (query1){
			coral::ICursor& cursor1 = query1->execute();

			while ( cursor1.next() ) {
				const coral::AttributeList &row1 = cursor1.currentRow();

			
			
				ttcgr_param.Group = row1[0].data<int>();
				ttcgr_param.DutyCycle = row1[1].data<int>();
			
				std::vector <int> fbx;
				std::vector <int> fdx;
				std::vector <int> tdm;
				std::vector <int> edge;
				size_t k=0;
				STRING2VECTOR(row1[2].data<std::string>(),edge);
				STRING2VECTOR(row1[3].data<std::string>(),fbx);
				STRING2VECTOR(row1[4].data<std::string>(),fdx);
				STRING2VECTOR(row1[5].data<std::string>(),tdm);
				for (k = 0; k < edge.size(); k ++) {
					ttcgr_param.EdgeSelect[k]=edge[k];
					ttcgr_param.finebx[k]=fbx[k];
					ttcgr_param.finedx[k]=fdx[k];
					ttcgr_param.TDMdelay[k]=tdm[k];
				}
				ttc_param->Groups.push_back(ttcgr_param);
	

				nRows1++;
	
			}      
    }
    delete query1;query1=nullptr;
    printf("Total  %d    TTC GROUP records\n", nRows1);

    //   was query 2    
  
    int nrdtmrocs = 0;
    if (part_nr<33) {
      nrdtmrocs = 104;
    }
    else if (part_nr>=33) {
      nrdtmrocs = 120;
    }

  
    dtmroc_condData[0].data<int>() = part_nr*100000+phi[0]*1000+1;
    dtmroc_condData[1].data<int>() = part_nr*100000+phi[phi.size()-1]*1000+
      nrdtmrocs;
    
    if(m_verbose) std::cout << "DTMROC from " << part_nr*100000+phi[0]*1000+1 
	      << " to " << part_nr*100000+phi[phi.size()-1]*1000+nrdtmrocs 
	      << std::endl;
    int nRows2 = 0;
		if (query2){
      coral::ICursor& cursor2 = query2->execute();
			
			while ( cursor2.next() ) {
				const coral::AttributeList &row2 = cursor2.currentRow();
				dtmroc_param.ChipID = row2[0].data<int>();
				dtmroc_param.Chip_Valid = row2[1].data<short>();
				dtmroc_param.RODgroup = row2[2].data<short>();
				dtmroc_param.RODinput = row2[3].data<int>();
				dtmroc_param.TTC_Line_FE = row2[5].data<short>();
				dtmroc_param.HW_addr_FE = row2[4].data<short>();
				dtmroc_param.Thresh0_Low_FE = row2[6].data<short>();
				dtmroc_param.Thresh0_High_FE = row2[7].data<short>();
				dtmroc_param.Thresh1_Low_FE = row2[8].data<short>();
				dtmroc_param.Thresh1_High_FE = row2[9].data<short>();
				dtmroc_param.VT_DAC0_FE = row2[10].data<short>();
				dtmroc_param.VT_DAC1_FE = row2[11].data<short>();
				dtmroc_param.Mask_FE = row2[12].data<int>();
				dtmroc_param.Pipe_Latency_FE = row2[13].data<short>();
				dtmroc_param.Clock_FE = row2[14].data<std::string>();
				ttc_param->dtmroc_params.push_back(dtmroc_param);
				nRows2++;
			}      
    }
    delete query2; query2=nullptr;
    printf("Total  %d    DTMROC records\n", nRows2);
    ttc_params.push_back(ttc_param);
    ++nRows;
  }
  delete query0; query0=nullptr;
  printf("Total  %d   TTC  records\n", nRows);



      
    


  m_session->transaction().commit();


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

  // 	for (size_t k = 0; k < ttc_params.size(); k++) {
  // 	  delete ttc_params[k];
  // 	}

  return 1;

}
//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and fill a TTCobj_t
/// data structure, therefore also info from DTMROCs connected
/// and from corresponding TTCGROUPs are retrieved

TTCobj_t* COOLCORALClient::GetTTC(int ttc_id){
  //    if(m_verbose) std::cout << "\nGet TTC Queries and SubQueries"  << std::endl;


  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  TTCobj_t *ttc_param=NULL;
  TTCGroupobj_t ttcgr_param;
  DTMROCobj_t  dtmroc_param;

  int part_nr = ttc_id/10000 - 4  ;
  int crate_id = ttc_id/100 - (part_nr+4)*100;
  int slot_id = ttc_id - (part_nr+4)*10000 - crate_id*100;

  //
  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string  DTMROC_TABLE;
  std::string   BCONNECT_TABLE;

    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
    BCONNECT_TABLE = "BCONNECT";

    
  m_session->transaction().start(true);

  std::vector<int> phi;



  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(BCONNECT_TABLE).newQuery();

  query00->addToOutputList("phi" ); 
  //   query00->addToOutputList("phi2" ); 
  //    if (part_nr <33){
  //   query00->addToOutputList("phi3" ); 
  //  query00->addToOutputList("phi4" ); 
  // }

  std::string conn_cond = BCONNECT_TABLE;
  conn_cond += ".Partition = :part_nr AND ";
  conn_cond += BCONNECT_TABLE;
  conn_cond += ".Crate = :crate_id AND ";
  conn_cond += BCONNECT_TABLE;
  conn_cond += ".Slot = :slot_id";


  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<int>( "part_nr" );
  conn_condData.extend<int>( "crate_id" );
  conn_condData.extend<int>( "slot_id" );
    
  query00->setCondition( conn_cond, conn_condData);
        

    
  conn_condData[0].data<int>() = part_nr;
  conn_condData[1].data<int>() = crate_id;
  conn_condData[2].data<int>() = slot_id;

  coral::ICursor& cursor00 = query00->execute();
  int nRowsM = 0;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();

      
    if ( row00[0].data<int>()>0) phi.push_back(row00[0].data<int>());
    //       if ( row00[1].data<int>()>0) phi.push_back(row00[1].data<int>());
    //       if (part_nr<33){
    // 	if ( row00[2].data<int>()>0) phi.push_back(row00[2].data<int>());
    // 	if ( row00[3].data<int>()>0) phi.push_back(row00[3].data<int>());
    //       }

    nRowsM++;
  }
  delete query00;


  if (phi.size()==0) {
    if(m_verbose) std::cout << "Provide phi values, please!" << std::endl;
  }

  // QUERY on TTC

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Comment" ); 

  std::string ttc_cond = TTC_TABLE;
  ttc_cond += ".ttc_UID = :ttcnr";

  coral::AttributeList ttc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_condData.extend<int>( "ttcnr" );
    
  query0->setCondition( ttc_cond, ttc_condData);

  ttc_condData[0].data<int>() = ttc_id;

  //    if(m_verbose) std::cout << "TTC ID = " << ttc_id << std::endl;

  // query on TTCGROUP

  coral::IQuery* query1 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();

  query1->addToOutputList("Group" );
  query1->addToOutputList("DutyCycle" );
  query1->addToOutputList("EdgeSelect");
  query1->addToOutputList("finebx" );
  query1->addToOutputList("finedx" );
  query1->addToOutputList("TDMdelay");

  std::string ttcgr_cond = TTCGR_TABLE;
  ttcgr_cond += ".ttcgr_UID >= :ttcgr0 AND ";
  ttcgr_cond +=  TTCGR_TABLE;
  ttcgr_cond +=  ".ttcgr_UID <= :ttcgr1";


  //"BTTCGR.ttcgr_UID >= :ttcgr0 AND BTTCGR.ttcgr_UID <= :ttcgr1";

  coral::AttributeList ttcgr_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_condData.extend<int>( "ttcgr0" );
  ttcgr_condData.extend<int>( "ttcgr1" );
    
    
  query1->setCondition( ttcgr_cond, ttcgr_condData);
    
  // query on dtmroc

  coral::IQuery* query2 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  query2->addToOutputList( "ChipID" ); 
  query2->addToOutputList( "ChipValid" ); 
  query2->addToOutputList( "RODGroup" ); 
  query2->addToOutputList( "RODInput" ); 
  query2->addToOutputList( "HW_Addr_FE" );
  query2->addToOutputList( "TTC_Line_FE" );
  query2->addToOutputList( "Thresh0_Low_FE" );
  query2->addToOutputList( "Thresh0_High_FE" );
  query2->addToOutputList( "Thresh1_Low_FE" );
  query2->addToOutputList( "Thresh1_High_FE" );
  query2->addToOutputList( "VT_DAC0_FE" );
  query2->addToOutputList( "VT_DAC1_FE" ); 
  query2->addToOutputList( "Mask_FE" ); 
  query2->addToOutputList( "Pipe_Latency_FE" ); 
  query2->addToOutputList( "Clock_FE" ); 

  std::string dtmroc_cond = DTMROC_TABLE;
  dtmroc_cond += ".dtmroc_UID >= :roc1 AND ";
  dtmroc_cond += DTMROC_TABLE;
  dtmroc_cond += ".dtmroc_UID <= :roc104";

  //"BDTMROC.dtmroc_UID >= :roc1 AND BDTMROC.dtmroc_UID <= :roc104";

  coral::AttributeList dtmroc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_condData.extend<int>( "roc1" );
  dtmroc_condData.extend<int>( "roc104" );

  query2->setCondition( dtmroc_cond, dtmroc_condData);
    
  int nRows = 0;
  int nRows1 = 0;
  int nRows2 = 0;

  coral::ICursor& cursor0 = query0->execute();
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    ttc_param = new TTCobj_t;

    ttc_param->VMEslot = row0[1].data<int>();
    ttc_param->Delay = row0[2].data<int>();
    ttc_param->ArShaping = row0[3].data<short>();
    ttc_param->SetSendID = row0[4].data<short>();
    ttc_param->DetID = row0[0].data<int>();
    ttc_param->Comment =  row0[5].data<std::string>();


    

    ttcgr_condData[0].data<int>() = ttc_id*10;

    int nrgroups=3;
    // dangerous fixed number: to be understood

    //      if (part_nr<33){
    //	nrgroups = phi.size() - 1;
    // }
    // else if (part_nr>=33){
    //	nrgroups = 2*phi.size()-1;
    // }

    ttcgr_condData[1].data<int>() = ttc_id*10 + nrgroups;

    //      if(m_verbose) std::cout << "TTCGROUP = " << ttcgr_condData[0].data<int>() << std::endl;
    // if(m_verbose) std::cout << "TTCGROUP = " << ttcgr_condData[1].data<int>() << std::endl;
    if (query1){
			coral::ICursor& cursor1 = query1->execute();
			while ( cursor1.next() ) {
				const coral::AttributeList &row1 = cursor1.currentRow();

			
			
				ttcgr_param.Group = row1[0].data<int>();
				ttcgr_param.DutyCycle = row1[1].data<int>();
			
				std::vector <int> fbx;
				std::vector <int> fdx;
				std::vector <int> tdm;
				std::vector <int> edge;
				size_t k=0;
				STRING2VECTOR(row1[2].data<std::string>(),edge);
				STRING2VECTOR(row1[3].data<std::string>(),fbx);
				STRING2VECTOR(row1[4].data<std::string>(),fdx);
				STRING2VECTOR(row1[5].data<std::string>(),tdm);
				for (k = 0; k < edge.size(); k ++) {
					ttcgr_param.EdgeSelect[k]=edge[k];
					ttcgr_param.finebx[k]=fbx[k];
					ttcgr_param.finedx[k]=fdx[k];
					ttcgr_param.TDMdelay[k]=tdm[k];
				}
				ttc_param->Groups.push_back(ttcgr_param);
	

				nRows1++;
	
			}      
    }
    delete query1;query1=nullptr;
    
    //   was query 2    
    int nrdtmrocs = 0;
    if (part_nr<33) {
      nrdtmrocs = 104;
    }
    else if (part_nr>=33) {
      nrdtmrocs = 240;
    }
    
    dtmroc_condData[0].data<int>() = part_nr*100000+phi[0]*1000;
    dtmroc_condData[1].data<int>() = part_nr*100000+phi[phi.size()-1]*1000+
      (nrdtmrocs-1);
    
    //    if(m_verbose) std::cout << "DTMROC from " << part_nr*100000+phi[0]*1000+1 
    //   	      << " to " << part_nr*100000+phi[phi.size()-1]*1000+nrdtmrocs 
    //   	      << std::endl;

    
    

    if (query2){
    	coral::ICursor& cursor2 = query2->execute();
			while ( cursor2.next() ) {
				const coral::AttributeList &row2 = cursor2.currentRow();
			
				dtmroc_param.ChipID = row2[0].data<int>();
				dtmroc_param.Chip_Valid = row2[1].data<short>();
				dtmroc_param.RODgroup = row2[2].data<short>();
				dtmroc_param.RODinput = row2[3].data<int>();
				dtmroc_param.TTC_Line_FE = row2[5].data<short>();
				dtmroc_param.HW_addr_FE = row2[4].data<short>();
				dtmroc_param.Thresh0_Low_FE = row2[6].data<short>();
				dtmroc_param.Thresh0_High_FE = row2[7].data<short>();
				dtmroc_param.Thresh1_Low_FE = row2[8].data<short>();
				dtmroc_param.Thresh1_High_FE = row2[9].data<short>();
				dtmroc_param.VT_DAC0_FE = row2[10].data<short>();
				dtmroc_param.VT_DAC1_FE = row2[11].data<short>();
				dtmroc_param.Mask_FE = row2[12].data<int>();
				dtmroc_param.Pipe_Latency_FE = row2[13].data<short>();
				dtmroc_param.Clock_FE = row2[14].data<std::string>();

				ttc_param->dtmroc_params.push_back(dtmroc_param);
			
				nRows2++;
	
			}      
    }
    delete query2;query2=nullptr;
 

    //      ttc_params.push_back(ttc_param);
    ++nRows;
  }
  delete query0;


  m_session->transaction().commit();


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

  // 	for (size_t k = 0; k < ttc_params.size(); k++) {
  // 	  delete ttc_params[k];
  // 	}


  if(m_verbose) std::cout << "****************************" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRowsM << " Mapping records  *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows << " TTC records      *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows1 << " TTCGROUP records *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows2 << " DTMROC records *" << std::endl;
  if(m_verbose) std::cout << "****************************" << std::endl;


  return ttc_param;

}

//------------------------------------------------------
/// Read ROD information from the DB for one ROD module
/// identified by an input int (identifier) and fill a RODobj_t
/// data structure, therefore also info from DTMROCs connected
/// are retrieved
RODobj_t* COOLCORALClient::GetROD(int rod_id){
  //    if(m_verbose) std::cout << "\nGet ROD Queries and SubQueries"  << std::endl;


  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  RODobj_t *rod_param=NULL;
  DTMROCobj_t  dtmroc_param;

  int part_nr = rod_id/10000;
  int phi_id = (rod_id - (part_nr)*10000)/100;

  //
  int nrdtmrocs;
  int firstroc = 1;
  if (part_nr<33) {
    nrdtmrocs = 104;
  }
  //    else if (part_nr>=33) {
  //   nrdtmrocs = 120;
  //  }

  if (part_nr>=33) {
    if (rod_id%2==0){
      phi_id = phi_id/2;
      firstroc = 121;
      nrdtmrocs = 240;
    }
    else {
      phi_id = (phi_id + 1)/2;
      firstroc = 1;
      nrdtmrocs = 120;
    }
  }

  //     if(m_verbose) std::cout << "ROD_ID = " << rod_id << std::endl;
  //     if(m_verbose) std::cout << "phi_id = " << phi_id << std::endl;
  //     if(m_verbose) std::cout << "first roc = " << firstroc << std::endl;
  //     if(m_verbose) std::cout << "nr of roc = " << nrdtmrocs << std::endl;


  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string  DTMROC_TABLE;
  std::string   BCONNECT_TABLE;


    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
    BCONNECT_TABLE = "BCONNECT";


    
  m_session->transaction().start(true);

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
    
  query0->addToOutputList("RODVMESlot" ); 
  query0->addToOutputList("RODHalf" ); 
  query0->addToOutputList("EdgeSelect0" ); 
  query0->addToOutputList("EdgeSelect1" ); 
  query0->addToOutputList("EdgeSelect2" ); 
  query0->addToOutputList("EdgeSelect3" ); 
  query0->addToOutputList("GolDelay0" ); 
  query0->addToOutputList("GolDelay1" ); 
  query0->addToOutputList("GolDelay2" ); 
  query0->addToOutputList("GolDelay3" ); 
  query0->addToOutputList("RODDetID" ); 
  //    query0->addToOutputList("FEMode" ); 

  std::string rod_cond = ROD_TABLE;
  rod_cond += ".rod_UID = :rodnr";

  //"BROD.rod_UID = :rodnr";
  coral::AttributeList rod_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_condData.extend<int>( "rodnr" );
    
  query0->setCondition( rod_cond, rod_condData);
        

  rod_condData[0].data<int>() = rod_id;

  //    if(m_verbose) std::cout << "ROD ID = " << rod_id << std::endl;

    
  // query on dtmroc

  coral::IQuery* query2 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  query2->addToOutputList( "ChipID" ); 
  query2->addToOutputList( "ChipValid" ); 
  query2->addToOutputList( "RODGroup" ); 
  query2->addToOutputList( "RODInput" ); 
  query2->addToOutputList( "HW_Addr_FE" );
  query2->addToOutputList( "TTC_Line_FE" );
  query2->addToOutputList( "Thresh0_Low_FE" );
  query2->addToOutputList( "Thresh0_High_FE" );
  query2->addToOutputList( "Thresh1_Low_FE" );
  query2->addToOutputList( "Thresh1_High_FE" );
  query2->addToOutputList( "VT_DAC0_FE" );
  query2->addToOutputList( "VT_DAC1_FE" ); 
  query2->addToOutputList( "Mask_FE" ); 
  query2->addToOutputList( "Pipe_Latency_FE" ); 
  query2->addToOutputList( "Clock_FE" ); 

  std::string dtmroc_cond = DTMROC_TABLE;
  dtmroc_cond += ".dtmroc_UID >= :roc1 AND ";
  dtmroc_cond += DTMROC_TABLE;
  dtmroc_cond += ".dtmroc_UID <= :roc104";
  //    std::string dtmroc_cond = "BDTMROC.dtmroc_UID >= :roc1 AND BDTMROC.dtmroc_UID <= :roc104";
  coral::AttributeList dtmroc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_condData.extend<int>( "roc1" );
  dtmroc_condData.extend<int>( "roc104" );

  query2->setCondition( dtmroc_cond, dtmroc_condData);
    
  int nRows = 0;
  int nRows2 = 0;

  // this code looks erroneous
  coral::ICursor& cursor0 = query0->execute();
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    if (rod_param) delete rod_param;
    rod_param = new RODobj_t;
    rod_param->VMEslot = row0[0].data<int>();
    rod_param->RODhalf = row0[1].data<int>();
    rod_param->EdgeSelect[0] = row0[2].data<int>();
    rod_param->EdgeSelect[1] = row0[3].data<int>();
    rod_param->EdgeSelect[2] = row0[4].data<int>();
    rod_param->EdgeSelect[3] = row0[5].data<int>();
    rod_param->GolDelay[0] = row0[6].data<int>();
    rod_param->GolDelay[1] = row0[7].data<int>();
    rod_param->GolDelay[2] = row0[8].data<int>();
    rod_param->GolDelay[3] = row0[9].data<int>();
    rod_param->DetID = row0[10].data<int>();
    
    dtmroc_condData[0].data<int>() = part_nr*100000+phi_id*1000+(firstroc-1);
    dtmroc_condData[1].data<int>() = part_nr*100000+phi_id*1000+
      (nrdtmrocs-1);

    if (query2){
			coral::ICursor& cursor2 = query2->execute();
			while ( cursor2.next() ) {
				const coral::AttributeList &row2 = cursor2.currentRow();
			
				dtmroc_param.ChipID = row2[0].data<int>();
				dtmroc_param.Chip_Valid = row2[1].data<short>();
				dtmroc_param.RODgroup = row2[2].data<short>();
				dtmroc_param.RODinput = row2[3].data<int>();
				dtmroc_param.TTC_Line_FE = row2[5].data<short>();
				dtmroc_param.HW_addr_FE = row2[4].data<short>();
				dtmroc_param.Thresh0_Low_FE = row2[6].data<short>();
				dtmroc_param.Thresh0_High_FE = row2[7].data<short>();
				dtmroc_param.Thresh1_Low_FE = row2[8].data<short>();
				dtmroc_param.Thresh1_High_FE = row2[9].data<short>();
				dtmroc_param.VT_DAC0_FE = row2[10].data<short>();
				dtmroc_param.VT_DAC1_FE = row2[11].data<short>();
				dtmroc_param.Mask_FE = row2[12].data<int>();
				dtmroc_param.Pipe_Latency_FE = row2[13].data<short>();
				dtmroc_param.Clock_FE = row2[14].data<std::string>();
				rod_param->dtmroc_params.push_back(dtmroc_param);
				nRows2++;
			}      
    }
    delete query2; query2=nullptr;
    ++nRows;
  }
  delete query0;

  m_session->transaction().commit();


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
	
  if(m_verbose) std::cout << "****************************" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows << " ROD records      *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows2 << " DTMROC records *" << std::endl;
  if(m_verbose) std::cout << "****************************" << std::endl;

  return rod_param;

}

//--------------------------------------------------------------------------
//---------------------------------------------------------------------------
/// Drop COOL folders

void COOLCORALClient::dropFolders(const std::string& part_trt){
  if(m_verbose) std::cout << "\nCOOL Client: Drop folders" << std::endl;
    
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  std::string ROOT_FOLDER = "/TRT";
  std::string TTC_FOLDER;
  std::string TTCGR_FOLDER;
  std::string ROD_FOLDER;
  std::string DTMROC_FOLDER;


  if (part_trt=="barrel"){
    TTC_FOLDER = "/TRT/BTTC";
    TTCGR_FOLDER = "/TRT/BTTCGR";
    ROD_FOLDER = "/TRT/BROD";
    DTMROC_FOLDER = "/TRT/BDTMROC";
  }
  else if (part_trt=="endcap") {
    TTC_FOLDER = "/TRT/ECTTC";
    TTCGR_FOLDER = "/TRT/ECTTCGR";
    ROD_FOLDER = "/TRT/ECROD";
    DTMROC_FOLDER = "/TRT/ECDTMROC";
  }

    
  if(m_verbose) std::cout << "Deleting the old folders: " << std::endl;
  if(m_verbose) std::cout <<  TTC_FOLDER << std::endl;
  if(m_verbose) std::cout <<  TTCGR_FOLDER << std::endl;
  if(m_verbose) std::cout <<  ROD_FOLDER << std::endl;
  if(m_verbose) std::cout <<  DTMROC_FOLDER << std::endl;
  if(m_verbose) std::cout <<  ROOT_FOLDER << std::endl;
    

  //if(m_db->existsFolder("/folder1")) m_db->dropNode("/folder1");
  //m_db->createFolderSet("/folder1");
  m_db->dropNode(TTC_FOLDER);
  m_db->dropNode(TTCGR_FOLDER);
  m_db->dropNode(ROD_FOLDER);
  m_db->dropNode(DTMROC_FOLDER);
  //    dbSvc.dropDatabase( m_dbId );
  m_db->dropNode(ROOT_FOLDER);
  //    m_db->dropDatabase();

  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;


}
//---------------------------------------------------------------------------
void COOLCORALClient::createFolders(const std::string& part_trt){
  if(m_verbose) std::cout << "\nCOOL Client: Create folders" << std::endl;




  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);


  std::string ROOT_FOLDER = "/TRT";
  std::string TTC_FOLDER;
  std::string TTCGR_FOLDER;
  std::string ROD_FOLDER;
  std::string DTMROC_FOLDER;


  if (part_trt=="barrel"){
    TTC_FOLDER = "/TRT/BTTC";
    TTCGR_FOLDER = "/TRT/BTTCGR";
    ROD_FOLDER = "/TRT/BROD";
    DTMROC_FOLDER = "/TRT/BDTMROC";
  }
  else if (part_trt=="endcap") {
    TTC_FOLDER = "/TRT/ECTTC";
    TTCGR_FOLDER = "/TRT/ECTTCGR";
    ROD_FOLDER = "/TRT/ECROD";
    DTMROC_FOLDER = "/TRT/ECDTMROC";
  }
    
  if(m_verbose) std::cout << "Dropping old folders" << std::endl;
  if(m_verbose) std::cout <<  TTC_FOLDER << std::endl;
  if(m_verbose) std::cout <<  TTCGR_FOLDER << std::endl;
  if(m_verbose) std::cout <<  ROD_FOLDER << std::endl;
  if(m_verbose) std::cout <<  DTMROC_FOLDER << std::endl;

    

  m_db->dropNode(TTC_FOLDER);
  m_db->dropNode(TTCGR_FOLDER);
  m_db->dropNode(ROD_FOLDER);
  m_db->dropNode(DTMROC_FOLDER);
  m_db->dropNode(ROOT_FOLDER);
    

  if(m_verbose) std::cout << "Creating the folders: " << std::endl;
  if(m_verbose) std::cout <<  TTC_FOLDER << std::endl;
  if(m_verbose) std::cout <<  TTCGR_FOLDER << std::endl;
  if(m_verbose) std::cout <<  ROD_FOLDER << std::endl;
  if(m_verbose) std::cout <<  DTMROC_FOLDER << std::endl;

  // introducing a bug on purpose
  if (part_trt=="barrel"){
    m_db->createFolderSet(ROOT_FOLDER);
  }

  //    cool::ExtendedAttributeListSpecification spec_ttc;
  //    spec_ttc.push_back("ttc_UID","unsigned int");
  //    spec_ttc.push_back("ttc_iovfk","unsigned long long");
  //    spec_ttc.push_back("tag","string");
  cool::RecordSpecification spec_ttc;
  spec_ttc.extend("ttc_UID",cool::StorageType::UInt32);
  spec_ttc.extend("ttc_iovfk",cool::StorageType::UInt63);
  spec_ttc.extend("tag",cool::StorageType::String4k);
  //   coral::AttributeSpecification::typeNameForId( typeid(std::string) ) 
  cool::FolderSpecification ttcFolderSpec(cool::FolderVersioning::SINGLE_VERSION,spec_ttc);
  m_db->createFolder(TTC_FOLDER, ttcFolderSpec , "TTC" );//why is description not athena standard?

  //    cool::ExtendedAttributeListSpecification spec_ttcgr;
  //    spec_ttcgr.push_back("ttcgr_UID","unsigned int");
  //    spec_ttcgr.push_back("ttcgr_iovfk","unsigned long long");
  //    spec_ttcgr.push_back("tag","string");
  cool::RecordSpecification spec_ttcgr;
  spec_ttcgr.extend("ttcgr_UID",cool::StorageType::UInt32);
  spec_ttcgr.extend("ttcgr_iovfk",cool::StorageType::UInt63);
  spec_ttcgr.extend("tag",cool::StorageType::String4k);
    
  cool::FolderSpecification ttcgrFolderSpec(cool::FolderVersioning::SINGLE_VERSION,spec_ttcgr);  
  m_db->createFolder(TTCGR_FOLDER, ttcgrFolderSpec, "TTCGR");
    
  //    cool::ExtendedAttributeListSpecification spec_rod;
  //    spec_rod.push_back("rod_UID","unsigned int");
  //    spec_rod.push_back("rod_iovfk","unsigned long long");
  //    spec_rod.push_back("tag","string");
  cool::RecordSpecification spec_rod;
  spec_rod.extend("rod_UID",cool::StorageType::UInt32);
  spec_rod.extend("rod_iovfk",cool::StorageType::UInt63);
  spec_rod.extend("tag",cool::StorageType::String4k);
  cool::FolderSpecification rodFolderSpec(cool::FolderVersioning::SINGLE_VERSION,spec_rod);  
  m_db->createFolder(ROD_FOLDER, rodFolderSpec, "ROD");

  //    cool::ExtendedAttributeListSpecification spec_dtmroc;
  //    spec_dtmroc.push_back("dtmroc_UID","unsigned int");
  //    spec_dtmroc.push_back("dtmroc_iovfk","unsigned long long");
  //    spec_dtmroc.push_back("tag","string");
  cool::RecordSpecification spec_dtmroc;
  spec_dtmroc.extend("dtmroc_UID",cool::StorageType::UInt32);
  spec_dtmroc.extend("dtmroc_iovfk",cool::StorageType::UInt63);
  spec_dtmroc.extend("tag",cool::StorageType::String4k);
  cool::FolderSpecification dtmrocFolderSpec(cool::FolderVersioning::SINGLE_VERSION,spec_dtmroc);  
  m_db->createFolder(DTMROC_FOLDER, dtmrocFolderSpec, "DTMROC");


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;



}

//------------------------------------------------------------------------
/// Print the content a COOL folder identified by a string specifying
/// the TRT partition
void COOLCORALClient::printFoldersContent(const std::string& part_trt){

  if(m_verbose) std::cout << "part_trt = " << part_trt << std::endl;

  if(m_verbose) std::cout << "\nCOOL Client: Print folders content" << std::endl;

  std::string ROOT_FOLDER = "/TRT";
  std::string TTC_FOLDER;
  std::string TTCGR_FOLDER;
  std::string ROD_FOLDER;
  std::string DTMROC_FOLDER;


    TTC_FOLDER = "/TRT/TTC";
    TTCGR_FOLDER = "/TRT/TTCGR";
    ROD_FOLDER = "/TRT/ROD";
    DTMROC_FOLDER = "/TRT/DTMROC";



  cool::IFolderPtr ttc_fld = m_db->getFolder (TTC_FOLDER);
  cool::IFolderPtr ttcgr_fld = m_db->getFolder (TTCGR_FOLDER);
  cool::IFolderPtr rod_fld = m_db->getFolder (ROD_FOLDER);
  cool::IFolderPtr dtmroc_fld = m_db->getFolder (DTMROC_FOLDER);

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  if(m_verbose) std::cout << "Print folder content: " << TTC_FOLDER << std::endl;
  //cool::IFolderPtr f1 = m_db->getFolder(TTC_FOLDER );
  cool::ValidityKey since = 0;
  cool::ValidityKey until = cool::ValidityKeyMax;
   

  //printf("Max Key %d\n", until );
  cool::IObjectIteratorPtr objects = ttc_fld->browseObjects( since, until, ChannelSelection::all() );
  //  cool::IObjectIteratorPtr objects = ttc_fld->browseObjects( since, until, 0 );
  if(m_verbose) std::cout
    << "         Id  Ch  IOV       Payload             Insertion time"
    << std::endl;
  int run=0;
  while ( objects->goToNext() ) {
    // cool::IObjectPtr obj = objects->next();
    if(m_verbose) std::cout << objects->currentRef() << std::endl;
    run++;
  }
  printf("Total: %d  rows\n",run);


  if(m_verbose) std::cout << "Print folder content: " << TTCGR_FOLDER << std::endl;
  objects = ttcgr_fld->browseObjects( since, until, ChannelSelection::all() );
  run=0;
  while ( objects->goToNext() ) {
    //const cool::IObject& obj = objects->currentRef();
    //   if(m_verbose) std::cout << objects->next() << std::endl;
    run++;
  }
  printf("Total: %d  rows\n",run);

  if(m_verbose) std::cout << "Print folder content: " << ROD_FOLDER << std::endl;
  objects = rod_fld->browseObjects( since, until, ChannelSelection::all() );
  run=0;
  while ( objects->goToNext() ) {
    //const cool::IObject& obj = objects->currentRef();
    //   if(m_verbose) std::cout << objects->next() << std::endl;
    run++;
  }
  printf("Total: %d  rows\n",run);


  if(m_verbose) std::cout << "Print folder content: " << DTMROC_FOLDER << std::endl;
  objects = dtmroc_fld->browseObjects( since, until, ChannelSelection::all() );
  run=0;
  while ( objects->goToNext() ) {
    //const cool::IObject& obj = objects->currentRef();
    //      if(m_verbose) std::cout << objects->next() << std::endl;
    run++;
  }
  printf("Total: %d  rows\n",run);






  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;


}


//------------------------------------------------------------------------------
/// Fill all COOL tables and all CORAL tables specifying a string to indicate
/// the TRT partition and a string to indicate a tag
void COOLCORALClient::fillFolderTables(const std::string& part_trt, const std::string& tag){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Fill folders and tables" << std::endl;

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  m_session->transaction().start();



  struct timeval nunc_time;
  unsigned long long nunc_usecs;
    


  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string DTMROC_TABLE;
  std::string TTC_FOLDER;
  std::string TTCGR_FOLDER;
  std::string ROD_FOLDER;
  std::string DTMROC_FOLDER;

    
  if (part_trt=="barrel"){
    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
    TTC_FOLDER = "/TRT/BTTC";
    TTCGR_FOLDER = "/TRT/BTTCGR";
    ROD_FOLDER = "/TRT/BROD";
    DTMROC_FOLDER = "/TRT/BDTMROC";
  }
  else if (part_trt=="endcap") {
    TTC_TABLE = "ECTTC";
    TTCGR_TABLE = "ECTTCGR";
    ROD_TABLE = "ECROD";
    DTMROC_TABLE = "ECDTMROC";
    TTC_FOLDER = "/TRT/ECTTC";
    TTCGR_FOLDER = "/TRT/ECTTCGR";
    ROD_FOLDER = "/TRT/ECROD";
    DTMROC_FOLDER = "/TRT/ECDTMROC";
  }

  if(m_verbose) std::cout << "\nCOOL Client: Filling folders" << std::endl;

  cool::IFolderPtr ttc_fld = m_db->getFolder (TTC_FOLDER);
  cool::IFolderPtr ttcgr_fld = m_db->getFolder (TTCGR_FOLDER);
  cool::IFolderPtr rod_fld = m_db->getFolder (ROD_FOLDER);
  cool::IFolderPtr dtmroc_fld = m_db->getFolder (DTMROC_FOLDER);
    

  ttc_fld->setupStorageBuffer();
  ttcgr_fld->setupStorageBuffer();
  rod_fld->setupStorageBuffer();
  dtmroc_fld->setupStorageBuffer();


  gettimeofday(&nunc_time, NULL);

  //    nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);
  nunc_usecs = nunc_time.tv_sec;


  //
  // Update the key and use it now
  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( "KEYGEN" ).dataEditor();

  coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer.extend<long long>( "Key" );

  std::string updateAction = "Key = Key + :offset";
  std::string updateCondition = "Key > :kk";
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
  updateData.extend<long long>("offset");
  updateData.extend<long long>("kk");
  updateData[0].data<long long>() = 1;
  updateData[1].data<long long>() = 999;
  long rowsUpdated = keyeditor.updateRows( updateAction, updateCondition, updateData );
  if(m_verbose) std::cout << "Updated " << rowsUpdated << " rows" << std::endl;

  // UP

  m_session->transaction().commit();

  m_session->transaction().start();


  // PUT here a query on KEYGEN table
  coral::IQuery* query00 = m_session->nominalSchema().tableHandle("KEYGEN").newQuery();

  query00->addToOutputList("Key" ); 

  std::string conn_cond = "KEYGEN.Key > :kk";

  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<long long>( "kk" );
    
  query00->setCondition( conn_cond, conn_condData);
        

    
  conn_condData[0].data<long long>() = 999;

  long long key;

  coral::ICursor& cursor00 = query00->execute();
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();
    key = row00[0].data<long long>();
  }
  delete query00;

  if(m_verbose) std::cout << "Key = " << key << std::endl;


  // Update the key and use it now
  coral::ITableDataEditor& cqeditor = m_session->nominalSchema().tableHandle( "C_QUERIES" ).dataEditor();

  coral::AttributeList rowBuffer1 ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer1.extend<std::string>( "Object" );
  rowBuffer1.extend<long long>( "Key" );
  rowBuffer1.extend<long long>( "Time" );

  std::string& cqobj = rowBuffer1[ "Object" ].data<std::string>();
  long long& cqkey = rowBuffer1[ "Key" ].data<long long>(); 
  long long& cqtime = rowBuffer1[ "Time" ].data<long long>(); 

  cqobj = part_trt;
  cqkey = key;
  cqtime = nunc_usecs;

  cqeditor.insertRow( rowBuffer1 );





  //    ValidityKey since = nunc_usecs/10;
  ValidityKey since = nunc_usecs;   // was key, but it doesn't make sense

  ValidityKey until = ValidityKeyMax;

  //    coral::AttributeList ttc_cool_row(ttc_fld->payloadSpecification());
  //    coral::AttributeList ttcgr_cool_row(ttcgr_fld->payloadSpecification());
  //    coral::AttributeList rod_cool_row(rod_fld->payloadSpecification());
  //    coral::AttributeList dtmroc_cool_row(dtmroc_fld->payloadSpecification());

  //(ttc_fld->folderAttributes()).attributeList() ;
  //coral::AttributeList ttcgr_cool_row=(ttcgr_fld->folderAttributes()).attributeList() ;
  //coral::AttributeList rod_cool_row=(rod_fld->folderAttributes()).attributeList();
  //coral::AttributeList dtmroc_cool_row=(dtmroc_fld->folderAttributes()).attributeList();
  //ttc_cool_row.toOutputStream(std::cout);
  //coral::AttributeListSpecification ttc_spec;
  //ttc_spec.extend("ttc_UID",
  //                     coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  //coral::AttributeList ttc_cool_row(ttc_spec);
  /*
    coral::AttributeList ttc_cool_row;
    cool::Record ttc_rec(ttc_fld->payloadSpecification(),ttc_cool_row); 
    coral::AttributeList ttcgr_cool_row;
    cool::Record ttcgr_rec(ttcgr_fld->payloadSpecification(),ttcgr_cool_row); 
    coral::AttributeList rod_cool_row;
    cool::Record rod_rec(rod_fld->payloadSpecification(),rod_cool_row); 
    coral::AttributeList dtmroc_cool_row;
    cool::Record dtmroc_rec(dtmroc_fld->payloadSpecification(),dtmroc_cool_row); 
  */

  cool::RecordSpecification ttc_rec =(ttc_fld->payloadSpecification());
  coral::AttributeList ttc_cool_row ATLAS_THREAD_SAFE = Record(ttc_rec).attributeList(); // ok
  cool::RecordSpecification ttcgr_rec =(ttcgr_fld->payloadSpecification());
  coral::AttributeList ttcgr_cool_row ATLAS_THREAD_SAFE = Record(ttcgr_rec).attributeList(); // ok
  cool::RecordSpecification rod_rec =(rod_fld->payloadSpecification());
  coral::AttributeList rod_cool_row ATLAS_THREAD_SAFE = Record(rod_rec).attributeList(); // ok
  cool::RecordSpecification dtmroc_rec =(dtmroc_fld->payloadSpecification());
  coral::AttributeList dtmroc_cool_row ATLAS_THREAD_SAFE = Record(dtmroc_rec).attributeList(); // ok



  if(m_verbose) std::cout << "Filling table " << TTC_TABLE  << std::endl;
  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);
  // ttc_row.toOutputStream( std::cout ) << std::endl;
  //--------
  // TRT-TTC quantities

  unsigned int tuid;
  unsigned long long iovfkttc=0;
  short tbyteswap;
  unsigned int tphysaddr;
  unsigned int detid; // hex
  unsigned int vmeslot; //hex
  unsigned int delay;
  short shaping;
  short sendid;
  std::string ttcname;
  std::string clocksource;
  std::string tfragtype;
  std::string tcomment;
	
  //--------//

	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,32);

  int run=0;	
	
  const char* filename="ttc.dat";
  if (part_trt=="barrel"){
    filename = "RCDTTC.dat";
    //filename = "TTC06.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDTTC.dat";
  }
  ifstream itAfile(filename);

  std::vector<char> buffer(999);
  if (itAfile.is_open()){
    while (itAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> tuid >> iovfkttc
	   >> tbyteswap >> tphysaddr
	   >> detid >> vmeslot
	   >> delay
	   >> shaping >> sendid 
	   >>  clocksource >> ttcname 
	   >>  tfragtype >> tcomment;
	    
      //	    if(m_verbose) std::cout << "Read TTC data done..." << std::endl;
	
      ttc_row["ttc_UID"].setValue <int> (tuid);
      // 	    ttc_row["ttc_iovfk"].setValue <long long> (iovfkttc);
      // 	    ttc_row["ttc_iovfk"].setValue <long long> (nunc_usecs/10);
      ttc_row["ttc_iovfk"].setValue <long long> (key);
      ttc_row["ByteSwapping"].setValue <short> (tbyteswap);
      ttc_row["PhysicalAddress"].setValue <int> (tphysaddr);
      ttc_row["DetID"].setValue <int> (detid);
      ttc_row["VMESlot"].setValue <int> (vmeslot);
      ttc_row["Delay"].setValue <int> (delay);
      ttc_row["ArShaping"].setValue <short> (shaping);
      ttc_row["SetSendID"].setValue <short> (sendid);
      ttc_row["Name"].setValue <std::string> (ttcname);
      ttc_row["ClockSource"].setValue <std::string> (clocksource);
      ttc_row["InputFragmentType"].setValue <std::string> (tfragtype);
      ttc_row["Comment"].setValue <std::string> (tcomment);
      ttc_row["ttc_tag"].setValue <std::string> (tag);
	    

      ttc_cool_row["ttc_UID"].setValue <unsigned int> (tuid);
      // 
      //	    ttc_cool_row["ttc_iovfk"].setValue <unsigned long long> (iovfkttc);
      //	    ttc_cool_row["ttc_iovfk"].setValue <unsigned long long> (nunc_usecs/10);
      ttc_cool_row["ttc_iovfk"].setValue <unsigned long long> (key);
      ttc_cool_row["tag"].setValue <std::string> (tag);
      cool::Record ttcRecord(ttc_rec,ttc_cool_row); 
      ttc_fld->storeObject( since, until, ttcRecord, tuid);

      run++;
	    
      //ttc_editor.insertRow(ttc_row);
      ttc_bulk->processNextIteration();
    }
  } 


  itAfile.close();

  ttc_fld->flushStorageBuffer();	    

  ttc_bulk->flush();
  delete ttc_bulk;
	
  printf("Total: %d  rows\n",run);

  //----  TTCGROUP ------//

  if(m_verbose) std::cout << "Filling table"<< TTCGR_TABLE  << std::endl;
  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
  // ttc_row.toOutputStream( std::cout ) << std::endl;
  //--------
  // TRT-TTC quantities

  unsigned int tgruid;
  unsigned int group;
  unsigned int duty;
  std::string edgeselect; // vector of 10 elements
  std::string bx; // vector of 10 elements
  std::string dout; // vector of 10 elements
  std::string tdmdelay; // vector of 10 elements
  std::string tgrname;

  std::vector <int> my_vector;
	

  //--------//

	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,16);

  unsigned int bxsz;
  std::vector <unsigned int> bxv(10);
	
  unsigned int doutsz;
  std::vector <unsigned int> doutv(10);
	
  unsigned int edgesz;
  std::vector <unsigned int> edgev(10);
	
  unsigned int tdmdelaysz;
  std::vector <unsigned int> tdmdelayv(10);
	
  run=0;	
	
  //		ifstream itGRfile ("RCDTTCGROUP.dat");
  //		ifstream itGRfile ("TTCGROUPTEMP.dat");

  if (part_trt=="barrel"){
    filename = "RCDTTCGROUP.dat";
    // filename = "TTCGROUPTEMP.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDTTCGROUP.dat";
  }

  ifstream itGRfile (filename);
  //		ifstream itGRfile ("ECRCDTTCGROUP.dat");
  //	std::vector<char> buffer(999);
  if (itGRfile.is_open()){
    while (itGRfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> tgruid >> tgrname >> group 
	   >> duty
	   >> edgesz 
	   >> edgev[0] >> edgev[1] >> edgev[2] >> edgev[3] >> edgev[4]
	   >> edgev[5] >> edgev[6] >> edgev[7] >> edgev[8] >> edgev[9] 
	   >> bxsz
	   >> bxv[0] >> bxv[1] >> bxv[2] >> bxv[3] >> bxv[4] >> bxv[5] 
	   >> bxv[6]  >> bxv[7] >> bxv[8] >> bxv[9]  
	   >>  doutsz 
	   >> doutv[0] >> doutv[1] >> doutv[2] >> doutv[3] >> doutv[4] 
	   >> doutv[5] >> doutv[6]  >> doutv[7] >> doutv[8] >> doutv[9] 
	   >> tdmdelaysz
	   >> tdmdelayv[0] >> tdmdelayv[1] >> tdmdelayv[2] 
	   >> tdmdelayv[3] >> tdmdelayv[4] >> tdmdelayv[5] 
	   >> tdmdelayv[6]  >> tdmdelayv[7] >> tdmdelayv[8] 
	   >> tdmdelayv[9];
	    
	    
      VECTOR2STRING (bxv, bx);
      VECTOR2STRING (doutv, dout);
      VECTOR2STRING (edgev, edgeselect);
      VECTOR2STRING (tdmdelayv, tdmdelay);
	    
	
      ttcgr_row["ttcgr_UID"].setValue <int> (tgruid);
      //	    ttcgr_row["ttcgr_iovfk"].setValue <long long> (iovfkttc);
      //	    ttcgr_row["ttcgr_iovfk"].setValue <long long> (nunc_usecs/10);
      ttcgr_row["ttcgr_iovfk"].setValue <long long> (key);
      ttcgr_row["Group"].setValue <int> (group);
      ttcgr_row["DutyCycle"].setValue <int> (duty);
      ttcgr_row["EdgeSelect"].setValue <std::string> (edgeselect);
      ttcgr_row["finebx"].setValue <std::string> (bx);
      ttcgr_row["finedx"].setValue <std::string> (dout);
      ttcgr_row["TDMdelay"].setValue <std::string> (tdmdelay);
      ttcgr_row["TTCGRNAME"].setValue <std::string> (tgrname);
      ttcgr_row["ttcgr_tag"].setValue <std::string> (tag);
	    

      ttcgr_cool_row["ttcgr_UID"].setValue <unsigned int> (tgruid);
      //   ttcgr_cool_row["ttcgr_iovfk"].setValue <unsigned long long> (iovfkttc);
      //   ttcgr_cool_row["ttcgr_iovfk"].setValue <unsigned long long> (nunc_usecs/10);
      ttcgr_cool_row["ttcgr_iovfk"].setValue <unsigned long long> (key);
      ttcgr_cool_row["tag"].setValue <std::string> (tag);
      cool::Record ttcgrRecord(ttcgr_rec, ttcgr_cool_row);
      ttcgr_fld->storeObject( since, until, ttcgrRecord, tgruid);


      run++;
	    
      //ttc_editor.insertRow(ttcgr_row);
      ttcgr_bulk->processNextIteration();
    }
  } 

	    

  itGRfile.close();

  ttcgr_fld->flushStorageBuffer();	    
  ttcgr_bulk->flush();
  delete ttcgr_bulk;
	
  printf("Total: %d  rows\n",run);





  //-----------    ROD      ----------------------------------------------------
  if(m_verbose) std::cout << "Filling table"<< ROD_TABLE  << std::endl;
  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);
  //  rod_row.toOutputStream( std::cout ) << std::endl;
  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);


  unsigned int rodid;
  short rodbyteswap;
  unsigned int rodphysaddr;
  unsigned int roddetid;    // hex
  unsigned int rodvmeslot;  // hex
  unsigned int rodhalf; 
  unsigned int rodES0;  // one per each group
  unsigned int rodES1;
  unsigned int rodES2;
  unsigned int rodES3;
  unsigned int rodGol0;  // one per each group
  unsigned int rodGol1;
  unsigned int rodGol2;
  unsigned int rodGol3;
  short rodstatus;
	
  std::string rodfragtype;
  std::string rodname;
  std::string rcomment;
	
	
  // Barrel A
  run=0;
  if (part_trt=="barrel"){
    filename = "RCDROD.dat";
    //filename = "ROD06.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDROD.dat";
  }

  ifstream irAfile (filename);


  if (irAfile.is_open()){
    while (irAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> rodid 
	   >> rodbyteswap >> rodphysaddr
	   >> roddetid >> rodvmeslot
	   >> rodhalf
	   >> std::hex >> rodES0 >> rodES1
	   >> rodES2 >> rodES3 >> std::dec
	   >> rodGol0 >> rodGol1
	   >> rodGol2 >> rodGol3
	   >> rodstatus >> rodfragtype
	   >> rodname >> rcomment;

      rod_row["rod_UID"].setValue <int> (rodid);
      //    rod_row["rod_iovfk"].setValue <long long> (iovfkttc);
      // rod_row["rod_iovfk"].setValue <long long> (nunc_usecs/10);
      rod_row["rod_iovfk"].setValue <long long> (key);
      rod_row["RODByteSwapping"].setValue <short> (rodbyteswap);
      rod_row["RODPhysicalAddress"].setValue <int> (rodphysaddr);
      rod_row["RODDetID"].setValue <int> (roddetid);
      rod_row["RODVMESlot"].setValue <int> (rodvmeslot);
      rod_row["RODHalf"].setValue <int> (rodhalf);
      rod_row["EdgeSelect0"].setValue <int> (rodES0);
      rod_row["EdgeSelect1"].setValue <int> (rodES1);
      rod_row["EdgeSelect2"].setValue <int> (rodES2);
      rod_row["EdgeSelect3"].setValue <int> (rodES3);
      rod_row["GolDelay0"].setValue <int> (rodGol0);
      rod_row["GolDelay1"].setValue <int> (rodGol1);
      rod_row["GolDelay2"].setValue <int> (rodGol2);
      rod_row["GolDelay3"].setValue <int> (rodGol3);
      rod_row["RODStatus"].setValue <short> (rodstatus);
      rod_row["RODInputFragmentType"].setValue <std::string> (rodfragtype);
      rod_row["RODName"].setValue <std::string> (rodname);
      rod_row["Comment"].setValue <std::string> (rcomment);
      rod_row["rod_tag"].setValue <std::string> (tag);
	    
      rod_cool_row["rod_UID"].setValue <unsigned int> (rodid);
      //	    rod_cool_row["rod_iovfk"].setValue <unsigned long long> (iovfkttc);
      //	    rod_cool_row["rod_iovfk"].setValue <unsigned long long> (nunc_usecs/10);
      rod_cool_row["rod_iovfk"].setValue <unsigned long long> (key);
      rod_cool_row["tag"].setValue <std::string> (tag);
      cool::Record rodRecord(rod_rec, rod_cool_row);
      rod_fld->storeObject( since, until, rodRecord, rodid);

  

 
      rod_bulk->processNextIteration();
      run++;
    }
  }
  irAfile.close();


  rod_fld->flushStorageBuffer();	    
  rod_bulk->flush();
  delete rod_bulk;

  printf("Total: %d  rows\n",run);
  // DTMROC
	
  if(m_verbose) std::cout << "Filling table"<< DTMROC_TABLE  << std::endl;
  coral::ITableDataEditor& dtmroc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList dtmroc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_editor.rowBuffer(dtmroc_row);
  //    dtmroc_row.toOutputStream( std::cout ) << std::endl;
  coral::IBulkOperation* dtmroc_bulk= dtmroc_editor.bulkInsert(dtmroc_row,32);
	
  unsigned int uid;
  short chipvalid;
  short byteswap;
  unsigned int physaddr;
  short rodgroup, rodinput, hwaddr, ttcline;
  short thresh0Low, thresh0High;
  short thresh1Low, thresh1High;
  short vtdac0, vtdac1;
  unsigned int mask; // hex
  short pipelatency;
	
  std::string fragtype;
  std::string clock;
  std::string comment;
  std::string dtname;
	
  int num_size,num_phi,num_chip, chipID;
  char size;
  // Barrel A
  run=0;
  if (part_trt=="barrel"){
    filename = "RCDDTMROC.dat";
    //filename = "DTMROC06.dat";
  }
  else if (part_trt=="endcap"){
    filename = "ECRCDDTMROC.dat";
  }
  ifstream idAfile (filename);

  if (idAfile.is_open()){
    while (idAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
      std::istringstream sbuf(&buffer[0]);
      sbuf >> byteswap >> physaddr
	   >> fragtype >> dtname
	   >> uid >> chipvalid
	   >> rodgroup >> rodinput
	   >> hwaddr >> ttcline
	   >> thresh0Low >> thresh0High
	   >> thresh1Low >> thresh1High
	   >> vtdac0 >> vtdac1
	   >> mask >> pipelatency
	   >> clock
	   >> comment;

	    
      char boardch;
      int boardn;
	    
      if (part_trt=="barrel"){
	sscanf(dtname.c_str(),"roc-%d%c_%d",&num_phi,&size,&num_chip);
	if(size=='A')num_size=31;
	else num_size=32;
	chipID=num_size*100000+num_phi*1000+num_chip;
      }
      else if (part_trt=="endcap"){
	sscanf(dtname.c_str(),"roc-%d%c_%c%d_%d",&num_phi,&size,&boardch,&boardn,&num_chip);
	if(size=='A')num_size=33;
	else num_size=34;
	chipID=num_size*100000+num_phi*1000+num_chip;
      }


      dtmroc_row["dtmroc_UID"].setValue <int> (chipID);
      //	    dtmroc_row["dtmroc_iovfk"].setValue <long long> (iovfkttc);
      //	    dtmroc_row["dtmroc_iovfk"].setValue <long long> (nunc_usecs/10);
      dtmroc_row["dtmroc_iovfk"].setValue <long long> (key);
      dtmroc_row["DTMROCByteSwapping"].setValue <short> (byteswap);
      dtmroc_row["DTMROCPhysicalAddress"].setValue <int> 
	(physaddr);
      dtmroc_row["ChipID"].setValue <int> (uid);
      dtmroc_row["ChipValid"].setValue <short> (chipvalid);
      dtmroc_row["RODGroup"].setValue <short> (rodgroup);
      dtmroc_row["RODInput"].setValue <int> (rodinput);
      dtmroc_row["HW_Addr_FE"].setValue <short> (hwaddr);
      dtmroc_row["TTC_Line_FE"].setValue <short> (ttcline);
      dtmroc_row["Thresh0_Low_FE"].setValue <short> (thresh0Low);
      dtmroc_row["Thresh0_High_FE"].setValue <short> (thresh0High);
      dtmroc_row["Thresh1_Low_FE"].setValue <short> (thresh1Low);
      dtmroc_row["Thresh1_High_FE"].setValue <short> (thresh1High);
      dtmroc_row["VT_DAC0_FE"].setValue <short> (vtdac0);
      dtmroc_row["VT_DAC1_FE"].setValue <short> (vtdac1);
      dtmroc_row["Mask_FE"].setValue <int> (mask);
      dtmroc_row["Pipe_Latency_FE"].setValue <short> (pipelatency);
      dtmroc_row["DTMROCName"].setValue <std::string> (dtname);
      dtmroc_row["DTMROCInputFragmentType"].setValue <std::string>
	(fragtype);
      dtmroc_row["Clock_FE"].setValue <std::string> (clock);
      dtmroc_row["Comment"].setValue <std::string> (comment);
      dtmroc_row["roc_tag"].setValue <std::string> (tag);
	    
      dtmroc_cool_row["dtmroc_UID"].setValue <unsigned int> (uid);
      dtmroc_cool_row["dtmroc_iovfk"].setValue <unsigned long long> (key);
      //	    dtmroc_cool_row["dtmroc_iovfk"].setValue <unsigned long long> (nunc_usecs/10);
      //	    dtmroc_cool_row["dtmroc_iovfk"].setValue <unsigned long long> (iovfkttc);
      dtmroc_cool_row["tag"].setValue <std::string> (tag);
      cool::Record dtmrocRecord(dtmroc_rec, dtmroc_cool_row);
      dtmroc_fld->storeObject( since, until, dtmrocRecord, chipID);

      dtmroc_bulk->processNextIteration();
      run++;
    }
  }
  idAfile.close();


  dtmroc_fld->flushStorageBuffer();	    
  dtmroc_bulk->flush();
  delete dtmroc_bulk;


  printf("Total: %d  rows\n",run);
  //if(m_verbose) std::cout<<std::endl;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;







  m_session->transaction().commit();

}


//------------------------------------------------------
/// Get TTC module information for a TTC modle identified by an int
/// TTC identifier. It fills a TTCobj_t structure. It reads as well
/// all info about the relevant TTCGROUPs and all the connected
/// DTMROCs
TTCobj_t* COOLCORALClient::GetTTCOOL(int ttc_id){
  //    if(m_verbose) std::cout << "\nGet TTC Queries and SubQueries"  << std::endl;


  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  TTCobj_t *ttc_param=NULL;
  TTCGroupobj_t ttcgr_param;
  DTMROCobj_t  dtmroc_param;

  int part_nr = ttc_id/10000 - 4  ;
  int crate_id = ttc_id/100 - (part_nr+4)*100;
  int slot_id = ttc_id - (part_nr+4)*10000 - crate_id*100;

  std::string ROOT_FOLDER = "/TRT";
  
  std::string TTC_TABLE = "HIST_TTC";
  std::string TTCGR_TABLE = "HIST_TTCGR";
  std::string ROD_TABLE = "HIST_ROD";
  std::string DTMROC_TABLE = "HIST_DTMROC";
  std::string BCONNECT_TABLE = "BCONNECT";
  std::string TTC_FOLDER = "/TRT/TTC";
  std::string TTCGR_FOLDER = "/TRT/TTCGR";
  std::string ROD_FOLDER = "/TRT/ROD";
  std::string DTMROC_FOLDER = "/TRT/DTMROC";
  
    
  m_session->transaction().start(true);


  cool::IFolderPtr ttc_fld = m_db->getFolder (TTC_FOLDER);
  cool::IFolderPtr ttcgr_fld = m_db->getFolder (TTCGR_FOLDER);
  cool::IFolderPtr rod_fld = m_db->getFolder (ROD_FOLDER);
  cool::IFolderPtr dtmroc_fld = m_db->getFolder (DTMROC_FOLDER);

  struct timeval nunc_time;
  unsigned long long nunc_usecs;
    
  gettimeofday(&nunc_time, NULL);

  // iovfk is time in nanoseconds
  //    nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);
  nunc_usecs = (nunc_time.tv_sec);

  cool::ValidityKey since = nunc_usecs;
  cool::ValidityKey until = cool::ValidityKeyMax;

  std::vector<int> phi;


  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(BCONNECT_TABLE).newQuery();

  query00->addToOutputList("phi" ); 

  std::string conn_cond = BCONNECT_TABLE;
  conn_cond += ".Partition = :part_nr AND ";
  conn_cond += BCONNECT_TABLE;
  conn_cond += ".Crate = :crate_id AND ";
  conn_cond += BCONNECT_TABLE;
  conn_cond += ".Slot = :slot_id";


  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<int>( "part_nr" );
  conn_condData.extend<int>( "crate_id" );
  conn_condData.extend<int>( "slot_id" );
    
  query00->setCondition( conn_cond, conn_condData);
        

    
  conn_condData[0].data<int>() = part_nr;
  conn_condData[1].data<int>() = crate_id;
  conn_condData[2].data<int>() = slot_id;

  coral::ICursor& cursor00 = query00->execute();
  int nRowsM = 0;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();

      
    phi.push_back(row00[0].data<int>());

    nRowsM++;
  }
  delete query00;



  // QUERY on TTC

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Comment" ); 


  // Getting the key from COOL
  cool::IObjectIteratorPtr objects = ttc_fld->browseObjects( since, until, ttc_id );

  //    int run=0;
  unsigned long long fk;
  while ( objects->goToNext() ) {
    const cool::IObject& obj = objects->currentRef();
    FromString(fk,obj.payloadValue("ttc_iovfk"));
    //      if(m_verbose) std::cout << "FK = " << fk << std::endl;
  }
    
     
  std::string ttc_cond = TTC_TABLE;
  //    ttc_cond += ".ttc_UID = :ttcnr";
  ttc_cond += ".ttc_UID = :ttcnr AND ";
  ttc_cond += TTC_TABLE;
  ttc_cond += ".ttc_iovfk = :ttcio";
    
  coral::AttributeList ttc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_condData.extend<int>( "ttcnr" );
  ttc_condData.extend<long long>( "ttcio" );
    
  query0->setCondition( ttc_cond, ttc_condData);

  ttc_condData[0].data<int>() = ttc_id;
  ttc_condData[1].data<long long>() = fk;

  //    if(m_verbose) std::cout << "TTC ID = " << ttc_id << std::endl;

  // query on TTCGROUP

  coral::IQuery* query1 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();

  query1->addToOutputList("Group" );
  query1->addToOutputList("DutyCycle" );
  query1->addToOutputList("EdgeSelect");
  query1->addToOutputList("finebx" );
  query1->addToOutputList("finedx" );
  query1->addToOutputList("TDMdelay");



  std::string ttcgr_cond = TTCGR_TABLE;
  ttcgr_cond += ".ttcgr_UID >= :ttcgr0 AND ";
  ttcgr_cond +=  TTCGR_TABLE;
  //    ttcgr_cond +=  ".ttcgr_UID <= :ttcgr1";
  ttcgr_cond +=  ".ttcgr_UID <= :ttcgr1 AND ";
  ttcgr_cond +=  TTCGR_TABLE;
  ttcgr_cond +=  ".ttcgr_iovfk = :ttcgrio";


  //"BTTCGR.ttcgr_UID >= :ttcgr0 AND BTTCGR.ttcgr_UID <= :ttcgr1";

  coral::AttributeList ttcgr_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_condData.extend<int>( "ttcgr0" );
  ttcgr_condData.extend<int>( "ttcgr1" );
  ttcgr_condData.extend<long long>( "ttcgrio" );
    
    
  query1->setCondition( ttcgr_cond, ttcgr_condData);
    
  // query on dtmroc

  coral::IQuery* query2 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  query2->addToOutputList( "ChipID" ); 
  query2->addToOutputList( "ChipValid" ); 
  query2->addToOutputList( "RODGroup" ); 
  query2->addToOutputList( "RODInput" ); 
  query2->addToOutputList( "HW_Addr_FE" );
  query2->addToOutputList( "TTC_Line_FE" );
  query2->addToOutputList( "Thresh0_Low_FE" );
  query2->addToOutputList( "Thresh0_High_FE" );
  query2->addToOutputList( "Thresh1_Low_FE" );
  query2->addToOutputList( "Thresh1_High_FE" );
  query2->addToOutputList( "VT_DAC0_FE" );
  query2->addToOutputList( "VT_DAC1_FE" ); 
  query2->addToOutputList( "Mask_FE" ); 
  query2->addToOutputList( "Pipe_Latency_FE" ); 
  query2->addToOutputList( "Clock_FE" ); 

  std::string dtmroc_cond = DTMROC_TABLE;
  dtmroc_cond += ".dtmroc_UID >= :roc1 AND ";
  dtmroc_cond += DTMROC_TABLE;
  //    dtmroc_cond += ".dtmroc_UID <= :roc104";
  dtmroc_cond += ".dtmroc_UID <= :roc104 AND ";
  dtmroc_cond += DTMROC_TABLE;
  dtmroc_cond += ".dtmroc_iovfk = :rocio";

  //"BDTMROC.dtmroc_UID >= :roc1 AND BDTMROC.dtmroc_UID <= :roc104";

  coral::AttributeList dtmroc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_condData.extend<int>( "roc1" );
  dtmroc_condData.extend<int>( "roc104" );
  dtmroc_condData.extend<long long>( "rocio" );

  query2->setCondition( dtmroc_cond, dtmroc_condData);
    
  int nRows = 0;
  int nRows1 = 0;
  int nRows2 = 0;
   


  coral::ICursor& cursor0 = query0->execute();
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    if (ttc_param) delete ttc_param;
    ttc_param = new TTCobj_t;
    ttc_param->VMEslot = row0[1].data<int>();
    ttc_param->Delay = row0[2].data<int>();
    ttc_param->ArShaping = row0[3].data<short>();
    ttc_param->SetSendID = row0[4].data<short>();
    ttc_param->DetID = row0[0].data<int>();
    ttc_param->Comment =  row0[5].data<std::string>();
    ttcgr_condData[0].data<int>() = ttc_id*10;
    int nrgroups=3;
    ttcgr_condData[1].data<int>() = ttc_id*10 + nrgroups;
    cool::IObjectIteratorPtr grobjs = ttcgr_fld->browseObjects( since, until, ttc_id*10 );
    while ( grobjs->goToNext() ) {
      const cool::IObject& obj = grobjs->currentRef();
      FromString(fk,obj.payloadValue("ttcgr_iovfk"));
    }
    ttcgr_condData[2].data<long long>() = fk;
    if (query1){
			coral::ICursor& cursor1 = query1->execute();
			while ( cursor1.next() ) {
				const coral::AttributeList &row1 = cursor1.currentRow();
				ttcgr_param.Group = row1[0].data<int>();
				ttcgr_param.DutyCycle = row1[1].data<int>();
			
				std::vector <int> fbx;
				std::vector <int> fdx;
				std::vector <int> tdm;
				std::vector <int> edge;
				size_t k=0;
				STRING2VECTOR(row1[2].data<std::string>(),edge);
				STRING2VECTOR(row1[3].data<std::string>(),fbx);
				STRING2VECTOR(row1[4].data<std::string>(),fdx);
				STRING2VECTOR(row1[5].data<std::string>(),tdm);
				for (k = 0; k < edge.size(); k ++) {
					ttcgr_param.EdgeSelect[k]=edge[k];
					ttcgr_param.finebx[k]=fbx[k];
					ttcgr_param.finedx[k]=fdx[k];
					ttcgr_param.TDMdelay[k]=tdm[k];
				}
				ttc_param->Groups.push_back(ttcgr_param);
				nRows1++;
			}      
    }
    delete query1;query1=nullptr;
    //   was query 2    
    int nrdtmrocs = 0;
    if (part_nr<33) {
      nrdtmrocs = 104;
    }
    else if (part_nr>=33) {
      nrdtmrocs = 240;
    }
    
    dtmroc_condData[0].data<int>() = part_nr*100000+phi[0]*1000;
    dtmroc_condData[1].data<int>() = part_nr*100000+phi[phi.size()-1]*1000+
      nrdtmrocs;
    cool::IObjectIteratorPtr rocobjs = dtmroc_fld->browseObjects( since, until, part_nr*100000+phi[0]*1000+1 );
    // this is buggy because assumes all dtmrocs have the same key

    unsigned long long fkroc = 0;
    while ( rocobjs->goToNext() ) {
      const cool::IObject& rocobj = rocobjs->currentRef();
      FromString(fkroc,rocobj.payloadValue("dtmroc_iovfk"));
      if(m_verbose) std::cout << "FK ROC = " << fkroc << std::endl;
    }
    dtmroc_condData[2].data<long long>() = fkroc;
    if (query2){
			coral::ICursor& cursor2 = query2->execute();
			while ( cursor2.next() ) {
				const coral::AttributeList &row2 = cursor2.currentRow();
			
				dtmroc_param.ChipID = row2[0].data<int>();
				dtmroc_param.Chip_Valid = row2[1].data<short>();
				dtmroc_param.RODgroup = row2[2].data<short>();
				dtmroc_param.RODinput = row2[3].data<int>();
				dtmroc_param.TTC_Line_FE = row2[5].data<short>();
				dtmroc_param.HW_addr_FE = row2[4].data<short>();
				dtmroc_param.Thresh0_Low_FE = row2[6].data<short>();
				dtmroc_param.Thresh0_High_FE = row2[7].data<short>();
				dtmroc_param.Thresh1_Low_FE = row2[8].data<short>();
				dtmroc_param.Thresh1_High_FE = row2[9].data<short>();
				dtmroc_param.VT_DAC0_FE = row2[10].data<short>();
				dtmroc_param.VT_DAC1_FE = row2[11].data<short>();
				dtmroc_param.Mask_FE = row2[12].data<int>();
				dtmroc_param.Pipe_Latency_FE = row2[13].data<short>();
				dtmroc_param.Clock_FE = row2[14].data<std::string>();

				ttc_param->dtmroc_params.push_back(dtmroc_param);
			
				nRows2++;
	
			}      
    }
    delete query2; query2=nullptr;
    ++nRows;
  }
  delete query0;


  m_session->transaction().commit();


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  if(m_verbose) std::cout << "****************************" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRowsM << " Mapping records  *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows << " TTC records      *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows1 << " TTCGROUP records *" << std::endl;
  if(m_verbose) std::cout << "* Total " << nRows2 << " DTMROC records *" << std::endl;
  if(m_verbose) std::cout << "****************************" << std::endl;


  return ttc_param;

}


//------------------------------------------------------------------------------
/// Create and fill the CORAL queries auxiliary table and the KEYGEN key generation
/// table 
void COOLCORALClient::createAuxTables(){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Create AUX tables" << std::endl;
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  std::vector <std::string> tmp_strings;


  m_session->transaction().start();

  

  if(m_verbose) std::cout << "Deleting the old tables: " << std::endl;
  if(m_verbose) std::cout <<  "C_QUERIES" << std::endl;
  if(m_verbose) std::cout <<  "KEYGEN" << std::endl;

  m_session->nominalSchema().dropIfExistsTable("C_QUERIES");
  m_session->nominalSchema().dropIfExistsTable("KEYGEN");

  //-------------------CORAL Queries aux table

  if(m_verbose) std::cout << "Creating table: C_QUERIES" << std::endl;

       
  // Create the table for CORAL queries
  coral::TableDescription coque_columns( "SchemaDefinition_COQUE" );
  coque_columns.setName( "C_QUERIES" );
  coque_columns.insertColumn( "Object",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  coque_columns.insertColumn( "Key",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  coque_columns.insertColumn( "Time",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );


  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("Object");
  coque_columns.createIndex ("obj_idx", tmp_strings);


  // Create the actual table
  m_session->nominalSchema().createTable( coque_columns );

  //-------------------KEYGEN table

  if(m_verbose) std::cout << "Creating table: KEYGEN" << std::endl;

       
  // Create the table for CORAL queries
  coral::TableDescription key_columns( "SchemaDefinition_KEY" );
  key_columns.setName( "KEYGEN" );
  key_columns.insertColumn( "Key",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );


  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("Key");
  key_columns.createIndex ("key_idx", tmp_strings);


  // Create the actual table
  m_session->nominalSchema().createTable( key_columns );


  // Fill the first key

  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( "KEYGEN" ).dataEditor();

  coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer.extend<long long>( "Key" );

  long long& key = rowBuffer[ "Key" ].data<long long>();

  key = 1000;

  keyeditor.insertRow( rowBuffer );


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

 
  m_session->transaction().commit();

}

//------------------------------------------------------------------------------
/// Method to update the key in the KEYGEN table
void COOLCORALClient::UpdateKey(){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Update Key" << std::endl;
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  m_session->transaction().start();

  // Update the  key

  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( "KEYGEN" ).dataEditor();

  coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer.extend<long long>( "Key" );

  std::string updateAction = "Key = Key + :offset";
  std::string updateCondition = "Key > :kk";
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
  updateData.extend<long long>("offset");
  updateData.extend<long long>("kk");
  updateData[0].data<long long>() = 1;
  updateData[1].data<long long>() = 999;
  long rowsUpdated = keyeditor.updateRows( updateAction, updateCondition, updateData );
  if(m_verbose) std::cout << "Updated " << rowsUpdated << " rows" << std::endl;

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

 
  m_session->transaction().commit();

}


// ---------------------------------------------------
/// Fill TTC table with one entry
void COOLCORALClient::fillTTC(const TTC_t& ttc){

  //    if(m_verbose) std::cout << "\nCOOLCORAL Client: Fill TTC info" << std::endl;

  //    struct timeval start_time, end_time;
  //  int total_usecs;
  //  gettimeofday(&start_time, NULL);

  m_session->transaction().start();



  struct timeval nunc_time;
  //unsigned long long nunc_usecs;
    
  gettimeofday(&nunc_time, NULL);

  // iovfk is time in nanoseconds
  //nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);
  //nunc_usecs = (nunc_time.tv_usec);
  //    iovfkttc = (unsigned long long) nunc_usecs/10;
  unsigned long long nunc_sec = nunc_time.tv_sec;
  std::string tag = "DAQ";

  std::string TTC_TABLE="BTTC";

  

 

  //    if(m_verbose) std::cout << "Filling table " << TTC_TABLE  << std::endl;
  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);



	
  //    coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,32);

  ttc_row["ttc_UID"].setValue <int> (ttc.ttc_UID);
  ttc_row["ttc_iovfk"].setValue <long long> (nunc_sec);
  ttc_row["ByteSwapping"].setValue <short> (ttc.ByteSwapping);
  ttc_row["PhysicalAddress"].setValue <int> (ttc.PhysicalAddress);
  ttc_row["DetID"].setValue <int> (ttc.DetID);
  ttc_row["VMESlot"].setValue <int> (ttc.VMEslot);
  ttc_row["Delay"].setValue <int> (ttc.Delay);
  ttc_row["ArShaping"].setValue <short> (ttc.ArShaping);
  ttc_row["SetSendID"].setValue <short> (ttc.SetSendID);
  ttc_row["Name"].setValue <std::string> (ttc.Name);
  ttc_row["ClockSource"].setValue <std::string> (ttc.ClockSource);
  ttc_row["InputFragmentType"].setValue <std::string> (ttc.InputFragmentType);
  ttc_row["Comment"].setValue <std::string> (ttc.Comment);
  ttc_row["ttc_tag"].setValue <std::string> (tag);

	    
  ttc_editor.insertRow(ttc_row);

  m_session->transaction().commit();

}

// ---------------------------------------------------
/// Fill ROD table with one entry
void COOLCORALClient::fillROD(const ROD_t& rod){

  m_session->transaction().start();



  struct timeval nunc_time;
    
  gettimeofday(&nunc_time, NULL);
  std::string ROD_TABLE="BROD";
  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);
  rod_row["rod_UID"].setValue <int> (rod.rod_UID);
  rod_row["rod_iovfk"].setValue <long long> (rod.rod_iovfk);
  rod_row["RODByteSwapping"].setValue <short> (rod.RODByteSwapping);
  rod_row["RODPhysicalAddress"].setValue <int> (rod.RODPhysicalAddress);
  rod_row["RODDetID"].setValue <int> (rod.RODDetID);
  rod_row["RODVMESlot"].setValue <int> (rod.RODVMEslot);
  rod_row["RODHalf"].setValue <int> (rod.RODhalf);
  rod_row["EdgeSelect0"].setValue <int> (rod.EdgeSelect0);
  rod_row["EdgeSelect1"].setValue <int> (rod.EdgeSelect1);
  rod_row["EdgeSelect2"].setValue <int> (rod.EdgeSelect2);
  rod_row["EdgeSelect3"].setValue <int> (rod.EdgeSelect3);
  rod_row["GolDelay0"].setValue <int> (rod.GolDelay0);
  rod_row["GolDelay1"].setValue <int> (rod.GolDelay1);
  rod_row["GolDelay2"].setValue <int> (rod.GolDelay2);
  rod_row["GolDelay3"].setValue <int> (rod.GolDelay3);
  rod_row["RODStatus"].setValue <short> (rod.RODStatus);
  rod_row["RODInputFragmentType"].setValue <std::string> (rod.RODInputFragmentType);
  rod_row["RODName"].setValue <std::string> (rod.RODName);
  rod_row["Comment"].setValue <std::string> (rod.Comment);
  rod_row["rod_tag"].setValue <std::string> (rod.rod_tag);

	    
  rod_editor.insertRow(rod_row);

  m_session->transaction().commit();

}


// ---------------------------------------------------
/// Fill TTC GROUP table with one entry
void COOLCORALClient::fillTTCGR(const TTCGroup_t& ttcgr){

  m_session->transaction().start();



  struct timeval nunc_time;
    
  gettimeofday(&nunc_time, NULL);

  std::string TTCGR_TABLE="BTTCGR";
  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
  ttcgr_row["ttcgr_UID"].setValue <int> (ttcgr.ttcgr_UID);
  ttcgr_row["ttcgr_iovfk"].setValue <long long> (ttcgr.ttcgr_iovfk);
  ttcgr_row["Group"].setValue <int> (ttcgr.Group);
  ttcgr_row["DutyCycle"].setValue <int> (ttcgr.DutyCycle);
  ttcgr_row["EdgeSelect"].setValue <std::string> (ttcgr.EdgeSelect);
  ttcgr_row["finebx"].setValue <std::string> (ttcgr.finebx);
  ttcgr_row["finedx"].setValue <std::string> (ttcgr.finedx);
  ttcgr_row["TDMdelay"].setValue <std::string> (ttcgr.TDMdelay);
  ttcgr_row["TTCGRNAME"].setValue <std::string> (ttcgr.TTCGRNAME);
  ttcgr_row["ttcgr_tag"].setValue <std::string> (ttcgr.ttcgr_tag);

	    
  ttcgr_editor.insertRow(ttcgr_row);

  m_session->transaction().commit();

}

// ---------------------------------------------------
/// Fill TTC GROUP table with one entry
void COOLCORALClient::fillROC(const DTMROC_t& roc){

  m_session->transaction().start();



  struct timeval nunc_time;
    
  gettimeofday(&nunc_time, NULL);

  std::string DTMROC_TABLE="BDTMROC";
  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);
  roc_row["dtmroc_UID"].setValue <int> (roc.dtmroc_UID);
  roc_row["dtmroc_iovfk"].setValue <long long> (roc.dtmroc_iovfk);
  roc_row["DTMROCByteSwapping"].setValue <short> (roc.DTMROCByteSwapping);
  roc_row["DTMROCPhysicalAddress"].setValue <int> (roc.DTMROCPhysicalAddress);
  roc_row["ChipID"].setValue <int> (roc.ChipID);
  roc_row["ChipValid"].setValue <short> (roc.Chip_Valid);
  roc_row["RODGroup"].setValue <short> (roc.RODgroup);
  roc_row["RODInput"].setValue <int> (roc.RODinput);
  roc_row["HW_Addr_FE"].setValue <short> (roc.HW_addr_FE);
  roc_row["TTC_Line_FE"].setValue <short> (roc.TTC_Line_FE);
  roc_row["Thresh0_Low_FE"].setValue <short> (roc.Thresh0_Low_FE);
  roc_row["Thresh0_High_FE"].setValue <short> (roc.Thresh0_High_FE);
  roc_row["Thresh1_Low_FE"].setValue <short> (roc.Thresh1_Low_FE);
  roc_row["Thresh1_High_FE"].setValue <short> (roc.Thresh1_High_FE);
  roc_row["VT_DAC0_FE"].setValue <short> (roc.VT_DAC0_FE);
  roc_row["VT_DAC1_FE"].setValue <short> (roc.VT_DAC1_FE);
  roc_row["Mask_FE"].setValue <int> (roc.Mask_FE);
  roc_row["Pipe_Latency_FE"].setValue <short> (roc.Pipe_Latency_FE);
  roc_row["DTMROCName"].setValue <std::string> (roc.DTMROCName);
  roc_row["DTMROCInputFragmentType"].setValue <std::string> (roc.DTMROCInputFragmentType);
  roc_row["Clock_FE"].setValue <std::string> (roc.Clock_FE);
  roc_row["Comment"].setValue <std::string> (roc.Comment);
  roc_row["roc_tag"].setValue <std::string> (roc.roc_tag);
    
    
  roc_editor.insertRow(roc_row);
  m_session->transaction().commit();

}


// ---------------------------------------------------
/// Change ROC row
void COOLCORALClient::changeROC(const DTMROC_t& roc, const std::string& param, double value ){


  m_session->transaction().start();

  std::string DTMROC_TABLE = "BDTMROC";

  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();

    std::string condStr = "dtmroc_UID =:ident AND dtmroc_iovfk =:key ";
    condStr += " AND roc_tag =:tag";

    std::string updateAction = param;
    updateAction += " = :newval";

    coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
    updateData.extend<short>("newval");
    updateData.extend<int>("ident");
    updateData.extend<long long>("key");
    updateData.extend<std::string>("tag");
    updateData[0].data <short>()= (short)value;
    updateData[1].data <int>()= (int)roc.dtmroc_UID;
    updateData[2].data <long long>()= (long long)roc.dtmroc_iovfk;
    updateData[3].data <std::string>()= roc.roc_tag;
     
    roc_editor.updateRows( updateAction, condStr, updateData );

  m_session->transaction().commit();

}



// ---------------------------------------------------
/// Fill ROD table with one entry
void COOLCORALClient::fillRODV(const std::vector<ROD_t>& rodv){

  m_session->transaction().start();



  struct timeval nunc_time;
    
  gettimeofday(&nunc_time, NULL);

  std::string ROD_TABLE="BROD";

    
 

  //    if(m_verbose) std::cout << "Filling table " << ROD_TABLE  << std::endl;
  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row  ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);

  std::vector<ROD_t>::const_iterator v_iter;

	
    
  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);

  for (v_iter = rodv.begin(); v_iter != rodv.end(); ++v_iter) {
    rod_row["rod_UID"].setValue <int> ((*v_iter).rod_UID);
    rod_row["rod_iovfk"].setValue <long long> ((*v_iter).rod_iovfk);
    rod_row["RODByteSwapping"].setValue <short> ((*v_iter).RODByteSwapping);
    rod_row["RODPhysicalAddress"].setValue <int> ((*v_iter).RODPhysicalAddress);
    rod_row["RODDetID"].setValue <int> ((*v_iter).RODDetID);
    rod_row["RODVMESlot"].setValue <int> ((*v_iter).RODVMEslot);
    rod_row["RODHalf"].setValue <int> ((*v_iter).RODhalf);
    rod_row["EdgeSelect0"].setValue <int> ((*v_iter).EdgeSelect0);
    rod_row["EdgeSelect1"].setValue <int> ((*v_iter).EdgeSelect1);
    rod_row["EdgeSelect2"].setValue <int> ((*v_iter).EdgeSelect2);
    rod_row["EdgeSelect3"].setValue <int> ((*v_iter).EdgeSelect3);
    rod_row["GolDelay0"].setValue <int> ((*v_iter).GolDelay0);
    rod_row["GolDelay1"].setValue <int> ((*v_iter).GolDelay1);
    rod_row["GolDelay2"].setValue <int> ((*v_iter).GolDelay2);
    rod_row["GolDelay3"].setValue <int> ((*v_iter).GolDelay3);
    rod_row["RODStatus"].setValue <short> ((*v_iter).RODStatus);
    rod_row["RODInputFragmentType"].setValue <std::string> ((*v_iter).RODInputFragmentType);
    rod_row["RODName"].setValue <std::string> ((*v_iter).RODName);
    rod_row["Comment"].setValue <std::string> ((*v_iter).Comment);
    rod_row["rod_tag"].setValue <std::string> ((*v_iter).rod_tag);

    rod_bulk->processNextIteration();
	    
    //    rod_editor.insertRow(rod_row);

  }

  rod_bulk->flush();
  delete rod_bulk;
    

  // measuring elapsed time
  //     gettimeofday(&end_time, NULL);
  //         total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
  //                  (end_time.tv_usec-start_time.tv_usec);
  //         if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;







  m_session->transaction().commit();

}


// ---------------------------------------------------
/// Fill TTC GROUP table with one entry
void COOLCORALClient::fillROCV(const std::vector<DTMROC_t>& rocv){

  m_session->transaction().start();
  struct timeval nunc_time;
  gettimeofday(&nunc_time, NULL);
  std::string DTMROC_TABLE="BDTMROC";
  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);
  coral::IBulkOperation* roc_bulk= roc_editor.bulkInsert(roc_row,32);

  std::vector<DTMROC_t>::const_iterator v_iter;

  for (v_iter = rocv.begin(); v_iter != rocv.end(); ++v_iter) {
    roc_row["dtmroc_UID"].setValue <int> ((*v_iter).dtmroc_UID);
    roc_row["dtmroc_iovfk"].setValue <long long> ((*v_iter).dtmroc_iovfk);
    roc_row["DTMROCByteSwapping"].setValue <short> ((*v_iter).DTMROCByteSwapping);
    roc_row["DTMROCPhysicalAddress"].setValue <int> ((*v_iter).DTMROCPhysicalAddress);
    roc_row["ChipID"].setValue <int> ((*v_iter).ChipID);
    roc_row["ChipValid"].setValue <short> ((*v_iter).Chip_Valid);
    roc_row["RODGroup"].setValue <short> ((*v_iter).RODgroup);
    roc_row["RODInput"].setValue <int> ((*v_iter).RODinput);
    roc_row["HW_Addr_FE"].setValue <short> ((*v_iter).HW_addr_FE);
    roc_row["TTC_Line_FE"].setValue <short> ((*v_iter).TTC_Line_FE);
    roc_row["Thresh0_Low_FE"].setValue <short> ((*v_iter).Thresh0_Low_FE);
    roc_row["Thresh0_High_FE"].setValue <short> ((*v_iter).Thresh0_High_FE);
    roc_row["Thresh1_Low_FE"].setValue <short> ((*v_iter).Thresh1_Low_FE);
    roc_row["Thresh1_High_FE"].setValue <short> ((*v_iter).Thresh1_High_FE);
    roc_row["VT_DAC0_FE"].setValue <short> ((*v_iter).VT_DAC0_FE);
    roc_row["VT_DAC1_FE"].setValue <short> ((*v_iter).VT_DAC1_FE);
    roc_row["Mask_FE"].setValue <int> ((*v_iter).Mask_FE);
    roc_row["Pipe_Latency_FE"].setValue <short> ((*v_iter).Pipe_Latency_FE);
    roc_row["DTMROCName"].setValue <std::string> ((*v_iter).DTMROCName);
    roc_row["DTMROCInputFragmentType"].setValue <std::string> ((*v_iter).DTMROCInputFragmentType);
    roc_row["Clock_FE"].setValue <std::string> ((*v_iter).Clock_FE);
    roc_row["Comment"].setValue <std::string> ((*v_iter).Comment);
    roc_row["roc_tag"].setValue <std::string> ((*v_iter).roc_tag);
    
    roc_bulk->processNextIteration();    
  }



	    

  roc_bulk->flush();
  delete roc_bulk;
  m_session->transaction().commit();

}

// ---------------------------------------------------
/// Fill TTC GROUP table with one entry
void COOLCORALClient::fillTTCGRV(const std::vector<TTCGroup_t>& ttcgrv){

  m_session->transaction().start();



  struct timeval nunc_time;
    
  gettimeofday(&nunc_time, NULL);
  std::string TTCGR_TABLE="BTTCGR";
  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,4);
  std::vector<TTCGroup_t>::const_iterator v_iter;
  for (v_iter = ttcgrv.begin(); v_iter != ttcgrv.end(); ++v_iter) {
    ttcgr_row["ttcgr_UID"].setValue <int> ((*v_iter).ttcgr_UID);
    ttcgr_row["ttcgr_iovfk"].setValue <long long> ((*v_iter).ttcgr_iovfk);
    ttcgr_row["Group"].setValue <int> ((*v_iter).Group);
    ttcgr_row["DutyCycle"].setValue <int> ((*v_iter).DutyCycle);
    ttcgr_row["EdgeSelect"].setValue <std::string> ((*v_iter).EdgeSelect);
    ttcgr_row["finebx"].setValue <std::string> ((*v_iter).finebx);
    ttcgr_row["finedx"].setValue <std::string> ((*v_iter).finedx);
    ttcgr_row["TDMdelay"].setValue <std::string> ((*v_iter).TDMdelay);
    ttcgr_row["TTCGRNAME"].setValue <std::string> ((*v_iter).TTCGRNAME);
    ttcgr_row["ttcgr_tag"].setValue <std::string> ((*v_iter).ttcgr_tag);
    ttcgr_bulk->processNextIteration();    
  }



	    

  ttcgr_bulk->flush();
  delete ttcgr_bulk;
  m_session->transaction().commit();

}


// ---------------------------------------------------
/// Fill TTC table with one entry
void COOLCORALClient::fillTTCV(const std::vector<TTC_t>& ttcv){


  //     struct timeval start_time, end_time;
  //     int total_usecs;
  //     gettimeofday(&start_time, NULL);

  m_session->transaction().start();



  struct timeval nunc_time;
    gettimeofday(&nunc_time, NULL);



  std::string TTC_TABLE="BTTC";

    
 

  //    if(m_verbose) std::cout << "Filling table " << TTC_TABLE  << std::endl;
  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);



	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,32);

    
  std::vector<TTC_t>::const_iterator v_iter;

  for (v_iter = ttcv.begin(); v_iter != ttcv.end(); ++v_iter) {


    ttc_row["ttc_UID"].setValue <int> ((*v_iter).ttc_UID);
    ttc_row["ttc_iovfk"].setValue <long long> ((*v_iter).ttc_iovfk);
    ttc_row["ByteSwapping"].setValue <short> ((*v_iter).ByteSwapping);
    ttc_row["PhysicalAddress"].setValue <int> ((*v_iter).PhysicalAddress);
    ttc_row["DetID"].setValue <int> ((*v_iter).DetID);
    ttc_row["VMESlot"].setValue <int> ((*v_iter).VMEslot);
    ttc_row["Delay"].setValue <int> ((*v_iter).Delay);
    ttc_row["ArShaping"].setValue <short> ((*v_iter).ArShaping);
    ttc_row["SetSendID"].setValue <short> ((*v_iter).SetSendID);
    ttc_row["Name"].setValue <std::string> ((*v_iter).Name);
    ttc_row["ClockSource"].setValue <std::string> ((*v_iter).ClockSource);
    ttc_row["InputFragmentType"].setValue <std::string> ((*v_iter).InputFragmentType);
    ttc_row["Comment"].setValue <std::string> ((*v_iter).Comment);
    ttc_row["ttc_tag"].setValue <std::string> ((*v_iter).ttc_tag);

	    
    ttc_bulk->processNextIteration();    
	
  }



	    

  ttc_bulk->flush();
  delete ttc_bulk;

  m_session->transaction().commit();

}

// ---------------------------------------------------
/// Fill TTC GROUP table with one entry
void COOLCORALClient::fillBarrel(const Barrel_t& Barrel){

  m_session->transaction().start();



  struct timeval nunc_time;
    
  gettimeofday(&nunc_time, NULL);


  unsigned long long nunc_sec = nunc_time.tv_sec;
  std::string tag = "DAQ";

  std::string TTCGR_TABLE="BTTCGR";
  std::string ROD_TABLE="BROD";
  std::string DTMROC_TABLE="BDTMROC";

  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,4);

  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);

  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);
  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);

  coral::IBulkOperation* roc_bulk= roc_editor.bulkInsert(roc_row,32);


  std::vector<TTCGroup_t>::const_iterator v_iter1;
  std::vector<ROD_t>::const_iterator v_iter2;
  std::vector<DTMROC_t>::const_iterator v_iter3;


  for (v_iter1 = (Barrel.ttcgr).begin(); v_iter1 != (Barrel.ttcgr).end(); ++v_iter1) {   
    ttcgr_row["ttcgr_UID"].setValue <int> ((*v_iter1).ttcgr_UID);
    ttcgr_row["ttcgr_iovfk"].setValue <long long> (nunc_sec);
    ttcgr_row["Group"].setValue <int> ((*v_iter1).Group);
    ttcgr_row["DutyCycle"].setValue <int> ((*v_iter1).DutyCycle);
    ttcgr_row["EdgeSelect"].setValue <std::string> ((*v_iter1).EdgeSelect);
    ttcgr_row["finebx"].setValue <std::string> ((*v_iter1).finebx);
    ttcgr_row["finedx"].setValue <std::string> ((*v_iter1).finedx);
    ttcgr_row["TDMdelay"].setValue <std::string> ((*v_iter1).TDMdelay);
    ttcgr_row["TTCGRNAME"].setValue <std::string> ((*v_iter1).TTCGRNAME);
    ttcgr_row["ttcgr_tag"].setValue <std::string> (tag);
      
    ttcgr_bulk->processNextIteration();    
  }
    
  ttcgr_bulk->flush();
  delete ttcgr_bulk;


  for (v_iter2 = (Barrel.rod).begin(); v_iter2 != (Barrel.rod).end(); ++v_iter2) {
    rod_row["rod_UID"].setValue <int> ((*v_iter2).rod_UID);
    rod_row["rod_iovfk"].setValue <long long> (nunc_sec);
    rod_row["RODByteSwapping"].setValue <short> ((*v_iter2).RODByteSwapping);
    rod_row["RODPhysicalAddress"].setValue <int> ((*v_iter2).RODPhysicalAddress);
    rod_row["RODDetID"].setValue <int> ((*v_iter2).RODDetID);
    rod_row["RODVMESlot"].setValue <int> ((*v_iter2).RODVMEslot);
    rod_row["RODHalf"].setValue <int> ((*v_iter2).RODhalf);
    rod_row["EdgeSelect0"].setValue <int> ((*v_iter2).EdgeSelect0);
    rod_row["EdgeSelect1"].setValue <int> ((*v_iter2).EdgeSelect1);
    rod_row["EdgeSelect2"].setValue <int> ((*v_iter2).EdgeSelect2);
    rod_row["EdgeSelect3"].setValue <int> ((*v_iter2).EdgeSelect3);
    rod_row["GolDelay0"].setValue <int> ((*v_iter2).GolDelay0);
    rod_row["GolDelay1"].setValue <int> ((*v_iter2).GolDelay1);
    rod_row["GolDelay2"].setValue <int> ((*v_iter2).GolDelay2);
    rod_row["GolDelay3"].setValue <int> ((*v_iter2).GolDelay3);
    rod_row["RODStatus"].setValue <short> ((*v_iter2).RODStatus);
    rod_row["RODInputFragmentType"].setValue <std::string> ((*v_iter2).RODInputFragmentType);
    rod_row["RODName"].setValue <std::string> ((*v_iter2).RODName);
    rod_row["Comment"].setValue <std::string> ((*v_iter2).Comment);
    rod_row["rod_tag"].setValue <std::string> (tag);
      
    rod_bulk->processNextIteration();
  }

  rod_bulk->flush();
  delete rod_bulk;

  for (v_iter3 = (Barrel.roc).begin(); v_iter3 != (Barrel.roc).end(); ++v_iter3) {
    roc_row["dtmroc_UID"].setValue <int> ((*v_iter3).dtmroc_UID);
    roc_row["dtmroc_iovfk"].setValue <long long> (nunc_sec);
    roc_row["DTMROCByteSwapping"].setValue <short> ((*v_iter3).DTMROCByteSwapping);
    roc_row["DTMROCPhysicalAddress"].setValue <int> ((*v_iter3).DTMROCPhysicalAddress);
    roc_row["ChipID"].setValue <int> ((*v_iter3).ChipID);
    roc_row["ChipValid"].setValue <short> ((*v_iter3).Chip_Valid);
    roc_row["RODGroup"].setValue <short> ((*v_iter3).RODgroup);
    roc_row["RODInput"].setValue <int> ((*v_iter3).RODinput);
    roc_row["HW_Addr_FE"].setValue <short> ((*v_iter3).HW_addr_FE);
    roc_row["TTC_Line_FE"].setValue <short> ((*v_iter3).TTC_Line_FE);
    roc_row["Thresh0_Low_FE"].setValue <short> ((*v_iter3).Thresh0_Low_FE);
    roc_row["Thresh0_High_FE"].setValue <short> ((*v_iter3).Thresh0_High_FE);
    roc_row["Thresh1_Low_FE"].setValue <short> ((*v_iter3).Thresh1_Low_FE);
    roc_row["Thresh1_High_FE"].setValue <short> ((*v_iter3).Thresh1_High_FE);
    roc_row["VT_DAC0_FE"].setValue <short> ((*v_iter3).VT_DAC0_FE);
    roc_row["VT_DAC1_FE"].setValue <short> ((*v_iter3).VT_DAC1_FE);
    roc_row["Mask_FE"].setValue <int> ((*v_iter3).Mask_FE);
    roc_row["Pipe_Latency_FE"].setValue <short> ((*v_iter3).Pipe_Latency_FE);
    roc_row["DTMROCName"].setValue <std::string> ((*v_iter3).DTMROCName);
    roc_row["DTMROCInputFragmentType"].setValue <std::string> ((*v_iter3).DTMROCInputFragmentType);
    roc_row["Clock_FE"].setValue <std::string> ((*v_iter3).Clock_FE);
    roc_row["Comment"].setValue <std::string> ((*v_iter3).Comment);
    roc_row["roc_tag"].setValue <std::string> (tag);
      
    roc_bulk->processNextIteration();    
  }

  roc_bulk->flush();
  delete roc_bulk;


  // measuring elapsed time
  //     gettimeofday(&end_time, NULL);
  //         total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
  //                  (end_time.tv_usec-start_time.tv_usec);
  //         if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

  m_session->transaction().commit();

}


//------------------------------------------------------------------------------
/// Create and fill CORAL only Connection table for a partition, identified by an input string
void COOLCORALClient::createMap(){

    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);
  std::string BCONNECT_TABLE;



  BCONNECT_TABLE = "BCONNECT";
 
  m_session->transaction().start();


  if(m_verbose) std::cout << "Deleting the old tables: " << std::endl;
  if(m_verbose) std::cout <<  BCONNECT_TABLE << std::endl;

  m_session->nominalSchema().dropIfExistsTable(BCONNECT_TABLE);
  //-------------------TRT BARREL CONNECT Table

  if(m_verbose) std::cout << "Creating table: " << BCONNECT_TABLE << std::endl;

       
  // Create the table
  coral::TableDescription bconn_columns( "SchemaDefinition_BCONNECT" );
  bconn_columns.setName( BCONNECT_TABLE );
  bconn_columns.insertColumn( "Partition",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "Crate",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "Slot",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  bconn_columns.insertColumn( "phi",
			      coral::AttributeSpecification::typeNameForId( typeid(int) ) );
    
  std::vector <std::string> tmp_strings;
    
  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  // put an index on const
  //    if (part_trt=="barrel"){
  tmp_strings.clear();
  tmp_strings.push_back ("Partition");
  bconn_columns.createIndex ("bpart_idx", tmp_strings);
      
  tmp_strings.clear();
  tmp_strings.push_back ("Crate");
  bconn_columns.createIndex ("bcrate_idx", tmp_strings);
      
  tmp_strings.clear();
  tmp_strings.push_back ("Slot");
  bconn_columns.createIndex ("bslot_idx", tmp_strings);
 
  bconn_columns.setNotNullConstraint ("Partition");
  bconn_columns.setNotNullConstraint ("Crate");
  bconn_columns.setNotNullConstraint ("Slot");

  // Create the actual table
  m_session->nominalSchema().createTable( bconn_columns );

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;


 
  m_session->transaction().commit();



}


//------------------------------------------------------------------------------
/// Create and fill CORAL only Connection table for a partition, identified by an input string
void COOLCORALClient::fillMap(){
  std::string BCONNECT_TABLE;



  BCONNECT_TABLE = "BCONNECT";

  m_session->transaction().start();

  if(m_verbose) std::cout << "Filling table " << BCONNECT_TABLE  << std::endl;
  coral::ITableDataEditor& bconn_editor = m_session->nominalSchema().tableHandle(BCONNECT_TABLE ).dataEditor();
  coral::AttributeList bconn_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  bconn_editor.rowBuffer(bconn_row);


  //--------
  // TRT-BCONNECT quantities

  Mapping_t conn_struct;
	
  coral::IBulkOperation* bconn_bulk= bconn_editor.bulkInsert(bconn_row,16);

  int run=0;	
  const char* filename;
  filename = "PITConnectTable.dat";
  ifstream itAfile (filename);
  std::vector<char> buffer(999);
  if (itAfile.is_open()){
    while (itAfile.getline(&buffer[0],999,'\n')) {
      std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));

      std::istringstream sbuf(&buffer[0]);
      sbuf >> conn_struct.part >> conn_struct.crate 
	   >> conn_struct.slot 
	   >> conn_struct.phi;


      bconn_row["Partition"].setValue <int> (conn_struct.part);
      bconn_row["Crate"].setValue <int> (conn_struct.crate);
      bconn_row["Slot"].setValue <int> (conn_struct.slot);
      bconn_row["phi"].setValue <int> (conn_struct.phi);
      run++;
      bconn_bulk->processNextIteration();
    }
  } 
  if(m_verbose) std::cout << "Read CONNECT data done..." << std::endl;

  itAfile.close();

  bconn_bulk->flush();
  delete bconn_bulk;
	
  printf("Total: %d  rows\n",run);
	
 
  m_session->transaction().commit();



}

//------------------------------------------------------------------------------
/// Create and fill CORAL only Connection table for a partition, identified by an input string
void COOLCORALClient::fillMapV(const std::vector<Mapping_t>& conn_struct){

  std::string BCONNECT_TABLE;



  BCONNECT_TABLE = "BCONNECT";

  m_session->transaction().start();

  if(m_verbose) std::cout << "Filling table " << BCONNECT_TABLE  << std::endl;
  coral::ITableDataEditor& bconn_editor = m_session->nominalSchema().tableHandle(BCONNECT_TABLE ).dataEditor();
  coral::AttributeList bconn_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  bconn_editor.rowBuffer(bconn_row);


  //--------
  // TRT-BCONNECT quantities

  std::vector<Mapping_t>::const_iterator conn_iter;
 
  coral::IBulkOperation* bconn_bulk= bconn_editor.bulkInsert(bconn_row,16);

  for (conn_iter=conn_struct.begin(); conn_iter!=conn_struct.end();++conn_iter) {
	  
    bconn_row["Partition"].setValue <int> ((*conn_iter).part);
    bconn_row["Crate"].setValue <int> ((*conn_iter).crate);
    bconn_row["Slot"].setValue <int> ((*conn_iter).slot);
    bconn_row["phi"].setValue <int> ((*conn_iter).phi);
	  
   
    bconn_bulk->processNextIteration();
	  
  }
  if(m_verbose) std::cout << "Read CONNECT data done..." << std::endl;

  bconn_bulk->flush();
  delete bconn_bulk;
  m_session->transaction().commit();
}




//--------------------------------------------------------
/// Starting from a phi value get the TTC_ID, TTCGROUP_ID,
/// ROD_ID, DTMROC_ID
int COOLCORALClient::GetIDS(int phi){
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);
    

  int part_id;
  int crate_id;
  int slot_id;

  int ttc_id;
  int ttcgr_id;
  int rod_id;
  int dtmroc_id;

  //
  std::string TTC_TABLE = "BTTC";
  std::string TTCGR_TABLE = "BTTCGR";
  std::string ROD_TABLE = "BROD";
  std::string  DTMROC_TABLE = "BDTMROC";
  std::string   BCONNECT_TABLE = "BCONNECT";

  m_session->transaction().start(true);


  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(BCONNECT_TABLE).newQuery();

  query00->addToOutputList("Partition" ); 
  query00->addToOutputList("Crate" ); 
  query00->addToOutputList("Slot" ); 

  std::string conn_cond = BCONNECT_TABLE;
  conn_cond += ".phi = :phi_val";


  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<int>( "phi_val" );
    
  query00->setCondition( conn_cond, conn_condData);
        

    
  conn_condData[0].data<int>() = phi;

  coral::ICursor& cursor00 = query00->execute();
  int nRowsM = 0;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();
      
    part_id = row00[0].data<int>();
    crate_id = row00[1].data<int>();
    slot_id = row00[2].data<int>();
      
    ttc_id = (part_id+4)*10000 + crate_id*100 + slot_id;

    if(m_verbose) std::cout << std::endl;
    if(m_verbose) std::cout << "****************************" << std::endl;

    if (part_id < 33){
      if(m_verbose) std::cout << "TRT Barrel Partition Side ";
      if (part_id < 32) {
	if(m_verbose) std::cout << "A" << phi << std::endl;
      }
      else { 
	if(m_verbose) std::cout << "C" << std::endl;
	if(m_verbose) std::cout << "Phi = " << phi << std::endl;
      }
    }
    else {
      if(m_verbose) std::cout << "TRT EndCap Partition Side ";
      if (part_id < 34) {
	if(m_verbose) std::cout << "A" << std::endl; 
	if(m_verbose) std::cout << "Phi = " << phi << std::endl;
      }
      else { 
	if(m_verbose) std::cout << "C" << std::endl;
	if(m_verbose) std::cout << "Phi = " << phi << std::endl;
      }
    }

 
    if(m_verbose) std::cout << std::endl;

    if(m_verbose) std::cout << "TTC ID = " << ttc_id << std::endl;

    if(m_verbose) std::cout << std::endl;

    if(m_verbose) std::cout << "TTCGROUP ID = ";
    for (int i=0; i<4; i++){
      ttcgr_id = ttc_id*10 + i;
      if(m_verbose) std::cout << ttcgr_id << " ";
    }
    if(m_verbose) std::cout << std::endl;

    rod_id = part_id*100 + phi;

    if(m_verbose) std::cout << std::endl;
      
    if(m_verbose) std::cout << "ROD ID = " << rod_id << std::endl;

    if(m_verbose) std::cout << std::endl;

    if(m_verbose) std::cout << "DTMROC_ID = ";
    for (int j=0; j<104; j++) {
      dtmroc_id = part_id*100000 + phi*1000 + j;
      if(m_verbose) std::cout << dtmroc_id << " ";
    }

    if(m_verbose) std::cout << std::endl;
    if(m_verbose) std::cout << "****************************" << std::endl;
    if(m_verbose) std::cout << std::endl;
   
    nRowsM++;
  }
  delete query00;

  m_session->transaction().commit();


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

  return 0;
	
}

// - here the GetOne series

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier), a tag and fills a TTC_t
int COOLCORALClient::GetOneTTC(int ttc_id, const std::string& tag, TTC_t &ttc){

  std::string TTC_TABLE = "BTTC";

  m_session->transaction().start(true);

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
  query0->addToOutputList("ttc_UID" );
  query0->addToOutputList("ttc_iovfk" );
  query0->addToOutputList("ByteSwapping" );
  query0->addToOutputList("PhysicalAddress" );
  query0->addToOutputList("DetID" );
  query0->addToOutputList("VMESlot" );
  query0->addToOutputList("Delay" );
  query0->addToOutputList("ArShaping" );
  query0->addToOutputList("SetSendID" );
  query0->addToOutputList("Name" );
  query0->addToOutputList("ClockSource" );
  query0->addToOutputList("InputFragmentType" );
  query0->addToOutputList("Comment" );
  query0->addToOutputList("ttc_tag" );

  std::string ttc_cond = "ttc_UID = :ttcnr AND ttc_tag = :tag";
  coral::AttributeList ttc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_condData.extend<int>( "ttcnr" );
  ttc_condData.extend<std::string>( "tag" );

  query0->setCondition( ttc_cond, ttc_condData);
  ttc_condData[0].data<int>() = ttc_id;
  ttc_condData[1].data<std::string>() = tag;

  coral::ICursor& cursor0 = query0->execute();

  int result = -1;

  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    ttc.ttc_UID           = row0[0].data<int>();
    ttc.ttc_iovfk         = row0[1].data<long long>();
    ttc.ByteSwapping      = row0[2].data<short>();
    ttc.PhysicalAddress   = row0[3].data<int>();
    ttc.DetID             = row0[4].data<int>();
    ttc.VMEslot           = row0[5].data<int>();
    ttc.Delay             = row0[6].data<int>();
    ttc.ArShaping         = row0[7].data<short>();
    ttc.SetSendID         = row0[8].data<short>();
    ttc.Name              = row0[9].data<std::string>();
    ttc.ClockSource       = row0[10].data<std::string>();
    ttc.InputFragmentType = row0[11].data<std::string>();
    ttc.Comment           = row0[12].data<std::string>();
    ttc.ttc_tag           = row0[13].data<std::string>();

    result = 0;

  }
  delete query0;

  m_session->transaction().commit();

  return result;

}


//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier), a tag, a key and fills a TTC_t
int COOLCORALClient::GetOneTTC(int ttc_id, long long key, const std::string& tag, TTC_t &ttc){
    
  std::string TTC_TABLE = "HIST_TTC";
    
  m_session->transaction().start(true);
    
  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("ttc_UID" ); 
  query0->addToOutputList("ttc_iovfk" ); 
  query0->addToOutputList("ByteSwapping" ); 
  query0->addToOutputList("PhysicalAddress" ); 
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Name" ); 
  query0->addToOutputList("ClockSource" ); 
  query0->addToOutputList("InputFragmentType" ); 
  query0->addToOutputList("Comment" ); 
  query0->addToOutputList("ttc_tag" ); 
    
  std::string ttc_cond = "ttc_UID = :ttcnr AND ttc_iovfk = :ttckey";
  ttc_cond += " AND ttc_tag = :tag";
  coral::AttributeList ttc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_condData.extend<int>( "ttcnr" );
  ttc_condData.extend<long long>( "ttckey" );
  ttc_condData.extend<std::string>( "tag" );
    
  query0->setCondition( ttc_cond, ttc_condData);
    
  ttc_condData[0].data<int>() = ttc_id;
  ttc_condData[1].data<long long>() = key;
  ttc_condData[2].data<std::string>() = tag;
    
  coral::ICursor& cursor0 = query0->execute();
    
  int result = -1;

  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    ttc.ttc_UID           = row0[0].data<int>();
    ttc.ttc_iovfk         = row0[1].data<long long>();
    ttc.ByteSwapping      = row0[2].data<short>();
    ttc.PhysicalAddress   = row0[3].data<int>();
    ttc.DetID             = row0[4].data<int>();
    ttc.VMEslot           = row0[5].data<int>();
    ttc.Delay             = row0[6].data<int>();
    ttc.ArShaping         = row0[7].data<short>();
    ttc.SetSendID         = row0[8].data<short>();
    ttc.Name              = row0[9].data<std::string>();
    ttc.ClockSource       = row0[10].data<std::string>();
    ttc.InputFragmentType = row0[11].data<std::string>();
    ttc.Comment           = row0[12].data<std::string>();
    ttc.ttc_tag           = row0[13].data<std::string>();

    result = 0; 
  }
  delete query0;
    
  m_session->transaction().commit();
    
  return result;
    
}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier), a tag and fills a TTCGroup_t
int COOLCORALClient::GetOneTTCGR(int ttcgr_id, const std::string& tag, TTCGroup_t &ttcgr){

  std::string TTCGR_TABLE = "BTTCGR";

  m_session->transaction().start(true);

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();

  query0->addToOutputList("ttcgr_UID" );
  query0->addToOutputList("ttcgr_iovfk" );
  query0->addToOutputList("Group" );
  query0->addToOutputList("DutyCycle" );
  query0->addToOutputList("EdgeSelect" );
  query0->addToOutputList("finebx" );
  query0->addToOutputList("finedx" );
  query0->addToOutputList("TDMdelay" );
  query0->addToOutputList("TTCGRNAME" );
  query0->addToOutputList("ttcgr_tag" );

  std::string ttcgr_cond = "ttcgr_UID = :ttcgrnr AND ttcgr_tag = :tag";

  coral::AttributeList ttcgr_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_condData.extend<int>( "ttcgrnr" );
  ttcgr_condData.extend<std::string>( "tag" );

  query0->setCondition( ttcgr_cond, ttcgr_condData);

  ttcgr_condData[0].data<int>() = ttcgr_id;
  ttcgr_condData[1].data<std::string>() = tag;

  coral::ICursor& cursor0 = query0->execute();

  int result = -1;

  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();

    ttcgr.ttcgr_UID         = row0[0].data<int>();
    ttcgr.ttcgr_iovfk       = row0[1].data<long long>();
    ttcgr.Group             = row0[2].data<int>();
    ttcgr.DutyCycle         = row0[3].data<int>();
    ttcgr.EdgeSelect        = row0[4].data<std::string>();
    ttcgr.finebx            = row0[5].data<std::string>();
    ttcgr.finedx            = row0[6].data<std::string>();
    ttcgr.TDMdelay          = row0[7].data<std::string>();
    ttcgr.TTCGRNAME         = row0[8].data<std::string>();
    ttcgr.ttcgr_tag         = row0[9].data<std::string>();

    result =0;
  }
  delete query0;

  m_session->transaction().commit();

  return result;

}



//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier), a tag, a key 
/// and fills a TTCGroup_t
int COOLCORALClient::GetOneTTCGR(int ttcgr_id, long long key, const std::string& tag, TTCGroup_t &ttcgr){
    
  std::string TTCGR_TABLE = "HIST_TTCGR";
    
  m_session->transaction().start(true);
    
  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();
    
  query0->addToOutputList("ttcgr_UID" ); 
  query0->addToOutputList("ttcgr_iovfk" ); 
  query0->addToOutputList("Group" ); 
  query0->addToOutputList("DutyCycle" ); 
  query0->addToOutputList("EdgeSelect" ); 
  query0->addToOutputList("finebx" ); 
  query0->addToOutputList("finedx" ); 
  query0->addToOutputList("TDMdelay" ); 
  query0->addToOutputList("TTCGRNAME" ); 
  query0->addToOutputList("ttcgr_tag" ); 
    
  std::string ttcgr_cond = "ttcgr_UID = :ttcgrnr AND ttcgr_iovfk = :ttckey";
  ttcgr_cond += " AND ttcgr_tag = :tag";
  coral::AttributeList ttcgr_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_condData.extend<int>( "ttcgrnr" );
  ttcgr_condData.extend<long long>( "ttckey" );
  ttcgr_condData.extend<std::string>( "tag" );
    
  query0->setCondition( ttcgr_cond, ttcgr_condData);
    
  ttcgr_condData[0].data<int>() = ttcgr_id;
  ttcgr_condData[1].data<long long>() = key;
  ttcgr_condData[2].data<std::string>() = tag;
    
  coral::ICursor& cursor0 = query0->execute();
   
  int result = -1;
 
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();

    ttcgr.ttcgr_UID         = row0[0].data<int>();
    ttcgr.ttcgr_iovfk       = row0[1].data<long long>();
    ttcgr.Group             = row0[2].data<int>();
    ttcgr.DutyCycle         = row0[3].data<int>();
    ttcgr.EdgeSelect        = row0[4].data<std::string>();
    ttcgr.finebx            = row0[5].data<std::string>();
    ttcgr.finedx            = row0[6].data<std::string>();
    ttcgr.TDMdelay          = row0[7].data<std::string>();
    ttcgr.TTCGRNAME         = row0[8].data<std::string>();
    ttcgr.ttcgr_tag         = row0[9].data<std::string>(); 
  
    result = 0;
    
  }
  delete query0;
    
  m_session->transaction().commit();
    
  return result;
    
}

//------------------------------------------------------
/// Read ROD information from the DB for one ROD module
/// identified by an input int (identifier), a tag and fills a ROD_t
int COOLCORALClient::GetOneROD(int rod_id, const std::string& tag, ROD_t &rod){

  std::string ROD_TABLE = "BROD";

  m_session->transaction().start(true);

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
  query0->addToOutputList("rod_UID");
  query0->addToOutputList("rod_iovfk");
  query0->addToOutputList("RODByteSwapping");
  query0->addToOutputList("RODPhysicalAddress");
  query0->addToOutputList("RODDetID");
  query0->addToOutputList("RODVMESlot");
  query0->addToOutputList("RODHalf");
  query0->addToOutputList("EdgeSelect0");
  query0->addToOutputList("EdgeSelect1");
  query0->addToOutputList("EdgeSelect2");
  query0->addToOutputList("EdgeSelect3");
  query0->addToOutputList("GolDelay0");
  query0->addToOutputList("GolDelay1");
  query0->addToOutputList("GolDelay2");
  query0->addToOutputList("GolDelay3");
  query0->addToOutputList("RODStatus");
  query0->addToOutputList("RODInputFragmentType");
  query0->addToOutputList("RODName");
  query0->addToOutputList("Comment");
  query0->addToOutputList("rod_tag");

  std::string rod_cond = "rod_UID = :rodnr AND rod_tag = :tag";
  coral::AttributeList rod_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_condData.extend<int>( "rodnr" );
  rod_condData.extend<std::string>( "tag" );

  query0->setCondition( rod_cond, rod_condData);
  rod_condData[0].data<int>() = rod_id;
  rod_condData[1].data<std::string>() = tag;

  coral::ICursor& cursor0 = query0->execute();

  int result = -1;

  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();

    rod.rod_UID              = row0[0].data<int>();
    rod.rod_iovfk            = row0[1].data<long long>();
    rod.RODByteSwapping      = row0[2].data<short>();
    rod.RODPhysicalAddress   = row0[3].data<int>();
    rod.RODDetID             = row0[4].data<int>();
    rod.RODVMEslot           = row0[5].data<int>();
    rod.RODhalf              = row0[6].data<int>();
    rod.EdgeSelect0          = row0[7].data<int>();
    rod.EdgeSelect1          = row0[8].data<int>();
    rod.EdgeSelect2          = row0[9].data<int>();
    rod.EdgeSelect3          = row0[10].data<int>();
    rod.GolDelay0            = row0[11].data<int>();
    rod.GolDelay1            = row0[12].data<int>();
    rod.GolDelay2            = row0[13].data<int>();
    rod.GolDelay3            = row0[14].data<int>();
    rod.RODStatus            = row0[15].data<short>();
    rod.RODInputFragmentType = row0[16].data<std::string>();
    rod.RODName              = row0[17].data<std::string>();
    rod.Comment              = row0[18].data<std::string>();
    rod.rod_tag              = row0[19].data<std::string>();


    result = 0;
  }
  delete query0;

  m_session->transaction().commit();

  return result;

}


//------------------------------------------------------
/// Read ROD information from the DB for one ROD module
/// identified by an input int (identifier), a tag, a key and fills a ROD_t
int COOLCORALClient::GetOneROD(int rod_id, long long key, const std::string& tag, ROD_t &rod){
    
  std::string ROD_TABLE = "HIST_ROD";
    
  m_session->transaction().start(true);
    
  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
    
  query0->addToOutputList("rod_UID"); 
  query0->addToOutputList("rod_iovfk"); 
  query0->addToOutputList("RODByteSwapping"); 
  query0->addToOutputList("RODPhysicalAddress"); 
  query0->addToOutputList("RODDetID");
  query0->addToOutputList("RODVMESlot");
  query0->addToOutputList("RODHalf"); 
  query0->addToOutputList("EdgeSelect0");
  query0->addToOutputList("EdgeSelect1"); 
  query0->addToOutputList("EdgeSelect2"); 
  query0->addToOutputList("EdgeSelect3");
  query0->addToOutputList("GolDelay0"); 
  query0->addToOutputList("GolDelay1"); 
  query0->addToOutputList("GolDelay2"); 
  query0->addToOutputList("GolDelay3"); 
  query0->addToOutputList("RODStatus");
  query0->addToOutputList("RODInputFragmentType");
  query0->addToOutputList("RODName");
  query0->addToOutputList("Comment"); 
  query0->addToOutputList("rod_tag"); 
	    
  std::string rod_cond = "rod_UID = :rodnr AND rod_iovfk = :rodkey";
  rod_cond += " AND rod_tag = :tag";
  coral::AttributeList rod_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_condData.extend<int>( "rodnr" );
  rod_condData.extend<long long>( "rodkey" );
  rod_condData.extend<std::string>( "tag" );
    
  query0->setCondition( rod_cond, rod_condData);
    
  rod_condData[0].data<int>() = rod_id;
  rod_condData[1].data<long long>() = key;
  rod_condData[2].data<std::string>() = tag;
    
  coral::ICursor& cursor0 = query0->execute();

  int result = -1;
    
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();

    rod.rod_UID              = row0[0].data<int>();
    rod.rod_iovfk            = row0[1].data<long long>();
    rod.RODByteSwapping      = row0[2].data<short>();
    rod.RODPhysicalAddress   = row0[3].data<int>();
    rod.RODDetID             = row0[4].data<int>();
    rod.RODVMEslot           = row0[5].data<int>();
    rod.RODhalf              = row0[6].data<int>();
    rod.EdgeSelect0          = row0[7].data<int>();
    rod.EdgeSelect1          = row0[8].data<int>();
    rod.EdgeSelect2          = row0[9].data<int>();
    rod.EdgeSelect3          = row0[10].data<int>();
    rod.GolDelay0            = row0[11].data<int>();
    rod.GolDelay1            = row0[12].data<int>();
    rod.GolDelay2            = row0[13].data<int>();
    rod.GolDelay3            = row0[14].data<int>();
    rod.RODStatus            = row0[15].data<short>();
    rod.RODInputFragmentType = row0[16].data<std::string>();
    rod.RODName              = row0[17].data<std::string>();
    rod.Comment              = row0[18].data<std::string>();
    rod.rod_tag              = row0[19].data<std::string>();

    result = 0;
  }
  delete query0;
    
  m_session->transaction().commit();
    
  return result;
    
}

//------------------------------------------------------
/// Read DTMROC information from the DB for one DTMROC module
/// identified by an input int (identifier), a tag and fills a DTMROC_t
int COOLCORALClient::GetOneROC(int dtmroc_id, const std::string& tag, DTMROC_t &roc){

  std::string DTMROC_TABLE = "BDTMROC";

  m_session->transaction().start(true);


  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  query0->addToOutputList("dtmroc_UID");
  query0->addToOutputList("dtmroc_iovfk");
  query0->addToOutputList("DTMROCByteSwapping");
  query0->addToOutputList("DTMROCPhysicalAddress");
  query0->addToOutputList("ChipID");
  query0->addToOutputList("ChipValid");
  query0->addToOutputList("RODGroup");
  query0->addToOutputList("RODInput");
  query0->addToOutputList("HW_Addr_FE");
  query0->addToOutputList("TTC_Line_FE");
  query0->addToOutputList("Thresh0_Low_FE");
  query0->addToOutputList("Thresh0_High_FE");
  query0->addToOutputList("Thresh1_Low_FE");
  query0->addToOutputList("Thresh1_High_FE");
  query0->addToOutputList("VT_DAC0_FE");
  query0->addToOutputList("VT_DAC1_FE");
  query0->addToOutputList("Mask_FE");
  query0->addToOutputList("Pipe_Latency_FE");
  query0->addToOutputList("DTMROCName");
  query0->addToOutputList("DTMROCInputFragmentType");
  query0->addToOutputList("Clock_FE");
  query0->addToOutputList("Comment");
  query0->addToOutputList("roc_tag");

  std::string dtmroc_cond = "dtmroc_UID = :dtmrocnr AND roc_tag = :tag";
  coral::AttributeList dtmroc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_condData.extend<int>( "dtmrocnr" );
  dtmroc_condData.extend<std::string>( "tag" );
  query0->setCondition( dtmroc_cond, dtmroc_condData);

  dtmroc_condData[0].data<int>() = dtmroc_id;
  dtmroc_condData[1].data<std::string>() = tag;

  coral::ICursor& cursor0 = query0->execute();


  int result = -1;
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();

    roc.dtmroc_UID              = row0[0].data<int>();
    roc.dtmroc_iovfk            = row0[1].data<long long>();
    roc.DTMROCByteSwapping      = row0[2].data<short>();
    roc.DTMROCPhysicalAddress   = row0[3].data<int>();
    roc.ChipID                  = row0[4].data<int>();
    roc.Chip_Valid              = row0[5].data<short>();
    roc.RODgroup                = row0[6].data<short>();
    roc.RODinput                = row0[7].data<int>();
    roc.HW_addr_FE              = row0[8].data<short>();
    roc.TTC_Line_FE             = row0[9].data<short>();
    roc.Thresh0_Low_FE          = row0[10].data<short>();
    roc.Thresh0_High_FE         = row0[11].data<short>();
    roc.Thresh1_Low_FE          = row0[12].data<short>();
    roc.Thresh1_High_FE         = row0[13].data<short>();
    roc.VT_DAC0_FE              = row0[14].data<short>();
    roc.VT_DAC1_FE              = row0[15].data<short>();
    roc.Mask_FE                 = row0[16].data<int>();
    roc.Pipe_Latency_FE         = row0[17].data<short>();
    roc.DTMROCName              = row0[18].data<std::string>();
    roc.DTMROCInputFragmentType = row0[19].data<std::string>();
    roc.Clock_FE                = row0[20].data<std::string>();
    roc.Comment                 = row0[21].data<std::string>();
    roc.roc_tag                 = row0[22].data<std::string>();

    result = 0;

  }
  delete query0;

  m_session->transaction().commit();

  return result;

}


//------------------------------------------------------
/// Read DTMROC information from the DB for one DTMROC module
/// identified by an input int (identifier), a tag, a key
/// and fills a DTMROC_t
int COOLCORALClient::GetOneROC(int dtmroc_id, long long key, const std::string& tag, DTMROC_t &roc){
    
  std::string DTMROC_TABLE = "HIST_DTMROC";
    
  m_session->transaction().start(true);
    
    
  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();
    
  query0->addToOutputList("dtmroc_UID"); 
  query0->addToOutputList("dtmroc_iovfk"); 
  query0->addToOutputList("DTMROCByteSwapping"); 
  query0->addToOutputList("DTMROCPhysicalAddress"); 
  query0->addToOutputList("ChipID");
  query0->addToOutputList("ChipValid");
  query0->addToOutputList("RODGroup"); 
  query0->addToOutputList("RODInput");
  query0->addToOutputList("HW_Addr_FE"); 
  query0->addToOutputList("TTC_Line_FE"); 
  query0->addToOutputList("Thresh0_Low_FE");
  query0->addToOutputList("Thresh0_High_FE");
  query0->addToOutputList("Thresh1_Low_FE");
  query0->addToOutputList("Thresh1_High_FE");
  query0->addToOutputList("VT_DAC0_FE"); 
  query0->addToOutputList("VT_DAC1_FE"); 
  query0->addToOutputList("Mask_FE"); 
  query0->addToOutputList("Pipe_Latency_FE"); 
  query0->addToOutputList("DTMROCName");
  query0->addToOutputList("DTMROCInputFragmentType");
  query0->addToOutputList("Clock_FE"); 
  query0->addToOutputList("Comment"); 
  query0->addToOutputList("roc_tag"); 
	    
  std::string dtmroc_cond = "dtmroc_UID = :dtmrocnr AND dtmroc_iovfk = :dtmrockey";
  dtmroc_cond += " AND roc_tag = :tag";
  coral::AttributeList dtmroc_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  dtmroc_condData.extend<int>( "dtmrocnr" );
  dtmroc_condData.extend<long long>( "dtmrockey" );
  dtmroc_condData.extend<std::string>( "tag" );
  query0->setCondition( dtmroc_cond, dtmroc_condData);
    
  dtmroc_condData[0].data<int>() = dtmroc_id;
  dtmroc_condData[1].data<long long>() = key;
  dtmroc_condData[2].data<std::string>() = tag;
    
  coral::ICursor& cursor0 = query0->execute();
   

  int result = -1;
 
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
         
    roc.dtmroc_UID              = row0[0].data<int>();
    roc.dtmroc_iovfk            = row0[1].data<long long>();
    roc.DTMROCByteSwapping      = row0[2].data<short>();
    roc.DTMROCPhysicalAddress   = row0[3].data<int>();
    roc.ChipID                  = row0[4].data<int>();
    roc.Chip_Valid              = row0[5].data<short>();
    roc.RODgroup                = row0[6].data<short>();
    roc.RODinput                = row0[7].data<int>();
    roc.HW_addr_FE              = row0[8].data<short>();
    roc.TTC_Line_FE             = row0[9].data<short>();
    roc.Thresh0_Low_FE          = row0[10].data<short>();
    roc.Thresh0_High_FE         = row0[11].data<short>();
    roc.Thresh1_Low_FE          = row0[12].data<short>();
    roc.Thresh1_High_FE         = row0[13].data<short>();
    roc.VT_DAC0_FE              = row0[14].data<short>();
    roc.VT_DAC1_FE              = row0[15].data<short>();
    roc.Mask_FE                 = row0[16].data<int>();
    roc.Pipe_Latency_FE         = row0[17].data<short>();
    roc.DTMROCName              = row0[18].data<std::string>();
    roc.DTMROCInputFragmentType = row0[19].data<std::string>();
    roc.Clock_FE                = row0[20].data<std::string>();
    roc.Comment                 = row0[21].data<std::string>();
    roc.roc_tag                 = row0[22].data<std::string>(); 

    result = 0;
      
  }
  delete query0;
    
  m_session->transaction().commit();
    
  return result;
    
}



// - End of the GetOne series

  
//--------------------------------------------------------
/// Starting from a phi value and a detector string get the 
/// TTC_ID, TTCGROUP_ID,
/// ROD_ID, DTMROC_ID
int COOLCORALClient::GetFullIDS(int phi, const std::string& detector){
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);
    

  int part_id;
  int crate_id;
  int slot_id;

  int ttc_id;
  int ttcgr_id;
  int rod_id;
  int dtmroc_id;





  std::string TTC_TABLE;
  std::string TTCGR_TABLE;
  std::string ROD_TABLE;
  std::string  DTMROC_TABLE;
  std::string   BCONNECT_TABLE = "BCONNECT";


    
  if (detector=="barrel"){
    TTC_TABLE = "BTTC";
    TTCGR_TABLE = "BTTCGR";
    ROD_TABLE = "BROD";
    DTMROC_TABLE = "BDTMROC";
  }
  else if (detector=="endcap") {
    TTC_TABLE = "ECTTC";
    TTCGR_TABLE = "ECTTCGR";
    ROD_TABLE = "ECROD";
    DTMROC_TABLE = "ECDTMROC";
  }



  m_session->transaction().start(true);


  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(BCONNECT_TABLE).newQuery();

  query00->addToOutputList("Partition" ); 
  query00->addToOutputList("Crate" ); 
  query00->addToOutputList("Slot" ); 

  std::string conn_cond = BCONNECT_TABLE;
  conn_cond += ".phi = :phi_val";


  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<int>( "phi_val" );
    
  query00->setCondition( conn_cond, conn_condData);
        

    
  conn_condData[0].data<int>() = phi;

  coral::ICursor& cursor00 = query00->execute();
  int nRowsM = 0;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();
      
    part_id = row00[0].data<int>();
    crate_id = row00[1].data<int>();
    slot_id = row00[2].data<int>();
      
    ttc_id = (part_id+4)*10000 + crate_id*100 + slot_id;

    if(m_verbose) std::cout << std::endl;
    if(m_verbose) std::cout << "****************************" << std::endl;

    if (part_id < 33){
      if(m_verbose) std::cout << "TRT Barrel Partition Side ";
      if (part_id < 32) {
	if(m_verbose) std::cout << "A" << phi << std::endl;
      }
      else { 
	if(m_verbose) std::cout << "C" << std::endl;
	if(m_verbose) std::cout << "Phi = " << phi << std::endl;
      }
    }
    else {
      if(m_verbose) std::cout << "TRT EndCap Partition Side ";
      if (part_id < 34) {
	if(m_verbose) std::cout << "A" << std::endl; 
	if(m_verbose) std::cout << "Phi = " << phi << std::endl;
      }
      else { 
	if(m_verbose) std::cout << "C" << std::endl;
	if(m_verbose) std::cout << "Phi = " << phi << std::endl;
      }
    }

 
    if(m_verbose) std::cout << std::endl;

    if(m_verbose) std::cout << "TTC ID = " << ttc_id << std::endl;

    if(m_verbose) std::cout << std::endl;

    if(m_verbose) std::cout << "TTCGROUP ID = ";
    for (int i=0; i<4; i++){
      ttcgr_id = ttc_id*10 + i;
      if(m_verbose) std::cout << ttcgr_id << " ";
    }
    if(m_verbose) std::cout << std::endl;

    rod_id = part_id*100 + phi;

    if(m_verbose) std::cout << std::endl;
      
    if(m_verbose) std::cout << "ROD ID = " << rod_id << std::endl;

    if(m_verbose) std::cout << std::endl;

    if(m_verbose) std::cout << "DTMROC_ID = ";
    if (detector=="barrel"){
      for (int j=0; j<104; j++) {
	dtmroc_id = part_id*100000 + phi*1000 + j;
	if(m_verbose) std::cout << dtmroc_id << " ";
      }
    }
    else if (detector=="endcap"){
      for (int j=0; j<120; j++) {
	dtmroc_id = part_id*100000 + phi*1000 + j;
	if(m_verbose) std::cout << dtmroc_id << " ";
      }
    }

    if(m_verbose) std::cout << std::endl;
    if(m_verbose) std::cout << "****************************" << std::endl;
    if(m_verbose) std::cout << std::endl;
    
    nRowsM++;
  }
  delete query00;




  m_session->transaction().commit();


  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

  return 0;
	
}




//------------------------------------------------------------------------------
/// Method to update the key in the KEYGEN table
void COOLCORALClient::UpdateThr(std::vector<DTMROC*>* chips){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Update Thresholds" << std::endl;
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  m_session->transaction().start();

  coral::ITableDataEditor& dtmeditor = m_session->nominalSchema().tableHandle( "BDTMROC" ).dataEditor();

  // Update the  key
  std::vector<DTMROC*>::const_iterator chip_iter;
  for(chip_iter = chips->begin(); chip_iter != chips->end(); ++chip_iter){

    DTMROC* chip = *chip_iter;

    coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

    rowBuffer.extend<short>( "Thresh0_Low_FE" );
    rowBuffer.extend<short>( "Thresh0_High_FE" );
    rowBuffer.extend<short>( "Thresh1_Low_FE" );
    rowBuffer.extend<short>( "Thresh1_High_FE" );
    
    std::string updateAction1 = " Thresh0_Low_FE = :value";
    std::string updateAction2 = " Thresh0_High_FE = :value";
    std::string updateAction3 = " Thresh1_Low_FE = :value";
    std::string updateAction4 = " Thresh1_High_FE = :value";
    std::string updateCondition = "Comment = :kk AND HW_Addr_FE = :ll";

    coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.

    std::ostringstream inp_com;
    inp_com << (chip->board).phi << "-" << (chip->board).name;

    std::ostringstream inp_hw_addr;
    inp_hw_addr << chip->hwaddr;

    if(m_verbose) std::cout << "Board name = " << inp_com.str() 
			  << " and HW addr = " << inp_hw_addr.str() << std::endl;

    updateData.extend<short>("value");
    updateData.extend<std::string>("kk");
    updateData.extend<std::string>("ll");
    
    updateData[0].data<short>() = chip->lthresh0;
    updateData[1].data<std::string>() = inp_com.str();
    updateData[2].data<std::string>() = inp_hw_addr.str();


    long rowsUpdated = dtmeditor.updateRows( updateAction1, updateCondition, updateData );
    updateData[0].data<short>() = chip->hthresh0;
    dtmeditor.updateRows( updateAction2, updateCondition, updateData );
    updateData[0].data<short>() = chip->lthresh1;
    dtmeditor.updateRows( updateAction3, updateCondition, updateData );
    updateData[0].data<short>() = chip->hthresh1;
    dtmeditor.updateRows( updateAction4, updateCondition, updateData );
    if(m_verbose) std::cout << "Updated " << rowsUpdated << " DTMROCs" << std::endl;

  }
 
  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec for " << chips->size() << " DTMROCs" << std::endl;
  
  m_session->transaction().commit();

}

  
//------------------------------------------------------------------------------
/// Method to update the key in the KEYGEN table
void COOLCORALClient::UpdateFine(std::vector<TTC_Line*>* ttclines){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Update fine delays" << std::endl;
    
  int part_id=0;
  int crate_id=0;
  int slot_id=0;

  int ttc_id=0;
  int ttcgr_id=0;

  std::string   BCONNECT_TABLE = "BCONNECT";

  m_session->transaction().start();

  std::vector<TTC_Line*>::const_iterator ttcline_iter;
  for(ttcline_iter = ttclines->begin(); ttcline_iter != ttclines->end(); ++ttcline_iter){
    TTC_Line* ttcline = *ttcline_iter;

    //**********************************************
    // Beginning of writing part
    // 
    // Open the query
    coral::IQuery* query00 = m_session->nominalSchema().tableHandle(BCONNECT_TABLE).newQuery();

    query00->addToOutputList("Crate" ); 
    query00->addToOutputList("Slot" ); 

    std::string conn_cond = BCONNECT_TABLE;
    conn_cond += ".phi = :phi_val";
    conn_cond += " AND ";
    conn_cond += BCONNECT_TABLE;
    conn_cond += ".Partition = :part";

    coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
    conn_condData.extend<int>( "phi_val" );
    conn_condData.extend<int>( "part" );
    
    query00->setCondition( conn_cond, conn_condData);
        
    conn_condData[0].data<int>() = ttcline->phi;

    if(ttcline->name.find('E') != std::string::npos)
      part_id = (ttcline->side=="A" ? 33:34);
    else
      part_id = (ttcline->side=="A" ? 31:32);
      
    conn_condData[1].data<int>() = part_id;
    
    coral::ICursor& cursor00 = query00->execute();
    while ( cursor00.next() ) {
      const coral::AttributeList &row00 = cursor00.currentRow();
      
      crate_id = row00[0].data<int>();
      slot_id = row00[1].data<int>();
      
      ttc_id = (part_id+4)*10000 + crate_id*100 + slot_id;
    }

    // Update the  fields
    coral::ITableDataEditor& dtmeditor = m_session->nominalSchema().tableHandle( "BTTCGR" ).dataEditor();

    coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

    rowBuffer.extend<short>( "EdgeSelect" );
    rowBuffer.extend<short>( "finebx" );
    rowBuffer.extend<short>( "finedx" );
    rowBuffer.extend<short>( "TDMdelay" );
    
    std::string updateAction1 = "EdgeSelect = :value";
    std::string updateAction2 = "finebx = :value";
    std::string updateAction3 = "finedx = :value";
    std::string updateAction4 = "TDMdelay = :value";

    std::string updateCondition = "ttcgr_UID = :kk";
    
    coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.

    ttcline->group += (int)(ttcline->line/10);
    ttcline->line = (ttcline->line%10);

    if (m_verbose) {
      std::cout << "GROUP = " <<     ttcline->group  << std::endl;
      std::cout << "LINE = " <<     ttcline->line  << std::endl;
    }


    ttcgr_id = ttc_id*10 + ttcline->group;
    if(m_verbose) std::cout << "TTCGR_ID = " << ttcgr_id << std::endl;
      

    updateData.extend<std::string>("value");
    updateData.extend<int>("kk");
    updateData[1].data<int>() = ttcgr_id;

    //----------------------------------------------
    // Here we need to get the values in the database, so we don't overwrite
    // values that are not meant to be changed
    std::ostringstream cond;
    cond << "ttcgr_UID = " << std::dec << ttcgr_id;

    coral::IQuery* query = m_session->nominalSchema().tableHandle("BTTCGR").newQuery();
    coral::AttributeList selectData ATLAS_THREAD_SAFE; // Not shared, so ok.
    query ->setCondition(cond.str(),selectData);

    query->addToOutputList("ttcgr_UID");
    query->addToOutputList("ttcgr_iovfk");
    query->addToOutputList("EdgeSelect");
    query->addToOutputList("TDMdelay");
    query->addToOutputList("finedx");
    query->addToOutputList("finebx");

    std::string EdgeSelect_fdb = "10 0 0 0 0 0 0 0 0 0 0";
    std::string TDMdelay_fdb   = "10 0 0 0 0 0 0 0 0 0 0";
    std::string finedx_fdb     = "10 0 0 0 0 0 0 0 0 0 0";
    std::string finebx_fdb     = "10 0 0 0 0 0 0 0 0 0 0";

    coral::ICursor& cursor = query->execute();
    int nRows = 0;
    while ( cursor.next() ) {
      ++nRows;
      const coral::AttributeList& sel = cursor.currentRow();

      EdgeSelect_fdb = sel["EdgeSelect"].data<std::string>();
      TDMdelay_fdb   = sel["TDMdelay"].data<std::string>();
      finedx_fdb     = sel["finedx"].data<std::string>();
      finebx_fdb     = sel["finebx"].data<std::string>();
    }
    delete query;
    
    // End of reading part
    //----------------------------------------------

    //----------------------------------------------
    // Here's the data we'll insert into the database
    std::vector <int> bxv(10);
    std::vector <int> doutv(10);
    std::vector <int> edgev(10);
    std::vector <int> tdmdelayv(10);
    
    std::string bx;	
    std::string dout; 	
    std::string edgeselect;	
    std::string tdmdelay;

    std::istringstream edge_iss(EdgeSelect_fdb);
    std::istringstream bx_iss(finebx_fdb);
    std::istringstream dout_iss(finedx_fdb);
    std::istringstream tdmdelay_iss(TDMdelay_fdb);

    int dummy;
    edge_iss >> dummy;
    bx_iss >> dummy;
    dout_iss >> dummy;
    tdmdelay_iss >> dummy;
    for(int i=0; i<10; ++i){
      bx_iss >> bxv[i];
      dout_iss >> doutv[i];
      edge_iss >> edgev[i];
      tdmdelay_iss >> tdmdelayv[i];
    }
      
    bxv[ttcline->line] = ttcline->bx;
    doutv[ttcline->line] = ttcline->dx;
    edgev[ttcline->line] = ttcline->strobe;
    tdmdelayv[ttcline->line] = ttcline->tdm;

    VECTOR2STRING (bxv, bx);
    VECTOR2STRING (doutv, dout);
    VECTOR2STRING (edgev, edgeselect);
    VECTOR2STRING (tdmdelayv, tdmdelay);
    //----------------------------------------------
    

    long rowsUpdated;
    updateData[0].data<std::string>() = edgeselect;
    rowsUpdated = dtmeditor.updateRows( updateAction1, updateCondition, updateData );

    updateData[0].data<std::string>() = bx;
    rowsUpdated = dtmeditor.updateRows( updateAction2, updateCondition, updateData );

    updateData[0].data<std::string>() = dout;
    rowsUpdated = dtmeditor.updateRows( updateAction3, updateCondition, updateData );

    updateData[0].data<std::string>() = tdmdelay;
    rowsUpdated = dtmeditor.updateRows( updateAction4, updateCondition, updateData );

    if(m_verbose) std::cout << "Updated " << rowsUpdated << " TTC GROUPS" << std::endl;

    delete query00;
    //**********************************************

  }
  m_session->transaction().commit();

}


//------------------------------------------------------------------------------
/// Method to update the key in the KEYGEN table
void COOLCORALClient::UpdateGol(const GOL* golinp,int goltype){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: GOL delay change" << std::endl;
    
  int rod_id = golinp->rod_id;

  if(m_verbose) std::cout << "ROD_ID = " << rod_id << std::endl;

  if(m_verbose) std::cout << "goltype = " << goltype << std::endl;

  m_session->transaction().start();



  // Update the  key



  coral::ITableDataEditor& dtmeditor = m_session->nominalSchema().tableHandle( "BROD" ).dataEditor();

  coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

  std::string extension = "GolDelay";
  extension += ToString(golinp->gol);



  rowBuffer.extend<short>( extension );

  //     std::string updateAction1 = "EdgeSelect = :value";
  std::string updateAction1 = extension;
  updateAction1 += " = :value";


  std::string updateCondition;
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
  
  updateCondition = "RODDetID = :kk";
  
  updateData.extend<int>("value");
  updateData.extend<int>("kk");
  
  
  updateData[0].data<int>() = golinp->delay;
  updateData[1].data<int>() = rod_id;
  
  long rowsUpdated = dtmeditor.updateRows( updateAction1, updateCondition, updateData );
  
  if(m_verbose) std::cout << "Updated " << rowsUpdated << " ROD" << std::endl;
  
  
  m_session->transaction().commit();
  
}

//------------------------------------------------------------------------------
/// Create CORAL only tables for a partition, identified by an input string
void COOLCORALClient::createHistTables(bool coolswitch){

  if(m_verbose) std::cout << "\nCOOLCORAL Client: Create tables" << std::endl;
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  m_session->transaction().start();

  
  std::string TTC_TABLE="HIST_TTC";
  std::string TTCGR_TABLE="HIST_TTCGR";
  std::string ROD_TABLE="HIST_ROD";
  std::string DTMROC_TABLE="HIST_DTMROC";

  if(m_verbose) std::cout << "Deleting the old barrel tables: " << std::endl;
  if(m_verbose) std::cout <<  TTC_TABLE << std::endl;
  if(m_verbose) std::cout <<  TTCGR_TABLE << std::endl;
  if(m_verbose) std::cout <<  ROD_TABLE << std::endl;
  if(m_verbose) std::cout <<  DTMROC_TABLE << std::endl;

  if(m_verbose) std::cout <<  "HISTORY" << std::endl;
  if(m_verbose) std::cout <<  "KEYGEN" << std::endl;


  m_session->nominalSchema().dropIfExistsTable(TTC_TABLE);
  m_session->nominalSchema().dropIfExistsTable(TTCGR_TABLE);
  m_session->nominalSchema().dropIfExistsTable(ROD_TABLE);
  m_session->nominalSchema().dropIfExistsTable(DTMROC_TABLE);

  m_session->nominalSchema().dropIfExistsTable("HISTORY");
  m_session->nominalSchema().dropIfExistsTable("KEYGEN");

  //-------------------TRT TTC Table

  if(m_verbose) std::cout << "Creating table: " << TTC_TABLE << std::endl;

       
  // Create the table
  coral::TableDescription ttc_columns( "SchemaDefinition_TTC" );
  ttc_columns.setName( TTC_TABLE );
  ttc_columns.insertColumn( "ttc_UID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "ttc_iovfk",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttc_columns.insertColumn( "ByteSwapping",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  ttc_columns.insertColumn( "PhysicalAddress",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "DetID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "VMESlot",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "Delay",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttc_columns.insertColumn( "ArShaping",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  ttc_columns.insertColumn( "SetSendID",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  ttc_columns.insertColumn( "Name",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "ClockSource",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "InputFragmentType",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "Comment",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttc_columns.insertColumn( "ttc_tag",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  std::vector <std::string> tmp_strings;

  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  tmp_strings.clear();
  tmp_strings.push_back ("ttc_UID");
  ttc_columns.createIndex ("httc_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("ttc_iovfk");
  // Define primary key
  //	      ttc_columns.setPrimaryKey( tmp_strings );
  // ttc_columns.setNotNullConstraint ("ttc_iovfk");
  // Define index
  ttc_columns.createIndex ("httcio_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("ttc_tag");
  ttc_columns.createIndex ("htag_idx", tmp_strings);
  
  ttc_columns.setNotNullConstraint ("ttc_UID");
  
  
  // Create the actual table
  m_session->nominalSchema().createTable( ttc_columns );
  
  //-------------------TRT TTCGR Table
  
  if(m_verbose) std::cout << "Creating table: " << TTCGR_TABLE << std::endl;

       
  // Create the table
  coral::TableDescription ttcgr_columns( "SchemaDefinition_TTCGR" );
  ttcgr_columns.setName( TTCGR_TABLE );
  ttcgr_columns.insertColumn( "ttcgr_UID",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgr_columns.insertColumn( "ttcgr_iovfk",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttcgr_columns.insertColumn( "Group",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgr_columns.insertColumn( "DutyCycle",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgr_columns.insertColumn( "EdgeSelect",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "finebx",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "finedx",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "TDMdelay",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "TTCGRNAME",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  ttcgr_columns.insertColumn( "ttcgr_tag",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );


  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  tmp_strings.clear();
  tmp_strings.push_back ("ttcgr_UID");
  ttcgr_columns.createIndex ("httcgr_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("ttcgr_iovfk");
  ttcgr_columns.createIndex ("httcgrio_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("ttcgr_tag");
  ttcgr_columns.createIndex ("hgrtag_idx", tmp_strings);
  ttcgr_columns.setNotNullConstraint ("ttcgr_UID");
  // Create the actual table
  
  m_session->nominalSchema().createTable( ttcgr_columns );
  
  //-------------------TRT ROD Table

  if(m_verbose) std::cout << "Creating table: " << ROD_TABLE << std::endl;


  // Create the table
  coral::TableDescription rod_columns( "SchemaDefinition_ROD" );
  rod_columns.setName( ROD_TABLE );
  rod_columns.insertColumn( "rod_UID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "rod_iovfk",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rod_columns.insertColumn( "RODByteSwapping",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  rod_columns.insertColumn( "RODPhysicalAddress",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODDetID",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODVMESlot",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODHalf",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect0",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect1",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect2",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "EdgeSelect3",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay0",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay1",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay2",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "GolDelay3",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rod_columns.insertColumn( "RODStatus",
			    coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  rod_columns.insertColumn( "RODInputFragmentType",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  rod_columns.insertColumn( "RODName",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  rod_columns.insertColumn( "Comment",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  rod_columns.insertColumn( "rod_tag",
			    coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK

  tmp_strings.clear();
  tmp_strings.push_back ("rod_UID");
  rod_columns.createIndex ("hrod_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("rod_iovfk");
  rod_columns.createIndex ("hrodio_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("rod_tag");
  rod_columns.createIndex ("hrtag_idx", tmp_strings);

  rod_columns.setNotNullConstraint ("rod_UID");



  // Create the actual table
  m_session->nominalSchema().createTable( rod_columns );

  //-------------------TRT DTMROC Table

  if(m_verbose) std::cout << "Creating table: " << DTMROC_TABLE << std::endl;


  // Create the table
  coral::TableDescription dtmroc_columns( "SchemaDefinition_DTMROC" );
  dtmroc_columns.setName( DTMROC_TABLE );
  dtmroc_columns.insertColumn( "dtmroc_UID",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "dtmroc_iovfk",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  dtmroc_columns.insertColumn( "DTMROCByteSwapping",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "DTMROCPhysicalAddress",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "ChipID",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "ChipValid",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "RODGroup",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "RODInput",
			       coral::AttributeSpecification::typeNameForId( typeid(int) ) );
  dtmroc_columns.insertColumn( "HW_Addr_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "TTC_Line_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh0_Low_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh0_High_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh1_Low_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Thresh1_High_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "VT_DAC0_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "VT_DAC1_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "Mask_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  dtmroc_columns.insertColumn( "Pipe_Latency_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(short) ) );
  dtmroc_columns.insertColumn( "DTMROCName",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "DTMROCInputFragmentType",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "Clock_FE",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "Comment",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  dtmroc_columns.insertColumn( "roc_tag",
			       coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );


  // Use creationTime as primary key
  // Unfortunately, there is no easy/standard way of having the
  // database server automatically assigning a value to a PK
  tmp_strings.clear();
  tmp_strings.push_back ("dtmroc_UID");
  dtmroc_columns.createIndex ("hdtmroc_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("dtmroc_iovfk");
  dtmroc_columns.createIndex ("hdtmrocio_idx", tmp_strings);
  
  tmp_strings.clear();
  tmp_strings.push_back ("roc_tag");
  dtmroc_columns.createIndex ("hroctag_idx", tmp_strings);
    
  dtmroc_columns.setNotNullConstraint ("dtmroc_UID");
    
    

  // Create the actual table
  m_session->nominalSchema().createTable( dtmroc_columns );



  //-------------------CORAL Queries aux table

  if(m_verbose) std::cout << "Creating table: HYSTORY" << std::endl;

       
  // Create the table for CORAL queries
  coral::TableDescription coque_columns( "SchemaDefinition_COQUE" );
  coque_columns.setName( "HISTORY" );
  coque_columns.insertColumn( "RUNNumber",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  coque_columns.insertColumn( "TAG",
			      coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );
  coque_columns.insertColumn( "Key",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  coque_columns.insertColumn( "Time",
			      coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );


  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("RUNNumber");
  coque_columns.createIndex ("runnoh_idx", tmp_strings);

  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("TAG");
  coque_columns.createIndex ("tagh_idx", tmp_strings);

  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("Key");
  coque_columns.createIndex ("keyh_idx", tmp_strings);

  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("Time");
  coque_columns.createIndex ("timeh_idx", tmp_strings);


  // Create the actual table
  m_session->nominalSchema().createTable( coque_columns );

  //-------------------KEYGEN table

  if(m_verbose) std::cout << "Creating table: KEYGEN" << std::endl;

       
  // Create the table for CORAL queries
  coral::TableDescription key_columns( "SchemaDefinition_KEY" );
  key_columns.setName( "KEYGEN" );
  key_columns.insertColumn( "Key",
			    coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );


  // put an index on const
  tmp_strings.clear();
  tmp_strings.push_back ("Key");
  key_columns.createIndex ("key_idx", tmp_strings);


  // Create the actual table
  m_session->nominalSchema().createTable( key_columns );


  // Fill the first key

  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( "KEYGEN" ).dataEditor();

  coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer.extend<long long>( "Key" );

  long long& key = rowBuffer[ "Key" ].data<long long>();

  key = 1000;

  keyeditor.insertRow( rowBuffer );


  if (coolswitch) {
  //Now creating the COOL Folders
  std::string ROOT_FOLDER = "/TRT";
  std::string TTC_FOLDER;
  std::string TTCGR_FOLDER;
  std::string ROD_FOLDER;
  std::string DTMROC_FOLDER;


  TTC_FOLDER = "/TRT/TTC";
  TTCGR_FOLDER = "/TRT/TTCGR";
  ROD_FOLDER = "/TRT/ROD";
  DTMROC_FOLDER = "/TRT/DTMROC";
    
  if(m_verbose) std::cout << "Dropping old folders" << std::endl;
  if(m_verbose) std::cout <<  TTC_FOLDER << std::endl;
  if(m_verbose) std::cout <<  TTCGR_FOLDER << std::endl;
  if(m_verbose) std::cout <<  ROD_FOLDER << std::endl;
  if(m_verbose) std::cout <<  DTMROC_FOLDER << std::endl;

  m_db->dropNode(TTC_FOLDER);
  m_db->dropNode(TTCGR_FOLDER);
  m_db->dropNode(ROD_FOLDER);
  m_db->dropNode(DTMROC_FOLDER);

  
if(m_verbose) std::cout << "Creating the folders: " << std::endl;
  if(m_verbose) std::cout <<  TTC_FOLDER << std::endl;
  if(m_verbose) std::cout <<  TTCGR_FOLDER << std::endl;
  if(m_verbose) std::cout <<  ROD_FOLDER << std::endl;
  if(m_verbose) std::cout <<  DTMROC_FOLDER << std::endl;
  
  if (m_db->existsFolderSet(ROOT_FOLDER)){
    m_db->dropNode(ROOT_FOLDER);
    m_db->createFolderSet(ROOT_FOLDER);
  }
  else 
    {
    m_db->createFolderSet(ROOT_FOLDER);
    }

  cool::RecordSpecification spec_ttc;
  spec_ttc.extend("ttc_iovfk",cool::StorageType::UInt63);
  spec_ttc.extend("tag",cool::StorageType::String4k);
	cool::FolderSpecification ttcFolderSpec(FolderVersioning::SINGLE_VERSION,spec_ttc);
  m_db->createFolder(TTC_FOLDER, ttcFolderSpec , "TTC");

  cool::RecordSpecification spec_ttcgr;
  spec_ttcgr.extend("ttcgr_iovfk",cool::StorageType::UInt63);
  spec_ttcgr.extend("tag",cool::StorageType::String4k);
    
  cool::FolderSpecification ttcgrFolderSpec(FolderVersioning::SINGLE_VERSION,spec_ttcgr);
  m_db->createFolder(TTCGR_FOLDER, ttcgrFolderSpec, "TTCGR");
    
  cool::RecordSpecification spec_rod;
  spec_rod.extend("rod_iovfk",cool::StorageType::UInt63);
  spec_rod.extend("tag",cool::StorageType::String4k);
  cool::FolderSpecification rodFolderSpec(FolderVersioning::SINGLE_VERSION,spec_rod);  
  m_db->createFolder(ROD_FOLDER, rodFolderSpec, "ROD");

  cool::RecordSpecification spec_dtmroc;
  spec_dtmroc.extend("dtmroc_iovfk",cool::StorageType::UInt63);
  spec_dtmroc.extend("tag",cool::StorageType::String4k);
  cool::FolderSpecification dtmrocFolderSpec(FolderVersioning::SINGLE_VERSION,spec_dtmroc);  
  m_db->createFolder(DTMROC_FOLDER, dtmrocFolderSpec, "DTMROC");

  }
  else 
    {
      if(m_verbose) std::cout << "*** No COOL folders created ***" 
			      << std::endl;
    }




  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;

 
  m_session->transaction().commit();

}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
int COOLCORALClient::GetLastTTC(){
    
  std::string TTC_TABLE="BTTC";
  //  int part_nr = ttc_id/10000 - 4  ;
    
  m_session->transaction().start(true);
    
    
  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("ttc_UID" ); 
  query0->addToOutputList("ttc_iovfk" ); 
  query0->addToOutputList("ByteSwapping" ); 
  query0->addToOutputList("PhysicalAddress" ); 
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Name" ); 
  query0->addToOutputList("ClockSource" ); 
  query0->addToOutputList("InputFragmentType" ); 
  query0->addToOutputList("Comment" ); 
  query0->addToOutputList("ttc_tag" ); 
    
//   std::string ttc_cond = " ";
//   coral::AttributeList ttc_condData;
//   ttc_condData.extend<int>( "ttcnr" );
    
//   query0->setCondition( ttc_cond, ttc_condData);
    
//   ttc_condData[0].data<int>() = ttc_id;
    
  coral::ICursor& cursor0 = query0->execute();
    
  int count=0;
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
      
    if(m_verbose) std::cout << "ttc_UID           = \t" << row0[0].data<int>() << std::endl;
    if(m_verbose) std::cout << "ttc_iovfk         = \t" << row0[1].data<long long>() << std::endl;
    if(m_verbose) std::cout << "ByteSwapping      = \t" << row0[2].data<short>() << std::endl;
    if(m_verbose) std::cout << "PhysicalAddress   = \t" << row0[3].data<int>() << std::endl;
    if(m_verbose) std::cout << "DetID             = \t" << row0[4].data<int>() << std::endl;
    if(m_verbose) std::cout << "VMEslot           = \t" << row0[5].data<int>() << std::endl;
    if(m_verbose) std::cout << "Delay             = \t" << row0[6].data<int>() << std::endl;
    if(m_verbose) std::cout << "ArShaping         = \t" << row0[7].data<short>() << std::endl;
    if(m_verbose) std::cout << "SetSendID         = \t" << row0[8].data<short>() << std::endl;
    if(m_verbose) std::cout << "Name              = \t" << row0[9].data<std::string>() << std::endl;
    if(m_verbose) std::cout << "ClockSource       = \t" << row0[10].data<std::string>() << std::endl;
    if(m_verbose) std::cout << "InputFragmentType = \t" << row0[11].data<std::string>() << std::endl;
    if(m_verbose) std::cout << "Comment           = \t" << row0[12].data<std::string>() << std::endl;
    if(m_verbose) std::cout << "ttc_tag           = \t" << row0[13].data<std::string>() << std::endl;
      
    count++;
  }
  delete query0;
  
  if(m_verbose) std::cout << "ttc_tagNumber of lines = " << count << std::endl;

  m_session->transaction().commit();
    
  return 1;
    
}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
void COOLCORALClient::GetLastValues(Detector_t& detector){
    
  std::string TTC_TABLE="BTTC";
  std::string TTCGR_TABLE="BTTCGR";
  std::string ROD_TABLE="BROD";
  std::string DTMROC_TABLE="BDTMROC";

  TTC_t ttc;
  TTCGroup_t ttcgr;
  ROD_t rod;
  DTMROC_t dtmroc;
  
//   std::vector<TTC_t> ttcv;
//   std::vector<TTCGR_t> ttcgrv;
//   std::vector<ROD_t> rodv;
//   std::vector<DTMROC_t> dtmrocv;

  m_session->transaction().start(true);

  // GET all TTCs

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("ttc_UID" ); 
  query0->addToOutputList("ttc_iovfk" ); 
  query0->addToOutputList("ByteSwapping" ); 
  query0->addToOutputList("PhysicalAddress" ); 
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Name" ); 
  query0->addToOutputList("ClockSource" ); 
  query0->addToOutputList("InputFragmentType" ); 
  query0->addToOutputList("Comment" ); 
  query0->addToOutputList("ttc_tag" ); 
        
  coral::ICursor& cursor0 = query0->execute();
    
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
      
    ttc.ttc_UID = row0[0].data<int>();
    ttc.ttc_iovfk = row0[1].data<long long>();
    ttc.ByteSwapping = row0[2].data<short>();
    ttc.PhysicalAddress = row0[3].data<int>();
    ttc.DetID = row0[4].data<int>();
    ttc.VMEslot = row0[5].data<int>();
    ttc.Delay = row0[6].data<int>();
    ttc.ArShaping =  row0[7].data<short>();
    ttc.SetSendID = row0[8].data<short>();
    ttc.Name = row0[9].data<std::string>();
    ttc.ClockSource = row0[10].data<std::string>();
    ttc.InputFragmentType = row0[11].data<std::string>();
    ttc.Comment = row0[12].data<std::string>() ;
    ttc.ttc_tag = row0[13].data<std::string>() ;
    
    (detector.ttc).push_back(ttc);
      
  }
  delete query0;

  coral::IQuery* query1 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();
    
  query1->addToOutputList("ttcgr_UID" ); 
  query1->addToOutputList("ttcgr_iovfk" ); 
  query1->addToOutputList("Group" ); 
  query1->addToOutputList("DutyCycle" ); 
  query1->addToOutputList("EdgeSelect" ); 
  query1->addToOutputList("finebx" ); 
  query1->addToOutputList("finedx" ); 
  query1->addToOutputList("TDMdelay" ); 
  query1->addToOutputList("TTCGRNAME" ); 
  query1->addToOutputList("ttcgr_tag" ); 
    
  coral::ICursor& cursor1 = query1->execute();
    
  while ( cursor1.next() ) {
    const coral::AttributeList &row1 = cursor1.currentRow();
    ttcgr.ttcgr_UID = row1[0].data<int>();
    ttcgr.ttcgr_iovfk = row1[1].data<long long>();
    ttcgr.Group = row1[2].data<int>();
    ttcgr.DutyCycle = row1[3].data<int>();
    ttcgr.EdgeSelect = row1[4].data<std::string>();
    ttcgr.finebx = row1[5].data<std::string>();
    ttcgr.finedx =  row1[6].data<std::string>();
    ttcgr.TDMdelay =  row1[7].data<std::string>();
    ttcgr.TTCGRNAME = row1[8].data<std::string>();
    ttcgr.ttcgr_tag = row1[9].data<std::string>();
    
    (detector.ttcgr).push_back(ttcgr);
  }
  delete query1;

  coral::IQuery* query2 = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
    
  query2->addToOutputList("rod_UID"); 
  query2->addToOutputList("rod_iovfk"); 
  query2->addToOutputList("RODByteSwapping"); 
  query2->addToOutputList("RODPhysicalAddress"); 
  query2->addToOutputList("RODDetID");
  query2->addToOutputList("RODVMESlot");
  query2->addToOutputList("RODHalf"); 
  query2->addToOutputList("EdgeSelect0");
  query2->addToOutputList("EdgeSelect1"); 
  query2->addToOutputList("EdgeSelect2"); 
  query2->addToOutputList("EdgeSelect3");
  query2->addToOutputList("GolDelay0"); 
  query2->addToOutputList("GolDelay1"); 
  query2->addToOutputList("GolDelay2"); 
  query2->addToOutputList("GolDelay3"); 
  query2->addToOutputList("RODStatus");
  query2->addToOutputList("RODInputFragmentType");
  query2->addToOutputList("RODName");
  query2->addToOutputList("Comment"); 
  query2->addToOutputList("rod_tag"); 
	    
    
  coral::ICursor& cursor2 = query2->execute();
    
  while ( cursor2.next() ) {
    const coral::AttributeList &row2 = cursor2.currentRow();

    rod.rod_UID = row2[0].data<int>();
    rod.rod_iovfk = row2[1].data<long long>();
    rod.RODByteSwapping = row2[2].data<short>();
    rod.RODPhysicalAddress = row2[3].data<int>();
    rod.RODDetID = row2[4].data<int>();
    rod.RODVMEslot = row2[5].data<int>();
    rod.RODhalf = row2[6].data<int>();
    rod.EdgeSelect0 = row2[7].data<int>();
    rod.EdgeSelect1 = row2[8].data<int>();
    rod.EdgeSelect2 = row2[9].data<int>();
    rod.EdgeSelect3 = row2[10].data<int>();
    rod.GolDelay0 = row2[11].data<int>();
    rod.GolDelay1 = row2[12].data<int>();
    rod.GolDelay2 = row2[13].data<int>();
    rod.GolDelay3 = row2[14].data<int>();
    rod.RODStatus = row2[15].data<short>() ;
    rod.RODInputFragmentType = row2[16].data<std::string>();
    rod.RODName = row2[17].data<std::string>();
    rod.Comment = row2[18].data<std::string>();
    rod.rod_tag = row2[19].data<std::string>();
  
    (detector.rod).push_back(rod);
      
  }
  delete query2;


  coral::IQuery* query3 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();
    
  query3->addToOutputList("dtmroc_UID"); 
  query3->addToOutputList("dtmroc_iovfk"); 
  query3->addToOutputList("DTMROCByteSwapping"); 
  query3->addToOutputList("DTMROCPhysicalAddress"); 
  query3->addToOutputList("ChipID");
  query3->addToOutputList("ChipValid");
  query3->addToOutputList("RODGroup"); 
  query3->addToOutputList("RODInput");
  query3->addToOutputList("HW_Addr_FE"); 
  query3->addToOutputList("TTC_Line_FE"); 
  query3->addToOutputList("Thresh0_Low_FE");
  query3->addToOutputList("Thresh0_High_FE");
  query3->addToOutputList("Thresh1_Low_FE");
  query3->addToOutputList("Thresh1_High_FE");
  query3->addToOutputList("VT_DAC0_FE"); 
  query3->addToOutputList("VT_DAC1_FE"); 
  query3->addToOutputList("Mask_FE"); 
  query3->addToOutputList("Pipe_Latency_FE"); 
  query3->addToOutputList("DTMROCName");
  query3->addToOutputList("DTMROCInputFragmentType");
  query3->addToOutputList("Clock_FE"); 
  query3->addToOutputList("Comment"); 
  query3->addToOutputList("roc_tag"); 
	    
    
  coral::ICursor& cursor3 = query3->execute();
    
  while ( cursor3.next() ) {
    const coral::AttributeList &row3 = cursor3.currentRow();

    dtmroc.dtmroc_UID    = row3[0].data<int>(); 
    dtmroc.dtmroc_iovfk    = row3[1].data<long long>(); 
    dtmroc.DTMROCByteSwapping    = row3[2].data<short>(); 
    dtmroc.DTMROCPhysicalAddress    = row3[3].data<int>(); 
    dtmroc.ChipID   = row3[4].data<int>();
    dtmroc.Chip_Valid   = row3[5].data<short>();
    dtmroc.RODgroup    = row3[6].data<short>(); 
    dtmroc.RODinput    = row3[7].data<int>();
    dtmroc.HW_addr_FE   = row3[8].data<short>(); 
    dtmroc.TTC_Line_FE   = row3[9].data<short>(); 
    dtmroc.Thresh0_Low_FE   = row3[10].data<short>();
    dtmroc.Thresh0_High_FE   = row3[11].data<short>();
    dtmroc.Thresh1_Low_FE   = row3[12].data<short>();
    dtmroc.Thresh1_High_FE    = row3[13].data<short>();
    dtmroc.VT_DAC0_FE   = row3[14].data<short>(); 
    dtmroc.VT_DAC1_FE   = row3[15].data<short>(); 
    dtmroc.Mask_FE   = row3[16].data<int>(); 
    dtmroc.Pipe_Latency_FE   = row3[17].data<short>(); 
    dtmroc.DTMROCName  = row3[18].data<std::string>();
    dtmroc.DTMROCInputFragmentType   = row3[19].data<std::string>();
    dtmroc.Clock_FE   = row3[20].data<std::string>(); 
    dtmroc.Comment   = row3[21].data<std::string>(); 
    dtmroc.roc_tag  = row3[22].data<std::string>(); 
    
    (detector.dtmroc).push_back(dtmroc);
      
  }
  delete query3;



  
//   if(m_verbose) std::cout << "Number of ttcv elements = " << detector.ttc.size() << std::endl;
//   if(m_verbose) std::cout << "Number of ttcgrv elements = " << detector.ttcgr.size() << std::endl;
//   if(m_verbose) std::cout << "Number of rodv elements = " << detector.rod.size() << std::endl;
//   if(m_verbose) std::cout << "Number of dtmrocv elements = " << detector.dtmroc.size() << std::endl;

  m_session->transaction().commit();
    
}

// ---------------------------------------------------
/// Fill Full Detector tables
/// It fills only the CORAL tables 
void COOLCORALClient::fillDetector(const Detector_t& Detector){


  m_session->transaction().start();

  struct timeval nunc_time;
  struct timeval start_time;
  struct timeval end_time;
  //unsigned long long nunc_usecs;
  unsigned long long total_usecs;
    
  gettimeofday(&nunc_time, NULL);
  gettimeofday(&start_time, NULL);

  //nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);

  std::string TTC_TABLE="HIST_TTC";
  std::string TTCGR_TABLE="HIST_TTCGR";
  std::string ROD_TABLE="HIST_ROD";
  std::string DTMROC_TABLE="HIST_DTMROC";


  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);
	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,4);

  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,4);

  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);

  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);

  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);

  coral::IBulkOperation* roc_bulk= roc_editor.bulkInsert(roc_row,32);


  std::vector<TTC_t>::const_iterator v_iter0;
  std::vector<TTCGroup_t>::const_iterator v_iter1;
  std::vector<ROD_t>::const_iterator v_iter2;
  std::vector<DTMROC_t>::const_iterator v_iter3;

  for (v_iter0 = (Detector.ttc).begin(); v_iter0 != (Detector.ttc).end(); ++v_iter0) {   
    ttc_row["ttc_UID"].setValue <int> ((*v_iter0).ttc_UID);
    ttc_row["ttc_iovfk"].setValue <long long> ((*v_iter0).ttc_iovfk);
    ttc_row["ByteSwapping"].setValue <short> ((*v_iter0).ByteSwapping);
    ttc_row["PhysicalAddress"].setValue <int> ((*v_iter0).PhysicalAddress);
    ttc_row["DetID"].setValue <int> ((*v_iter0).DetID);
    ttc_row["VMESlot"].setValue <int> ((*v_iter0).VMEslot);
    ttc_row["Delay"].setValue <int> ((*v_iter0).Delay);
    ttc_row["ArShaping"].setValue <short> ((*v_iter0).ArShaping);
    ttc_row["SetSendID"].setValue <short> ((*v_iter0).SetSendID);
    ttc_row["Name"].setValue <std::string> ((*v_iter0).Name);
    ttc_row["ClockSource"].setValue <std::string> ((*v_iter0).ClockSource);
    ttc_row["InputFragmentType"].setValue <std::string> ((*v_iter0).InputFragmentType);
    ttc_row["Comment"].setValue <std::string> ((*v_iter0).Comment);
    ttc_row["ttc_tag"].setValue <std::string> ((*v_iter0).ttc_tag);
    
    ttc_bulk->processNextIteration();   
  }
    
  ttc_bulk->flush();
  delete ttc_bulk;


  for (v_iter1 = (Detector.ttcgr).begin(); v_iter1 != (Detector.ttcgr).end(); ++v_iter1) {   
    ttcgr_row["ttcgr_UID"].setValue <int> ((*v_iter1).ttcgr_UID);
    ttcgr_row["ttcgr_iovfk"].setValue <long long> ((*v_iter1).ttcgr_iovfk);
    ttcgr_row["Group"].setValue <int> ((*v_iter1).Group);
    ttcgr_row["DutyCycle"].setValue <int> ((*v_iter1).DutyCycle);
    ttcgr_row["EdgeSelect"].setValue <std::string> ((*v_iter1).EdgeSelect);
    ttcgr_row["finebx"].setValue <std::string> ((*v_iter1).finebx);
    ttcgr_row["finedx"].setValue <std::string> ((*v_iter1).finedx);
    ttcgr_row["TDMdelay"].setValue <std::string> ((*v_iter1).TDMdelay);
    ttcgr_row["TTCGRNAME"].setValue <std::string> ((*v_iter1).TTCGRNAME);
    ttcgr_row["ttcgr_tag"].setValue <std::string> ((*v_iter1).ttcgr_tag);
      
    ttcgr_bulk->processNextIteration();    
  }
    
  ttcgr_bulk->flush();
  delete ttcgr_bulk;


  for (v_iter2 = (Detector.rod).begin(); v_iter2 != (Detector.rod).end(); ++v_iter2) {
    rod_row["rod_UID"].setValue <int> ((*v_iter2).rod_UID);
    rod_row["rod_iovfk"].setValue <long long> ((*v_iter2).rod_iovfk);
    rod_row["RODByteSwapping"].setValue <short> ((*v_iter2).RODByteSwapping);
    rod_row["RODPhysicalAddress"].setValue <int> ((*v_iter2).RODPhysicalAddress);
    rod_row["RODDetID"].setValue <int> ((*v_iter2).RODDetID);
    rod_row["RODVMESlot"].setValue <int> ((*v_iter2).RODVMEslot);
    rod_row["RODHalf"].setValue <int> ((*v_iter2).RODhalf);
    rod_row["EdgeSelect0"].setValue <int> ((*v_iter2).EdgeSelect0);
    rod_row["EdgeSelect1"].setValue <int> ((*v_iter2).EdgeSelect1);
    rod_row["EdgeSelect2"].setValue <int> ((*v_iter2).EdgeSelect2);
    rod_row["EdgeSelect3"].setValue <int> ((*v_iter2).EdgeSelect3);
    rod_row["GolDelay0"].setValue <int> ((*v_iter2).GolDelay0);
    rod_row["GolDelay1"].setValue <int> ((*v_iter2).GolDelay1);
    rod_row["GolDelay2"].setValue <int> ((*v_iter2).GolDelay2);
    rod_row["GolDelay3"].setValue <int> ((*v_iter2).GolDelay3);
    rod_row["RODStatus"].setValue <short> ((*v_iter2).RODStatus);
    rod_row["RODInputFragmentType"].setValue <std::string> ((*v_iter2).RODInputFragmentType);
    rod_row["RODName"].setValue <std::string> ((*v_iter2).RODName);
    rod_row["Comment"].setValue <std::string> ((*v_iter2).Comment);
    rod_row["rod_tag"].setValue <std::string> ((*v_iter2).rod_tag);
      
    rod_bulk->processNextIteration();
  }

  rod_bulk->flush();
  delete rod_bulk;

  for (v_iter3 = (Detector.dtmroc).begin(); v_iter3 != (Detector.dtmroc).end(); ++v_iter3) {
    roc_row["dtmroc_UID"].setValue <int> ((*v_iter3).dtmroc_UID);
    roc_row["dtmroc_iovfk"].setValue <long long> ((*v_iter3).dtmroc_iovfk);
    roc_row["DTMROCByteSwapping"].setValue <short> ((*v_iter3).DTMROCByteSwapping);
    roc_row["DTMROCPhysicalAddress"].setValue <int> ((*v_iter3).DTMROCPhysicalAddress);
    roc_row["ChipID"].setValue <int> ((*v_iter3).ChipID);
    roc_row["ChipValid"].setValue <short> ((*v_iter3).Chip_Valid);
    roc_row["RODGroup"].setValue <short> ((*v_iter3).RODgroup);
    roc_row["RODInput"].setValue <int> ((*v_iter3).RODinput);
    roc_row["HW_Addr_FE"].setValue <short> ((*v_iter3).HW_addr_FE);
    roc_row["TTC_Line_FE"].setValue <short> ((*v_iter3).TTC_Line_FE);
    roc_row["Thresh0_Low_FE"].setValue <short> ((*v_iter3).Thresh0_Low_FE);
    roc_row["Thresh0_High_FE"].setValue <short> ((*v_iter3).Thresh0_High_FE);
    roc_row["Thresh1_Low_FE"].setValue <short> ((*v_iter3).Thresh1_Low_FE);
    roc_row["Thresh1_High_FE"].setValue <short> ((*v_iter3).Thresh1_High_FE);
    roc_row["VT_DAC0_FE"].setValue <short> ((*v_iter3).VT_DAC0_FE);
    roc_row["VT_DAC1_FE"].setValue <short> ((*v_iter3).VT_DAC1_FE);
    roc_row["Mask_FE"].setValue <int> ((*v_iter3).Mask_FE);
    roc_row["Pipe_Latency_FE"].setValue <short> ((*v_iter3).Pipe_Latency_FE);
    roc_row["DTMROCName"].setValue <std::string> ((*v_iter3).DTMROCName);
    roc_row["DTMROCInputFragmentType"].setValue <std::string> ((*v_iter3).DTMROCInputFragmentType);
    roc_row["Clock_FE"].setValue <std::string> ((*v_iter3).Clock_FE);
    roc_row["Comment"].setValue <std::string> ((*v_iter3).Comment);
    roc_row["roc_tag"].setValue <std::string> ((*v_iter3).roc_tag);
      
    roc_bulk->processNextIteration();    
  }

  roc_bulk->flush();
  delete roc_bulk;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  
  m_session->transaction().commit();

}

// ---------------------------------------------------
/// Fill Full Detector tables 
/// This method fills both CORAL tables and COOL folders
/// The ValidityKey for since is the time of insertion
/// with a precision of the second. That should be improved
/// in the next release.
/// Empty space
void COOLCORALClient::fillHistDetector(const Detector_t& Detector, const std::string& tag){


  m_session->transaction().start();
  
  /// preparing the fill of history as well
  /// update the key before using it
  
  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( "KEYGEN" ).dataEditor();
  
  coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, so ok.
  
  rowBuffer.extend<long long>( "Key" );
  
  std::string updateAction = "Key = Key + :offset";
  std::string updateCondition = "Key > :kk";
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
  updateData.extend<long long>("offset");
  updateData.extend<long long>("kk");
  updateData[0].data<long long>() = 1;
  updateData[1].data<long long>() = 999;
  long rowsUpdated = keyeditor.updateRows( updateAction, updateCondition, updateData );
  if(m_verbose) std::cout << "Updated " << rowsUpdated << " key rows" << std::endl;
  
  // UP

  m_session->transaction().commit();

  m_session->transaction().start();

  struct timeval nunc_time;
  struct timeval start_time;
  struct timeval end_time;
  unsigned long long nunc_usecs;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  std::string TTC_TABLE="HIST_TTC";
  std::string TTCGR_TABLE="HIST_TTCGR";
  std::string ROD_TABLE="HIST_ROD";
  std::string DTMROC_TABLE="HIST_DTMROC";


  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);
	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,4);

  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,4);

  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);

  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);

  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);

  coral::IBulkOperation* roc_bulk= roc_editor.bulkInsert(roc_row,32);

  // all iterators for filling

  std::vector<TTC_t>::const_iterator v_iter0;
  std::vector<TTCGroup_t>::const_iterator v_iter1;
  std::vector<ROD_t>::const_iterator v_iter2;
  std::vector<DTMROC_t>::const_iterator v_iter3;



  // get a key for this operation
  // the key when creating an history entry
  // from the last table will be the same for all
  // Update the key and use it now

  // PUT here a query on KEYGEN table
  coral::IQuery* query00 = m_session->nominalSchema().tableHandle("KEYGEN").newQuery();

  query00->addToOutputList("Key" ); 

  std::string conn_cond = "KEYGEN.Key > :kk";

  coral::AttributeList conn_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  conn_condData.extend<long long>( "kk" );
    
  query00->setCondition( conn_cond, conn_condData);
    
  conn_condData[0].data<long long>() = 999;

  long long key;

  coral::ICursor& cursor00 = query00->execute();
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();
    key = row00[0].data<long long>();
  }
  delete query00;

  if(m_verbose) std::cout << "Key = " << key << std::endl;


  // Update the key and use it now
  coral::ITableDataEditor& cqeditor = m_session->nominalSchema().tableHandle( "HISTORY" ).dataEditor();

  coral::AttributeList rowBuffer1 ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer1.extend<long long>( "RUNNumber" );
  rowBuffer1.extend<std::string>( "TAG" );
  rowBuffer1.extend<long long>( "Key" );
  rowBuffer1.extend<long long>( "Time" );

  long long& cqrunno = rowBuffer1[ "RUNNumber" ].data<long long>(); 
  std::string& cqobj = rowBuffer1[ "TAG" ].data<std::string>();
  long long& cqkey = rowBuffer1[ "Key" ].data<long long>(); 
  long long& cqtime = rowBuffer1[ "Time" ].data<long long>(); 

  gettimeofday(&nunc_time, NULL);
  //  nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);
  nunc_usecs = (nunc_time.tv_sec);

  cqrunno=10101;
  cqobj = tag;
  cqkey = key;
  cqtime = nunc_usecs;

  cqeditor.insertRow( rowBuffer1 );

  // enable the COOL infrastructure
  
  std::string TTC_FOLDER = "/TRT/TTC";
  std::string TTCGR_FOLDER = "/TRT/TTCGR";
  std::string ROD_FOLDER = "/TRT/ROD";
  std::string DTMROC_FOLDER = "/TRT/DTMROC";

  cool::IFolderPtr ttc_fld = m_db->getFolder (TTC_FOLDER);
  cool::IFolderPtr ttcgr_fld = m_db->getFolder (TTCGR_FOLDER);
  cool::IFolderPtr rod_fld = m_db->getFolder (ROD_FOLDER);
  cool::IFolderPtr dtmroc_fld = m_db->getFolder (DTMROC_FOLDER);

  ttc_fld->setupStorageBuffer();
  ttcgr_fld->setupStorageBuffer();
  rod_fld->setupStorageBuffer();
  dtmroc_fld->setupStorageBuffer();


  ValidityKey since = nunc_usecs;  
  ValidityKey until = ValidityKeyMax;

  cool::RecordSpecification ttc_rec =(ttc_fld->payloadSpecification());
  coral::AttributeList ttc_cool_row ATLAS_THREAD_SAFE = Record(ttc_rec).attributeList(); // ok
  cool::RecordSpecification ttcgr_rec =(ttcgr_fld->payloadSpecification());
  coral::AttributeList ttcgr_cool_row ATLAS_THREAD_SAFE = Record(ttcgr_rec).attributeList(); // ok
  cool::RecordSpecification rod_rec =(rod_fld->payloadSpecification());
  coral::AttributeList rod_cool_row ATLAS_THREAD_SAFE = Record(rod_rec).attributeList();  // ok
  cool::RecordSpecification dtmroc_rec =(dtmroc_fld->payloadSpecification());
  coral::AttributeList dtmroc_cool_row ATLAS_THREAD_SAFE = Record(dtmroc_rec).attributeList(); // ok


  for (v_iter0 = (Detector.ttc).begin(); v_iter0 != (Detector.ttc).end(); ++v_iter0) {   
    ttc_row["ttc_UID"].setValue <int> ((*v_iter0).ttc_UID);
    //    ttc_row["ttc_iovfk"].setValue <long long> ((*v_iter0).ttc_iovfk);
    ttc_row["ttc_iovfk"].setValue <long long> (key);
    ttc_row["ByteSwapping"].setValue <short> ((*v_iter0).ByteSwapping);
    ttc_row["PhysicalAddress"].setValue <int> ((*v_iter0).PhysicalAddress);
    ttc_row["DetID"].setValue <int> ((*v_iter0).DetID);
    ttc_row["VMESlot"].setValue <int> ((*v_iter0).VMEslot);
    ttc_row["Delay"].setValue <int> ((*v_iter0).Delay);
    ttc_row["ArShaping"].setValue <short> ((*v_iter0).ArShaping);
    ttc_row["SetSendID"].setValue <short> ((*v_iter0).SetSendID);
    ttc_row["Name"].setValue <std::string> ((*v_iter0).Name);
    ttc_row["ClockSource"].setValue <std::string> ((*v_iter0).ClockSource);
    ttc_row["InputFragmentType"].setValue <std::string> ((*v_iter0).InputFragmentType);
    ttc_row["Comment"].setValue <std::string> ((*v_iter0).Comment);
    //    ttc_row["ttc_tag"].setValue <std::string> ((*v_iter0).ttc_tag);
    ttc_row["ttc_tag"].setValue <std::string> (tag);


    ttc_cool_row["ttc_iovfk"].setValue <unsigned long long> (key);
    ttc_cool_row["tag"].setValue <std::string> (tag);
    cool::Record ttcRecord(ttc_rec, ttc_cool_row);
    ttc_fld->storeObject( since, until, ttcRecord, (*v_iter0).ttc_UID);

    
    ttc_bulk->processNextIteration();   
  }
    
  ttc_fld->flushStorageBuffer();	    

  ttc_bulk->flush();
  delete ttc_bulk;


  for (v_iter1 = (Detector.ttcgr).begin(); v_iter1 != (Detector.ttcgr).end(); ++v_iter1) {   
    ttcgr_row["ttcgr_UID"].setValue <int> ((*v_iter1).ttcgr_UID);
    //    ttcgr_row["ttcgr_iovfk"].setValue <long long> ((*v_iter1).ttcgr_iovfk);
    ttcgr_row["ttcgr_iovfk"].setValue <long long> (key);
    ttcgr_row["Group"].setValue <int> ((*v_iter1).Group);
    ttcgr_row["DutyCycle"].setValue <int> ((*v_iter1).DutyCycle);
    ttcgr_row["EdgeSelect"].setValue <std::string> ((*v_iter1).EdgeSelect);
    ttcgr_row["finebx"].setValue <std::string> ((*v_iter1).finebx);
    ttcgr_row["finedx"].setValue <std::string> ((*v_iter1).finedx);
    ttcgr_row["TDMdelay"].setValue <std::string> ((*v_iter1).TDMdelay);
    ttcgr_row["TTCGRNAME"].setValue <std::string> ((*v_iter1).TTCGRNAME);
    //    ttcgr_row["ttcgr_tag"].setValue <std::string> ((*v_iter1).ttcgr_tag);
    ttcgr_row["ttcgr_tag"].setValue <std::string> (tag);
      
    ttcgr_cool_row["ttcgr_iovfk"].setValue <unsigned long long> (key);
    ttcgr_cool_row["tag"].setValue <std::string> (tag);
    cool::Record ttcgrRecord(ttcgr_rec, ttcgr_cool_row);
    ttcgr_fld->storeObject( since, until, ttcgrRecord, (*v_iter1).ttcgr_UID);
    ttcgr_bulk->processNextIteration();    
  }

  ttcgr_fld->flushStorageBuffer();	    
    
  ttcgr_bulk->flush();
  delete ttcgr_bulk;


  for (v_iter2 = (Detector.rod).begin(); v_iter2 != (Detector.rod).end(); ++v_iter2) {
    rod_row["rod_UID"].setValue <int> ((*v_iter2).rod_UID);
    rod_row["rod_iovfk"].setValue <long long> (key);
    rod_row["RODByteSwapping"].setValue <short> ((*v_iter2).RODByteSwapping);
    rod_row["RODPhysicalAddress"].setValue <int> ((*v_iter2).RODPhysicalAddress);
    rod_row["RODDetID"].setValue <int> ((*v_iter2).RODDetID);
    rod_row["RODVMESlot"].setValue <int> ((*v_iter2).RODVMEslot);
    rod_row["RODHalf"].setValue <int> ((*v_iter2).RODhalf);
    rod_row["EdgeSelect0"].setValue <int> ((*v_iter2).EdgeSelect0);
    rod_row["EdgeSelect1"].setValue <int> ((*v_iter2).EdgeSelect1);
    rod_row["EdgeSelect2"].setValue <int> ((*v_iter2).EdgeSelect2);
    rod_row["EdgeSelect3"].setValue <int> ((*v_iter2).EdgeSelect3);
    rod_row["GolDelay0"].setValue <int> ((*v_iter2).GolDelay0);
    rod_row["GolDelay1"].setValue <int> ((*v_iter2).GolDelay1);
    rod_row["GolDelay2"].setValue <int> ((*v_iter2).GolDelay2);
    rod_row["GolDelay3"].setValue <int> ((*v_iter2).GolDelay3);
    rod_row["RODStatus"].setValue <short> ((*v_iter2).RODStatus);
    rod_row["RODInputFragmentType"].setValue <std::string> ((*v_iter2).RODInputFragmentType);
    rod_row["RODName"].setValue <std::string> ((*v_iter2).RODName);
    rod_row["Comment"].setValue <std::string> ((*v_iter2).Comment);
    rod_row["rod_tag"].setValue <std::string> (tag);


    rod_cool_row["rod_iovfk"].setValue <unsigned long long> (key);
    rod_cool_row["tag"].setValue <std::string> (tag);
    cool::Record rodRecord(rod_rec, rod_cool_row);
    rod_fld->storeObject( since, until, rodRecord, (*v_iter2).rod_UID);

      
    rod_bulk->processNextIteration();
  }

  rod_fld->flushStorageBuffer();	    

  rod_bulk->flush();
  delete rod_bulk;

  for (v_iter3 = (Detector.dtmroc).begin(); v_iter3 != (Detector.dtmroc).end(); ++v_iter3) {
    roc_row["dtmroc_UID"].setValue <int> ((*v_iter3).dtmroc_UID);
    //    roc_row["dtmroc_iovfk"].setValue <long long> ((*v_iter3).dtmroc_iovfk);
    roc_row["dtmroc_iovfk"].setValue <long long> (key);
    roc_row["DTMROCByteSwapping"].setValue <short> ((*v_iter3).DTMROCByteSwapping);
    roc_row["DTMROCPhysicalAddress"].setValue <int> ((*v_iter3).DTMROCPhysicalAddress);
    roc_row["ChipID"].setValue <int> ((*v_iter3).ChipID);
    roc_row["ChipValid"].setValue <short> ((*v_iter3).Chip_Valid);
    roc_row["RODGroup"].setValue <short> ((*v_iter3).RODgroup);
    roc_row["RODInput"].setValue <int> ((*v_iter3).RODinput);
    roc_row["HW_Addr_FE"].setValue <short> ((*v_iter3).HW_addr_FE);
    roc_row["TTC_Line_FE"].setValue <short> ((*v_iter3).TTC_Line_FE);
    roc_row["Thresh0_Low_FE"].setValue <short> ((*v_iter3).Thresh0_Low_FE);
    roc_row["Thresh0_High_FE"].setValue <short> ((*v_iter3).Thresh0_High_FE);
    roc_row["Thresh1_Low_FE"].setValue <short> ((*v_iter3).Thresh1_Low_FE);
    roc_row["Thresh1_High_FE"].setValue <short> ((*v_iter3).Thresh1_High_FE);
    roc_row["VT_DAC0_FE"].setValue <short> ((*v_iter3).VT_DAC0_FE);
    roc_row["VT_DAC1_FE"].setValue <short> ((*v_iter3).VT_DAC1_FE);
    roc_row["Mask_FE"].setValue <int> ((*v_iter3).Mask_FE);
    roc_row["Pipe_Latency_FE"].setValue <short> ((*v_iter3).Pipe_Latency_FE);
    roc_row["DTMROCName"].setValue <std::string> ((*v_iter3).DTMROCName);
    roc_row["DTMROCInputFragmentType"].setValue <std::string> ((*v_iter3).DTMROCInputFragmentType);
    roc_row["Clock_FE"].setValue <std::string> ((*v_iter3).Clock_FE);
    roc_row["Comment"].setValue <std::string> ((*v_iter3).Comment);
    //    roc_row["roc_tag"].setValue <std::string> ((*v_iter3).roc_tag);
    roc_row["roc_tag"].setValue <std::string> (tag);

    dtmroc_cool_row["dtmroc_iovfk"].setValue <unsigned long long> (key);
    dtmroc_cool_row["tag"].setValue <std::string> (tag);
    cool::Record dtmrocRecord(dtmroc_rec, dtmroc_cool_row);
    dtmroc_fld->storeObject( since, until, dtmrocRecord, (*v_iter3).dtmroc_UID);
      
    roc_bulk->processNextIteration();    
  }

  dtmroc_fld->flushStorageBuffer();	    


  roc_bulk->flush();
  delete roc_bulk;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  
  m_session->transaction().commit();

}

//--------------------------------------------
///Gets the IoV key from OBSOLETE C_QUERIES
long long COOLCORALClient::GetIoVKey(long long since, long long until, const std::string& object){


  std::string QUERY_TABLE;
    QUERY_TABLE = "C_QUERIES";


  m_session->transaction().start(true);


  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(QUERY_TABLE).newQuery();

  query0->addToOutputList("Key" );
  query0->addToOutputList("Time" );


  std::string query_cond = "Object =:object ";
  query_cond += " AND ";
  query_cond += "Time >= :since";
  query_cond += " AND ";
  query_cond += "Time <= :until";

  coral::AttributeList query_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  query_condData.extend<std::string>( "object" );
  query_condData.extend<long long>( "since" );
  query_condData.extend<long long>( "until" );

  query0->setCondition( query_cond, query_condData);
  query0 ->addToOrderList("Time DESC");

  query_condData[0].data<std::string>() = object;
  query_condData[1].data<long long>() = since;
  query_condData[2].data<long long>() = until;

  coral::ICursor& cursor0 = query0->execute();

  long long key=0;

  if( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    key = row0[0].data<long long>();


  }
  delete query0;

  m_session->transaction().commit();

  return key;

}


//--------------------------------------------
/// Gets the IoV key based on time and tag
long long COOLCORALClient::GetIoVROC(int dtmroc_id,long long time, const std::string& tag){


  std::string QUERY_TABLE;
    QUERY_TABLE = "DTMROC_IOVS";

  m_session->transaction().start(true);


  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(QUERY_TABLE).newQuery();

  query0->addToOutputList("dtmroc_UID" );
  query0->addToOutputList("dtmroc_iovfk" );
  query0->addToOutputList("roc_tag" );


  std::string query_cond = "dtmroc_UID =:dtmroc_id ";
  query_cond += " AND roc_tag =:tag ";
  query_cond += " AND since_time <= :time";
  query_cond += " AND until_time >= :time";

  coral::AttributeList query_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  query_condData.extend<int>( "dtmroc_id" );
  query_condData.extend<long long>( "time" );
  query_condData.extend<std::string>( "tag" );

  query0->setCondition( query_cond, query_condData);

  query_condData[0].data<int>() = dtmroc_id;
  query_condData[1].data<long long>() = time;
  query_condData[2].data<std::string>() = tag;

  coral::ICursor& cursor0 = query0->execute();

  long long key = 0;

  if( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    key = row0[1].data<long long>();


  }
  delete query0;

  m_session->transaction().commit();

  return key;

}

//---------------------------------------------
/// Get the IoV key
long long COOLCORALClient::GetIoVROC(int dtmroc_id,long long since,long long until){


  std::string QUERY_TABLE;
    QUERY_TABLE = "BDTMROC";


  m_session->transaction().start(true);


  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(QUERY_TABLE).newQuery();

  query0->addToOutputList("dtmroc_UID" );
  query0->addToOutputList("dtmroc_iovfk" );


  std::string query_cond = "dtmroc_UID =:dtmroc_id ";
  query_cond += " AND ";
  query_cond += "dtmroc_iovfk >= :since";
  query_cond += " AND ";
  query_cond += "dtmroc_iovfk <= :until";

  coral::AttributeList query_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  query_condData.extend<int>( "dtmroc_id" );
  query_condData.extend<long long>( "since" );
  query_condData.extend<long long>( "until" );

  query0->setCondition( query_cond, query_condData);
  query0 ->addToOrderList("dtmroc_iovfk DESC");

  query_condData[0].data<int>() = dtmroc_id;
  query_condData[1].data<long long>() = since;
  query_condData[2].data<long long>() = until;

  coral::ICursor& cursor0 = query0->execute();

  long long key = 0;

  if( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
    key = row0[1].data<long long>();


  }
  delete query0;

  m_session->transaction().commit();

  return key;

}


//-------------------------------------------
/// Creates the IoV tables
void COOLCORALClient::createIoVKeyTables(){
  m_session->transaction().start();


  std::string TTCIOV_TABLE="TTC_IOVS";
  std::string TTCGRIOV_TABLE="TTCGR_IOVS";
  std::string RODIOV_TABLE="ROD_IOVS";
  std::string DTMROCIOV_TABLE="DTMROC_IOVS";

  std::cout << "Deleting the old barrel tables: " << std::endl;
  std::cout <<  TTCIOV_TABLE << std::endl;
  std::cout <<  TTCGRIOV_TABLE << std::endl;
  std::cout <<  RODIOV_TABLE << std::endl;
  std::cout <<  DTMROCIOV_TABLE << std::endl;


  m_session->nominalSchema().dropIfExistsTable(TTCIOV_TABLE);
  m_session->nominalSchema().dropIfExistsTable(TTCGRIOV_TABLE);
  m_session->nominalSchema().dropIfExistsTable(RODIOV_TABLE);
  m_session->nominalSchema().dropIfExistsTable(DTMROCIOV_TABLE);

//-------------------TRT TTCIOV Table

  std::cout << "Creating table: " << TTCIOV_TABLE << std::endl;


  // Create the table
  coral::TableDescription ttciov_columns( "SchemaDefinition_TTCIOV" );
  ttciov_columns.setName( TTCIOV_TABLE );
  ttciov_columns.insertColumn( "ttc_UID",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttciov_columns.insertColumn( "ttc_iovfk",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttciov_columns.insertColumn( "since_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttciov_columns.insertColumn( "until_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttciov_columns.insertColumn( "ttc_tag",
                coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  std::vector <std::string> tmp_strings;

    tmp_strings.clear();
    tmp_strings.push_back ("ttc_UID");
    ttciov_columns.createIndex ("h_ttc_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttc_iovfk");
    ttciov_columns.createIndex ("h_ttcio_idx", tmp_strings);


    ttciov_columns.setNotNullConstraint ("ttc_UID");


  // Create the actual table
  m_session->nominalSchema().createTable( ttciov_columns );

//-------------------TRT TTCGRIOV Table

  std::cout << "Creating table: " << TTCGRIOV_TABLE << std::endl;


  // Create the table
  coral::TableDescription ttcgriov_columns( "SchemaDefinition_TTCGRIOV" );
  ttcgriov_columns.setName( TTCGRIOV_TABLE );
  ttcgriov_columns.insertColumn( "ttcgr_UID",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  ttcgriov_columns.insertColumn( "ttcgr_iovfk",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttcgriov_columns.insertColumn( "since_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttcgriov_columns.insertColumn( "until_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  ttcgriov_columns.insertColumn( "ttcgr_tag",
                coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  //  std::vector <std::string> tmp_strings;

    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_UID");
    ttcgriov_columns.createIndex ("h_ttcgr_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("ttcgr_iovfk");
    ttcgriov_columns.createIndex ("h_ttcgrio_idx", tmp_strings);


    ttcgriov_columns.setNotNullConstraint ("ttcgr_UID");


  // Create the actual table
  m_session->nominalSchema().createTable( ttcgriov_columns );


//-------------------TRT RODIOV Table

  std::cout << "Creating table: " << RODIOV_TABLE << std::endl;


  // Create the table
  coral::TableDescription rodiov_columns( "SchemaDefinition_RODIOV" );
  rodiov_columns.setName( RODIOV_TABLE );
  rodiov_columns.insertColumn( "rod_UID",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rodiov_columns.insertColumn( "rod_iovfk",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rodiov_columns.insertColumn( "since_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rodiov_columns.insertColumn( "until_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rodiov_columns.insertColumn( "rod_tag",
                coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  //  std::vector <std::string> tmp_strings;

    tmp_strings.clear();
    tmp_strings.push_back ("rod_UID");
    rodiov_columns.createIndex ("h_rod_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("rod_iovfk");
    rodiov_columns.createIndex ("h_rodio_idx", tmp_strings);


    rodiov_columns.setNotNullConstraint ("rod_UID");


  // Create the actual table
  m_session->nominalSchema().createTable( rodiov_columns );




  //-------------------TRT DTMROCIOV Table

  std::cout << "Creating table: " << DTMROCIOV_TABLE << std::endl;


  // Create the table
  coral::TableDescription rociov_columns( "SchemaDefinition_DTMROCIOV" );
  rociov_columns.setName( DTMROCIOV_TABLE );
  rociov_columns.insertColumn( "dtmroc_UID",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ) );
  rociov_columns.insertColumn( "dtmroc_iovfk",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rociov_columns.insertColumn( "since_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rociov_columns.insertColumn( "until_time",
                coral::AttributeSpecification::typeNameForId( typeid(unsigned long long) ) );
  rociov_columns.insertColumn( "roc_tag",
                coral::AttributeSpecification::typeNameForId( typeid(std::string) ) );

  //  std::vector <std::string> tmp_strings;

    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_UID");
    rociov_columns.createIndex ("h_roc_idx", tmp_strings);

    tmp_strings.clear();
    tmp_strings.push_back ("dtmroc_iovfk");
    rociov_columns.createIndex ("h_rocio_idx", tmp_strings);


    rociov_columns.setNotNullConstraint ("dtmroc_UID");


  // Create the actual table
  m_session->nominalSchema().createTable( rociov_columns );


  m_session->transaction().commit();
  
}

//----------------------------------------------
/// Generates the GenIoVROCTable
void COOLCORALClient::GenIoVROCTable(){

  std::string DTMROC_TABLE="HIST_DTMROC";
  std::string DTMROCIOV_TABLE="DTMROC_IOVS";

  cool::ValidityKey since = 0;
  cool::ValidityKey until = cool::ValidityKeyMax;

  m_session->transaction().start();


  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  query0->addToOutputList("dtmroc_UID");
  query0->addToOutputList("dtmroc_iovfk");
  query0->addToOutputList("roc_tag");
  query0->addToOrderList("dtmroc_iovfk ASC"); 

//------------------------------
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle( DTMROCIOV_TABLE ).dataEditor();

    std::string updcondStr = "until_time = :max AND dtmroc_UID = :uid";
    updcondStr += " AND roc_tag = :tag";
    std::string updateAction = "until_time = :until";
    coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
    updateData.extend<long long>("max");
    updateData.extend<long long>("until");
    updateData.extend<int>("uid");
    updateData.extend<std::string>("tag");
    updateData[0].data <long long>()= cool::ValidityKeyMax; 

//------------------------------


  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROCIOV_TABLE ).dataEditor();

  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);

  coral::ICursor& cursor0 = query0->execute();

  int uid;
  long long key;
  std::string tag;

  //int result = -1;
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();

    uid = row0[0].data<int>();
    key = row0[1].data<long long>();
    tag = row0[2].data<std::string>();

    since=key;

    updateData[1].data <long long>()= key; 
    updateData[2].data <int>()= uid; 
    updateData[3].data <std::string>()= tag; 
    editor.updateRows( updateAction, updcondStr, updateData );

    roc_row["dtmroc_UID"].setValue <int> (uid);
    roc_row["dtmroc_iovfk"].setValue <long long> (key);
    roc_row["since_time"].setValue <long long> (since);
    roc_row["until_time"].setValue <long long> (until);
    roc_row["roc_tag"].setValue <std::string> (tag);
    roc_editor.insertRow(roc_row);


    //result = 1;
  }

  delete query0;

  m_session->transaction().commit();


}

//-----------------------------------------------
/// Updates IoVROCTable
void COOLCORALClient::UpdateIoVROCTable(int uid,long long time, const std::string& tag){

  std::string DTMROCIOV_TABLE="DTMROC_IOVS";

  //  cool::ValidityKey since = 0;
  cool::ValidityKey until = cool::ValidityKeyMax;

  m_session->transaction().start();

//------------------------------
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle( DTMROCIOV_TABLE ).dataEditor();

    std::string updcondStr = "until_time = :max AND dtmroc_UID = :uid";
    updcondStr += " AND roc_tag = :tag";
    std::string updateAction = "until_time = :until";
    coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
    updateData.extend<long long>("max");
    updateData.extend<long long>("until");
    updateData.extend<int>("uid");
    updateData.extend<std::string>("tag");
    updateData[0].data <long long>()= cool::ValidityKeyMax;
    updateData[1].data <long long>()= time;
    updateData[2].data <int>()= uid;
    updateData[3].data <std::string>()= tag;
    editor.updateRows( updateAction, updcondStr, updateData );

//------------------------------


  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROCIOV_TABLE ).dataEditor();

  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
    roc_editor.rowBuffer(roc_row);
    roc_row["dtmroc_UID"].setValue <int> (uid);
    roc_row["dtmroc_iovfk"].setValue <long long> (time);
    roc_row["since_time"].setValue <long long> (time);
    roc_row["until_time"].setValue <long long> (until);
    roc_row["roc_tag"].setValue <std::string> (tag);
    roc_editor.insertRow(roc_row);


  m_session->transaction().commit();
}


//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
int COOLCORALClient::GetTags (){ // Thread unsafe ctime is used.
    
  std::string TAG_TABLE="HISTORY";

  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  m_session->transaction().start(true);

  // GET all TAGS

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TAG_TABLE).newQuery();
    
  query0->addToOutputList("RUNNumber" ); 
  query0->addToOutputList("TAG" ); 
  query0->addToOutputList("Key" ); 
  query0->addToOutputList("Time" );
  query0->addToOrderList("Time ASC" ); 
  // to make it ordered must be an index


  coral::ICursor& cursor0 = query0->execute();

  std::cout << std::endl; 
  std::cout << "\t TAG\t Timestamp\t Run\t \t Date" << std::endl;
  std::cout << "\t -----------------------------------------------------------------"<< std::endl;
  std::cout << std::endl;
  while ( cursor0.next() ) {
    //  cursor0.next();
    const coral::AttributeList &row0 = cursor0.currentRow();
    
    time_t tempo = row0[2].data<long long>();


    char buf[32];
    std::cout << "** \t" 
	      << row0[1].data<std::string>() << "\t " 
      	      << row0[3].data<long long>() << "\t "
	      << row0[0].data<long long>() << "\t "
	      << ctime_r(&tempo, buf);
  }
  delete query0;

  std::cout << std::endl;

  
  

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  

  m_session->transaction().commit();
    
  return 1;
    
}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
Detector_t COOLCORALClient::GetHistValues(const std::string& tag,long long tstamp){
      
  std::string TAG_TABLE="HISTORY";
  
  long long key=0;

  std::string TTC_TABLE="HIST_TTC";
  std::string TTCGR_TABLE="HIST_TTCGR";
  std::string ROD_TABLE="HIST_ROD";
  std::string DTMROC_TABLE="HIST_DTMROC";


  TTC_t ttc;
  TTCGroup_t ttcgr;
  ROD_t rod;
  DTMROC_t dtmroc;
  
  Detector_t detector;


  m_session->transaction().start(true);

  // GET the key from the HISTORY table

  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(TAG_TABLE).newQuery();
    
  query00->addToOutputList("Key" ); 
  

  std::string cond = "TAG = :tag AND Time = :tstamp";

  coral::AttributeList condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  condData.extend<std::string>( "tag" );
  condData.extend<long long>( "tstamp" );

  query00->setCondition( cond, condData);

    condData[0].data<std::string>() = tag;
    condData[1].data<long long>() = tstamp;

  
  coral::ICursor& cursor00 = query00->execute();
    
  //  std::cout << "TAG\t Key \t Time" << std::endl;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();

    key = row00[0].data<long long>();
    std::cout << "Key = " << key <<  std::endl;
      
  }
  delete query00;


  

  // GET all TTCs

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0->addToOutputList("ttc_UID" ); 
  query0->addToOutputList("ttc_iovfk" ); 
  query0->addToOutputList("ByteSwapping" ); 
  query0->addToOutputList("PhysicalAddress" ); 
  query0->addToOutputList("DetID" ); 
  query0->addToOutputList("VMESlot" ); 
  query0->addToOutputList("Delay" ); 
  query0->addToOutputList("ArShaping" ); 
  query0->addToOutputList("SetSendID" ); 
  query0->addToOutputList("Name" ); 
  query0->addToOutputList("ClockSource" ); 
  query0->addToOutputList("InputFragmentType" ); 
  query0->addToOutputList("Comment" ); 
  query0->addToOutputList("ttc_tag" ); 
        

  std::string query_cond = "ttc_iovfk = :key";

  coral::AttributeList query_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  query_condData.extend<long long>( "key" );

  query0->setCondition( query_cond, query_condData);

  query_condData[0].data<long long>() = key;


  coral::ICursor& cursor0 = query0->execute();
    
  while ( cursor0.next() ) {
    const coral::AttributeList &row0 = cursor0.currentRow();
      
    ttc.ttc_UID = row0[0].data<int>();
    ttc.ttc_iovfk = row0[1].data<long long>();
    ttc.ByteSwapping = row0[2].data<short>();
    ttc.PhysicalAddress = row0[3].data<int>();
    ttc.DetID = row0[4].data<int>();
    ttc.VMEslot = row0[5].data<int>();
    ttc.Delay = row0[6].data<int>();
    ttc.ArShaping =  row0[7].data<short>();
    ttc.SetSendID = row0[8].data<short>();
    ttc.Name = row0[9].data<std::string>();
    ttc.ClockSource = row0[10].data<std::string>();
    ttc.InputFragmentType = row0[11].data<std::string>();
    ttc.Comment = row0[12].data<std::string>() ;
    ttc.ttc_tag = row0[13].data<std::string>() ;
    
    (detector.ttc).push_back(ttc);
      
  }
  delete query0;


  coral::IQuery* query1 = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();
    
  query1->addToOutputList("ttcgr_UID" ); 
  query1->addToOutputList("ttcgr_iovfk" ); 
  query1->addToOutputList("Group" ); 
  query1->addToOutputList("DutyCycle" ); 
  query1->addToOutputList("EdgeSelect" ); 
  query1->addToOutputList("finebx" ); 
  query1->addToOutputList("finedx" ); 
  query1->addToOutputList("TDMdelay" ); 
  query1->addToOutputList("TTCGRNAME" ); 
  query1->addToOutputList("ttcgr_tag" ); 
    
  query_cond = "ttcgr_iovfk = :key";

  query1->setCondition( query_cond, query_condData);


  coral::ICursor& cursor1 = query1->execute();
    
  while ( cursor1.next() ) {
    const coral::AttributeList &row1 = cursor1.currentRow();
    ttcgr.ttcgr_UID = row1[0].data<int>();
    ttcgr.ttcgr_iovfk = row1[1].data<long long>();
    ttcgr.Group = row1[2].data<int>();
    ttcgr.DutyCycle = row1[3].data<int>();
    ttcgr.EdgeSelect = row1[4].data<std::string>();
    ttcgr.finebx = row1[5].data<std::string>();
    ttcgr.finedx =  row1[6].data<std::string>();
    ttcgr.TDMdelay =  row1[7].data<std::string>();
    ttcgr.TTCGRNAME = row1[8].data<std::string>();
    ttcgr.ttcgr_tag = row1[9].data<std::string>();
    
    (detector.ttcgr).push_back(ttcgr);
  }
  delete query1;

  coral::IQuery* query2 = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
    
  query2->addToOutputList("rod_UID"); 
  query2->addToOutputList("rod_iovfk"); 
  query2->addToOutputList("RODByteSwapping"); 
  query2->addToOutputList("RODPhysicalAddress"); 
  query2->addToOutputList("RODDetID");
  query2->addToOutputList("RODVMESlot");
  query2->addToOutputList("RODHalf"); 
  query2->addToOutputList("EdgeSelect0");
  query2->addToOutputList("EdgeSelect1"); 
  query2->addToOutputList("EdgeSelect2"); 
  query2->addToOutputList("EdgeSelect3");
  query2->addToOutputList("GolDelay0"); 
  query2->addToOutputList("GolDelay1"); 
  query2->addToOutputList("GolDelay2"); 
  query2->addToOutputList("GolDelay3"); 
  query2->addToOutputList("RODStatus");
  query2->addToOutputList("RODInputFragmentType");
  query2->addToOutputList("RODName");
  query2->addToOutputList("Comment"); 
  query2->addToOutputList("rod_tag"); 
	    
  query_cond = "rod_iovfk = :key";

  query2->setCondition( query_cond, query_condData);
    
  coral::ICursor& cursor2 = query2->execute();
    
  while ( cursor2.next() ) {
    const coral::AttributeList &row2 = cursor2.currentRow();

    rod.rod_UID = row2[0].data<int>();
    rod.rod_iovfk = row2[1].data<long long>();
    rod.RODByteSwapping = row2[2].data<short>();
    rod.RODPhysicalAddress = row2[3].data<int>();
    rod.RODDetID = row2[4].data<int>();
    rod.RODVMEslot = row2[5].data<int>();
    rod.RODhalf = row2[6].data<int>();
    rod.EdgeSelect0 = row2[7].data<int>();
    rod.EdgeSelect1 = row2[8].data<int>();
    rod.EdgeSelect2 = row2[9].data<int>();
    rod.EdgeSelect3 = row2[10].data<int>();
    rod.GolDelay0 = row2[11].data<int>();
    rod.GolDelay1 = row2[12].data<int>();
    rod.GolDelay2 = row2[13].data<int>();
    rod.GolDelay3 = row2[14].data<int>();
    rod.RODStatus = row2[15].data<short>() ;
    rod.RODInputFragmentType = row2[16].data<std::string>();
    rod.RODName = row2[17].data<std::string>();
    rod.Comment = row2[18].data<std::string>();
    rod.rod_tag = row2[19].data<std::string>();
  
    (detector.rod).push_back(rod);
      
  }
  delete query2;


  coral::IQuery* query3 = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();
    
  query3->addToOutputList("dtmroc_UID"); 
  query3->addToOutputList("dtmroc_iovfk"); 
  query3->addToOutputList("DTMROCByteSwapping"); 
  query3->addToOutputList("DTMROCPhysicalAddress"); 
  query3->addToOutputList("ChipID");
  query3->addToOutputList("ChipValid");
  query3->addToOutputList("RODGroup"); 
  query3->addToOutputList("RODInput");
  query3->addToOutputList("HW_Addr_FE"); 
  query3->addToOutputList("TTC_Line_FE"); 
  query3->addToOutputList("Thresh0_Low_FE");
  query3->addToOutputList("Thresh0_High_FE");
  query3->addToOutputList("Thresh1_Low_FE");
  query3->addToOutputList("Thresh1_High_FE");
  query3->addToOutputList("VT_DAC0_FE"); 
  query3->addToOutputList("VT_DAC1_FE"); 
  query3->addToOutputList("Mask_FE"); 
  query3->addToOutputList("Pipe_Latency_FE"); 
  query3->addToOutputList("DTMROCName");
  query3->addToOutputList("DTMROCInputFragmentType");
  query3->addToOutputList("Clock_FE"); 
  query3->addToOutputList("Comment"); 
  query3->addToOutputList("roc_tag"); 
	    
    
  query_cond = "dtmroc_iovfk = :key";

  query3->setCondition( query_cond, query_condData);

  coral::ICursor& cursor3 = query3->execute();
    
  while ( cursor3.next() ) {
    const coral::AttributeList &row3 = cursor3.currentRow();

    dtmroc.dtmroc_UID    = row3[0].data<int>(); 
    dtmroc.dtmroc_iovfk    = row3[1].data<long long>(); 
    dtmroc.DTMROCByteSwapping    = row3[2].data<short>(); 
    dtmroc.DTMROCPhysicalAddress    = row3[3].data<int>(); 
    dtmroc.ChipID   = row3[4].data<int>();
    dtmroc.Chip_Valid   = row3[5].data<short>();
    dtmroc.RODgroup    = row3[6].data<short>(); 
    dtmroc.RODinput    = row3[7].data<int>();
    dtmroc.HW_addr_FE   = row3[8].data<short>(); 
    dtmroc.TTC_Line_FE   = row3[9].data<short>(); 
    dtmroc.Thresh0_Low_FE   = row3[10].data<short>();
    dtmroc.Thresh0_High_FE   = row3[11].data<short>();
    dtmroc.Thresh1_Low_FE   = row3[12].data<short>();
    dtmroc.Thresh1_High_FE    = row3[13].data<short>();
    dtmroc.VT_DAC0_FE   = row3[14].data<short>(); 
    dtmroc.VT_DAC1_FE   = row3[15].data<short>(); 
    dtmroc.Mask_FE   = row3[16].data<int>(); 
    dtmroc.Pipe_Latency_FE   = row3[17].data<short>(); 
    dtmroc.DTMROCName  = row3[18].data<std::string>();
    dtmroc.DTMROCInputFragmentType   = row3[19].data<std::string>();
    dtmroc.Clock_FE   = row3[20].data<std::string>(); 
    dtmroc.Comment   = row3[21].data<std::string>(); 
    dtmroc.roc_tag  = row3[22].data<std::string>(); 
    
    (detector.dtmroc).push_back(dtmroc);
      
  }
  delete query3;

  m_session->transaction().commit();
    
  return detector;
    
}

// ---------------------------------------------------
/// Fill Full Detector tables from History
/// Empty space
int COOLCORALClient::fillLastDetector(const Detector_t& Detector){


  m_session->transaction().start();
  
  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  std::string TTC_TABLE="BTTC";
  std::string TTCGR_TABLE="BTTCGR";
  std::string ROD_TABLE="BROD";
  std::string DTMROC_TABLE="BDTMROC";


  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);
	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,4);

  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,4);

  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);

  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);

  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);

  coral::IBulkOperation* roc_bulk= roc_editor.bulkInsert(roc_row,32);

  // now deleting

  std::string delcondStr = "ttc_UID > :uid ";
  coral::AttributeList deleteData ATLAS_THREAD_SAFE; // Not shared, so ok.
  deleteData.extend<int>("uid");
  deleteData[0].data<int>() = 0;
  ttc_editor.deleteRows(delcondStr,deleteData);

  delcondStr = "ttcgr_UID > :uid ";
  ttcgr_editor.deleteRows(delcondStr,deleteData);

  delcondStr = "rod_UID > :uid ";
  rod_editor.deleteRows(delcondStr,deleteData);

  delcondStr = "dtmroc_UID > :uid ";
  roc_editor.deleteRows(delcondStr,deleteData);

  



  // all iterators for filling

  std::vector<TTC_t>::const_iterator v_iter0;
  std::vector<TTCGroup_t>::const_iterator v_iter1;
  std::vector<ROD_t>::const_iterator v_iter2;
  std::vector<DTMROC_t>::const_iterator v_iter3;

  


  for (v_iter0 = (Detector.ttc).begin(); v_iter0 != (Detector.ttc).end(); ++v_iter0) {   
    ttc_row["ttc_UID"].setValue <int> ((*v_iter0).ttc_UID);
    ttc_row["ttc_iovfk"].setValue <long long> ((*v_iter0).ttc_iovfk);
    //    ttc_row["ttc_iovfk"].setValue <long long> (key);
    ttc_row["ByteSwapping"].setValue <short> ((*v_iter0).ByteSwapping);
    ttc_row["PhysicalAddress"].setValue <int> ((*v_iter0).PhysicalAddress);
    ttc_row["DetID"].setValue <int> ((*v_iter0).DetID);
    ttc_row["VMESlot"].setValue <int> ((*v_iter0).VMEslot);
    ttc_row["Delay"].setValue <int> ((*v_iter0).Delay);
    ttc_row["ArShaping"].setValue <short> ((*v_iter0).ArShaping);
    ttc_row["SetSendID"].setValue <short> ((*v_iter0).SetSendID);
    ttc_row["Name"].setValue <std::string> ((*v_iter0).Name);
    ttc_row["ClockSource"].setValue <std::string> ((*v_iter0).ClockSource);
    ttc_row["InputFragmentType"].setValue <std::string> ((*v_iter0).InputFragmentType);
    ttc_row["Comment"].setValue <std::string> ((*v_iter0).Comment);
    ttc_row["ttc_tag"].setValue <std::string> ((*v_iter0).ttc_tag);
    //    ttc_row["ttc_tag"].setValue <std::string> (tag);


    ttc_bulk->processNextIteration();   
  }
    

  ttc_bulk->flush();
  delete ttc_bulk;


  for (v_iter1 = (Detector.ttcgr).begin(); v_iter1 != (Detector.ttcgr).end(); ++v_iter1) {   
    ttcgr_row["ttcgr_UID"].setValue <int> ((*v_iter1).ttcgr_UID);
    ttcgr_row["ttcgr_iovfk"].setValue <long long> ((*v_iter1).ttcgr_iovfk);
    //    ttcgr_row["ttcgr_iovfk"].setValue <long long> (key);
    ttcgr_row["Group"].setValue <int> ((*v_iter1).Group);
    ttcgr_row["DutyCycle"].setValue <int> ((*v_iter1).DutyCycle);
    ttcgr_row["EdgeSelect"].setValue <std::string> ((*v_iter1).EdgeSelect);
    ttcgr_row["finebx"].setValue <std::string> ((*v_iter1).finebx);
    ttcgr_row["finedx"].setValue <std::string> ((*v_iter1).finedx);
    ttcgr_row["TDMdelay"].setValue <std::string> ((*v_iter1).TDMdelay);
    ttcgr_row["TTCGRNAME"].setValue <std::string> ((*v_iter1).TTCGRNAME);
    ttcgr_row["ttcgr_tag"].setValue <std::string> ((*v_iter1).ttcgr_tag);
    // ttcgr_row["ttcgr_tag"].setValue <std::string> (tag);

    ttcgr_bulk->processNextIteration();    
  }

    
  ttcgr_bulk->flush();
  delete ttcgr_bulk;


  for (v_iter2 = (Detector.rod).begin(); v_iter2 != (Detector.rod).end(); ++v_iter2) {
    rod_row["rod_UID"].setValue <int> ((*v_iter2).rod_UID);
    rod_row["rod_iovfk"].setValue <long long> ((*v_iter2).rod_iovfk);
    //    rod_row["rod_iovfk"].setValue <long long> (key);
    rod_row["RODByteSwapping"].setValue <short> ((*v_iter2).RODByteSwapping);
    rod_row["RODPhysicalAddress"].setValue <int> ((*v_iter2).RODPhysicalAddress);
    rod_row["RODDetID"].setValue <int> ((*v_iter2).RODDetID);
    rod_row["RODVMESlot"].setValue <int> ((*v_iter2).RODVMEslot);
    rod_row["RODHalf"].setValue <int> ((*v_iter2).RODhalf);
    rod_row["EdgeSelect0"].setValue <int> ((*v_iter2).EdgeSelect0);
    rod_row["EdgeSelect1"].setValue <int> ((*v_iter2).EdgeSelect1);
    rod_row["EdgeSelect2"].setValue <int> ((*v_iter2).EdgeSelect2);
    rod_row["EdgeSelect3"].setValue <int> ((*v_iter2).EdgeSelect3);
    rod_row["GolDelay0"].setValue <int> ((*v_iter2).GolDelay0);
    rod_row["GolDelay1"].setValue <int> ((*v_iter2).GolDelay1);
    rod_row["GolDelay2"].setValue <int> ((*v_iter2).GolDelay2);
    rod_row["GolDelay3"].setValue <int> ((*v_iter2).GolDelay3);
    rod_row["RODStatus"].setValue <short> ((*v_iter2).RODStatus);
    rod_row["RODInputFragmentType"].setValue <std::string> ((*v_iter2).RODInputFragmentType);
    rod_row["RODName"].setValue <std::string> ((*v_iter2).RODName);
    rod_row["Comment"].setValue <std::string> ((*v_iter2).Comment);
    rod_row["rod_tag"].setValue <std::string> ((*v_iter2).rod_tag);
    //    rod_row["rod_tag"].setValue <std::string> (tag);



      
    rod_bulk->processNextIteration();
  }



  rod_bulk->flush();
  delete rod_bulk;

  for (v_iter3 = (Detector.dtmroc).begin(); v_iter3 != (Detector.dtmroc).end(); ++v_iter3) {
    roc_row["dtmroc_UID"].setValue <int> ((*v_iter3).dtmroc_UID);
    roc_row["dtmroc_iovfk"].setValue <long long> ((*v_iter3).dtmroc_iovfk);
    //    roc_row["dtmroc_iovfk"].setValue <long long> (key);
    roc_row["DTMROCByteSwapping"].setValue <short> ((*v_iter3).DTMROCByteSwapping);
    roc_row["DTMROCPhysicalAddress"].setValue <int> ((*v_iter3).DTMROCPhysicalAddress);
    roc_row["ChipID"].setValue <int> ((*v_iter3).ChipID);
    roc_row["ChipValid"].setValue <short> ((*v_iter3).Chip_Valid);
    roc_row["RODGroup"].setValue <short> ((*v_iter3).RODgroup);
    roc_row["RODInput"].setValue <int> ((*v_iter3).RODinput);
    roc_row["HW_Addr_FE"].setValue <short> ((*v_iter3).HW_addr_FE);
    roc_row["TTC_Line_FE"].setValue <short> ((*v_iter3).TTC_Line_FE);
    roc_row["Thresh0_Low_FE"].setValue <short> ((*v_iter3).Thresh0_Low_FE);
    roc_row["Thresh0_High_FE"].setValue <short> ((*v_iter3).Thresh0_High_FE);
    roc_row["Thresh1_Low_FE"].setValue <short> ((*v_iter3).Thresh1_Low_FE);
    roc_row["Thresh1_High_FE"].setValue <short> ((*v_iter3).Thresh1_High_FE);
    roc_row["VT_DAC0_FE"].setValue <short> ((*v_iter3).VT_DAC0_FE);
    roc_row["VT_DAC1_FE"].setValue <short> ((*v_iter3).VT_DAC1_FE);
    roc_row["Mask_FE"].setValue <int> ((*v_iter3).Mask_FE);
    roc_row["Pipe_Latency_FE"].setValue <short> ((*v_iter3).Pipe_Latency_FE);
    roc_row["DTMROCName"].setValue <std::string> ((*v_iter3).DTMROCName);
    roc_row["DTMROCInputFragmentType"].setValue <std::string> ((*v_iter3).DTMROCInputFragmentType);
    roc_row["Clock_FE"].setValue <std::string> ((*v_iter3).Clock_FE);
    roc_row["Comment"].setValue <std::string> ((*v_iter3).Comment);
    roc_row["roc_tag"].setValue <std::string> ((*v_iter3).roc_tag);
    //roc_row["roc_tag"].setValue <std::string> (tag);

      
    roc_bulk->processNextIteration();    
  }


  roc_bulk->flush();
  delete roc_bulk;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  
  m_session->transaction().commit();

  return 1;
}

// ---------------------------------------------------
/// Clear Full Detector tables 
/// Empty space
void COOLCORALClient::clearCurrentTables(){


  m_session->transaction().start();
  
  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  std::string TTC_TABLE="BTTC";
  std::string TTCGR_TABLE="BTTCGR";
  std::string ROD_TABLE="BROD";
  std::string DTMROC_TABLE="BDTMROC";
  std::string CONNECT_TABLE="BCONNECT";


  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
	
  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
	
  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();

  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();

  coral::ITableDataEditor& conn_editor = m_session->nominalSchema().tableHandle(CONNECT_TABLE ).dataEditor();

  // now deleting

  std::string delcondStr = "ttc_UID > :uid";
  coral::AttributeList deleteData ATLAS_THREAD_SAFE; // Not shared, so ok.
  deleteData.extend<int>("uid");
  deleteData[0].data<int>() = 0;
  long deletedrows = ttc_editor.deleteRows(delcondStr,deleteData);
  std::cout << "TTC deleted rows = " << deletedrows << std::endl;
  delcondStr = "ttcgr_UID > :uid";
  deletedrows = ttcgr_editor.deleteRows(delcondStr,deleteData);
  std::cout << "TTCGR deleted rows = " << deletedrows << std::endl;
  delcondStr = "rod_UID > :uid";
  deletedrows = rod_editor.deleteRows(delcondStr,deleteData);
  std::cout << "ROD deleted rows = " << deletedrows << std::endl;
  delcondStr = "dtmroc_UID > :uid ";
  deletedrows = roc_editor.deleteRows(delcondStr,deleteData);
  std::cout << "DTMROC deleted rows = " << deletedrows << std::endl;
  delcondStr = "Partition > :uid";
  deletedrows = conn_editor.deleteRows(delcondStr,deleteData);
  std::cout << "CONNECTON deleted rows = " << deletedrows << std::endl;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  
  m_session->transaction().commit();

}

// ---------------------------------------------------
/// Fill Full Detector tables 
/// This method fills both CORAL tables and COOL folders
/// The ValidityKey for since is the time of insertion
/// with a precision of the second. That should be improved
/// in the next release.
/// Empty space
void COOLCORALClient::fillHistory(const Detector_t& Detector, const std::string& tag){



  m_session->transaction().start();

  struct timeval nunc_time;
  struct timeval start_time;
  struct timeval end_time;
  //  unsigned long long nunc_usecs;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  std::string TTC_TABLE="HIST_TTC";
  std::string TTCGR_TABLE="HIST_TTCGR";
  std::string ROD_TABLE="HIST_ROD";
  std::string DTMROC_TABLE="HIST_DTMROC";

  std::string TTCIOV_TABLE="TTC_IOVS";
  std::string TTCGRIOV_TABLE="TTCGR_IOVS";
  std::string RODIOV_TABLE="ROD_IOVS";
  std::string DTMROCIOV_TABLE="DTMROC_IOVS";


  coral::ITableDataEditor& ttc_editor = m_session->nominalSchema().tableHandle(TTC_TABLE ).dataEditor();
  coral::AttributeList ttc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttc_editor.rowBuffer(ttc_row);
	
  coral::IBulkOperation* ttc_bulk= ttc_editor.bulkInsert(ttc_row,16);

  coral::ITableDataEditor& ttciov_editor = m_session->nominalSchema().tableHandle(TTCIOV_TABLE ).dataEditor();
  coral::AttributeList ttciov_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttciov_editor.rowBuffer(ttciov_row);
	
  coral::IBulkOperation* ttciov_bulk= ttciov_editor.bulkInsert(ttciov_row,16);

  coral::ITableDataEditor& ttcgr_editor = m_session->nominalSchema().tableHandle(TTCGR_TABLE ).dataEditor();
  coral::AttributeList ttcgr_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgr_editor.rowBuffer(ttcgr_row);
	
  coral::IBulkOperation* ttcgr_bulk= ttcgr_editor.bulkInsert(ttcgr_row,4);

  coral::ITableDataEditor& ttcgriov_editor = m_session->nominalSchema().tableHandle(TTCGRIOV_TABLE ).dataEditor();
  coral::AttributeList ttcgriov_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  ttcgriov_editor.rowBuffer(ttcgriov_row);
	
  coral::IBulkOperation* ttcgriov_bulk= ttcgriov_editor.bulkInsert(ttcgriov_row,4);

  coral::ITableDataEditor& rod_editor = m_session->nominalSchema().tableHandle(ROD_TABLE ).dataEditor();
  coral::AttributeList rod_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rod_editor.rowBuffer(rod_row);

  coral::IBulkOperation* rod_bulk= rod_editor.bulkInsert(rod_row,32);

  coral::ITableDataEditor& rodiov_editor = m_session->nominalSchema().tableHandle(RODIOV_TABLE ).dataEditor();
  coral::AttributeList rodiov_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rodiov_editor.rowBuffer(rodiov_row);

  coral::IBulkOperation* rodiov_bulk= rodiov_editor.bulkInsert(rodiov_row,32);

  coral::ITableDataEditor& roc_editor = m_session->nominalSchema().tableHandle(DTMROC_TABLE ).dataEditor();
  coral::AttributeList roc_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  roc_editor.rowBuffer(roc_row);

  coral::IBulkOperation* roc_bulk= roc_editor.bulkInsert(roc_row,32);

  coral::ITableDataEditor& rociov_editor = m_session->nominalSchema().tableHandle(DTMROCIOV_TABLE ).dataEditor();
  coral::AttributeList rociov_row ATLAS_THREAD_SAFE; // Not shared, so ok.
  rociov_editor.rowBuffer(rociov_row);

  coral::IBulkOperation* rociov_bulk= rociov_editor.bulkInsert(rociov_row,32);

  // all iterators for filling

  std::vector<TTC_t>::const_iterator v_iter0;
  std::vector<TTCGroup_t>::const_iterator v_iter1;
  std::vector<ROD_t>::const_iterator v_iter2;
  std::vector<DTMROC_t>::const_iterator v_iter3;



  // get a key for this operation
  // the key when creating an history entry
  // from the last table will be the same for all
  // Update the key and use it now

  gettimeofday(&nunc_time, NULL);

  long long key = nunc_time.tv_sec;

  if(m_verbose) std::cout << "Key = " << key << std::endl;


  // Update the key and use it now
  coral::ITableDataEditor& cqeditor = m_session->nominalSchema().tableHandle( "HISTORY" ).dataEditor();

  coral::AttributeList rowBuffer1 ATLAS_THREAD_SAFE; // Not shared, so ok.

  rowBuffer1.extend<long long>( "RUNNumber" );
  rowBuffer1.extend<std::string>( "TAG" );
  rowBuffer1.extend<long long>( "Key" );
  rowBuffer1.extend<long long>( "Time" );

  long long& cqrunno = rowBuffer1[ "RUNNumber" ].data<long long>(); 
  std::string& cqobj = rowBuffer1[ "TAG" ].data<std::string>();
  long long& cqkey = rowBuffer1[ "Key" ].data<long long>(); 
  long long& cqtime = rowBuffer1[ "Time" ].data<long long>(); 

//   gettimeofday(&nunc_time, NULL);
//   //  nunc_usecs = (nunc_time.tv_sec) * 1000000 + (nunc_time.tv_usec);
  //nunc_usecs = (nunc_time.tv_sec);

  gettimeofday(&nunc_time, NULL);

  long long run = nunc_time.tv_sec;
  
  cqrunno = run;
  cqobj = tag;
  cqkey = key;
  cqtime = key;

  cqeditor.insertRow( rowBuffer1 );

  cool::ValidityKey until = cool::ValidityKeyMax;

  /////////

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();

  query0->addToOutputList("ttc_UID");
  query0->addToOutputList("ttc_iovfk");
  query0->addToOutputList("ttc_tag");
  query0->addToOrderList("ttc_iovfk ASC"); 

  std::string updcondStr = "until_time = :max AND ttc_UID = :uid";
  updcondStr += " AND ttc_tag = :tag";
  std::string updateAction = "until_time = :until";
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, so ok.
  updateData.extend<long long>("max");
  updateData.extend<long long>("until");
  updateData.extend<int>("uid");
  updateData.extend<std::string>("tag");
  updateData[0].data <long long>()= cool::ValidityKeyMax; 
  


  for (v_iter0 = (Detector.ttc).begin(); v_iter0 != (Detector.ttc).end(); ++v_iter0) {   
    ttc_row["ttc_UID"].setValue <int> ((*v_iter0).ttc_UID);
    //    ttc_row["ttc_iovfk"].setValue <long long> ((*v_iter0).ttc_iovfk);
    ttc_row["ttc_iovfk"].setValue <long long> (key);
    ttc_row["ByteSwapping"].setValue <short> ((*v_iter0).ByteSwapping);
    ttc_row["PhysicalAddress"].setValue <int> ((*v_iter0).PhysicalAddress);
    ttc_row["DetID"].setValue <int> ((*v_iter0).DetID);
    ttc_row["VMESlot"].setValue <int> ((*v_iter0).VMEslot);
    ttc_row["Delay"].setValue <int> ((*v_iter0).Delay);
    ttc_row["ArShaping"].setValue <short> ((*v_iter0).ArShaping);
    ttc_row["SetSendID"].setValue <short> ((*v_iter0).SetSendID);
    ttc_row["Name"].setValue <std::string> ((*v_iter0).Name);
    ttc_row["ClockSource"].setValue <std::string> ((*v_iter0).ClockSource);
    ttc_row["InputFragmentType"].setValue <std::string> ((*v_iter0).InputFragmentType);
    ttc_row["Comment"].setValue <std::string> ((*v_iter0).Comment);
    //    ttc_row["ttc_tag"].setValue <std::string> ((*v_iter0).ttc_tag);
    ttc_row["ttc_tag"].setValue <std::string> (tag);


    updateData[1].data <long long>() = key;
    updateData[2].data <int>() = (*v_iter0).ttc_UID;
    updateData[3].data <std::string>() = tag;
    ttciov_editor.updateRows(updateAction, updcondStr, updateData);

    ttciov_row["ttc_UID"].setValue <int> ((*v_iter0).ttc_UID);
    ttciov_row["ttc_iovfk"].setValue <long long> (key);
    ttciov_row["since_time"].setValue <long long> (key);
    ttciov_row["until_time"].setValue <long long> (until);
    ttciov_row["ttc_tag"].setValue <std::string> (tag);
    
    ttc_bulk->processNextIteration();
    ttciov_bulk->processNextIteration();
  }
    

  ttc_bulk->flush();
  delete ttc_bulk;

  ttciov_bulk->flush();
  delete ttciov_bulk;


  updcondStr = "until_time = :max AND ttcgr_UID = :uid";
  updcondStr += " AND ttcgr_tag = :tag";


  for (v_iter1 = (Detector.ttcgr).begin(); v_iter1 != (Detector.ttcgr).end(); ++v_iter1) {   
    ttcgr_row["ttcgr_UID"].setValue <int> ((*v_iter1).ttcgr_UID);
    //    ttcgr_row["ttcgr_iovfk"].setValue <long long> ((*v_iter1).ttcgr_iovfk);
    ttcgr_row["ttcgr_iovfk"].setValue <long long> (key);
    ttcgr_row["Group"].setValue <int> ((*v_iter1).Group);
    ttcgr_row["DutyCycle"].setValue <int> ((*v_iter1).DutyCycle);
    ttcgr_row["EdgeSelect"].setValue <std::string> ((*v_iter1).EdgeSelect);
    ttcgr_row["finebx"].setValue <std::string> ((*v_iter1).finebx);
    ttcgr_row["finedx"].setValue <std::string> ((*v_iter1).finedx);
    ttcgr_row["TDMdelay"].setValue <std::string> ((*v_iter1).TDMdelay);
    ttcgr_row["TTCGRNAME"].setValue <std::string> ((*v_iter1).TTCGRNAME);
    //    ttcgr_row["ttcgr_tag"].setValue <std::string> ((*v_iter1).ttcgr_tag);
    ttcgr_row["ttcgr_tag"].setValue <std::string> (tag);

    updateData[1].data <long long>() = key;
    updateData[2].data <int>() = (*v_iter1).ttcgr_UID;
    updateData[3].data <std::string>() = tag;
    ttcgriov_editor.updateRows(updateAction, updcondStr, updateData);

    ttcgriov_row["ttcgr_UID"].setValue <int> ((*v_iter1).ttcgr_UID);
    ttcgriov_row["ttcgr_iovfk"].setValue <long long> (key);
    ttcgriov_row["since_time"].setValue <long long> (key);
    ttcgriov_row["until_time"].setValue <long long> (until);
    ttcgriov_row["ttcgr_tag"].setValue <std::string> (tag);
    

      
    ttcgr_bulk->processNextIteration();    
    ttcgriov_bulk->processNextIteration();
  }

    
  ttcgr_bulk->flush();
  delete ttcgr_bulk;
  ttcgriov_bulk->flush();
  delete ttcgriov_bulk;


  updcondStr = "until_time = :max AND rod_UID = :uid";
  updcondStr += " AND rod_tag = :tag";


  for (v_iter2 = (Detector.rod).begin(); v_iter2 != (Detector.rod).end(); ++v_iter2) {
    rod_row["rod_UID"].setValue <int> ((*v_iter2).rod_UID);
    //    rod_row["rod_iovfk"].setValue <long long> ((*v_iter2).rod_iovfk);
    rod_row["rod_iovfk"].setValue <long long> (key);
    rod_row["RODByteSwapping"].setValue <short> ((*v_iter2).RODByteSwapping);
    rod_row["RODPhysicalAddress"].setValue <int> ((*v_iter2).RODPhysicalAddress);
    rod_row["RODDetID"].setValue <int> ((*v_iter2).RODDetID);
    rod_row["RODVMESlot"].setValue <int> ((*v_iter2).RODVMEslot);
    rod_row["RODHalf"].setValue <int> ((*v_iter2).RODhalf);
    rod_row["EdgeSelect0"].setValue <int> ((*v_iter2).EdgeSelect0);
    rod_row["EdgeSelect1"].setValue <int> ((*v_iter2).EdgeSelect1);
    rod_row["EdgeSelect2"].setValue <int> ((*v_iter2).EdgeSelect2);
    rod_row["EdgeSelect3"].setValue <int> ((*v_iter2).EdgeSelect3);
    rod_row["GolDelay0"].setValue <int> ((*v_iter2).GolDelay0);
    rod_row["GolDelay1"].setValue <int> ((*v_iter2).GolDelay1);
    rod_row["GolDelay2"].setValue <int> ((*v_iter2).GolDelay2);
    rod_row["GolDelay3"].setValue <int> ((*v_iter2).GolDelay3);
    rod_row["RODStatus"].setValue <short> ((*v_iter2).RODStatus);
    rod_row["RODInputFragmentType"].setValue <std::string> ((*v_iter2).RODInputFragmentType);
    rod_row["RODName"].setValue <std::string> ((*v_iter2).RODName);
    rod_row["Comment"].setValue <std::string> ((*v_iter2).Comment);
    //    rod_row["rod_tag"].setValue <std::string> ((*v_iter2).rod_tag);
    rod_row["rod_tag"].setValue <std::string> (tag);

    updateData[1].data <long long>() = key;
    updateData[2].data <int>() = (*v_iter2).rod_UID;
    updateData[3].data <std::string>() = tag;
    rodiov_editor.updateRows(updateAction, updcondStr, updateData);

    rodiov_row["rod_UID"].setValue <int> ((*v_iter2).rod_UID);
    rodiov_row["rod_iovfk"].setValue <long long> (key);
    rodiov_row["since_time"].setValue <long long> (key);
    rodiov_row["until_time"].setValue <long long> (until);
    rodiov_row["rod_tag"].setValue <std::string> (tag);
    

      
    rod_bulk->processNextIteration();    
    rodiov_bulk->processNextIteration();
  }


  rod_bulk->flush();
  delete rod_bulk;
  rodiov_bulk->flush();
  delete rodiov_bulk;


  updcondStr = "until_time = :max AND dtmroc_UID = :uid";
  updcondStr += " AND roc_tag = :tag";


  for (v_iter3 = (Detector.dtmroc).begin(); v_iter3 != (Detector.dtmroc).end(); ++v_iter3) {
    roc_row["dtmroc_UID"].setValue <int> ((*v_iter3).dtmroc_UID);
    //    roc_row["dtmroc_iovfk"].setValue <long long> ((*v_iter3).dtmroc_iovfk);
    roc_row["dtmroc_iovfk"].setValue <long long> (key);
    roc_row["DTMROCByteSwapping"].setValue <short> ((*v_iter3).DTMROCByteSwapping);
    roc_row["DTMROCPhysicalAddress"].setValue <int> ((*v_iter3).DTMROCPhysicalAddress);
    roc_row["ChipID"].setValue <int> ((*v_iter3).ChipID);
    roc_row["ChipValid"].setValue <short> ((*v_iter3).Chip_Valid);
    roc_row["RODGroup"].setValue <short> ((*v_iter3).RODgroup);
    roc_row["RODInput"].setValue <int> ((*v_iter3).RODinput);
    roc_row["HW_Addr_FE"].setValue <short> ((*v_iter3).HW_addr_FE);
    roc_row["TTC_Line_FE"].setValue <short> ((*v_iter3).TTC_Line_FE);
    roc_row["Thresh0_Low_FE"].setValue <short> ((*v_iter3).Thresh0_Low_FE);
    roc_row["Thresh0_High_FE"].setValue <short> ((*v_iter3).Thresh0_High_FE);
    roc_row["Thresh1_Low_FE"].setValue <short> ((*v_iter3).Thresh1_Low_FE);
    roc_row["Thresh1_High_FE"].setValue <short> ((*v_iter3).Thresh1_High_FE);
    roc_row["VT_DAC0_FE"].setValue <short> ((*v_iter3).VT_DAC0_FE);
    roc_row["VT_DAC1_FE"].setValue <short> ((*v_iter3).VT_DAC1_FE);
    roc_row["Mask_FE"].setValue <int> ((*v_iter3).Mask_FE);
    roc_row["Pipe_Latency_FE"].setValue <short> ((*v_iter3).Pipe_Latency_FE);
    roc_row["DTMROCName"].setValue <std::string> ((*v_iter3).DTMROCName);
    roc_row["DTMROCInputFragmentType"].setValue <std::string> ((*v_iter3).DTMROCInputFragmentType);
    roc_row["Clock_FE"].setValue <std::string> ((*v_iter3).Clock_FE);
    roc_row["Comment"].setValue <std::string> ((*v_iter3).Comment);
    //    roc_row["roc_tag"].setValue <std::string> ((*v_iter3).roc_tag);
    roc_row["roc_tag"].setValue <std::string> (tag);


    updateData[1].data <long long>() = key;
    updateData[2].data <int>() = (*v_iter3).dtmroc_UID;
    updateData[3].data <std::string>() = tag;
    rociov_editor.updateRows(updateAction, updcondStr, updateData);

    rociov_row["dtmroc_UID"].setValue <int> ((*v_iter3).dtmroc_UID);
    rociov_row["dtmroc_iovfk"].setValue <long long> (key);
    rociov_row["since_time"].setValue <long long> (key);
    rociov_row["until_time"].setValue <long long> (until);
    rociov_row["roc_tag"].setValue <std::string> (tag);
    

      
    roc_bulk->processNextIteration();    
    rociov_bulk->processNextIteration();
  }



  roc_bulk->flush();
  delete roc_bulk;
  rociov_bulk->flush();
  delete rociov_bulk;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  
  m_session->transaction().commit();

}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
void
COOLCORALClient::CompHistValues(const std::string& tag1,long long tstamp1, const std::string& tag2 , long long tstamp2, std::vector<Detector_t>& detectors ){
      
  std::string TAG_TABLE="HISTORY";
  
  long long key1=0;
  long long key2=0;

  std::string TTC_TABLE="HIST_TTC";
  std::string TTCGR_TABLE="HIST_TTCGR";
  std::string ROD_TABLE="HIST_ROD";
  std::string DTMROC_TABLE="HIST_DTMROC";

  // resize det vector and set refs to the dets
  detectors.resize(2);

  TTC_t ttc1;
  TTCGroup_t ttcgr1;
  ROD_t rod1;
  DTMROC_t dtmroc1;
  
  Detector_t& detector1 = detectors[0];

  TTC_t ttc2;
  TTCGroup_t ttcgr2;
  ROD_t rod2;
  DTMROC_t dtmroc2;
  
  Detector_t& detector2 = detectors[1];

  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);

  m_session->transaction().start(true);

  // GET the key from the HISTORY table

  coral::IQuery* query00 = m_session->nominalSchema().tableHandle(TAG_TABLE).newQuery();
    
  query00->addToOutputList("Key" ); 

  coral::IQuery* query01 = m_session->nominalSchema().tableHandle(TAG_TABLE).newQuery();
    
  query01->addToOutputList("Key" ); 
  

  std::string cond = "TAG = :tag AND Time = :tstamp";

  coral::AttributeList condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  condData.extend<std::string>( "tag" );
  condData.extend<long long>( "tstamp" );

  query00->setCondition( cond, condData);
  query01->setCondition( cond, condData);


  if (tag1!=" ") 
    {
      condData[0].data<std::string>() = tag1;
    }
  else 
    {
      std::cout << "*** ERROR: Empty TAG: " << tag1 << std::endl;
      detectors.clear();
      return;
    }
  if (tstamp1 > 0)
    {
      condData[1].data<long long>() = tstamp1;
    }
  else
    {
      std::cout << "*** ERROR: Invalid Time stamp: " <<  tstamp1 << std::endl;
      detectors.clear();
      return;
    }


  
  coral::ICursor& cursor00 = query00->execute();
    
  int nlines = 0;
  while ( cursor00.next() ) {
    const coral::AttributeList &row00 = cursor00.currentRow();

    key1 = row00[0].data<long long>();
    nlines++;
  }

  delete query00;
  
  if (nlines<1) {
      detectors.clear();
      return;
  }


  if (tag2!=" ") 
    {
      condData[0].data<std::string>() = tag2;
    }
  else 
    {
      std::cout << "*** ERROR: Empty TAG: " << tag2 << std::endl;
      detectors.clear();
      return;
    }
  if (tstamp2 > 0)
    {
      condData[1].data<long long>() = tstamp2;
    }
  else
    {
      std::cout << "*** ERROR: Invalid Time stamp: " << tstamp2 << std::endl;
      detectors.clear();
      return;
    }



  nlines = 0;
  
  coral::ICursor& cursor01 = query01->execute();
    
  //  std::cout << "TAG\t Key \t Time" << std::endl;
  while ( cursor01.next() ) {
    const coral::AttributeList &row01 = cursor01.currentRow();

    key2 = row01[0].data<long long>();
    //    std::cout << "Key = " << key2 <<  std::endl;
    nlines++;
  }

  delete query01;

  if (nlines<1) {
      detectors.clear();
      return;
  }

  

  // GET all TTCs

  coral::IQuery* query0a = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0a->addToOutputList("ttc_UID" ); 
  query0a->addToOutputList("ttc_iovfk" ); 
  query0a->addToOutputList("ByteSwapping" ); 
  query0a->addToOutputList("PhysicalAddress" ); 
  query0a->addToOutputList("DetID" ); 
  query0a->addToOutputList("VMESlot" ); 
  query0a->addToOutputList("Delay" ); 
  query0a->addToOutputList("ArShaping" ); 
  query0a->addToOutputList("SetSendID" ); 
  query0a->addToOutputList("Name" ); 
  query0a->addToOutputList("ClockSource" ); 
  query0a->addToOutputList("InputFragmentType" ); 
  query0a->addToOutputList("Comment" ); 
  query0a->addToOutputList("ttc_tag" ); 

  query0a->addToOrderList("ttc_UID ASC"); 


  coral::IQuery* query0b = m_session->nominalSchema().tableHandle(TTC_TABLE).newQuery();
    
  query0b->addToOutputList("ttc_UID" ); 
  query0b->addToOutputList("ttc_iovfk" ); 
  query0b->addToOutputList("ByteSwapping" ); 
  query0b->addToOutputList("PhysicalAddress" ); 
  query0b->addToOutputList("DetID" ); 
  query0b->addToOutputList("VMESlot" ); 
  query0b->addToOutputList("Delay" ); 
  query0b->addToOutputList("ArShaping" ); 
  query0b->addToOutputList("SetSendID" ); 
  query0b->addToOutputList("Name" ); 
  query0b->addToOutputList("ClockSource" ); 
  query0b->addToOutputList("InputFragmentType" ); 
  query0b->addToOutputList("Comment" ); 
  query0b->addToOutputList("ttc_tag" ); 

  query0b->addToOrderList("ttc_UID ASC"); 
        

  std::string query_cond = "ttc_iovfk = :key";

  coral::AttributeList query_condData ATLAS_THREAD_SAFE; // Not shared, so ok.
  query_condData.extend<long long>( "key" );

  query0a->setCondition( query_cond, query_condData);
  query0b->setCondition( query_cond, query_condData);

  query_condData[0].data<long long>() = key1;
  coral::ICursor& cursor0a = query0a->execute();
    
  while ( cursor0a.next() ) {
    const coral::AttributeList &row0a = cursor0a.currentRow();
      
    ttc1.ttc_UID = row0a[0].data<int>();
    ttc1.ttc_iovfk = row0a[1].data<long long>();
    ttc1.ByteSwapping = row0a[2].data<short>();
    ttc1.PhysicalAddress = row0a[3].data<int>();
    ttc1.DetID = row0a[4].data<int>();
    ttc1.VMEslot = row0a[5].data<int>();
    ttc1.Delay = row0a[6].data<int>();
    ttc1.ArShaping =  row0a[7].data<short>();
    ttc1.SetSendID = row0a[8].data<short>();
    ttc1.Name = row0a[9].data<std::string>();
    ttc1.ClockSource = row0a[10].data<std::string>();
    ttc1.InputFragmentType = row0a[11].data<std::string>();
    ttc1.Comment = row0a[12].data<std::string>() ;
    ttc1.ttc_tag = row0a[13].data<std::string>() ;
    
    (detector1.ttc).push_back(ttc1);
      
  }

  delete query0a;

  query_condData[0].data<long long>() = key2;
  coral::ICursor& cursor0b = query0b->execute();
    
  while ( cursor0b.next() ) {
    const coral::AttributeList &row0b = cursor0b.currentRow();
      
    ttc2.ttc_UID = row0b[0].data<int>();
    ttc2.ttc_iovfk = row0b[1].data<long long>();
    ttc2.ByteSwapping = row0b[2].data<short>();
    ttc2.PhysicalAddress = row0b[3].data<int>();
    ttc2.DetID = row0b[4].data<int>();
    ttc2.VMEslot = row0b[5].data<int>();
    ttc2.Delay = row0b[6].data<int>();
    ttc2.ArShaping =  row0b[7].data<short>();
    ttc2.SetSendID = row0b[8].data<short>();
    ttc2.Name = row0b[9].data<std::string>();
    ttc2.ClockSource = row0b[10].data<std::string>();
    ttc2.InputFragmentType = row0b[11].data<std::string>();
    ttc2.Comment = row0b[12].data<std::string>() ;
    ttc2.ttc_tag = row0b[13].data<std::string>() ;
    
    (detector2.ttc).push_back(ttc2);
      
  }
  delete query0b;


  coral::IQuery* query1a = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();

  coral::IQuery* query1b = m_session->nominalSchema().tableHandle(TTCGR_TABLE).newQuery();
    
  query1a->addToOutputList("ttcgr_UID" ); 
  query1a->addToOutputList("ttcgr_iovfk" ); 
  query1a->addToOutputList("Group" ); 
  query1a->addToOutputList("DutyCycle" ); 
  query1a->addToOutputList("EdgeSelect" ); 
  query1a->addToOutputList("finebx" ); 
  query1a->addToOutputList("finedx" ); 
  query1a->addToOutputList("TDMdelay" ); 
  query1a->addToOutputList("TTCGRNAME" ); 
  query1a->addToOutputList("ttcgr_tag" ); 

  query1a->addToOrderList("ttcgr_UID ASC"); 

  query1b->addToOutputList("ttcgr_UID" ); 
  query1b->addToOutputList("ttcgr_iovfk" ); 
  query1b->addToOutputList("Group" ); 
  query1b->addToOutputList("DutyCycle" ); 
  query1b->addToOutputList("EdgeSelect" ); 
  query1b->addToOutputList("finebx" ); 
  query1b->addToOutputList("finedx" ); 
  query1b->addToOutputList("TDMdelay" ); 
  query1b->addToOutputList("TTCGRNAME" ); 
  query1b->addToOutputList("ttcgr_tag" ); 

  query1b->addToOrderList("ttcgr_UID ASC"); 
    
  query_cond = "ttcgr_iovfk = :key";

  query1a->setCondition( query_cond, query_condData);

  query1b->setCondition( query_cond, query_condData);

  query_condData[0].data<long long>() = key1;
  
  coral::ICursor& cursor1a = query1a->execute();
    
  while ( cursor1a.next() ) {
    const coral::AttributeList &row1a = cursor1a.currentRow();
    ttcgr1.ttcgr_UID = row1a[0].data<int>();
    ttcgr1.ttcgr_iovfk = row1a[1].data<long long>();
    ttcgr1.Group = row1a[2].data<int>();
    ttcgr1.DutyCycle = row1a[3].data<int>();
    ttcgr1.EdgeSelect = row1a[4].data<std::string>();
    ttcgr1.finebx = row1a[5].data<std::string>();
    ttcgr1.finedx =  row1a[6].data<std::string>();
    ttcgr1.TDMdelay =  row1a[7].data<std::string>();
    ttcgr1.TTCGRNAME = row1a[8].data<std::string>();
    ttcgr1.ttcgr_tag = row1a[9].data<std::string>();
    
    (detector1.ttcgr).push_back(ttcgr1);
  }
  delete query1a;

  query_condData[0].data<long long>() = key2;

  coral::ICursor& cursor1b = query1b->execute();
    
  while ( cursor1b.next() ) {
    const coral::AttributeList &row1b = cursor1b.currentRow();
    ttcgr2.ttcgr_UID = row1b[0].data<int>();
    ttcgr2.ttcgr_iovfk = row1b[1].data<long long>();
    ttcgr2.Group = row1b[2].data<int>();
    ttcgr2.DutyCycle = row1b[3].data<int>();
    ttcgr2.EdgeSelect = row1b[4].data<std::string>();
    ttcgr2.finebx = row1b[5].data<std::string>();
    ttcgr2.finedx =  row1b[6].data<std::string>();
    ttcgr2.TDMdelay =  row1b[7].data<std::string>();
    ttcgr2.TTCGRNAME = row1b[8].data<std::string>();
    ttcgr2.ttcgr_tag = row1b[9].data<std::string>();
    
    (detector2.ttcgr).push_back(ttcgr2);
  }
  delete query1b;





  coral::IQuery* query2a = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();

  coral::IQuery* query2b = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
    
  query2a->addToOutputList("rod_UID"); 
  query2a->addToOutputList("rod_iovfk"); 
  query2a->addToOutputList("RODByteSwapping"); 
  query2a->addToOutputList("RODPhysicalAddress"); 
  query2a->addToOutputList("RODDetID");
  query2a->addToOutputList("RODVMESlot");
  query2a->addToOutputList("RODHalf"); 
  query2a->addToOutputList("EdgeSelect0");
  query2a->addToOutputList("EdgeSelect1"); 
  query2a->addToOutputList("EdgeSelect2"); 
  query2a->addToOutputList("EdgeSelect3");
  query2a->addToOutputList("GolDelay0"); 
  query2a->addToOutputList("GolDelay1"); 
  query2a->addToOutputList("GolDelay2"); 
  query2a->addToOutputList("GolDelay3"); 
  query2a->addToOutputList("RODStatus");
  query2a->addToOutputList("RODInputFragmentType");
  query2a->addToOutputList("RODName");
  query2a->addToOutputList("Comment"); 
  query2a->addToOutputList("rod_tag"); 
	 
  query2a->addToOrderList("rod_UID ASC"); 

  query2b->addToOutputList("rod_UID"); 
  query2b->addToOutputList("rod_iovfk"); 
  query2b->addToOutputList("RODByteSwapping"); 
  query2b->addToOutputList("RODPhysicalAddress"); 
  query2b->addToOutputList("RODDetID");
  query2b->addToOutputList("RODVMESlot");
  query2b->addToOutputList("RODHalf"); 
  query2b->addToOutputList("EdgeSelect0");
  query2b->addToOutputList("EdgeSelect1"); 
  query2b->addToOutputList("EdgeSelect2"); 
  query2b->addToOutputList("EdgeSelect3");
  query2b->addToOutputList("GolDelay0"); 
  query2b->addToOutputList("GolDelay1"); 
  query2b->addToOutputList("GolDelay2"); 
  query2b->addToOutputList("GolDelay3"); 
  query2b->addToOutputList("RODStatus");
  query2b->addToOutputList("RODInputFragmentType");
  query2b->addToOutputList("RODName");
  query2b->addToOutputList("Comment"); 
  query2b->addToOutputList("rod_tag"); 
	 
  query2b->addToOrderList("rod_UID ASC"); 
   
  query_cond = "rod_iovfk = :key";

  query2a->setCondition( query_cond, query_condData);

  query2b->setCondition( query_cond, query_condData);
    
  query_condData[0].data<long long>() = key1;

  coral::ICursor& cursor2a = query2a->execute();
    
  while ( cursor2a.next() ) {
    const coral::AttributeList &row2a = cursor2a.currentRow();

    rod1.rod_UID = row2a[0].data<int>();
    rod1.rod_iovfk = row2a[1].data<long long>();
    rod1.RODByteSwapping = row2a[2].data<short>();
    rod1.RODPhysicalAddress = row2a[3].data<int>();
    rod1.RODDetID = row2a[4].data<int>();
    rod1.RODVMEslot = row2a[5].data<int>();
    rod1.RODhalf = row2a[6].data<int>();
    rod1.EdgeSelect0 = row2a[7].data<int>();
    rod1.EdgeSelect1 = row2a[8].data<int>();
    rod1.EdgeSelect2 = row2a[9].data<int>();
    rod1.EdgeSelect3 = row2a[10].data<int>();
    rod1.GolDelay0 = row2a[11].data<int>();
    rod1.GolDelay1 = row2a[12].data<int>();
    rod1.GolDelay2 = row2a[13].data<int>();
    rod1.GolDelay3 = row2a[14].data<int>();
    rod1.RODStatus = row2a[15].data<short>() ;
    rod1.RODInputFragmentType = row2a[16].data<std::string>();
    rod1.RODName = row2a[17].data<std::string>();
    rod1.Comment = row2a[18].data<std::string>();
    rod1.rod_tag = row2a[19].data<std::string>();
  
    (detector1.rod).push_back(rod1);
      
  }
  delete query2a;

  query_condData[0].data<long long>() = key2;

  coral::ICursor& cursor2b = query2b->execute();
    
  while ( cursor2b.next() ) {
    const coral::AttributeList &row2b = cursor2b.currentRow();

    rod2.rod_UID = row2b[0].data<int>();
    rod2.rod_iovfk = row2b[1].data<long long>();
    rod2.RODByteSwapping = row2b[2].data<short>();
    rod2.RODPhysicalAddress = row2b[3].data<int>();
    rod2.RODDetID = row2b[4].data<int>();
    rod2.RODVMEslot = row2b[5].data<int>();
    rod2.RODhalf = row2b[6].data<int>();
    rod2.EdgeSelect0 = row2b[7].data<int>();
    rod2.EdgeSelect1 = row2b[8].data<int>();
    rod2.EdgeSelect2 = row2b[9].data<int>();
    rod2.EdgeSelect3 = row2b[10].data<int>();
    rod2.GolDelay0 = row2b[11].data<int>();
    rod2.GolDelay1 = row2b[12].data<int>();
    rod2.GolDelay2 = row2b[13].data<int>();
    rod2.GolDelay3 = row2b[14].data<int>();
    rod2.RODStatus = row2b[15].data<short>() ;
    rod2.RODInputFragmentType = row2b[16].data<std::string>();
    rod2.RODName = row2b[17].data<std::string>();
    rod2.Comment = row2b[18].data<std::string>();
    rod2.rod_tag = row2b[19].data<std::string>();
  
    (detector2.rod).push_back(rod2);
      
  }
  delete query2b;


  coral::IQuery* query3a = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();

  coral::IQuery* query3b = m_session->nominalSchema().tableHandle(DTMROC_TABLE).newQuery();
    
  query3a->addToOutputList("dtmroc_UID"); 
  query3a->addToOutputList("dtmroc_iovfk"); 
  query3a->addToOutputList("DTMROCByteSwapping"); 
  query3a->addToOutputList("DTMROCPhysicalAddress"); 
  query3a->addToOutputList("ChipID");
  query3a->addToOutputList("ChipValid");
  query3a->addToOutputList("RODGroup"); 
  query3a->addToOutputList("RODInput");
  query3a->addToOutputList("HW_Addr_FE"); 
  query3a->addToOutputList("TTC_Line_FE"); 
  query3a->addToOutputList("Thresh0_Low_FE");
  query3a->addToOutputList("Thresh0_High_FE");
  query3a->addToOutputList("Thresh1_Low_FE");
  query3a->addToOutputList("Thresh1_High_FE");
  query3a->addToOutputList("VT_DAC0_FE"); 
  query3a->addToOutputList("VT_DAC1_FE"); 
  query3a->addToOutputList("Mask_FE"); 
  query3a->addToOutputList("Pipe_Latency_FE"); 
  query3a->addToOutputList("DTMROCName");
  query3a->addToOutputList("DTMROCInputFragmentType");
  query3a->addToOutputList("Clock_FE"); 
  query3a->addToOutputList("Comment"); 
  query3a->addToOutputList("roc_tag"); 
	    
  query3a->addToOrderList("dtmroc_UID ASC"); 
    

  query3b->addToOutputList("dtmroc_UID"); 
  query3b->addToOutputList("dtmroc_iovfk"); 
  query3b->addToOutputList("DTMROCByteSwapping"); 
  query3b->addToOutputList("DTMROCPhysicalAddress"); 
  query3b->addToOutputList("ChipID");
  query3b->addToOutputList("ChipValid");
  query3b->addToOutputList("RODGroup"); 
  query3b->addToOutputList("RODInput");
  query3b->addToOutputList("HW_Addr_FE"); 
  query3b->addToOutputList("TTC_Line_FE"); 
  query3b->addToOutputList("Thresh0_Low_FE");
  query3b->addToOutputList("Thresh0_High_FE");
  query3b->addToOutputList("Thresh1_Low_FE");
  query3b->addToOutputList("Thresh1_High_FE");
  query3b->addToOutputList("VT_DAC0_FE"); 
  query3b->addToOutputList("VT_DAC1_FE"); 
  query3b->addToOutputList("Mask_FE"); 
  query3b->addToOutputList("Pipe_Latency_FE"); 
  query3b->addToOutputList("DTMROCName");
  query3b->addToOutputList("DTMROCInputFragmentType");
  query3b->addToOutputList("Clock_FE"); 
  query3b->addToOutputList("Comment"); 
  query3b->addToOutputList("roc_tag"); 
	    
  query3b->addToOrderList("dtmroc_UID ASC"); 

  query_cond = "dtmroc_iovfk = :key";

  query3a->setCondition( query_cond, query_condData);

  query3b->setCondition( query_cond, query_condData);

  query_condData[0].data<long long>() = key1;

  coral::ICursor& cursor3a = query3a->execute();
    
  while ( cursor3a.next() ) {
    const coral::AttributeList &row3a = cursor3a.currentRow();

    dtmroc1.dtmroc_UID    = row3a[0].data<int>(); 
    dtmroc1.dtmroc_iovfk    = row3a[1].data<long long>(); 
    dtmroc1.DTMROCByteSwapping    = row3a[2].data<short>(); 
    dtmroc1.DTMROCPhysicalAddress    = row3a[3].data<int>(); 
    dtmroc1.ChipID   = row3a[4].data<int>();
    dtmroc1.Chip_Valid   = row3a[5].data<short>();
    dtmroc1.RODgroup    = row3a[6].data<short>(); 
    dtmroc1.RODinput    = row3a[7].data<int>();
    dtmroc1.HW_addr_FE   = row3a[8].data<short>(); 
    dtmroc1.TTC_Line_FE   = row3a[9].data<short>(); 
    dtmroc1.Thresh0_Low_FE   = row3a[10].data<short>();
    dtmroc1.Thresh0_High_FE   = row3a[11].data<short>();
    dtmroc1.Thresh1_Low_FE   = row3a[12].data<short>();
    dtmroc1.Thresh1_High_FE    = row3a[13].data<short>();
    dtmroc1.VT_DAC0_FE   = row3a[14].data<short>(); 
    dtmroc1.VT_DAC1_FE   = row3a[15].data<short>(); 
    dtmroc1.Mask_FE   = row3a[16].data<int>(); 
    dtmroc1.Pipe_Latency_FE   = row3a[17].data<short>(); 
    dtmroc1.DTMROCName  = row3a[18].data<std::string>();
    dtmroc1.DTMROCInputFragmentType   = row3a[19].data<std::string>();
    dtmroc1.Clock_FE   = row3a[20].data<std::string>(); 
    dtmroc1.Comment   = row3a[21].data<std::string>(); 
    dtmroc1.roc_tag  = row3a[22].data<std::string>(); 
    
    (detector1.dtmroc).push_back(dtmroc1);
      
  }
  delete query3a;


  query_condData[0].data<long long>() = key2;

  coral::ICursor& cursor3b = query3b->execute();
    
  while ( cursor3b.next() ) {
    const coral::AttributeList &row3b = cursor3b.currentRow();

    dtmroc2.dtmroc_UID    = row3b[0].data<int>(); 
    dtmroc2.dtmroc_iovfk    = row3b[1].data<long long>(); 
    dtmroc2.DTMROCByteSwapping    = row3b[2].data<short>(); 
    dtmroc2.DTMROCPhysicalAddress    = row3b[3].data<int>(); 
    dtmroc2.ChipID   = row3b[4].data<int>();
    dtmroc2.Chip_Valid   = row3b[5].data<short>();
    dtmroc2.RODgroup    = row3b[6].data<short>(); 
    dtmroc2.RODinput    = row3b[7].data<int>();
    dtmroc2.HW_addr_FE   = row3b[8].data<short>(); 
    dtmroc2.TTC_Line_FE   = row3b[9].data<short>(); 
    dtmroc2.Thresh0_Low_FE   = row3b[10].data<short>();
    dtmroc2.Thresh0_High_FE   = row3b[11].data<short>();
    dtmroc2.Thresh1_Low_FE   = row3b[12].data<short>();
    dtmroc2.Thresh1_High_FE    = row3b[13].data<short>();
    dtmroc2.VT_DAC0_FE   = row3b[14].data<short>(); 
    dtmroc2.VT_DAC1_FE   = row3b[15].data<short>(); 
    dtmroc2.Mask_FE   = row3b[16].data<int>(); 
    dtmroc2.Pipe_Latency_FE   = row3b[17].data<short>(); 
    dtmroc2.DTMROCName  = row3b[18].data<std::string>();
    dtmroc2.DTMROCInputFragmentType   = row3b[19].data<std::string>();
    dtmroc2.Clock_FE   = row3b[20].data<std::string>(); 
    dtmroc2.Comment   = row3b[21].data<std::string>(); 
    dtmroc2.roc_tag  = row3b[22].data<std::string>(); 
    
    (detector2.dtmroc).push_back(dtmroc2);
      
  }
  delete query3b;

  // Diagnostic

  std::cout << "*********************************************" << std::endl;
  std::cout << "Configuration1: tag =  " << tag1 << " and tstamp = " << tstamp1 << std::endl;
  std::cout << "Configuration1: " << detector1.ttc.size() << "\t TTC modules" <<std::endl;
  std::cout << "Configuration1: " << detector1.ttcgr.size() << "\t TTCGR modules" <<std::endl;
  std::cout << "Configuration1: " << detector1.rod.size() << "\t ROD modules" <<std::endl;
  std::cout << "Configuration1: " << detector1.dtmroc.size() << "\t DTMROC modules" <<std::endl;
  std::cout << std::endl;
  std::cout << "Configuration2: tag =  " << tag2 << " and tstamp = " << tstamp2 << std::endl;
  std::cout << "Configuration2: " << detector2.ttc.size() << "\t TTC modules" <<std::endl;
  std::cout << "Configuration2: " << detector2.ttcgr.size() << "\t TTCGR modules" <<std::endl;
  std::cout << "Configuration2: " << detector2.rod.size() << "\t ROD modules" <<std::endl;
  std::cout << "Configuration2: " << detector2.dtmroc.size() << "\t DTMROC modules" <<std::endl;
  std::cout << std::endl;
  std::cout << "*********************************************" << std::endl;



  
//   if(m_verbose) std::cout << "Number of ttcv elements = " << detector.ttc.size() << std::endl;
//   if(m_verbose) std::cout << "Number of ttcgrv elements = " << detector.ttcgr.size() << std::endl;
//   if(m_verbose) std::cout << "Number of rodv elements = " << detector.rod.size() << std::endl;
//   if(m_verbose) std::cout << "Number of dtmrocv elements = " << detector.dtmroc.size() << std::endl;

  m_session->transaction().commit();

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
    
}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
int COOLCORALClient::DiffHistValues(const std::vector<Detector_t>& Detectors){
      

  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  std::cout << "Making the diff..." << std::endl;

  if (Detectors.size()<2) 
    {
      std::cout << "*** ERROR: You gave a wrong input for TAGs and Timestamps" << std::endl;
      return 0; 
    }

   Detector_t detector1=Detectors[0];
   Detector_t detector2=Detectors[1];
   int diffcounts=0;


   if (detector1.ttc.size()<1 || detector2.ttc.size()<1) {
     std::cout << "*** ERROR: You gave a wrong input for TAGs and Timestamps" << std::endl;
     return 0;
    }


  for (unsigned int i=0; i<detector1.ttc.size();i++)
    {
      if (detector1.ttc[i].ttc_UID-detector2.ttc[i].ttc_UID==0)
	{      
	  if (detector1.ttc[i].Delay-detector2.ttc[i].Delay!=0) {
	    std::cout << "Found a difference in Delay: " 
		      << detector1.ttc[i].Delay << " vs " 
		      << detector2.ttc[i].Delay << std::endl;
	    diffcounts++;
	  }
	  if (detector1.ttc[i].ArShaping-detector2.ttc[i].ArShaping!=0) {
	    std::cout << "Found a difference in ArShaping: " 
		      << detector1.ttc[i].ArShaping << " vs " 
		      << detector2.ttc[i].ArShaping << std::endl;
	    diffcounts++;
	  }
	}
      else {
	std::cout << "WARNING: the TTC UIDs do not match, so no comparison is possible" << std::endl;
      }
    }

  
      std::vector <int> fbx1;
      std::vector <int> fdx1;
      std::vector <int> tdm1;
      std::vector <int> edge1;

      std::vector <int> fbx2;
      std::vector <int> fdx2;
      std::vector <int> tdm2;
      std::vector <int> edge2;


  for (unsigned int i=0; i<detector1.ttcgr.size();i++)
    {
      if (detector1.ttcgr[i].ttcgr_UID-detector2.ttcgr[i].ttcgr_UID==0)
	{      
	  if (detector1.ttcgr[i].DutyCycle-detector2.ttcgr[i].DutyCycle!=0) {
	    std::cout << "Found a difference in DutyCycle: " 
		      << detector1.ttcgr[i].DutyCycle << " vs " 
		      << detector2.ttcgr[i].DutyCycle << std::endl;
	    diffcounts++;
	  }
	  edge1.clear();
	  edge2.clear();
	  STRING2VECTOR(detector1.ttcgr[i].EdgeSelect,edge1);
	  STRING2VECTOR(detector2.ttcgr[i].EdgeSelect,edge2);
	  for (unsigned int j =0; j<edge1.size();j++){
	    if (edge1[j]-edge2[j]!=0) {
	      std::cout << "Found a difference in EdgeSelect[" << j << "]: " 
			<< edge1[j] << " vs " 
			<< edge2[j] << std::endl;
	      diffcounts++; 
	    }
	  }
	  fbx1.clear();
	  fbx2.clear();
	  STRING2VECTOR(detector1.ttcgr[i].finebx,fbx1);
	  STRING2VECTOR(detector2.ttcgr[i].finebx,fbx2);
	  for (unsigned int j =0; j<fbx1.size();j++){
	    if (fbx1[j]-fbx2[j]!=0) {
	      std::cout << "Found a difference in finebx[" << j << "]: " 
			<< fbx1[j] << " vs " 
			<< fbx2[j] << std::endl;
	      diffcounts++; 
	    }
	  }
	  fdx1.clear();
	  fdx2.clear();
	  STRING2VECTOR(detector1.ttcgr[i].finedx,fdx1);
	  STRING2VECTOR(detector2.ttcgr[i].finedx,fdx2);
	  for (unsigned int j =0; j<fdx1.size();j++){
	    if (fdx1[j]-fdx2[j]!=0) {
	      std::cout << "Found a difference in finedx[" << j << "]: " 
			<< fdx1[j] << " vs " 
			<< fdx2[j] << std::endl;
	      diffcounts++; 
	    }
	  }
	  tdm1.clear();
	  tdm2.clear();
	  STRING2VECTOR(detector1.ttcgr[i].TDMdelay,tdm1);
	  STRING2VECTOR(detector2.ttcgr[i].TDMdelay,tdm2);
	  for (unsigned int j =0; j<tdm1.size();j++){
	    if (tdm1[j]-tdm2[j]!=0) {
	      std::cout << "Found a difference in TDMdelay[" << j << "]: " 
			<< tdm1[j] << " vs " 
			<< tdm2[j] << std::endl;
	      diffcounts++; 
	    }
	  }
	}
      else {
	std::cout << "WARNING: the TTCGR UIDs do not match, so no comparison is possible" << std::endl;
      }
    }


  for (unsigned int i=0; i<detector1.rod.size();i++)
    {
      if (detector1.rod[i].rod_UID-detector2.rod[i].rod_UID==0)
	{      
	  if (detector1.rod[i].EdgeSelect0-detector2.rod[i].EdgeSelect0!=0) {
	    std::cout << "Found a difference in EdgeSelect0: " 
		      << detector1.rod[i].EdgeSelect0 << " vs " 
		      << detector2.rod[i].EdgeSelect0 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].EdgeSelect1-detector2.rod[i].EdgeSelect1!=0) {
	    std::cout << "Found a difference in EdgeSelect1: " 
		      << detector1.rod[i].EdgeSelect1 << " vs " 
		      << detector2.rod[i].EdgeSelect1 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].EdgeSelect2-detector2.rod[i].EdgeSelect2!=0) {
	    std::cout << "Found a difference in EdgeSelect2: " 
		      << detector1.rod[i].EdgeSelect2 << " vs " 
		      << detector2.rod[i].EdgeSelect2 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].EdgeSelect3-detector2.rod[i].EdgeSelect3!=0) {
	    std::cout << "Found a difference in EdgeSelect3: " 
		      << detector1.rod[i].EdgeSelect3 << " vs " 
		      << detector2.rod[i].EdgeSelect3 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].GolDelay0-detector2.rod[i].GolDelay0!=0) {
	    std::cout << "Found a difference in GolDelay0: " 
		      << detector1.rod[i].GolDelay0 << " vs " 
		      << detector2.rod[i].GolDelay0 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].GolDelay1-detector2.rod[i].GolDelay1!=0) {
	    std::cout << "Found a difference in GolDelay1: " 
		      << detector1.rod[i].GolDelay1 << " vs " 
		      << detector2.rod[i].GolDelay1 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].GolDelay2-detector2.rod[i].GolDelay2!=0) {
	    std::cout << "Found a difference in GolDelay2: " 
		      << detector1.rod[i].GolDelay2 << " vs " 
		      << detector2.rod[i].GolDelay2 << std::endl;
	    diffcounts++;
	  }
	  if (detector1.rod[i].GolDelay3-detector2.rod[i].GolDelay3!=0) {
	    std::cout << "Found a difference in GolDelay3: " 
		      << detector1.rod[i].GolDelay3 << " vs " 
		      << detector2.rod[i].GolDelay3 << std::endl;
	    diffcounts++;
	  }
	}
      else {
	std::cout << "WARNING: the ROD UIDs do not match, so no comparison is possible" << std::endl;
      }
    }

  for (unsigned int i=0; i<detector1.dtmroc.size();i++)
    {
      if (detector1.dtmroc[i].dtmroc_UID-detector2.dtmroc[i].dtmroc_UID==0)
	{      
	  if (detector1.dtmroc[i].Thresh0_Low_FE-detector2.dtmroc[i].Thresh0_Low_FE!=0) {
	    std::cout << "Found a difference in Thresh0_Low_FE: " 
		      << detector1.dtmroc[i].Thresh0_Low_FE << " vs " 
		      << detector2.dtmroc[i].Thresh0_Low_FE << std::endl;
	    diffcounts++;
	  }
	  if (detector1.dtmroc[i].Thresh0_High_FE-detector2.dtmroc[i].Thresh0_High_FE!=0) {
	    std::cout << "Found a difference in Thresh0_High_FE: " 
		      << detector1.dtmroc[i].Thresh0_High_FE << " vs " 
		      << detector2.dtmroc[i].Thresh0_High_FE << std::endl;
	    diffcounts++;
	  }
	  if (detector1.dtmroc[i].Thresh1_Low_FE-detector2.dtmroc[i].Thresh1_Low_FE!=0) {
	    std::cout << "Found a difference in Thresh1_Low_FE: " 
		      << detector1.dtmroc[i].Thresh1_Low_FE << " vs " 
		      << detector2.dtmroc[i].Thresh1_Low_FE << std::endl;
	    diffcounts++;
	  }
	  if (detector1.dtmroc[i].Thresh1_High_FE-detector2.dtmroc[i].Thresh1_High_FE!=0) {
	    std::cout << "Found a difference in Thresh1_High_FE: " 
		      << detector1.dtmroc[i].Thresh1_High_FE << " vs " 
		      << detector2.dtmroc[i].Thresh1_High_FE << std::endl;
	    diffcounts++;
	  }
	  if (detector1.dtmroc[i].VT_DAC0_FE-detector2.dtmroc[i].VT_DAC0_FE!=0) {
	    std::cout << "Found a difference in VT_DAC0_FE: " 
		      << detector1.dtmroc[i].VT_DAC0_FE << " vs " 
		      << detector2.dtmroc[i].VT_DAC0_FE << std::endl;
	    diffcounts++;
	  }
	  if (detector1.dtmroc[i].VT_DAC1_FE-detector2.dtmroc[i].VT_DAC1_FE!=0) {
	    std::cout << "Found a difference in VT_DAC1_FE: " 
		      << detector1.dtmroc[i].VT_DAC1_FE << " vs " 
		      << detector2.dtmroc[i].VT_DAC1_FE << std::endl;
	    diffcounts++;
	  }
	}
      else {
	std::cout << "WARNING: the TTC UIDs do not match, so no comparison is possible" << std::endl;
      }
    }


  
  std::cout << "Found " << diffcounts << " differences" << std::endl;






  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
    
  return 0;
    
}

//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
int COOLCORALClient::GetRichTags (){ // Thread unsafe ctime is used.
    
  std::string TAG_TABLE="HISTORY";

  std::string ROD_TABLE="BROD";

  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  m_session->transaction().start(true);

  // GET all TAGS

  coral::IQuery* query0 = m_session->nominalSchema().tableHandle(TAG_TABLE).newQuery();
    
  query0->addToOutputList("RUNNumber" ); 
  query0->addToOutputList("TAG" ); 
  query0->addToOutputList("Key" ); 
  query0->addToOutputList("Time" );
  query0->addToOrderList("Time ASC" ); 
  // to make it ordered must be an index


  std::string output = "*";
  
  
  coral::ICursor& cursor0 = query0->execute();

  std::cout << std::endl;     
  std::cout << "\t TAG\t Timestamp\t Run\t \t Date" << std::endl;
  std::cout << "\t Stacks (partition,phi)" << std::endl;
  std::cout << "\t -----------------------------------------------------------------"<< std::endl;
  std::cout << std::endl;
  while ( cursor0.next() ) {
    //  cursor0.next();
    const coral::AttributeList &row0 = cursor0.currentRow();
    
    time_t tempo = row0[2].data<long long>();

    coral::IQuery* query01 = m_session->nominalSchema().tableHandle(ROD_TABLE).newQuery();
    
    query01->addToOutputList("rod_UID"); 
    query01->addToOrderList("rod_UID ASC" ); 
    
    coral::ICursor& cursor01 = query01->execute();
    
    
    while ( cursor01.next() ) {
      const coral::AttributeList &row01 = cursor01.currentRow();
      
      
      int rod_uid = row01[0].data<int>();
      int partition = rod_uid/10000;
      if (partition < 33) 
	{
	  int phi = (rod_uid - partition*10000)/100;
	  output += "(";
	  output += ToString(partition);
	  output += ",";
	  output += ToString(phi);
	  output +=") ";
	}
    else 
      {
	int phi = (rod_uid - partition*10000)/100;
	if ((rod_uid - partition*10000 - phi/100)<2){
	  output += "(";
	  output += ToString(partition);
	  output += ",";
	  output += ToString(phi);
	  output +=") ";
	}
      }
    }
    delete query01;
    

    char buf[32];
    std::cout << "** \t" 
	      << row0[1].data<std::string>() << "\t " 
      	      << row0[3].data<long long>() << "\t "
	      << row0[0].data<long long>() << "\t "
	      << ctime_r(&tempo, buf) << "\t"
	      << output << std::endl;
    output = "*";
  }
  delete query0;

  std::cout << std::endl;

  
  

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
  

  m_session->transaction().commit();
    
  return 1;
    
}


//------------------------------------------------------
/// Read TTC information from the DB for one TTC module
/// identified by an input int (identifier) and print on std out
int COOLCORALClient::DumpHistValues(const Detector_t& detector, const std::string& tag, long long tstamp){
      

  struct timeval start_time;
  struct timeval end_time;
  unsigned long long total_usecs;
    
  gettimeofday(&start_time, NULL);


  std::cout << "Dumping..." << std::endl;

  if (detector.ttc.size()<1) 
    {
      std::cout << "*** ERROR: You gave a wrong input" << std::endl;
      return 0; 
    }


  std::string filename = "DumpDB-";
  filename += tag;
  filename += ToString(tstamp);
  filename += ".dat";

  std::ofstream dumpfile(filename.c_str());


  dumpfile << "#\tTTC_UID\tDelay\tArShaping" << std::endl;

  for (unsigned int i=0; i<detector.ttc.size();i++)
    {
      dumpfile << "\t" << detector.ttc[i].ttc_UID
	       << "\t" << detector.ttc[i].Delay
	       << "\t" << detector.ttc[i].ArShaping << std::endl;
    }

  
      std::vector <int> fbx;
      std::vector <int> fdx;
      std::vector <int> tdm;
      std::vector <int> edge;


  dumpfile << "#\tTTCGR_UID\tDutyCycle\tEdgeSelect\tfinebx\tfinedx\tTDMdelay" << std::endl;


  for (unsigned int i=0; i<detector.ttcgr.size();i++)
    {
      dumpfile  << "\t" << detector.ttcgr[i].ttcgr_UID 
		<< "\t" << detector.ttcgr[i].DutyCycle; 
      edge.clear();
      STRING2VECTOR(detector.ttcgr[i].EdgeSelect,edge);
      for (unsigned int j =0; j<edge.size();j++){
	dumpfile << "\t" << edge[j]; 
      }
      fbx.clear();
      STRING2VECTOR(detector.ttcgr[i].finebx,fbx);
      for (unsigned int j =0; j<fbx.size();j++){
	dumpfile << "\t" << fbx[j];
      }
      fdx.clear();
      STRING2VECTOR(detector.ttcgr[i].finedx,fdx);
      for (unsigned int j =0; j<fdx.size();j++){
	dumpfile << "\t" << fdx[j];
      }
      tdm.clear();
      STRING2VECTOR(detector.ttcgr[i].TDMdelay,tdm);
      for (unsigned int j =0; j<tdm.size();j++){
	dumpfile << "\t" << tdm[j];
      }
      dumpfile << std::endl;
    }
  

  dumpfile << "#\tROD_UID\tEdgeSelect0\tEdgeSelect1\tEdgeSelect2\tEdgeSelect3\tGolDelay0\tGolDelay1\tGolDelay2\tGolDelay3" << std::endl;
  

  for (unsigned int i=0; i<detector.rod.size();i++)
    {
      dumpfile << "\t" << detector.rod[i].rod_UID 
	       << "\t" << detector.rod[i].EdgeSelect0 
	       << "\t" << detector.rod[i].EdgeSelect1
	       << "\t" << detector.rod[i].EdgeSelect2 
	       << "\t" << detector.rod[i].EdgeSelect3 
	       << "\t" << detector.rod[i].GolDelay0
	       << "\t" << detector.rod[i].GolDelay1
	       << "\t" << detector.rod[i].GolDelay2
	       << "\t" << detector.rod[i].GolDelay3 << std::endl;
    }

  dumpfile << "#\tROC_UID\tThresh0_Low_FE\tThresh0_High_FE\tThresh1_Low_FE\tThresh1_High_FE\tVT_DAC0_FE\tVT_DAC1_FE" << std::endl;


  for (unsigned int i=0; i<detector.dtmroc.size();i++)
    {
      dumpfile << "\t" << detector.dtmroc[i].dtmroc_UID
	       << "\t" << detector.dtmroc[i].Thresh0_Low_FE
	       << "\t" << detector.dtmroc[i].Thresh0_High_FE
	       << "\t" << detector.dtmroc[i].Thresh1_Low_FE
	       << "\t" << detector.dtmroc[i].Thresh1_High_FE
	       << "\t" << detector.dtmroc[i].VT_DAC0_FE
	       << "\t" << detector.dtmroc[i].VT_DAC1_FE << std::endl;
    }


  
  dumpfile.close();


  std::cout << "*** Dump File " << filename << " produced" << std::endl;


  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if(m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
    
  return 1;
    
}
