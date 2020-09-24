/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PCDkr.h"
#include "PixelCoralClientUtils/PixelCalibData.hh"

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
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------
/** Constructor.
    Open the default database and seal context.
 */
PCDkr::PCDkr(const std::string& connString,
             const std::string& tableName,
             int verbose) :
  m_connString(connString), m_pixeltable(tableName),
  m_verbose(verbose), m_session(0) {}

PCDkr::~PCDkr()
{
  if (m_verbose) cout << "Disconnecting \"" << m_connString << "\"" << endl;
  delete m_session;
}

/** initialize connection
 */
void PCDkr::init(coral::AccessMode access_mode)
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

/** dump DB
 */
void PCDkr::load()
{
  // start timer
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  transactionStartReadOnly();

  // build a query
  coral::IQuery* query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();

  query->addToOutputList("TAG");
  query->defineOutputType("TAG",coral::AttributeSpecification::typeNameForType<string>());
  query->addToOutputList("CID");
  query->defineOutputType("CID",coral::AttributeSpecification::typeNameForType<string>());
  query->addToOutputList("VALID");
  query->defineOutputType("VALID",coral::AttributeSpecification::typeNameForType<int>());

  coral::ICursor& cursor = query->execute();

  // go through the list of found tables
  int nRows = 0;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    if (m_verbose) {
      cout << "  TAG = " << row0[0].data<string>() 
	   << "  CID = " << row0[1].data<string>()
	   << "  VALID = " << row0[2].data<int>() << endl;
    }
    ++nRows;
  }
  delete query;

  transactionCommit();

  if (m_verbose) cout << "Total configurations: " << nRows << endl;

  // stop timer
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) cout << " Total time was " << total_usecs << " usec" << endl;
}

/** add a new record to DB
 */
void PCDkr::save(const std::string& tag, const std::string& cid)
{
  // start timer
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  // create tables if needed
  createTable();

  transactionStartUpdate(); 

  // fill header
  coral::ITableDataEditor& pixel_editor = m_session->nominalSchema().tableHandle(m_pixeltable ).dataEditor();

  coral::AttributeList pixel_row;
  pixel_row.extend<std::string>("TAG");
  pixel_row.extend<std::string>("CID");
  pixel_row.extend<int>("VALID");

  pixel_row["TAG"].data<std::string>() = tag;
  pixel_row["CID"].data<std::string>() = cid;
  pixel_row["VALID"].data<int>() = 0;

  pixel_editor.insertRow(pixel_row);
  transactionCommit();

  // stop timer
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) cout << " Total time was " << total_usecs << " usec" << endl;
}

/** part of save(): create the table
 */
void PCDkr::createTable()
{
  // create header
  transactionStartUpdate();
  if (!m_session->nominalSchema().existsTable(m_pixeltable)) {
    if (m_verbose) cout << "creating " << m_pixeltable << endl;
    coral::TableDescription pixel_columns( "SchemaDefinition_KR" );
    pixel_columns.setName( m_pixeltable );
    pixel_columns.insertColumn("TAG", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    pixel_columns.insertColumn("CID", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    pixel_columns.insertColumn( "VALID", coral::AttributeSpecification::typeNameForType<int>());
    pixel_columns.setPrimaryKey("TAG");
    m_session->nominalSchema().createTable( pixel_columns );
  }
  transactionCommit();
}

/** validate a record
    example: Integration_Basic/src/DmlOperations.cpp
 */
void PCDkr::validate(const std::string& tag)
{
  transactionStartUpdate(); 

  coral::ITableDataEditor& pixel_editor = m_session->nominalSchema().tableHandle(m_pixeltable ).dataEditor();

  string updateAction = "VALID = 1";
  string updateCondition = "TAG = :tvalue";
  coral::AttributeList updateData;
  updateData.extend<std::string>("tvalue");
  updateData[0].data<std::string>() = tag;

  int nRows = pixel_editor.updateRows(updateAction,updateCondition,updateData);
  cout << "Updated rows: " << nRows << endl;
  transactionCommit();
}

/** CORAL helper to start a transaction in readonly mode
 */
void PCDkr::transactionStartReadOnly(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(true /*ReadOnly*/);
}

/** CORAL helper to start a transaction in update mode
 */
void PCDkr::transactionStartUpdate(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(false /*update*/);
}

/** CORAL helper to commit changes at the end of a transaction
 */
void PCDkr::transactionCommit(){
  if (m_session->transaction().isActive()) m_session->transaction().commit();
}
