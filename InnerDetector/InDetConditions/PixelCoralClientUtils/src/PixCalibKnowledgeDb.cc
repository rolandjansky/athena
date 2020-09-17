/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PixCalibKnowledgeDb.h"

#include "CxxUtils/checker_macros.h"

// CORAL API
#include "CoralKernel/Context.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListException.h"
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
#include "RelationalAccess/ITableSchemaEditor.h"
#include "RelationalAccess/IBulkOperation.h"
#include "RelationalAccess/SchemaException.h"

// std lib
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
using namespace std;

//---------------------------------------------------------------------------
/** Constructor.
    Open the default database and seal context.
 */
PixCalibKnowledgeDb::PixCalibKnowledgeDb(const std::string& connString,
                                         const std::string& tableName,
                                         int verbose) :
  m_connString(connString), m_pixeltable(tableName),
  m_verbose(verbose), m_session(0) {}

PixCalibKnowledgeDb::~PixCalibKnowledgeDb()
{
  if (m_verbose) cout << "Disconnecting \"" << m_connString << "\"" << endl;
  delete m_session;
}

/** initialize connection
 */
void PixCalibKnowledgeDb::init(coral::AccessMode access_mode)
{
  if (m_verbose) cout << "Creating Connection Service to \"" << m_connString << "\"" << endl;
  coral::Context* context = &coral::Context::instance();
  coral::IHandle<coral::IConnectionService> lookSvcH = context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
    context->loadComponent("CORAL/Services/ConnectionService");
    lookSvcH = context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
    throw runtime_error("Could not locate the connection service");
  }

  m_session = lookSvcH->connect(m_connString, access_mode);
  if (m_verbose) cout << "Connection established" << endl;
}

void PixCalibKnowledgeDb::saveCorrespondingConfig
(long int UNIXTimeInSeconds, long int RunNumber, const std::string& calibtags_in_string, const std::string& idTag, const std::string& connTag, const std::string& cfgTag, const std::string& cfgModTag )
{

   // create tables if needed
   createTable();
 
   // get a new unique foreign key
   long long fk = updateKey();
   if( fk == -1 ) std::cout << "failed to get unique key?!" << std::endl;

   transactionStartUpdate(); 

  //fill config 
  transactionStartUpdate(); 

  coral::ITableDataEditor& pixel_editor = m_session->nominalSchema().tableHandle("CALIB_TAGS").dataEditor();

  coral::AttributeList pixel_row ATLAS_THREAD_SAFE; // Not shared, ok.
  pixel_row.extend<long int>("UNIXTimeInSeconds");
  pixel_row.extend<long int>("RunNumber");
  pixel_row.extend<std::string>("calibrationTags");
  pixel_row.extend<std::string>("idTag");
  pixel_row.extend<std::string>("connTag");
  pixel_row.extend<std::string>("cfgTag");
  pixel_row.extend<std::string>("cfgModTag");
  pixel_row.extend<long long>("FK");

  //   string idtag = "PIXEL";
  //   string conntag = "PIT-ALL-V36";
  //   string cfgTag = "PIT-20C-OLD";
  //   int cfgRev = 0;
  //   string cfgModTag = "PIT_MOD_MIXED_3000e";
  //   int cfgModRev = 0;
  
  pixel_row["UNIXTimeInSeconds"].data<long int>() = UNIXTimeInSeconds;
  pixel_row["RunNumber"].data<long int>() = RunNumber;
  pixel_row["calibrationTags"].data<std::string>() = calibtags_in_string;
  pixel_row["idTag"].data<std::string>() = idTag;
  pixel_row["connTag"].data<std::string>() = connTag;
  pixel_row["cfgTag"].data<std::string>() = cfgTag;
  pixel_row["cfgModTag"].data<std::string>() = cfgModTag;
  pixel_row["FK"].data<long long>() = fk;

  pixel_editor.insertRow(pixel_row);
  transactionCommit();
  std::cout << "done saving corresponding config" << std::endl;
}

