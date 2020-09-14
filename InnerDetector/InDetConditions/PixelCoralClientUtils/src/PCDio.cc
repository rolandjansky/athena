/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PCDio.h"
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
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <sys/time.h>
using namespace std;

//---------------------------------------------------------------------------
/** Constructor.
    Open the default database and seal context.
 */
PCDio::PCDio(string connString, string tableName, int verbose) :
  m_connString(connString), m_pixeltable(tableName),
  m_verbose(verbose), m_session(0) {}

PCDio::~PCDio()
{
  if (m_verbose) cout << "Disconnecting \"" << m_connString << "\"" << endl;
  delete m_session;
}

/** initialize connection
 */
void PCDio::init(coral::AccessMode access_mode)
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

/** DB -> pcd.sav
 */
void PCDio::load(string tag, int revision)
{
  // start timer
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

//// TEMP

  transactionStartReadOnly();
  set<string> tables = m_session->nominalSchema().listTables();
  transactionCommit();

  for (set<string>::const_iterator tName = tables.begin();
        tName != tables.end(); ++tName ){
    cout << "Table: <" << *tName << ">";

    transactionStartReadOnly();
    coral::IQuery* query = m_session->nominalSchema().tableHandle(*tName).newQuery();
    coral::ICursor& cursor = query->execute();
    int nRows = 0;
    while ( cursor.next() ) {
      ++nRows;
    }
    delete query;
    transactionCommit();

    cout << ", Rows = " << nRows << endl;

  }

  transactionStartReadOnly();
////

  // build a query
  coral::IQuery* query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();

  query->addToOutputList("TAG");
  query->defineOutputType("TAG",coral::AttributeSpecification::typeNameForType<string>());
  query->addToOutputList("REVISION");
  query->defineOutputType("REVISION",coral::AttributeSpecification::typeNameForType<int>());
  query->addToOutputList("SOURCES");
  query->defineOutputType("SOURCES",coral::AttributeSpecification::typeNameForType<string>());
  query->addToOutputList("DATE");
  query->defineOutputType("DATE",coral::AttributeSpecification::typeNameForType<time_t>());
  query->addToOutputList("FK");

  // construct query condition
  coral::AttributeList pixel_condData ATLAS_THREAD_SAFE; // Not shared, ok.
  string pixel_cond = m_pixeltable+".TAG = :tag";
  pixel_condData.extend<string>("tag");
  pixel_condData[0].data<string>() = tag;
  if (revision>0) {
    pixel_cond += " AND "+m_pixeltable+".REVISION = :revision";
    pixel_condData.extend<int>("revision");
    pixel_condData["revision"].data<int>() = revision;
  }
  query->setCondition( pixel_cond, pixel_condData);
  coral::ICursor& cursor = query->execute();

  int loaded_revision = 0;

  // go through the list of found tables
  int nRows = 0;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    if (m_verbose) {
      //time_t timet = GMTtoLocalTime(row0[3].data<time_t>());
      time_t timet = row0[3].data<time_t>();
      char buf[32];
      string time = ctime_r(&timet, buf);
      cout << "  TAG = " << row0[0].data<string>() 
	   << "  REVISION = " << row0[1].data<int>()
	   << "  SOURCES = " << row0[2].data<string>()
	   << "  DATE = " << time.substr(0,time.size()-1)
	   << "  FK = " << row0[4].data<long long>() << endl;
    }
    int current_revision = row0[1].data<int>();
    //row0.toOutputStream(cout)<<endl;
    ++nRows;
    // Show the variables
    set<string> tables = m_session->nominalSchema().listTables();
    for (set<string>::const_iterator tName = tables.begin();
	 tName != tables.end(); ++tName ){
      if ((*tName).find(m_pixeltable+"_") != string::npos) {
	if (m_verbose) {
	  cout << "found table " << (*tName) << endl;
	}

	// open the output text file
	ofstream* outPtr = 0;
	if (loaded_revision<current_revision) {
	  loaded_revision = current_revision;
	  outPtr = new ofstream("pcd.sav");
	}

	coral::IQuery* query_2 = m_session->nominalSchema().tableHandle(*tName).newQuery();

	// Get the list of columns
	const coral::ITableDescription& tableDesc =
	  m_session->nominalSchema().tableHandle(*tName).description();
	int NumCol = tableDesc.numberOfColumns();
	vector<string> colName; colName.resize(NumCol);
	vector<string> colType; colType.resize(NumCol);
	for(int i=0; i<NumCol; ++i){
	  const coral::IColumn& colDesc = tableDesc.columnDescription(i);     
	  colName[i] = colDesc.name();
	  colType[i] = colDesc.type();
	  query_2->addToOutputList(colDesc.name()); 
	}

	// Second query to get entries in each table
	string pixel_cond_2 = (*tName)+".FK = :fk";
	coral::AttributeList pixel_condData_2 ATLAS_THREAD_SAFE; // Not shared, ok
	pixel_condData_2.extend<long long>("fk");
	pixel_condData_2[0].data<long long>() = row0[4].data<long long>();
	query_2->setCondition( pixel_cond_2, pixel_condData_2);
	coral::ICursor& cursor_2 = query_2->execute();
	while ( cursor_2.next() ) {
	  // populate PixelChipSummaryData
	  const coral::AttributeList &row0_2 = cursor_2.currentRow();
	  PixelCoralClientUtils::PixelCalibData pcd;
	  int ix = 1;
	  int idmod = row0_2[ix++].data<int>();
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

	  if (outPtr) {
	    // print out PixelChipSummaryData
	    for (int i = 0; i<16; ++i) {
	      const PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData& pcsd = *pcd.getPixelChipSummaryData(i);
	      ofstream& out = *outPtr;
	      if (pcsd.getChipID()==0) out << idmod << endl;
	      out << "I" << pcsd.getChipID()
		  << " " << pcsd.getThreshold(0)
		  << " " << pcsd.getThresholdSigma(0)
		  << " " << pcsd.getNoise(0)
		  << " " << pcsd.getTimeWalk(0)
		  << " " << pcsd.getThreshold(1)
		  << " " << pcsd.getThresholdSigma(1)
		  << " " << pcsd.getNoise(1)
		  << " " << pcsd.getTimeWalk(1)
		  << " " << pcsd.getThreshold(2)
		  << " " << pcsd.getThresholdSigma(2)
		  << " " << pcsd.getNoise(2)
		  << " " << pcsd.getTimeWalk(2)
		  << " " << pcsd.getQ2TotA(0)
		  << " " << pcsd.getQ2TotE(0)
		  << " " << pcsd.getQ2TotC(0)
		  << " " << pcsd.getQ2TotA(2)
		  << " " << pcsd.getQ2TotE(2)
		  << " " << pcsd.getQ2TotC(2)
		  << " " << pcsd.getTotP1()
		  << " " << pcsd.getTotP2() << endl;
	    }
	  }
	}
	delete query_2;
	if (outPtr) {
	  outPtr->close();
	  outPtr = 0;
	}
      }
    }
  }
  delete query;

  transactionCommit();

  if (m_verbose) cout << "Total analyses: " << nRows << endl;

  if (revision==0) {
    if (m_verbose) cout << "Loaded revision: " << loaded_revision << endl;
    // save loaded revision number to a file
    ofstream outr("pcd.rev");
    outr << loaded_revision << endl;
    outr.close();
  }

  // stop timer
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) cout << " Total time was " << total_usecs << " usec" << endl;
}

