/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PCDDb.h"
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

#include "CxxUtils/checker_macros.h"

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
PCDDb::PCDDb(std::string connString, std::string tableName, bool verbose, bool /*load_text*/) :
  m_verbose(verbose),
  m_session(0), m_connString(connString), m_pixeltable(tableName),
  m_query(0), m_query_2(0), m_cursor(0)
{ 
  //char* ctmp = getenv("CAN_CALIB_DB");
  //if (ctmp) m_connString = ctmp;
  // Create the RCDConnectionService
  if (m_verbose) cout << "Creating Connection Service to \"" << m_connString << "\"" << endl;

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
  m_session = lookSvcH->connect( m_connString, coral::Update );
}

PCDDb::~PCDDb()
{
  if (m_session) {
    bool was_there = m_query!=0;

    // cleanup
    delete m_query_2; // ok if m_query_2==0
    delete m_query; // ditto
    if (was_there) transactionCommit();

    if (m_verbose) cout << "Disconnecting \"" << m_connString << "\"" << endl;
    delete m_session;
  }
}

bool PCDDb::init ATLAS_NOT_THREAD_SAFE (std::string tag, int revision) // Thread unsafe coral::AttributeList class is used.
{
  if (m_verbose) cout << "PCDDb::init(" << tag << ", " << revision << ")" << endl;

  transactionStartReadOnly();
  m_query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();

  m_query->addToOutputList("TAG");
  m_query->defineOutputType("TAG",coral::AttributeSpecification::typeNameForType<std::string>());
  m_query->addToOutputList("REVISION");
  m_query->defineOutputType("REVISION",coral::AttributeSpecification::typeNameForType<int>());
  m_query->addToOutputList("SOURCES");
  m_query->defineOutputType("SOURCES",coral::AttributeSpecification::typeNameForType<std::string>());
  m_query->addToOutputList("DATE");
  m_query->defineOutputType("DATE",coral::AttributeSpecification::typeNameForType<time_t>());
  m_query->addToOutputList("FK");

  coral::AttributeList pixel_condData;
  std::string pixel_cond = m_pixeltable+".TAG = :tag";
  pixel_condData.extend<string>( "tag" );
  pixel_condData[0].data<string>() = tag;
  pixel_cond += " AND "+m_pixeltable+".REVISION = :revision";
  pixel_condData.extend<int>( "revision" );
  pixel_condData["revision"].data<int>() = revision;
  m_query->setCondition( pixel_cond, pixel_condData);
  coral::ICursor& cursor = m_query->execute();

  while (cursor.next()) {
    const coral::AttributeList &row0 = cursor.currentRow();
    if (m_verbose) {
      time_t timet = row0[3].data<time_t>();
      string time = ctime(&timet);
      cout << "  TAG = " << row0[0].data<string>()
	   << "  REVISION = " << row0[1].data<int>()
	   << "  SOURCES = " << row0[2].data<string>()
	   << "  TIME = " << time.substr(0,time.size()-1)
	   << "  FK = " << row0[4].data<long long>() << endl;
    }
    std::set<string> tables = m_session->nominalSchema().listTables();
    for (std::set<string>::const_iterator tName = tables.begin();
	 tName != tables.end(); ++tName) {
      if ((*tName).find(m_pixeltable+"_") != string::npos) {
	if (m_verbose) cout << "found table " << *tName << endl;
	m_query_2 = m_session->nominalSchema().tableHandle(*tName).newQuery();

	// Get the list of columns
	const coral::ITableDescription& tableDesc = m_session->nominalSchema().tableHandle(*tName).description();
	int NumCol = tableDesc.numberOfColumns();
	std::vector<string> colName; colName.resize(NumCol);
	std::vector<string> colType; colType.resize(NumCol);
	for(int i = 0; i<NumCol; ++i){
	  const coral::IColumn& colDesc = tableDesc.columnDescription(i);
	  colName[i] = colDesc.name();
	  colType[i] = colDesc.type();
	  m_query_2->addToOutputList(colDesc.name());
	  if (m_verbose) cout << "  " << i << ": " << colName[i] <<"     ("<< colType[i] << ")" << endl;
	}

	// Second query to get entries
	string pixel_cond_2 = (*tName)+".FK = :fk";
	coral::AttributeList pixel_condData_2;
	pixel_condData_2.extend<long long>( "fk" );
	pixel_condData_2[0].data<long long>() = row0[4].data<long long>();
	m_query_2->setCondition( pixel_cond_2, pixel_condData_2);
	coral::ICursor& cursor_2 = m_query_2->execute();

	// .. and keep the cursor for data extraction
	m_cursor = &cursor_2;

	// success
	return true;
      }
    }
  }

  // table not found
  return false;
}

int PCDDb::next()
{
  if (!m_cursor->next()) return 0;
  const coral::AttributeList& row0_2 = m_cursor->currentRow();
  int idmod = row0_2[1].data<int>();
  return idmod;
}

bool PCDDb::set(int idmod_cur, PixelCoralClientUtils::PixelCalibData& pcd)
{
  const coral::AttributeList& row0_2 = m_cursor->currentRow();
  int ix = 1;
  int idmod = row0_2[ix++].data<int>();
  if (idmod != idmod_cur) return false;

  for (int i = 0; i<16; ++i) {
    PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData pcsd;
    pcsd.setChipID(i);
    int iv[4];
    for (int m=0; m<4; ++m) iv[m] = row0_2[ix++].data<int>();
    pcsd.setThreshold(0,iv[0],iv[1],iv[2],iv[3]);
    for (int m=0; m<4; ++m) iv[m] = row0_2[ix++].data<int>();
    pcsd.setThreshold(1,iv[0],iv[1],iv[2],iv[3]);
    for (int m=0; m<4; ++m) iv[m] = row0_2[ix++].data<int>();
    pcsd.setThreshold(2,iv[0],iv[1],iv[2],iv[3]);
    float fv[3];
    for (int m=0; m<3; ++m) fv[m] = row0_2[ix++].data<float>();
    pcsd.setQ2Tot(0,fv[0],fv[1],fv[2]);
    for (int m=0; m<3; ++m) fv[m] = row0_2[ix++].data<float>();
    pcsd.setQ2Tot(2,fv[0],fv[1],fv[2]);
    for (int m=0; m<2; ++m) fv[m] = row0_2[ix++].data<float>();
    pcsd.setTotRes(fv[0],fv[1]);
    pcd.add(pcsd);
  }

  return true;
}

/** CORAL helper to start a transaction in readonly mode
 */
void PCDDb::transactionStartReadOnly(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(true /*ReadOnly*/);
} 



/** CORAL helper to start a transaction in update mode
 */
void PCDDb::transactionStartUpdate(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(false /*update*/);
}



/** CORAL helper to commit changes at the end of a transaction
 */
void PCDDb::transactionCommit(){
  if (m_session->transaction().isActive()) m_session->transaction().commit();
}