//for reading back calibrationTags corresponding to a given time, not sure if this is needed
void PixCalibKnowledgeDb::readCorrespondingCalibTag (long int Utime){

  transactionStartReadOnly();
  //build a query
  coral::IQuery* query = m_session->nominalSchema().tableHandle("CALIB_TAGS").newQuery();
  
  query->addToOutputList("UNIXTimeInSeconds");
  query->defineOutputType("UNIXTimeInSeconds",coral::AttributeSpecification::typeNameForType<long int>());
  query->addToOutputList("calibrationTags");
  query->defineOutputType("calibrationTags",coral::AttributeSpecification::typeNameForType<string>());

  coral::AttributeList pixel_knowledgeData ATLAS_THREAD_SAFE; // Not shared, ok
  string pixel_knowledge = "CALIB_TAGS.UNIXTimeInSeconds <= :Utime";
  pixel_knowledgeData.extend<long int>("Utime");
  pixel_knowledgeData[0].data<long int>() = Utime;

  query->setCondition( pixel_knowledge, pixel_knowledgeData);
    
  coral::ICursor& cursor = query->execute();

  //  long long foreignkey = -1;
  string calibtags = "" ;
  long int Utime_h = -99999999;

  // go through list of found tables, highest Utime has the correct calibTag-set
 
 while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    long int UNIXTimeInSeconds = row0[0].data<long int>();
    string calibrationTags = row0[1].data<string>();
    if( Utime_h <= UNIXTimeInSeconds ){
//       cout << "  UNIXTimeInSeconds = " << UNIXTimeInSeconds
// 	   << "  calibrationTags = " << calibrationTags << endl; 
      Utime_h = UNIXTimeInSeconds;
      calibtags = calibrationTags;
    }
  }
  delete query;

  //  if( foreignkey != -1 ) calibtag = findTag(foreignkey);
  
  if( calibtags == ""  ){
    cout << " corresponding calibration-tags not found!" << endl;
  }
  else {
    std::cout << "UNIXTimeInSeconds " << Utime << " corresponds to the following calibration-tags:\n" << calibtags << std::endl;
  }
  transactionCommit();

}

/** part of save(): create new tables
 */
void PixCalibKnowledgeDb::createTable ()
{
  createConfigurationTable();
  createAuxTables();
}

/** part of save(): create config-tags table
 */
void PixCalibKnowledgeDb::createConfigurationTable()
{
  // create header
  transactionStartUpdate();
  string PIXEL_TABLE_CFG = "CALIB_TAGS";
  if (!m_session->nominalSchema().existsTable(PIXEL_TABLE_CFG)) {
    if (m_verbose) cout << "creating " << PIXEL_TABLE_CFG << endl;
    coral::TableDescription cfg_columns( "SchemaDefinition_PIXEL_CFG" );
    cfg_columns.setName( PIXEL_TABLE_CFG );
    cfg_columns.insertColumn("UNIXTimeInSeconds", coral::AttributeSpecification::typeNameForType<long int>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    cfg_columns.insertColumn( "RunNumber", coral::AttributeSpecification::typeNameForType<long long>());
    cfg_columns.insertColumn("calibrationTags", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    cfg_columns.insertColumn("idTag", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    cfg_columns.insertColumn("connTag", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    cfg_columns.insertColumn("cfgTag", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    cfg_columns.insertColumn("cfgModTag", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    cfg_columns.insertColumn( "FK", coral::AttributeSpecification::typeNameForType<long long>());
    m_session->nominalSchema().createTable( cfg_columns );
  }
  transactionCommit();
}

/** part of save(): create key table
 */
void PixCalibKnowledgeDb::createAuxTables()
{
  transactionStartUpdate();
  string FK_TABLE = "CALIB_KEYGEN";
  if (!m_session->nominalSchema().existsTable(FK_TABLE)) {
    coral::TableDescription key_columns( "SchemaDefinition_KEY" );
    key_columns.setName( FK_TABLE );
    key_columns.insertColumn( "KEY", coral::AttributeSpecification::typeNameForType<long long>());
    key_columns.createIndex ("KEY_INDEX", "KEY");
    m_session->nominalSchema().createTable( key_columns );
    // fill the first key
    coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( FK_TABLE ).dataEditor();
    coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, ok.
    rowBuffer.extend<long long>( "KEY" );
    long long& key = rowBuffer[ "KEY" ].data<long long>();
    key = 1000;
    keyeditor.insertRow( rowBuffer );
  }
  transactionCommit();
}

/** part of save(): update the key
 */
long long PixCalibKnowledgeDb::updateKey()
{
  std::string FK_TABLE = "CALIB_KEYGEN";
  transactionStartUpdate();

  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( FK_TABLE ).dataEditor();
  std::string updateAction = "KEY = KEY + :offset";
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, ok.
  updateData.extend<long long>("offset");
  updateData[0].data<long long>() = 1;
  keyeditor.updateRows( updateAction, "", updateData );

  coral::IQuery* query = m_session->nominalSchema().tableHandle(FK_TABLE).newQuery();
  query->addToOutputList("KEY");
  coral::ICursor& cursor = query->execute();
  long long key = -1;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    key = row0[0].data<long long>();
    if (m_verbose) std::cout << " KEY = " << key << std::endl;
  }
  transactionCommit();
  return key;
}

/** CORAL helper to start a transaction in readonly mode
 */
void PixCalibKnowledgeDb::transactionStartReadOnly(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(true /*ReadOnly*/);
}

/** CORAL helper to start a transaction in update mode
 */
void PixCalibKnowledgeDb::transactionStartUpdate(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(false /*update*/);
}

/** CORAL helper to commit changes at the end of a transaction
 */
void PixCalibKnowledgeDb::transactionCommit(){
  if (m_session->transaction().isActive()) m_session->transaction().commit();
}