/** pcd.dat -> DB
 */
void PCDio::save(string tag, int revision, string sources)
{
  // start timer
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  // create tables if needed
  createTable();

  // get a new foreign key
  long long fk = updateKey();
  
  transactionStartUpdate(); 

  // fill header
  coral::ITableDataEditor& pixel_editor = m_session->nominalSchema().tableHandle(m_pixeltable ).dataEditor();

  coral::AttributeList pixel_row ATLAS_THREAD_SAFE; // Not shared, ok.
  pixel_row.extend<std::string>("TAG");
  pixel_row.extend<int>("REVISION");
  pixel_row.extend<std::string>("SOURCES");
  pixel_row.extend<time_t>("DATE");
  pixel_row.extend<long long>("FK");

  pixel_row["TAG"].data<std::string>() = tag;
  pixel_row["REVISION"].data<int>() = revision;
  pixel_row["SOURCES"].data<std::string>() = sources;
  pixel_row["DATE"].data<time_t>() = time(0);
  pixel_row["FK"].data<long long>() = fk;

  pixel_editor.insertRow(pixel_row);
  transactionCommit();

  // fill pcd
  transactionStartUpdate();
  {
    coral::ITableDataEditor& pixel_editor_2 = m_session->nominalSchema().tableHandle(m_pixeltable+"_PCD").dataEditor();
    coral::AttributeList pixel_row_2 ATLAS_THREAD_SAFE; // Not shared, ok
    pixel_editor_2.rowBuffer(pixel_row_2);
    int mysize = 1;
    coral::IBulkOperation* pixel_bulk_2= pixel_editor_2.bulkInsert(pixel_row_2,mysize);

    int nRows = 0;

    ifstream in("pcd.dat");
    while (in) {
      int idmod; in >> idmod;
      if (in.eof()) break;

      pixel_row_2["FK"].setValue <long long> (fk);
      pixel_row_2["IDMOD"].setValue <int> (idmod);

      bool ok = true;
      for (int i = 0; i<16; ++i) {
	char dummy; in >> dummy;
	if (dummy!='I') { ok = false; break; }
	int idchip; in >> idchip;
	if (idchip!=i) { ok = false; break; }

        char buf[256];
	int thres, sigma, noise, timewalk;
	in >> thres >> sigma >> noise >> timewalk;
	sprintf(buf,"THRESHOLD1I%d",i);
	pixel_row_2[buf].setValue <int> (thres);
	sprintf(buf,"THRESHOLD2I%d",i);
	pixel_row_2[buf].setValue <int> (sigma);
	sprintf(buf,"THRESHOLD3I%d",i);
	pixel_row_2[buf].setValue <int> (noise);
	sprintf(buf,"THRESHOLD4I%d",i);
	pixel_row_2[buf].setValue <int> (timewalk);

	in >> thres >> sigma >> noise >> timewalk;
	sprintf(buf,"THRESHOLDLONG1I%d",i);
	pixel_row_2[buf].setValue <int> (thres);
	sprintf(buf,"THRESHOLDLONG2I%d",i);
	pixel_row_2[buf].setValue <int> (sigma);
	sprintf(buf,"THRESHOLDLONG3I%d",i);
	pixel_row_2[buf].setValue <int> (noise);
	sprintf(buf,"THRESHOLDLONG4I%d",i);
	pixel_row_2[buf].setValue <int> (timewalk);

	in >> thres >> sigma >> noise >> timewalk;
	sprintf(buf,"THRESHOLDGANGED1I%d",i);
	pixel_row_2[buf].setValue <int> (thres);
	sprintf(buf,"THRESHOLDGANGED2I%d",i);
	pixel_row_2[buf].setValue <int> (sigma);
	sprintf(buf,"THRESHOLDGANGED3I%d",i);
	pixel_row_2[buf].setValue <int> (noise);
	sprintf(buf,"THRESHOLDGANGED4I%d",i);
	pixel_row_2[buf].setValue <int> (timewalk);

	float A, E, C;
	in >> A >> E >> C;
	sprintf(buf,"Q2TOT1I%d",i);
	pixel_row_2[buf].setValue <float> (A);
	sprintf(buf,"Q2TOT2I%d",i);
	pixel_row_2[buf].setValue <float> (E);
	sprintf(buf,"Q2TOT3I%d",i);
	pixel_row_2[buf].setValue <float> (C);

	in >> A >> E >> C;
	sprintf(buf,"Q2TOTGANGED1I%d",i);
	pixel_row_2[buf].setValue <float> (A);
	sprintf(buf,"Q2TOTGANGED2I%d",i);
	pixel_row_2[buf].setValue <float> (E);
	sprintf(buf,"Q2TOTGANGED3I%d",i);
	pixel_row_2[buf].setValue <float> (C);

	float p1, p2;
	in >> p1 >> p2;
	sprintf(buf,"TOTRES1I%d",i);
	pixel_row_2[buf].setValue <float> (p1);
	sprintf(buf,"TOTRES2I%d",i);
	pixel_row_2[buf].setValue <float> (p2);
      }

      ++nRows;
      pixel_bulk_2->processNextIteration();
      if (!ok) break;
    }
    if (m_verbose) cout << "PixelCalibData: loaded " << nRows << " chips" << endl;

    pixel_bulk_2->flush();
    delete pixel_bulk_2;
  }
  transactionCommit();

  // stop timer
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) cout << " Total time was " << total_usecs << " usec" << endl;
}

/** part of save(): create new tables
 */
void PCDio::createTable()
{
  createAuxTables();
  createHeaderTable();
  createDataTable();
}

/** part of save(): create header table
 */
void PCDio::createHeaderTable()
{
  // create header
  transactionStartUpdate();
  if (!m_session->nominalSchema().existsTable(m_pixeltable)) {
    if (m_verbose) cout << "creating " << m_pixeltable << endl;
    coral::TableDescription pixel_columns( "SchemaDefinition_PIXEL" );
    pixel_columns.setName( m_pixeltable );
    pixel_columns.insertColumn("TAG", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    pixel_columns.insertColumn("REVISION", coral::AttributeSpecification::typeNameForType<int>());
    pixel_columns.insertColumn("SOURCES", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    pixel_columns.insertColumn( "DATE", coral::AttributeSpecification::typeNameForType<time_t>());
    pixel_columns.insertColumn( "FK", coral::AttributeSpecification::typeNameForType<long long>());
    pixel_columns.setUniqueConstraint("FK","FK_UNIQ");
    m_session->nominalSchema().createTable( pixel_columns );
  }
  transactionCommit();
}

/** part of save(): create data table
 */
void PCDio::createDataTable()
{
  transactionStartUpdate();
  string PIXEL_TABLE_DATA = m_pixeltable + "_PCD";
  if (!m_session->nominalSchema().existsTable(PIXEL_TABLE_DATA)) {
    if (m_verbose) cout << "creating " << PIXEL_TABLE_DATA << endl;
    coral::TableDescription pixel_columns( "SchemaDefinition_PIXEL" );
    pixel_columns.setName(PIXEL_TABLE_DATA);
    pixel_columns.insertColumn("FK", coral::AttributeSpecification::typeNameForType<long long>());
    pixel_columns.insertColumn("IDMOD",coral::AttributeSpecification::typeNameForType<int>());
    char buf[256];
    for (int i = 0; i<16; ++i) {
      for (int j = 1; j<=4; ++j) {
	sprintf(buf,"THRESHOLD%dI%d",j,i);
	pixel_columns.insertColumn(buf,coral::AttributeSpecification::typeNameForType<int>());
      }
      for (int j = 1; j<=4; ++j) {
	sprintf(buf,"THRESHOLDLONG%dI%d",j,i);
	pixel_columns.insertColumn(buf,coral::AttributeSpecification::typeNameForType<int>());
      }
      for (int j = 1; j<=4; ++j) {
	sprintf(buf,"THRESHOLDGANGED%dI%d",j,i);
	pixel_columns.insertColumn(buf,coral::AttributeSpecification::typeNameForType<int>());
      }
      for (int j = 1; j<=3; ++j) {
	sprintf(buf,"Q2TOT%dI%d",j,i);
	pixel_columns.insertColumn(buf,coral::AttributeSpecification::typeNameForType<float>());
      }
      for (int j = 1; j<=3; ++j) {
	sprintf(buf,"Q2TOTGANGED%dI%d",j,i);
	pixel_columns.insertColumn(buf,coral::AttributeSpecification::typeNameForType<float>());
      }
      for (int j = 1; j<=2; ++j) {
	sprintf(buf,"TOTRES%dI%d",j,i);
	pixel_columns.insertColumn(buf,coral::AttributeSpecification::typeNameForType<float>());
      }
    }

    pixel_columns.createForeignKey("PCD_FK","FK",m_pixeltable,"FK");
    //pixel_columns.createIndex("PCD_CONNECTIVITY_IDX", "CONNECTIVITY");
    //pixel_columns.createIndex("PCD_VARIABLE_IDX", "VARIABLE");
    //pixel_columns.setNotNullConstraint ( "CONNECTIVITY" );
    //pixel_columns.setNotNullConstraint ( "VARIABLE" );

    m_session->nominalSchema().createTable( pixel_columns );
  }
  transactionCommit();
}

/** part of save(): create key table
 */
void PCDio::createAuxTables()
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
    coral::AttributeList rowBuffer ATLAS_THREAD_SAFE; // Not shared, ok
    rowBuffer.extend<long long>( "KEY" );
    long long& key = rowBuffer[ "KEY" ].data<long long>();
    key = 1000;
    keyeditor.insertRow( rowBuffer );
  }
  transactionCommit();
}

/** part of save(): update the key
 */
long long PCDio::updateKey()
{
  std::string FK_TABLE = "CALIB_KEYGEN";
  transactionStartUpdate();

  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( FK_TABLE ).dataEditor();
  std::string updateAction = "KEY = KEY + :offset";
  coral::AttributeList updateData ATLAS_THREAD_SAFE; // Not shared, ok
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
void PCDio::transactionStartReadOnly(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(true /*ReadOnly*/);
}

/** CORAL helper to start a transaction in update mode
 */
void PCDio::transactionStartUpdate(){
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(false /*update*/);
}

/** CORAL helper to commit changes at the end of a transaction
 */
void PCDio::transactionCommit(){
  if (m_session->transaction().isActive()) m_session->transaction().commit();
}
