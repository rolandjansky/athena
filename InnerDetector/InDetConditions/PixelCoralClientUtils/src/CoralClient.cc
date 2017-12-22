/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////
///
/** @file Coralclient.cpp

C++ class for creating the calibration DB schema,
fill the DB, read it back

*/
///////////////////////////////////////////////////

#include "PixelCoralClientUtils/CoralClient.hh"
#include "PixelCoralClientUtils/SpecialPixelMap.hh"
#include "PixelCoralClientUtils/PixelCalibData.hh"

#include <assert.h>
#include <cstring>

// definition of table types
enum ETableTypeId {kBoolType, kUIntType, kFloatType, kAvresType, kPixelMapType, kCalibData, kNTableTypes};
const char *s_tableTypeName[kNTableTypes]={"BOOL","UINT","FLOAT","AVRES","SPECIALPIXELS","CALIBDATA"};

/** Returns the string used to identify a particular type
 */
template <typename T>
ETableTypeId variableType();


template <> ETableTypeId variableType<float>() {
  return kFloatType;
}

template <> ETableTypeId variableType<bool>() {
  return kBoolType;
}

template <> ETableTypeId variableType<unsigned int>() {
  return kUIntType;
}

template <> ETableTypeId variableType<CAN::AverageResult_t>() {
  return kAvresType;
}

template <> ETableTypeId variableType<PixelMap_t>() {
  return kPixelMapType;
}

template <> ETableTypeId variableType<PixelCoralClientUtils::PixelCalibData>() {
  return kCalibData;
}

template <class T> inline std::string tableTypeName() {
  return s_tableTypeName[ variableType<T>() ];
}

/** Create the RCDConnectionService
 */
coral::IHandle<coral::IConnectionService> PixCoralClient::connectionService(bool verbose)
{
   if(verbose) std::cout << "Creating Connection Service"<<std::endl;

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
   return lookSvcH;
}


std::string connectionString(const std::string &connection_string_in) {
  if (connection_string_in.empty()) {
    char * ctmp = getenv("CAN_CALIB_DB");
    if (ctmp != NULL) {
      return ctmp;
    }
  }
  return connection_string_in;
}


//---------------------------------------------------------------------------
/** Constructor.
    Open a database identified by the connection string.
*/
PixCoralClient::PixCoralClient(std::string id1, bool verbose, coral::AccessMode access_mode, const char* tableName)
  : m_connString(connectionString(id1)),
    m_accessMode(access_mode),
    m_connectionService(connectionService(verbose)),
    m_session(0),
    m_pixeltable(tableName),
    m_verbose(verbose)
{ 
}


//---------------------------------------------------------------------------
/** Constructor.
    Open the default database and seal context.
*/
PixCoralClient::PixCoralClient(bool verbose, coral::AccessMode access_mode, const char* tableName)
  : m_connString(connectionString("")),
    m_accessMode(access_mode),
    m_connectionService(connectionService(verbose)),
    m_session(0),
    m_pixeltable(tableName),
    m_verbose(verbose)
{ 
}


void PixCoralClient::disconnect() {
  if (m_verbose) {
    std::cout << "INFO [PixCoralClient::disconnect] disconnect from " << m_connString << "." << std::endl;
  }
  delete m_session;
  m_session=NULL;
}

void PixCoralClient::connect() {
  if (!m_session) {
    if (m_verbose) {
      std::cout << "INFO [PixCoralClient::connect] Try to connect to " << m_connString << "." << std::endl;
    }
    m_session = m_connectionService->connect( m_connString, m_accessMode );
  }
}


//---------------------------------------------------------------------------
/** Destructor.
    Close the connection
*/
PixCoralClient::~PixCoralClient() {
  disconnect();
  m_connectionService->purgeConnectionPool();
}



  
//---------------------------------------------------------------------------
/** Get the list of tables on the database and print it on stdout.
 */
void PixCoralClient::printTables(const char* /* option */)
{
  if (m_verbose) std::cout << "\nCOOLCORAL Client: list of tables" << std::endl;

  transactionStartReadOnly();

  std::set<std::string> tables = m_session->nominalSchema().listTables();
  transactionCommit();
  for ( std::set<std::string>::iterator tName = tables.begin();
	tName != tables.end(); ++tName ){
    std::cout << "\t" << *tName << std::endl;
  }
}




//--------------------------------------------------------------------------
/** Get the description of a table identified by an input string on the database and print it on stdout.
 */
void PixCoralClient::printTableDesc(std::string tableName)
{
  if (m_verbose) std::cout << "\nCOOLCORAL Client: " << tableName <<" Table description" << std::endl;

  transactionStartReadOnly();

  const coral::ITableDescription& tableDesc =
    m_session->nominalSchema().tableHandle(tableName).description();
  std::string Name = tableDesc.name();
  int NumCol = tableDesc.numberOfColumns();
  std::vector<std::string> colName; colName.resize(NumCol);
  std::vector<std::string> colType; colType.resize(NumCol);
  for(int i=0; i<NumCol; ++i){
    const coral::IColumn& colDesc = tableDesc.columnDescription(i);     
    colName[i] = colDesc.name();
    colType[i] = colDesc.type();
  }
 
  transactionCommit();
    
  std::cout << "\nName         : "<< Name << std::endl;
  //std::cout << "\nType        :"<< Type << std::endl;
  printf(" Num of cols : %d",NumCol);
  std::cout << "\n Columns     :"<< std::endl;
  for(int i=0; i<NumCol; ++i) 
    std::cout <<"	"<< colName[i] <<"	("<< colType[i]<<")"<< std::endl;
}




//---------------------------------------------------------------------------
/** Get the content of a table identified by an input string on the 
    database and print its number of lines.
 */
void PixCoralClient::printTableContent(std::string tableName){
  if (m_verbose) std::cout << "\nCOOLCORAL Client: " << tableName 
			   <<" Table content"<< std::endl;
   
  transactionStartReadOnly();
    
  coral::IQuery* query = m_session->nominalSchema().tableHandle(tableName).newQuery();
  coral::ICursor& cursor = query->execute();
  int nRows = 0;
  while ( cursor.next() ) {
    ++nRows;
  }
  delete query;

  transactionCommit();

  printf("Total  %d    records\n", nRows);
}





//---------------------------------------------------------------------------
/** Create CORAL tables 
 */
void PixCoralClient::createTables(const char* option)
{
  if (m_verbose) {
    std::cout << "\nCOOLCORAL Client: Create tables";
    if (option) std::cout << " with option " << option;
    std::cout << std::endl;
  }
    
  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  createAuxTables();

  transactionStartUpdate();

  //     std::cout << " Deleting the old table: " << m_pixeltable << std::endl;
  //     m_session->nominalSchema().dropIfExistsTable(m_pixeltable);

  if (!m_session->nominalSchema().existsTable(m_pixeltable)) {
    if (m_verbose) std::cout << " Creating table: " << m_pixeltable << std::endl;
    coral::TableDescription pixel_columns( "SchemaDefinition_PIXEL" );
    pixel_columns.setName( m_pixeltable );
    // modified -- A.X.
    if (option==0) {
      pixel_columns.insertColumn( "ANAL_ID",
				  coral::AttributeSpecification::typeNameForType<CAN::SerialNumber_t>());
    } else {
      pixel_columns.insertColumn("TAG", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
      pixel_columns.insertColumn("REVISION", coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("SOURCES", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 1000, /*fixed_size=*/ false);
    }
    pixel_columns.insertColumn( "DATE",
				coral::AttributeSpecification::typeNameForType<time_t>());
    pixel_columns.insertColumn( "FK",
				coral::AttributeSpecification::typeNameForType<long long>());

    // modified -- A.X.
    if (option==0) {
      pixel_columns.createIndex("ANAL_ID_IDX", "ANAL_ID");
      pixel_columns.setNotNullConstraint ("ANAL_ID");
    }
    pixel_columns.setUniqueConstraint("FK","FK_UNIQ");

    // Create the actual table
    try {
      m_session->nominalSchema().createTable( pixel_columns );
    } catch (coral::TableAlreadyExistingException & ex) {
      //Must have been created in parallel
    }
  }

  transactionCommit();

  // modified -- A.X.
  if (option==0) {
    createTable<bool>();
    createTable<unsigned int>();
    createTable<float>();
    createTable<CAN::AverageResult_t>();
    createTable<PixelMap_t>();
  } else {
    createTable<PixelCoralClientUtils::PixelCalibData>();
  }

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;
}





//---------------------------------------------------------------------------  
/** Drop CORAL tables 
 */
void PixCoralClient::dropTables(){
  if (m_verbose) std::cout << "\nCOOLCORAL Client: Drop tables" << std::endl;

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);


  transactionStartUpdate();

  std::set<std::string> tables = m_session->nominalSchema().listTables();

  if (tables.size() > 0)
    if (m_verbose) std::cout << "Deleting the old tables: " << std::endl;
  for ( std::set<std::string>::reverse_iterator tName = tables.rbegin();
	tName != tables.rend(); ++tName ){
    if ((*tName).find(m_pixeltable) != std::string::npos) {
      if (m_verbose) std::cout << "\t" << *tName << std::endl;
      m_session->nominalSchema().dropTable(*tName);
    }
  }

  transactionCommit();

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << "Total time was " << total_usecs << " usec" << std::endl;
}




//---------------------------------------------------------------------------
/** Fill analysis result tables
 */
void PixCoralClient::fillTables(CAN::SerialNumber_t serialNumber,
				CAN::AnalysisResultList_t *results){
  if (m_verbose) std::cout << "\nCOOLCORAL Client: Fill tables" << std::endl;

  // get a new foreign key
  long long fk = updateKey();

  transactionStartUpdate();

  // Create if not yet there
  if (!m_session->nominalSchema().existsTable(m_pixeltable)) {
    createTables();
    transactionStartUpdate();
  }

  // Find bool variables
  if (results->begin<bool>() != results->end<bool>()) {
    if (!m_session->nominalSchema().existsTable(m_pixeltable+"_BOOL")) {
      createTable<bool>();
      transactionStartUpdate();
    }
  }

  // Find unsigned int variables
  if (results->begin<unsigned int>() != results->end<unsigned int>()) {
    if (!m_session->nominalSchema().existsTable(m_pixeltable+"_UINT")) {
      createTable<unsigned int>();
      transactionStartUpdate();
    }
  }

  // Find float variables
  if (results->begin<float>() != results->end<float>()) {
    if (!m_session->nominalSchema().existsTable(m_pixeltable+"_FLOAT")) {
      createTable<float>();
      transactionStartUpdate();
    }
  }

  // Find AverageResult_t variables
  if (results->begin<CAN::AverageResult_t>() != results->end<CAN::AverageResult_t>()) {
    if (!m_session->nominalSchema().existsTable(m_pixeltable+"_AVRES")) {
      createTable<CAN::AverageResult_t>();
      transactionStartUpdate();
    }
  }

  // Find special pixel map variables
  if (results->begin<PixelMap_t>() != results->end<PixelMap_t>()) {
    if (!m_session->nominalSchema().existsTable(m_pixeltable+"_SPECIALPIXELS")) {
      createTable<PixelMap_t>();
      transactionStartUpdate();
    }
  }

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  if (m_verbose) std::cout << " Filling table " << m_pixeltable << " for analysis "<< serialNumber << std::endl;
  coral::ITableDataEditor& pixel_editor = m_session->nominalSchema().tableHandle(m_pixeltable ).dataEditor();
  coral::AttributeList pixel_row;

  //unsigned int id = serialNumber;
  time_t date = time(0);
    
  struct timeval nunc_time;
  gettimeofday(&nunc_time, NULL);

  int run = 0;

  pixel_row.extend<CAN::SerialNumber_t>("ANAL_ID");
  pixel_row.extend<time_t>("DATE");
  pixel_row.extend<long long>("FK");
  pixel_row["ANAL_ID"].data<CAN::SerialNumber_t>() = serialNumber;
  pixel_row["DATE"].data<time_t>() = LocalToGMTTime(date);
  pixel_row["FK"].data<long long>() = fk;
  pixel_editor.insertRow(pixel_row);

  transactionCommit();

  run += fillTable<bool>(fk,results);
  run += fillTable<unsigned int>(fk,results);
  run += fillTable<float>(fk,results);
  run += fillTable<CAN::AverageResult_t>(fk,results);
  run += fillTable<PixelMap_t>(fk,results);

  if (m_verbose) printf("\n Total: %d  rows\n",run);

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;

}

/** fill PCD table -- A.X.
 */
void PixCoralClient::fillTables(const char* option){

  if (option==NULL || !strlen(option)) { return; }

  if (m_verbose) {
    std::cout << "\nCOOLCORAL Client: Fill tables";
    if (option) std::cout << " with option " << option;
    std::cout << std::endl;
  }

  // get a new foreign key
  long long fk = updateKey();

  transactionStartUpdate();

    
  // Create if not yet there
  if (!m_session->nominalSchema().existsTable(m_pixeltable)) {
    createTables(option);
    transactionStartUpdate();
  }


  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  if (m_verbose) std::cout << " Filling table " << m_pixeltable << std::endl;
  coral::ITableDataEditor& pixel_editor = m_session->nominalSchema().tableHandle(m_pixeltable ).dataEditor();
  coral::AttributeList pixel_row;

  //unsigned int id = serialNumber;
  time_t date = time(0);
    
  struct timeval nunc_time;
  gettimeofday(&nunc_time, NULL);

  int run = 0;

  pixel_row.extend<std::string>("TAG");
  pixel_row.extend<int>("REVISION");
  pixel_row.extend<std::string>("SOURCES");
  pixel_row.extend<time_t>("DATE");
  pixel_row.extend<long long>("FK");

  // parse option -- A.X.
  std::string tag("");
  std::string revision("");
  std::string sources("");
  std::string delimiters(", ");
  std::string str(option);
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    if (str.substr(lastPos,4)=="tag:")
      tag = str.substr(lastPos+4, pos-lastPos-4);
    if (str.substr(lastPos,9)=="revision:")
      revision = str.substr(lastPos+9, pos-lastPos-9);
    if (str.substr(lastPos,8)=="sources:")
      sources = str.substr(lastPos+8, pos-lastPos-8);
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
  if (m_verbose) {
    std::cout << "TAG: " << tag << std::endl;
    std::cout << "REVISION: " << atoi(revision.c_str()) << std::endl;
    std::cout << "SOURCES: " << sources << std::endl;
  }

  pixel_row["TAG"].data<std::string>() = tag;
  pixel_row["REVISION"].data<int>() = atoi(revision.c_str());
  pixel_row["SOURCES"].data<std::string>() = sources;
  pixel_row["DATE"].data<time_t>() = LocalToGMTTime(date);
  pixel_row["FK"].data<long long>() = fk;

  pixel_editor.insertRow(pixel_row);

  transactionCommit();

  run += fillTablePixelCalibData(fk, option);

  if (m_verbose) printf("\n Total: %d  rows\n",run);

  // measuring elapsed time
  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;

}



//---------------------------------------------------------------------------
/** Get results for a particular analysis ID
    Optionally get only variable "varname" and/or connectivity object connName
 */
void PixCoralClient::queryTable(CAN::SerialNumber_t anal_id, std::string varname, std::string connName){

  std::cout << "\n COOLCORAL Client: " << m_pixeltable <<" Putting table content in AnalysisResultList for analysis " << anal_id;
  if (varname != "") std::cout<<", variable " << varname;
  if (connName != "") std::cout<<", object " << connName;
  std::cout<< std::endl;

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  transactionStartReadOnly();

  // Get all entries for anal_id
  coral::IQuery* query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();
  query->addToOutputList("ANAL_ID"); 
  query->defineOutputType("ANAL_ID",coral::AttributeSpecification::typeNameForType<CAN::SerialNumber_t>()); 
  query->addToOutputList("DATE"); 
  query->defineOutputType("DATE",coral::AttributeSpecification::typeNameForType<time_t>()); 
  query->addToOutputList("FK"); 
  std::string pixel_cond = m_pixeltable+".ANAL_ID = :analid";
  coral::AttributeList pixel_condData;
  pixel_condData.extend<CAN::SerialNumber_t>( "analid" );
  query->setCondition( pixel_cond, pixel_condData);
  pixel_condData[0].data<CAN::SerialNumber_t>() = anal_id;
  coral::ICursor& cursor = query->execute();

  int nRows = 0;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    std::time_t timet = GMTtoLocalTime(row0[1].data<time_t>());
    std::string time = ctime(&timet);
    std::cout << "  ANAL_ID = " << row0[0].data<CAN::SerialNumber_t>() 
	      << "  DATE = " << time.substr(0,time.size()-1)
	      << "   FK = " << row0[2].data<long long>() << std::endl; 
    //row0.toOutputStream(std::cout)<<endl;
    ++nRows;
    // Show the variables
    std::set<std::string> tables = m_session->nominalSchema().listTables();
    for ( std::set<std::string>::const_iterator tName = tables.begin();
	tName != tables.end(); ++tName ){
      if ((*tName).find(m_pixeltable+"_") != std::string::npos) {
	coral::IQuery* query_2 = m_session->nominalSchema().tableHandle(*tName).newQuery();

	// Get the list of columns
	const coral::ITableDescription& tableDesc =
	  m_session->nominalSchema().tableHandle(*tName).description();
	int NumCol = tableDesc.numberOfColumns();
	std::vector<std::string> colName; colName.resize(NumCol);
	std::vector<std::string> colType; colType.resize(NumCol);
	for(int i=0; i<NumCol; ++i){
	  const coral::IColumn& colDesc = tableDesc.columnDescription(i);     
	  colName[i] = colDesc.name();
	  colType[i] = colDesc.type();
	  query_2->addToOutputList(colDesc.name()); 
	  //if (m_verbose) std::cout <<"	"<<i<<": "<< colName[i] <<"	("<< colType[i]<<")"<< std::endl;
	}

	// Second query to get entries in each table
	std::string pixel_cond_2 = (*tName)+".FK = :analfk";
	coral::AttributeList pixel_condData_2;
	pixel_condData_2.extend<long long>( "analfk" );
	pixel_condData_2[0].data<long long>() = row0[2].data<long long>();
	if (varname != "") {
	  pixel_cond_2 += " AND "+(*tName)+".VARIABLE = :var";
	  pixel_condData_2.extend<std::string>("var");
	  pixel_condData_2["var"].data<std::string>() = varname;
	}
	if (connName != "") {
	  pixel_cond_2 += " AND "+(*tName)+".CONNECTIVITY = :conn";
	  pixel_condData_2.extend<std::string>("conn");
	  pixel_condData_2["conn"].data<std::string>() = connName;
	}
	query_2->setCondition( pixel_cond_2, pixel_condData_2);
	coral::ICursor& cursor_2 = query_2->execute();
	while ( cursor_2.next() ) {
	  const coral::AttributeList &row0_2 = cursor_2.currentRow();
 	  std::cout << "      conn = " << row0_2[1].data<std::string>();
	  std::cout<< "  ("<<colType[3]<<")"<<row0_2[2].data<std::string>()<<"=";
	  // If only I could use colType[i] and pass it to the template somehow
	  for(int i=3; i<NumCol; ++i){
	    if (colType[i] == "float")
	      std::cout<< "  ("<<colType[i]<<")"<<colName[i]<<"=" << row0_2[i].data<float>();
	    else if (colType[i] == "int")
	      std::cout<< "  ("<<colType[i]<<")"<<colName[i]<<"=" << row0_2[i].data<int>();
	    else if (colType[i] == "long long")
	      std::cout<< "  ("<<colType[i]<<")"<<colName[i]<<"=" << row0_2[i].data<long long>();
	    else if (colType[i] == "bool")
	      std::cout<< "  ("<<colType[i]<<")"<<colName[i]<<"=" << row0_2[i].data<bool>();
	  }
	  std::cout<< std::endl; 
	}
	delete query_2;
      }
    }
  }
  delete query;

  transactionCommit();

  if (m_verbose) printf(" Total  %d    analyses\n", nRows);

  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;
}




//--------------------------------------------------------------------------
/** Create and fill the CALIB_KEYGEN key generation table 
 */
void PixCoralClient::createAuxTables(){
  std::string FK_TABLE = "CALIB_KEYGEN";
  transactionStartUpdate();

  //   std::cout << "Deleting the old table: " <<  FK_TABLE << std::endl;
  //   m_session->nominalSchema().dropIfExistsTable(FK_TABLE);

  if (!m_session->nominalSchema().existsTable(FK_TABLE)) {
    if (m_verbose) std::cout << "Creating table: " <<FK_TABLE << std::endl;
    coral::TableDescription key_columns( "SchemaDefinition_KEY" );
    key_columns.setName( FK_TABLE );
    key_columns.insertColumn( "KEY",
			      coral::AttributeSpecification::typeNameForType<long long>());
    key_columns.createIndex ("KEY_INDEX", "KEY");
    // Create the actual table
    try {
      m_session->nominalSchema().createTable( key_columns );
      // Fill the first key
      coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( FK_TABLE ).dataEditor();
      coral::AttributeList rowBuffer;
      rowBuffer.extend<long long>( "KEY" );
      long long& key = rowBuffer[ "KEY" ].data<long long>();
      key = 1000;
      keyeditor.insertRow( rowBuffer );
    }  catch (coral::TableAlreadyExistingException & ex) {
      //Must have been created in parallel
    }
  }
  transactionCommit();
}




//---------------------------------------------------------------------------
/** Method to update the key in the KEYGEN table
 */
long long PixCoralClient::updateKey(){
  if (m_verbose) std::cout << "\nCOOLCORAL Client: Update Key" << std::endl;
  std::string FK_TABLE = "CALIB_KEYGEN";
  transactionStartUpdate();
  if (!m_session->nominalSchema().existsTable(FK_TABLE)) {
    createAuxTables();
    transactionStartUpdate();
  }

  coral::ITableDataEditor& keyeditor = m_session->nominalSchema().tableHandle( FK_TABLE ).dataEditor();
  std::string updateAction = "KEY = KEY + :offset";
  coral::AttributeList updateData;
  updateData.extend<long long>("offset");
  updateData[0].data<long long>() = 1;
  //long rowsUpdated = 
  keyeditor.updateRows( updateAction, "", updateData );
  //std::cout << " Updated " << rowsUpdated << " rows" << std::endl;

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





/** Template to create a table for variable columnName.
    Works only for simple types (one value to save)
*/
template <typename T>
void PixCoralClient::createTable(){

  transactionStartUpdate();

  std::string PIXEL_TABLE_DATA=m_pixeltable+"_"+tableTypeName<T>();

  if (!m_session->nominalSchema().existsTable(PIXEL_TABLE_DATA)) {
    if (m_verbose) {
      std::cout << "\nCOOLCORAL Client:  Creating table: " << PIXEL_TABLE_DATA <<" for ";
      try {
	std::cout <<tableTypeName<T>() << std::endl;
      } catch (const coral::AttributeListException&) {
	std::cout <<typeid(T).name() << std::endl;
      }
    }

    // Create the table
    coral::TableDescription pixel_columns( "SchemaDefinition_PIXEL" );
    pixel_columns.setName( PIXEL_TABLE_DATA );
    pixel_columns.insertColumn("FK", coral::AttributeSpecification::typeNameForType<long long>());
    pixel_columns.insertColumn("CONNECTIVITY", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 100, /*fixed_size=*/ false);
    pixel_columns.insertColumn("VARIABLE", coral::AttributeSpecification::typeNameForType<std::string>(), /*size in BYTES = */ 100, /*fixed_size=*/ false);
    
    if (typeid(T)==typeid(CAN::AverageResult_t)) {
      pixel_columns.insertColumn("N", coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("MEAN", coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("RMS", coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("LOW99", coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("UP99", coral::AttributeSpecification::typeNameForType<float>());
    } else if (typeid(T)==typeid(PixelMap_t)) {
      // A size > 4000 triggers the use of a CLOB object
      pixel_columns.insertColumn("VALUE", coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ 4001, /*fixed_size=*/ false);
    // added -- A.X.
    } else if (typeid(T)==typeid(PixelCoralClientUtils::PixelCalibData)) {
      pixel_columns.insertColumn("IDMOD",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("CHIP",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLD1",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLD2",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLD3",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLD4",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDLONG1",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDLONG2",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDLONG3",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDLONG4",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDGANGED1",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDGANGED2",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDGANGED3",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("THRESHOLDGANGED4",coral::AttributeSpecification::typeNameForType<int>());
      pixel_columns.insertColumn("Q2TOT1",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("Q2TOT2",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("Q2TOT3",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("Q2TOTGANGED1",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("Q2TOTGANGED2",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("Q2TOTGANGED3",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("TOTRES1",coral::AttributeSpecification::typeNameForType<float>());
      pixel_columns.insertColumn("TOTRES2",coral::AttributeSpecification::typeNameForType<float>());
    } else {
      pixel_columns.insertColumn("VALUE", coral::AttributeSpecification::typeNameForType<T>());
    }

    //set the foreign key
    pixel_columns.createForeignKey(tableTypeName<T>()+"_FK","FK",m_pixeltable,"FK");
    // create indices
    pixel_columns.createIndex(tableTypeName<T>()+"_CONNECTIVITY_IDX", "CONNECTIVITY");
    pixel_columns.createIndex(tableTypeName<T>()+"_VARIABLE_IDX", "VARIABLE");
    pixel_columns.setNotNullConstraint ( "CONNECTIVITY" );
    pixel_columns.setNotNullConstraint ( "VARIABLE" );

    // Create the actual table
    try {
      //coral::ITable& table = 
      m_session->nominalSchema().createTable( pixel_columns );
    } catch (coral::TableAlreadyExistingException & ex) {
      //Must have been created in parallel
    }
  }

  transactionCommit();
}

template <class T>
std::string valueQuote() {
  return std::string();
}

template <>
std::string valueQuote<std::string>() {
  return std::string("'");
}

template <class T>
void createCondition(const std::vector<T> &search_list,
		  const std::string &table_name,
		  const std::string &var_name,
		  const std::string &bind_name,
		  coral::AttributeList &attribut_list,
		  std::string &condition,
		  bool use_attr_list=false)
{
  if (!search_list.empty()) {
    condition += table_name;
    condition += ".";
    condition += var_name;

    if (search_list.size()==1) {
      attribut_list.extend< T >( bind_name );
      assert(attribut_list.size()>0); 
      attribut_list[attribut_list.size()-1].setValue( search_list.back() );
      condition += " = :";
      condition += bind_name;
    }
    else {
      if (use_attr_list) {

	condition += " in (";

	std::string sep=":";
	std::string sep2=",:";

	std::stringstream a_var_name;
	unsigned int counter=attribut_list.size();
	for(typename std::vector<T>::const_iterator iter=search_list.begin();
	    iter != search_list.end();
	    ++iter, ++counter) {

	  a_var_name.clear();
	  a_var_name << bind_name << counter;
	  attribut_list.extend< T >( a_var_name.str() );
	  attribut_list[counter].setValue< T >( *iter );

	  condition += sep;
	  condition += a_var_name.str();
	  sep = sep2;
	}
	condition += ")";
      }
      else {
	condition += " in (";
	typename std::vector<T>::const_iterator iter=search_list.begin();

	std::string quote(valueQuote<T>());
	condition += quote;
	condition += *iter;
	condition += quote;
	for(;
	    iter != search_list.end();
	    ++iter) {
	  condition += ',';
	  condition += quote;
	  condition += *iter;
	  condition += quote;
	}
	condition += ")";
      }
    }
  }
}


enum EValueType {kValFloat,kValInt, kValUInt, kValBool, kValString, kValExtendedAverageValue, kValAverageValue, kNValueTypes};
//---------------------------------------------------------------------------
void PixCoralClient::getAnalysisResultsFromDB(std::vector<CAN::SerialNumber_t> analysis_id,
					      const std::vector<std::string> &connName,
					      IPixResultsDbListener &listener) {

  if (analysis_id.empty()) return;
  struct timeval start_time, end_time;
  int total_usecs;
  unsigned int n_rows=0;

  if (m_verbose) {
    std::cout << "\n COOLCORAL Client: " << m_pixeltable <<" Putting table content in AnalysisResultList for:";
    std::cout<<"\n        analysis :" ;
    for (std::vector<CAN::SerialNumber_t>::const_iterator iter = analysis_id.begin();
	 iter != analysis_id.end();
	 ++iter) {
      std::cout << *iter;
    }
    std::cout << std::endl;
    gettimeofday(&start_time, NULL);
  }
  
  char *test=getenv("CANRESULTS_USE_ATTRL");
  bool use_attr_list=false;
  if (test && strcmp(test,"y")==0) {
    use_attr_list=true;
  }

  transactionStartReadOnly();

  // want to produce query : 
  // select t1.ANAL_ID, t2.CONNECTIVITY, t2.VARIABLE, t2.VALUE from 
  //   (select ANAL_ID,FK from CALIB_ANAL where ANAL_ID = 1394 ) t1,
  //   CALIB_ANAL_FLOAT t2 where (t2.FK=t1.FK);

  std::set<std::string> tables = m_session->nominalSchema().listTables();
  for ( std::set<std::string>::const_iterator tName = tables.begin();
	tName != tables.end(); ++tName ){

    if ((*tName).find(m_pixeltable+"_") != std::string::npos) {
      // only consider tables CALIB_ANAL_FLOAT etc. not CALIB_ANAL.

      // define output columns depending on the table type :
      // first identify table type
      std::string col_type;
      EValueType value_type=kNValueTypes;
      const coral::ITableDescription& tableDesc =
	m_session->nominalSchema().tableHandle(*tName).description();

      try {
	const coral::IColumn& colDesc = tableDesc.columnDescription("VALUE");

	col_type= colDesc.type();
	value_type=kNValueTypes;
	if (col_type == "float") {
	  value_type = kValFloat;
	}
	else if (col_type == "int") {
	  value_type=kValInt;
	}
	else if (col_type == "unsigned int") {
	  value_type=kValUInt;
	}
	else if (col_type == "bool") {
	  value_type=kValBool;
	}
	else if (col_type == "string") {
	  value_type=kValString;
	}
      }
      catch(coral::InvalidColumnNameException &) {

	try {
	  // check whether the columns exist
	  tableDesc.columnDescription("N");
	  tableDesc.columnDescription("MEAN");
	  tableDesc.columnDescription("RMS");
	  value_type = kValAverageValue;
	}
	catch(coral::InvalidColumnNameException &) {
	}

	try {
	  // check whether the columns exist
	  tableDesc.columnDescription("LOW99");
	  tableDesc.columnDescription("UP99");
	  value_type = kValExtendedAverageValue;
	}
	catch(coral::InvalidColumnNameException &) {
	}

      }
      if (value_type>=kNValueTypes) {
	std::cerr << "ERROR [CoralClient::getAnalysisResultsFromDB] Failed to identify value type for " << *tName << "."<< std::endl;
	continue;
      }

      // now define output columns
      std::auto_ptr<coral::IQuery> query( m_session->nominalSchema().newQuery() );
      if (!query.get()) {
	std::cerr << "ERROR [CoralClient::getAnalysisResultsFromDB] Failed to create queries for table " << *tName << "."<< std::endl;
	continue;
      }
      query->addToTableList(*tName, "V");

      query->addToOutputList("V.CONNECTIVITY");
      query->addToOutputList("V.VARIABLE");

      switch (value_type) {
      case kValAverageValue: {
	std::cerr << "ERROR [CoralClient::getAnalysisResultsFromDB] unsupported table " << *tName << std::endl;
	return;
      }
      case kValExtendedAverageValue: {
	query->addToOutputList("V.N");
	query->addToOutputList("V.MEAN");
	query->addToOutputList("V.RMS");
	query->addToOutputList("V.LOW99");
	query->addToOutputList("V.UP99");
	break;
      }
      default: {
	query->addToOutputList("V.VALUE");
      }
      }

      query->setRowCacheSize(2000);

      // select all entries which match the serial number and return the rod list key
      coral::IQueryDefinition& rod_list_query = query->defineSubQuery( "R" );
      rod_list_query.addToOutputList( "I.FK" ,"FK");
      rod_list_query.addToTableList("CALIB_ANAL","I");

      coral::AttributeList analysis_id_condition_inputs;
      std::string analysis_id_condition;
      createCondition<CAN::SerialNumber_t>(analysis_id, "I","ANAL_ID","id",analysis_id_condition_inputs,analysis_id_condition,use_attr_list);
      rod_list_query.setCondition(analysis_id_condition, analysis_id_condition_inputs);
      query->addToTableList( "R" );

      // Get the list of columns

      // prepare the query condition
      coral::AttributeList condition_inputs;
      std::string condition;
      condition = "R.FK = V.FK";
      if (!connName.empty() ) {
	condition+= " AND ";
	createCondition<std::string>(connName, "V","CONNECTIVITY","conn",condition_inputs, condition,use_attr_list);
      }
      query->setCondition(condition, condition_inputs);
      coral::ICursor& cursor = query->execute();

      while ( cursor.next() ) {

	  const coral::AttributeList &row = cursor.currentRow();
	  const std::string &conn = row[0].data<std::string>();
	  const std::string &the_var_name (row[1].data<std::string>());
	  n_rows++;
	  if (m_verbose) std::cout<< "    ("<<col_type << " -> " << value_type <<") =";

	    switch (value_type) {
	    case kValFloat: {
	      if (m_verbose) std::cout << row[2].data<float>();
	      listener.newValue( the_var_name, conn, row[2].data<float>() );
	      break; 
	    }
	    case kValInt: {
	      if (m_verbose) std::cout << row[2].data<int>();
	      listener.newValue( the_var_name, conn, static_cast<unsigned int>(row[2].data<int>()) );
	      break;
	    }
	    case kValUInt: {
	      if (m_verbose) std::cout << row[2].data<unsigned int>();
	      listener.newValue( the_var_name, conn, static_cast<unsigned int>(row[2].data<unsigned int>()) );
	      break;
	    }
	    case kValBool: {
	      if (m_verbose) std::cout << row[2].data<bool>();
	      listener.newValue( the_var_name, conn, row[2].data<bool>() );
	      break;
	    }
	    case kValString: {
	      if (m_verbose) std::cout << row[2].data<std::string>();
	      listener.newValue( the_var_name, conn,CLOBtoPixelMap(row[2].data<std::string>()));
	      break;
	    }
	    case kValExtendedAverageValue: {
	      if (m_verbose) {
		std::cout<<"    (avres)"
			 << the_var_name 
			 <<"="<<row[2].data<int>()
			 <<"/"<<row[3].data<float>()
			 <<"/"<<row[4].data<float>()
			 <<"/"<<row[5].data<float>()
			 <<"/"<<row[6].data<float>();
	      }

	      CAN::AverageResult_t avres(row[2].data<int>(),row[3].data<float>(),row[4].data<float>(),row[5].data<float>(),row[6].data<float>());
	      listener.newValue( the_var_name, conn,  avres );
	      break;
	    }
	    default:
	      std::cerr << "ERROR [CoralClient::getAnalysisResultsFromDB] Unsupported column type " << col_type << std::endl;
	    }
	    if (m_verbose) std::cout<< std::endl; 
	  }
    }
  }

  transactionCommit();

  if (m_verbose) { 
    std::cout << " Total  " << n_rows << "    analyses" << std::endl;

    gettimeofday(&end_time, NULL);
    total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
      (end_time.tv_usec-start_time.tv_usec);
    std::cout << " Total time was " << total_usecs << " usec" << std::endl;
  }
}

// helper function to store values into table rows.

// all types
template <typename T> inline void addValueToRow(coral::AttributeList &attribute_list, const T &value) {
  attribute_list["VALUE"].data<T>() = value;
}

template <typename T> inline void extendAttributeList(coral::AttributeList &attribute_list) {
  attribute_list.extend<T>("VALUE");
}


// special handling for AverageResult
template <> inline void addValueToRow<CAN::AverageResult_t>(coral::AttributeList &attribute_list, const CAN::AverageResult_t &value) {
  attribute_list["N"].setValue((int)value.n());
  attribute_list["MEAN"].setValue(value.value(CAN::AverageResult_t::kMean));
  attribute_list["RMS"].setValue(value.value(CAN::AverageResult_t::kRMS));
  attribute_list["LOW99"].setValue(value.value(CAN::AverageResult_t::kLower99));
  attribute_list["UP99"].setValue(value.value(CAN::AverageResult_t::kUpper99));
}

template <> inline void extendAttributeList<CAN::AverageResult_t>(coral::AttributeList &attribute_list) {
  attribute_list.extend<int>("N");
  attribute_list.extend<float>("MEAN");
  attribute_list.extend<float>("RMS");
  attribute_list.extend<float>("LOW99");
  attribute_list.extend<float>("UP99");
}


// special handling for PixelMap_t
template <> inline void addValueToRow<PixelMap_t>(coral::AttributeList &attribute_list, const PixelMap_t &value) {
  attribute_list["VALUE"].setValue(PixCoralClient::PixelMapToCLOB(value));
}

template <> inline void extendAttributeList<PixelMap_t>(coral::AttributeList &attribute_list) {
  attribute_list.extend<std::string>("VALUE");
}


// type name helper functions
template <typename T> inline std::string typeName() {
   return coral::AttributeSpecification::typeNameForType<T>();
}

template <> inline std::string typeName<CAN::AverageResult_t>() {
   return std::string("CAN::AverageResult_t");
}

template <> inline std::string typeName<PixelMap_t>() {
   return std::string("PixelMap_t");
}

void emptyNamesErrorMessage(unsigned int empty_var_names, unsigned int empty_conn_names, const std::string &table_name)
{
  if (empty_conn_names>0 || empty_var_names>0) {
    std::cerr << "ERROR [PixCoralClient::fillTable] Tried to fill ";
    if (empty_var_names>0) {
      std::cerr << empty_var_names << " result(s) without variable name ";
      if (empty_conn_names>0) {
	std::cerr << "and ";
      }
    }
    if (empty_conn_names>0) {
      std::cerr << empty_conn_names << " result(s) without connectivity name ";
    }
    std::cerr << " into " << table_name << std::endl;
  }
}

/** Template to fill a table with analysis results.
    Works only for simple types (one value to save)
*/
template <typename T>
int PixCoralClient::fillTable(long long fk, CAN::AnalysisResultList_t *results){

  if (m_verbose) std::cout << "\nCOOLCORAL Client: fill table for type "<< tableTypeName<T>() << std::endl;

  transactionStartUpdate();

  int nrows = 0;

  std::string table_name = m_pixeltable+"_"+tableTypeName<T>();
  unsigned int empty_var_names=0;
  unsigned int empty_conn_names=0;
  for (typename std::map<std::string, std::map<std::string, T> >::const_iterator iter=results->begin<T>();
       iter != results->end<T>();
       iter ++) {

    if (iter->first.empty()) {
      empty_var_names++;
      continue;
    }
    
    std::string tname = iter->first;
    if (m_verbose) std::cout << " Filling table " << table_name << " for variable " << tname  << std::endl;
    coral::ITableDataEditor& pixel_editor_2 = m_session->nominalSchema().tableHandle(table_name).dataEditor();
    coral::AttributeList pixel_row_2;
    pixel_row_2.extend<long long>("FK");
    pixel_row_2.extend<std::string>("CONNECTIVITY");
    pixel_row_2.extend<std::string>("VARIABLE");
    //    pixel_row_2.extend<T>("VALUE");
    extendAttributeList<T>(pixel_row_2);
    {
    std::auto_ptr<coral::IBulkOperation>  pixel_bulk_2(pixel_editor_2.bulkInsert(pixel_row_2,iter->second.size()));
    for (typename std::map<std::string, T >::const_iterator val_iter=iter->second.begin();
	 val_iter != iter->second.end();
	 val_iter ++) {

      if (val_iter->first.empty()) {
	empty_conn_names++;
	continue;
      }

      pixel_row_2["FK"].data<long long>() = fk;
      pixel_row_2["CONNECTIVITY"].data<std::string>() = val_iter->first;
      pixel_row_2["VARIABLE"].data<std::string>() = tname;
      //	pixel_row_2["VALUE"].data<T>() = val_iter->second;
      addValueToRow(pixel_row_2, val_iter->second);
      ++nrows;
      pixel_bulk_2->processNextIteration();
    }
    pixel_bulk_2->flush();
    }
  }

  if (empty_conn_names>0 || empty_var_names>0) {
    emptyNamesErrorMessage(empty_var_names, empty_conn_names, table_name);
  }

  transactionCommit();
  return nrows;
}



// /** Fill a table with analysis results of type AverageResult_t.
//     Couldn't use the templated fillTable<T> above because the object
//     structure is different...
// */
// int PixCoralClient::fillTableAverageResult(long long fk, CAN::AnalysisResultList_t *results){
//   if (m_verbose) std::cout << "\nCOOLCORAL Client: fill table for CAN::AverageResult_t" << std::endl;
//   int nrows = 0;
//   transactionStartUpdate();
//   for (std::map<std::string, std::map<std::string, CAN::AverageResult_t> >::const_iterator iter=results->begin<CAN::AverageResult_t>();
//        iter != results->end<CAN::AverageResult_t>();
//        iter ++) {
//     if (m_verbose) std::cout << " Filling table " << m_pixeltable+"_AVRES"  << std::endl;
//     coral::ITableDataEditor& pixel_editor_2 = m_session->nominalSchema().tableHandle(m_pixeltable+"_AVRES").dataEditor();
//     coral::AttributeList pixel_row_2;
//     pixel_editor_2.rowBuffer(pixel_row_2);
//     coral::IBulkOperation* pixel_bulk_2= pixel_editor_2.bulkInsert(pixel_row_2,iter->second.size());
//     for (std::map<std::string, CAN::AverageResult_t >::const_iterator val_iter=iter->second.begin();
// 	 val_iter != iter->second.end();
// 	 val_iter ++) {
//       pixel_row_2["FK"].setValue <long long> (fk);
//       pixel_row_2["CONNECTIVITY"].setValue <std::string> (val_iter->first);
//       pixel_row_2["VARIABLE"].setValue <std::string> (iter->first);
//       pixel_row_2["N"].setValue((int)val_iter->second.n());
//       pixel_row_2["MEAN"].setValue(val_iter->second.value(CAN::AverageResult_t::kMean));
//       pixel_row_2["RMS"].setValue(val_iter->second.value(CAN::AverageResult_t::kRMS));
//       pixel_row_2["LOW99"].setValue(val_iter->second.value(CAN::AverageResult_t::kLower99));
//       pixel_row_2["UP99"].setValue(val_iter->second.value(CAN::AverageResult_t::kUpper99));
//       ++nrows;
//       pixel_bulk_2->processNextIteration();
//     }
//     pixel_bulk_2->flush();
//     delete pixel_bulk_2;
//   }
//   transactionCommit();
//   return nrows;
// }



// /** Fill a table with analysis results of type PixelMap_t.
//     Couldn't use the templated fillTable<T> above because the CLOB
//     has to be constructed first.
// */
// int PixCoralClient::fillTableSpecialPixelMap(long long fk, CAN::AnalysisResultList_t *results){
//   if (m_verbose) std::cout << "\nCOOLCORAL Client: fill table for PixelMap_t" << std::endl;
//   int nrows = 0;
//   transactionStartUpdate();
//   for (std::map<std::string, std::map<std::string, PixelMap_t> >::const_iterator iter=results->begin<PixelMap_t>();
//        iter != results->end<PixelMap_t>();
//        iter ++) {
//     if (m_verbose) std::cout << " Filling table " << m_pixeltable+"_SPECIALPIXELS"  << std::endl;
//     coral::ITableDataEditor& pixel_editor_2 = m_session->nominalSchema().tableHandle(m_pixeltable+"_SPECIALPIXELS").dataEditor();
//     coral::AttributeList pixel_row_2;
//     pixel_editor_2.rowBuffer(pixel_row_2);
//     coral::IBulkOperation* pixel_bulk_2= pixel_editor_2.bulkInsert(pixel_row_2,iter->second.size());
//     for (std::map<std::string, PixelMap_t >::const_iterator val_iter=iter->second.begin();
// 	 val_iter != iter->second.end();
// 	 val_iter ++) {
//       pixel_row_2["FK"].setValue <long long> (fk);
//       pixel_row_2["CONNECTIVITY"].setValue <std::string> (val_iter->first);
//       pixel_row_2["VARIABLE"].setValue <std::string> (iter->first);
//       pixel_row_2["VALUE"].setValue(PixelMapToCLOB(val_iter->second));
//       ++nrows;
//       pixel_bulk_2->processNextIteration();
//     }
//     pixel_bulk_2->flush();
//     delete pixel_bulk_2;
//   }
//   transactionCommit();
//   return nrows;
// }



/** Fill a table with PixelCalibData -- A.X.
    Couldn't use the templated fillTable<T> above because the object
    structure is different...
*/
int PixCoralClient::fillTablePixelCalibData(long long fk, const char* option){
  if (m_verbose) std::cout << "\nCOOLCORAL Client: fill table for PixelCoralClientUtils::PixelCalibData" << std::endl;

  const char* filename = strstr(option,"file:");
  if (filename==0) {
    std::cout << "incorrect option (no file:)" << std::endl;
    return 0;
  }

  std::ifstream in(filename+5);
  if (!in.is_open()) {
    std::cout << "could not open calibration file" << std::endl;
    return 0;
  }

  int nrows = 0;
  transactionStartUpdate();

  {
    coral::ITableDataEditor& pixel_editor_2 = m_session->nominalSchema().tableHandle(m_pixeltable+"_PCD").dataEditor();
    coral::AttributeList pixel_row_2;
    pixel_editor_2.rowBuffer(pixel_row_2);
    int mysize = 1;
    coral::IBulkOperation* pixel_bulk_2= pixel_editor_2.bulkInsert(pixel_row_2,mysize);

    while (in) {
      int idmod; in >> idmod;
      if (in.eof()) break;

      bool ok = true;
      for (int i = 0; i<16; ++i) {
	char dummy; in >> dummy;
	if (dummy!='I') { ok = false; break; }
	int idchip; in >> idchip;
	if (idchip!=i) { ok = false; break; }

	pixel_row_2["FK"].setValue <long long> (fk);
	pixel_row_2["IDMOD"].setValue <int> (idmod);
	pixel_row_2["CHIP"].setValue <int> (idchip);
	int thres, sigma, noise, timewalk;
	in >> thres >> sigma >> noise >> timewalk;
	pixel_row_2["THRESHOLD1"].setValue <int> (thres);
	pixel_row_2["THRESHOLD2"].setValue <int> (sigma);
	pixel_row_2["THRESHOLD3"].setValue <int> (noise);
	pixel_row_2["THRESHOLD4"].setValue <int> (timewalk);
	in >> thres >> sigma >> noise >> timewalk;
	pixel_row_2["THRESHOLDLONG1"].setValue <int> (thres);
	pixel_row_2["THRESHOLDLONG2"].setValue <int> (sigma);
	pixel_row_2["THRESHOLDLONG3"].setValue <int> (noise);
	pixel_row_2["THRESHOLDLONG4"].setValue <int> (timewalk);
	in >> thres >> sigma >> noise >> timewalk;
	pixel_row_2["THRESHOLDGANGED1"].setValue <int> (thres);
	pixel_row_2["THRESHOLDGANGED2"].setValue <int> (sigma);
	pixel_row_2["THRESHOLDGANGED3"].setValue <int> (noise);
	pixel_row_2["THRESHOLDGANGED4"].setValue <int> (timewalk);
	float A, E, C;
	in >> A >> E >> C;
	pixel_row_2["Q2TOT1"].setValue <float> (A);
	pixel_row_2["Q2TOT2"].setValue <float> (E);
	pixel_row_2["Q2TOT3"].setValue <float> (C);
	in >> A >> E >> C;
	pixel_row_2["Q2TOTGANGED1"].setValue <float> (A);
	pixel_row_2["Q2TOTGANGED2"].setValue <float> (E);
	pixel_row_2["Q2TOTGANGED3"].setValue <float> (C);
	float p1, p2;
	in >> p1 >> p2;
	pixel_row_2["TOTRES1"].setValue <float> (p1);
	pixel_row_2["TOTRES2"].setValue <float> (p2);

	++nrows;
	pixel_bulk_2->processNextIteration();
      }
      if (!ok) break;
    }
    if (m_verbose) std::cout << "PixelCalibData: loaded " << nrows << " chips" << std::endl;

    pixel_bulk_2->flush();
    delete pixel_bulk_2;
  }
  transactionCommit();
  return nrows;
}



//---------------------------------------------------------------------------
/** Get results for a particular analysis ID
    and fill an AnalysisResultList_t container
    Optionally get only variable "varname" and/or connectivity object connName
 */
CAN::AnalysisResultList_t PixCoralClient::getAnalysisResultsFromDB(CAN::SerialNumber_t anal_id, std::string varname, std::string connName){

  if (m_verbose) {
    std::cout << "\n COOLCORAL Client: " << m_pixeltable <<" Putting table content in AnalysisResultList for analysis " << anal_id;
    if (varname != "") std::cout<<", variable " << varname;
    if (connName != "") std::cout<<", object " << connName;
    std::cout<< std::endl;
  }

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  transactionStartReadOnly();

  CAN::AnalysisResultList_t anal_results;

  // Get all entries for anal_id
  coral::IQuery* query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();
  query->addToOutputList("ANAL_ID"); 
  query->defineOutputType("ANAL_ID",coral::AttributeSpecification::typeNameForType<CAN::SerialNumber_t>()); 
  query->addToOutputList("DATE"); 
  query->defineOutputType("DATE",coral::AttributeSpecification::typeNameForType<time_t>()); 
  query->addToOutputList("FK"); 
  std::string pixel_cond = m_pixeltable+".ANAL_ID = :analid";
  coral::AttributeList pixel_condData;
  pixel_condData.extend<CAN::SerialNumber_t>( "analid" );
  query->setCondition( pixel_cond, pixel_condData);
  pixel_condData[0].data<CAN::SerialNumber_t>() = anal_id;
  coral::ICursor& cursor = query->execute();

  int nRows = 0;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    if (m_verbose) {
      std::time_t timet = GMTtoLocalTime(row0[1].data<time_t>());
      std::string time = ctime(&timet);
      std::cout << "  ANAL_ID = " << row0[0].data<CAN::SerialNumber_t>() 
		<< "  DATE = " << time.substr(0,time.size()-1)
		<< "   FK = " << row0[2].data<long long>() << std::endl;
    }
    //row0.toOutputStream(std::cout)<<endl;
    ++nRows;
    // Show the variables
    std::set<std::string> tables = m_session->nominalSchema().listTables();
    for ( std::set<std::string>::const_iterator tName = tables.begin();
	tName != tables.end(); ++tName ){
      if ((*tName).find(m_pixeltable+"_") != std::string::npos) {
	coral::IQuery* query_2 = m_session->nominalSchema().tableHandle(*tName).newQuery();

	// Get the list of columns
	const coral::ITableDescription& tableDesc =
	  m_session->nominalSchema().tableHandle(*tName).description();
	int NumCol = tableDesc.numberOfColumns();
	std::vector<std::string> colName; colName.resize(NumCol);
	std::vector<std::string> colType; colType.resize(NumCol);
	//if (m_verbose) std::cout << "\n     Name: "<< *tName << std::endl;
	for(int i=0; i<NumCol; ++i){
	  const coral::IColumn& colDesc = tableDesc.columnDescription(i);     
	  colName[i] = colDesc.name();
	  colType[i] = colDesc.type();
	  query_2->addToOutputList(colDesc.name()); 
	  //if (m_verbose) std::cout <<"	"<<i<<": "<< colName[i] <<"	("<< colType[i]<<")"<< std::endl;
	}

	// Second query to get entries in each table
	std::string pixel_cond_2 = (*tName)+".FK = :analfk";
	coral::AttributeList pixel_condData_2;
	pixel_condData_2.extend<long long>( "analfk" );
	pixel_condData_2[0].data<long long>() = row0[2].data<long long>();
	if (varname != "") {
	  pixel_cond_2 += " AND "+(*tName)+".VARIABLE = :var";
	  pixel_condData_2.extend<std::string>("var");
	  pixel_condData_2["var"].data<std::string>() = varname;
	}
	if (connName != "") {
	  pixel_cond_2 += " AND "+(*tName)+".CONNECTIVITY = :conn";
	  pixel_condData_2.extend<std::string>("conn");
	  pixel_condData_2["conn"].data<std::string>() = connName;
	}
	query_2->setCondition( pixel_cond_2, pixel_condData_2);
	coral::ICursor& cursor_2 = query_2->execute();
	while ( cursor_2.next() ) {
	  const coral::AttributeList &row0_2 = cursor_2.currentRow();
	  const std::string conn = row0_2[1].data<std::string>();
 	  if (m_verbose) std::cout << "      conn = " << conn;
	  if (NumCol == 4) { // single value
	    if (m_verbose) std::cout<< "  ("<<colType[3]<<")"<<row0_2[2].data<std::string>()<<"=";
	    if (colType[3] == "float") {
	      if (m_verbose) std::cout << row0_2[3].data<float>();
	      anal_results.addValue<float>(row0_2[2].data<std::string>(),conn,row0_2[3].data<float>());
	    } else if (colType[3] == "int") {
	      if (m_verbose) std::cout << row0_2[3].data<int>();
	      anal_results.addValue<unsigned int>(row0_2[2].data<std::string>(),conn,row0_2[3].data<int>());
	    } else if (colType[3] == "bool") {
	      if (m_verbose) std::cout << row0_2[3].data<bool>();
	      anal_results.addValue<bool>(row0_2[2].data<std::string>(),conn,row0_2[3].data<bool>());
	    } else if (colType[3] == "string") {
	      if (m_verbose) std::cout << row0_2[3].data<std::string>();
	      anal_results.addValue<PixelMap_t>(row0_2[2].data<std::string>(),conn,CLOBtoPixelMap(row0_2[3].data<std::string>()));
	    }

	  } else { //AverageResult_t
	    if (m_verbose) std::cout<<"  (avres)"<<row0_2[2].data<std::string>()<<"="<<row0_2[3].data<int>()<<"/"<<row0_2[4].data<float>()<<"/"<<row0_2[5].data<float>()<<"/"<<row0_2[6].data<float>()<<"/"<<row0_2[7].data<float>();
	    CAN::AverageResult_t avres(row0_2[3].data<int>(),row0_2[4].data<float>(),row0_2[5].data<float>(),row0_2[6].data<float>(),row0_2[7].data<float>());
	    anal_results.addValue<CAN::AverageResult_t>(row0_2[2].data<std::string>(),conn,avres);
	  }
	  if (m_verbose) std::cout<< std::endl; 
	}
	delete query_2;
      }
    }
  }
  delete query;

  transactionCommit();

  if (m_verbose) printf(" Total  %d    analyses\n", nRows);

  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;
  return anal_results;
}






//---------------------------------------------------------------------------
/** Get results for a particular analysis ID and a vector of connectivity
    objects connName and fill an AnalysisResultList_t container
    Optionally get only variable "varname" 
 */
CAN::AnalysisResultList_t PixCoralClient::getAnalysisResultsFromDB(CAN::SerialNumber_t anal_id, const std::vector<std::string> &connName, std::string varname){

  if (m_verbose) {
    std::cout << "\n COOLCORAL Client: " << m_pixeltable <<" Putting table content in AnalysisResultList for analysis " << anal_id;
    if (varname != "") std::cout<<", variable " << varname;
    if (!connName.empty()) {
      std::cout<<", objects ";
      for (std::vector<std::string>::const_iterator it=connName.begin();
	   it != connName.end(); ++it)
	std::cout << *it <<", ";
    }
    std::cout<< std::endl;
  }

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  transactionStartReadOnly();

  CAN::AnalysisResultList_t anal_results;

  // Get all entries for anal_id
  coral::IQuery* query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();
  query->addToOutputList("ANAL_ID"); 
  query->defineOutputType("ANAL_ID",coral::AttributeSpecification::typeNameForType<CAN::SerialNumber_t>()); 
  query->addToOutputList("DATE"); 
  query->defineOutputType("DATE",coral::AttributeSpecification::typeNameForType<time_t>()); 
  query->addToOutputList("FK"); 
  std::string pixel_cond = m_pixeltable+".ANAL_ID = :analid";
  coral::AttributeList pixel_condData;
  pixel_condData.extend<CAN::SerialNumber_t>( "analid" );
  query->setCondition( pixel_cond, pixel_condData);
  pixel_condData[0].data<CAN::SerialNumber_t>() = anal_id;
  coral::ICursor& cursor = query->execute();

  int nRows = 0;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    if (m_verbose) {
      std::time_t timet = GMTtoLocalTime(row0[1].data<time_t>());
      std::string time = ctime(&timet);
      std::cout << "  ANAL_ID = " << row0[0].data<CAN::SerialNumber_t>() 
		<< "  DATE = " << time.substr(0,time.size()-1)
		<< "   FK = " << row0[2].data<long long>() << std::endl;
    }
    //row0.toOutputStream(std::cout)<<endl;
    ++nRows;
    // Show the variables
    std::set<std::string> tables = m_session->nominalSchema().listTables();
    for ( std::set<std::string>::const_iterator tName = tables.begin();
	tName != tables.end(); ++tName ){
      if ((*tName).find(m_pixeltable+"_") != std::string::npos) {
	coral::IQuery* query_2 = m_session->nominalSchema().tableHandle(*tName).newQuery();

	// Get the list of columns
	const coral::ITableDescription& tableDesc =
	  m_session->nominalSchema().tableHandle(*tName).description();
	int NumCol = tableDesc.numberOfColumns();
	std::vector<std::string> colName; colName.resize(NumCol);
	std::vector<std::string> colType; colType.resize(NumCol);
	for(int i=0; i<NumCol; ++i){
	  const coral::IColumn& colDesc = tableDesc.columnDescription(i);     
	  colName[i] = colDesc.name();
	  colType[i] = colDesc.type();
	  query_2->addToOutputList(colDesc.name()); 
	  //if (m_verbose) std::cout <<"	"<<i<<": "<< colName[i] <<"	("<< colType[i]<<")"<< std::endl;
	}

	// Second query to get entries in each table
	std::string pixel_cond_2 = (*tName)+".FK = :analfk";
	coral::AttributeList pixel_condData_2;
	pixel_condData_2.extend<long long>( "analfk" );
	pixel_condData_2[0].data<long long>() = row0[2].data<long long>();
	if (varname != "") {
	  pixel_cond_2 += " AND "+(*tName)+".VARIABLE = :var";
	  pixel_condData_2.extend<std::string>("var");
	  pixel_condData_2["var"].data<std::string>() = varname;
	}
	if (!connName.empty()) {
	  // now restrict to certain connectivity names.
	  pixel_cond_2 += " AND (";
	  for (std::vector<std::string>::const_iterator conn_iter=connName.begin();
	       conn_iter != connName.end();
	       ++conn_iter) {
	    unsigned int counter=pixel_condData_2.size();
	    std::stringstream var_name;
	    var_name << "conn" << counter;
	    pixel_condData_2.extend<std::string>( var_name.str() );
	    assert( pixel_condData_2.size() > counter ); // sanity check
	    pixel_condData_2[counter].data<std::string>() = *conn_iter;

	    pixel_cond_2 += *tName;
	    pixel_cond_2 += ".CONNECTIVITY = :";
	    pixel_cond_2 += var_name.str();
	    if (connName.end()-conn_iter > 1 ) {
	      pixel_cond_2 += " OR ";
	    }
	  }
	  pixel_cond_2 += ")";
	}
	query_2->setCondition( pixel_cond_2, pixel_condData_2);
	coral::ICursor& cursor_2 = query_2->execute();
	while ( cursor_2.next() ) {
	  const coral::AttributeList &row0_2 = cursor_2.currentRow();
	  const std::string conn = row0_2[1].data<std::string>();
 	  if (m_verbose) std::cout << "      conn = " << conn;
	  if (NumCol == 4) { // single value
	    if (m_verbose) std::cout<< "  ("<<colType[3]<<")"<<row0_2[2].data<std::string>()<<"=";
	    if (colType[3] == "float") {
	      if (m_verbose) std::cout << row0_2[3].data<float>();
	      anal_results.addValue<float>(row0_2[2].data<std::string>(),conn,row0_2[3].data<float>());
	    } else if (colType[3] == "int") {
	      if (m_verbose) std::cout << row0_2[3].data<int>();
	      anal_results.addValue<unsigned int>(row0_2[2].data<std::string>(),conn,row0_2[3].data<int>());
	    } else if (colType[3] == "bool") {
	      if (m_verbose) std::cout << row0_2[3].data<bool>();
	      anal_results.addValue<bool>(row0_2[2].data<std::string>(),conn,row0_2[3].data<bool>());
	    } else if (colType[3] == "string") {
	      if (m_verbose) std::cout << row0_2[3].data<std::string>();
	      anal_results.addValue<PixelMap_t>(row0_2[2].data<std::string>(),conn,CLOBtoPixelMap(row0_2[3].data<std::string>()));
	    }

	  } else { //AverageResult_t
	    if (m_verbose) std::cout<<"  (avres)"<<row0_2[2].data<std::string>()<<"="<<row0_2[3].data<int>()<<"/"<<row0_2[4].data<float>()<<"/"<<row0_2[5].data<float>()<<"/"<<row0_2[6].data<float>()<<"/"<<row0_2[7].data<float>();
	    CAN::AverageResult_t avres(row0_2[3].data<int>(),row0_2[4].data<float>(),row0_2[5].data<float>(),row0_2[6].data<float>(),row0_2[7].data<float>());
	    anal_results.addValue<CAN::AverageResult_t>(row0_2[2].data<std::string>(),conn,avres);
	  }
	  if (m_verbose) std::cout<< std::endl; 
	}
	delete query_2;
      }
    }
  }
  delete query;

  transactionCommit();

  if (m_verbose) printf(" Total  %d    analyses\n", nRows);

  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;
  return anal_results;
}


/** access calibration data using tag / revision -- A.X.
 */
void PixCoralClient::getCalibrationDataFromDB(const char* option){
  if (m_verbose) {
    std::cout << "\n COOLCORAL Client: " << m_pixeltable << " loading calibration data using option " << option << std::endl;
  }

  struct timeval start_time, end_time;
  int total_usecs;
  gettimeofday(&start_time, NULL);

  transactionStartReadOnly();

  // Get all entries for anal_id
  coral::IQuery* query = m_session->nominalSchema().tableHandle(m_pixeltable).newQuery();

  query->addToOutputList("TAG");
  query->defineOutputType("TAG",coral::AttributeSpecification::typeNameForType<std::string>());
  query->addToOutputList("REVISION");
  query->defineOutputType("REVISION",coral::AttributeSpecification::typeNameForType<int>());
  query->addToOutputList("SOURCES");
  query->defineOutputType("SOURCES",coral::AttributeSpecification::typeNameForType<std::string>());
  query->addToOutputList("DATE");
  query->defineOutputType("DATE",coral::AttributeSpecification::typeNameForType<time_t>());
  query->addToOutputList("FK");

  // parse option
  std::string tag("");
  std::string revision("");
  std::string sources("");
  std::string delimiters(", ");
  std::string str(option);
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    if (str.substr(lastPos,4)=="tag:")
      tag = str.substr(lastPos+4, pos-lastPos-4);
    if (str.substr(lastPos,9)=="revision:")
      revision = str.substr(lastPos+9, pos-lastPos-9);
    if (str.substr(lastPos,8)=="sources:")
      sources = str.substr(lastPos+8, pos-lastPos-8);
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }

  coral::AttributeList pixel_condData;
  std::string pixel_cond = m_pixeltable+".TAG = :tag";
  pixel_condData.extend<std::string>( "tag" );
  pixel_condData[0].data<std::string>() = tag;
  // can make it conditional (e.g. if revision!="")
  pixel_cond += " AND "+m_pixeltable+".REVISION = :revision";
  pixel_condData.extend<int>( "revision" );
  pixel_condData["revision"].data<int>() = atoi(revision.c_str());
  query->setCondition( pixel_cond, pixel_condData);
  coral::ICursor& cursor = query->execute();

  int nRows = 0;
  while ( cursor.next() ) {
    const coral::AttributeList &row0 = cursor.currentRow();
    if (m_verbose) {
      std::time_t timet = GMTtoLocalTime(row0[3].data<time_t>());
      std::string time = ctime(&timet);
      std::cout << "  TAG = " << row0[0].data<std::string>() 
		<< "  REVISION = " << row0[1].data<int>()
		<< "  SOURCES = " << row0[2].data<std::string>()
		<< "  DATE = " << time.substr(0,time.size()-1)
		<< "  FK = " << row0[4].data<long long>() << std::endl;
    }
    //row0.toOutputStream(std::cout)<<endl;
    ++nRows;
    // Show the variables
    std::set<std::string> tables = m_session->nominalSchema().listTables();
    for ( std::set<std::string>::const_iterator tName = tables.begin();
	tName != tables.end(); ++tName ){
      if ((*tName).find(m_pixeltable+"_") != std::string::npos) {
	if (m_verbose) {
	  std::cout << "found table " << (*tName) << std::endl;
	}

	coral::IQuery* query_2 = m_session->nominalSchema().tableHandle(*tName).newQuery();

	// Get the list of columns
	const coral::ITableDescription& tableDesc =
	  m_session->nominalSchema().tableHandle(*tName).description();
	int NumCol = tableDesc.numberOfColumns();
	std::vector<std::string> colName; colName.resize(NumCol);
	std::vector<std::string> colType; colType.resize(NumCol);
	for(int i=0; i<NumCol; ++i){
	  const coral::IColumn& colDesc = tableDesc.columnDescription(i);     
	  colName[i] = colDesc.name();
	  colType[i] = colDesc.type();
	  query_2->addToOutputList(colDesc.name()); 
	  if (m_verbose) std::cout <<"	"<<i<<": "<< colName[i] <<"	("<< colType[i]<<")"<< std::endl;
	}

	// Second query to get entries in each table
	std::string pixel_cond_2 = (*tName)+".FK = :fk";
	coral::AttributeList pixel_condData_2;
	pixel_condData_2.extend<long long>( "fk" );
	pixel_condData_2[0].data<long long>() = row0[4].data<long long>();
	query_2->setCondition( pixel_cond_2, pixel_condData_2);
	coral::ICursor& cursor_2 = query_2->execute();
	// TO BE FIXED: the whole thing should not go to a single pcd :)
	PixelCoralClientUtils::PixelCalibData pcd;
	while ( cursor_2.next() ) {
	  const coral::AttributeList &row0_2 = cursor_2.currentRow();
	  //int ix = 3;
	  //int idmod = row0_2[ix++].data<int>();
	  int ix = 4;
	  PixelCoralClientUtils::PixelCalibData::PixelChipSummaryData pcsd;
	  int idchip = row0_2[ix++].data<int>();
	  pcsd.setChipID(idchip);
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
	if (m_verbose) {
	  pcd.print();
	}
	delete query_2;
      }
    }
  }
  delete query;

  transactionCommit();

  if (m_verbose) printf(" Total  %d    analyses\n", nRows);

  gettimeofday(&end_time, NULL);
  total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
    (end_time.tv_usec-start_time.tv_usec);
  if (m_verbose) std::cout << " Total time was " << total_usecs << " usec" << std::endl;
}



/** CORAL helper to start a transaction in readonly mode
 */
void PixCoralClient::transactionStartReadOnly(){
  connect();
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(true /*ReadOnly*/);
}



/** CORAL helper to start a transaction in update mode
 */
void PixCoralClient::transactionStartUpdate(){
  connect();
  if (m_session->transaction().isActive()) m_session->transaction().rollback();
  m_session->transaction().start(false /*update*/);
}



/** CORAL helper to commit changes at the end of a transaction
 */
void PixCoralClient::transactionCommit(){
  if (m_session->transaction().isActive()) m_session->transaction().commit();
}

namespace PixA {
  /** Translate between chip and module coordinates.
   * copied from PixelDataContainer
   *
   * @authors Joern Grosse-Knetter <joern.grosse-knetter@uni-bonn.de>
   */
  class PixelCoord_t
  {
  public:
    static const int s_nChip = 16;
    static const int s_nPix  = 2880;
    static const int s_nSpecialPix  = 448;
    static const int s_nCol  = 18;
    static const int s_nRow  = 160;

    PixelCoord_t(int xval, int yval) {
      if(yval<s_nRow){ // chips 0-7
	m_row  = yval;
	m_col  = xval%s_nCol;
	m_chip = xval/s_nCol;
      } else{
	m_row  = 2*s_nRow-1- yval;
	m_col  = s_nCol-1  - xval%s_nCol;
	m_chip = s_nChip-1 - xval/s_nCol;
      }
    }

    PixelCoord_t(int chip, int col, int row) : m_chip(chip), m_col(col), m_row(row) {}

    int chip() const {return m_chip;}
    int col()  const {return m_col;}
    int row()  const {return m_row;}

    int pixel() const { return chip()*s_nPix+row()+s_nRow*col(); }

    int x() const {

      if(m_chip<s_nChip/2) {
	return m_chip*s_nCol+m_col;
      }
      else {
	return (s_nCol-1-m_col + s_nCol*(s_nChip-1-m_chip));
      }

    }

    int y() const {
      if (m_chip<s_nChip/2) {
	return m_row;
      }
      else {
	return 2*s_nRow-1-m_row;
      }
    }

  private:
    int m_chip;
    int m_col;
    int m_row;
  };



  unsigned int encodePixelID(unsigned int mod_column, unsigned int mod_row) {
    unsigned int chip = 0, column = 0, row = 0;

    PixelCoord_t pix(mod_column, mod_row);
    chip = pix.chip();
    column = pix.col();
    row = pix.row();

    return PixelCoralClientUtils::ModuleSpecialPixelMap::encodePixelID(chip,column,row);

  }

}

/**
  Converts a CAN::PixelMap_t object to a CLOB compatible with
  ModuleSpecialPixelMap
*/
std::string PixCoralClient::PixelMapToCLOB(const PixelMap_t & pixmap) {
  std::map<unsigned int, unsigned int> pixels;
  for (PixelMap_t::const_iterator it=pixmap.begin(); it != pixmap.end(); ++it) {
    pixels[PixA::encodePixelID((*it).first.first,(*it).first.second)] = (*it).second;
  }
  unsigned int module_status = 0;
  std::vector<unsigned int> chip_status;
  std::vector<std::vector<unsigned int> > column_pair_status;
  PixelCoralClientUtils::ModuleSpecialPixelMap special = PixelCoralClientUtils::ModuleSpecialPixelMap(pixels,module_status,chip_status,column_pair_status);
  special.markSpecialRegions();
  //std::cout<<"CLOB: "<<special.to_string()<<std::endl;;
  return special.to_string();
}

/**
  Converts a CLOB compatible with ModuleSpecialPixelMap to a
  CAN::PixelMap_t object
*/
PixelMap_t PixCoralClient::CLOBtoPixelMap(const std::string & clob) {
  PixelCoralClientUtils::ModuleSpecialPixelMap special;
  special.fill_from_string(clob);
  PixelMap_t pixels;
  //std::cout<<"CLOB content: "<<std::endl;
  unsigned int fe, col, row;
  for (PixelCoralClientUtils::ModuleSpecialPixelMap::const_iterator it=special.begin(); it != special.end(); ++it) {
    //std::cout<<" "<<it->first<<" "<<it->second<<" ";
    fe = it->first%16;
    col = (it->first >> 4)%32;
    row = (it->first>>9);
    //std::cout<<fe<<" "<<col<<"/"<<row<<std::endl;
    PixA::PixelCoord_t pixel_coord(fe, col, row);
    pixels[std::make_pair(pixel_coord.x(), pixel_coord.y())] = it->second;//CLA
  }
  if (special.moduleStatus() != 0) {
    unsigned int status = special.moduleStatus();
    //    if (m_verbose) std::cout<<"Special module with status "<<status<<std::endl;
    for (unsigned int chip=0; chip<16; ++chip) {
      for (unsigned int c=0; c<18; ++c) {
	for (unsigned int r=0; r<160; ++r) {
	  PixA::PixelCoord_t pixel_coord(chip, c, r);
	  pixels[std::make_pair(pixel_coord.x(), pixel_coord.y())] = status;

	}
      }
    }
  } else if (special.hasSpecialChips()) {
    for (unsigned int chip=0; chip<16; ++chip) {
      unsigned int status = special.chipStatus(chip);
      if (status != 0) {
	//if (m_verbose) std::cout<<"Special chip "<<chip<<" with status "<<status<<std::endl;
	for (unsigned int c=0; c<18; ++c) {
	  for (unsigned int r=0; r<160; ++r) {
	    PixA::PixelCoord_t pixel_coord(chip, c, r);
	    pixels[std::make_pair(pixel_coord.x(), pixel_coord.y())] = status;
	  }
	}
      }
    }
  }
  if (special.hasSpecialColumnPairs()) {
    for (unsigned int chip=0; chip<16; ++chip) {
      if (special.hasSpecialColumnPairs(chip)) {
	for (unsigned int pair=0; pair<9; ++pair) {
	  unsigned int status = special.columnPairStatus(chip,pair);
	  if (status != 0) {
	    //	    if (m_verbose) std::cout<<"Special pair "<<pair<<" on chip "<<chip<<" with status "<<status<<std::endl;
	    for (int i=0; i<160; ++i) {
	      PixA::PixelCoord_t pixel_coord(chip, pair*2, i);
	      pixels[std::make_pair(pixel_coord.x(),pixel_coord.y())] = status;
	      PixA::PixelCoord_t pixel_coord2(chip, pair*2+1, i);
	      pixels[std::make_pair(pixel_coord2.x(),pixel_coord2.y())] = status;
	    }
	  }
	}
      }
    }
  }
    
  return pixels;
}



//------------------------------------------------
/// Get value from PVSS archive
//------------------------------------------------
double PixCoralClient::get_value_from_PVSSarch(std::string element_name, const coral::TimeStamp &since, const coral::TimeStamp &until)
{
  double result=0;

  transactionStartReadOnly();

  // Perform a query involving a subquery.

    coral::IQuery* query = m_session->nominalSchema().newQuery();
    query->addToOutputList( "A.ELEMENT_NAME", "NAME" );
    query->addToOutputList( "A.ALIAS", "ALIAS" );
    query->addToOutputList( "B.TS", "TS" );
    query->addToOutputList( "B.VALUE_NUMBER", "VALUE" );

    coral::IQueryDefinition& subQuery1 = query->defineSubQuery( "A" );
    subQuery1.addToOutputList( "ELEMENT_ID" );
    subQuery1.addToOutputList( "ELEMENT_NAME" );
    subQuery1.addToOutputList( "ALIAS" );
    subQuery1.addToTableList( "ELEMENTS" );
    coral::AttributeList cond1;
    cond1.extend<std::string>( "el_name" );
    subQuery1.setCondition( "ELEMENT_NAME = :el_name", cond1 );
    query->addToTableList( "A" );

    coral::IQueryDefinition& subQuery2 = query->defineSubQuery( "B" );
    subQuery2.addToOutputList( "ELEMENT_ID" );
    subQuery2.addToOutputList( "TS" );
    subQuery2.addToOutputList( "VALUE_NUMBER");
    subQuery2.addToTableList( "EVENTHISTORY" );
    query->addToTableList( "B" );

    coral::AttributeList time;
    time.extend<coral::TimeStamp>( "since" );
    time.extend<coral::TimeStamp>( "until" );

    query->setCondition( "A.ELEMENT_ID = B.ELEMENT_ID AND B.TS > :since AND B.TS < :until" , time);  
    query ->addToOrderList("B.TS DESC");


    cond1[0].data<std::string>() = element_name;
    time[0].data<coral::TimeStamp>() = since;
    time[1].data<coral::TimeStamp>() = until;

    coral::ICursor& cursor = query->execute();
    coral::AttributeList sel;
    if ( cursor.next() ){ sel = cursor.currentRow();

    result = sel[3].data<double>();
    std::cout << "Element name = " << sel[0].data<std::string>() << std::endl;
    std::cout << "Alias = " << sel[1].data<std::string>() << std::endl;
    std::cout << "Value = " << result << std::endl;
    
}
    //    cursor.currentRow().toOutputStream( std::cout ) << std::endl;

    std::cout << "Result = " << result << std::endl;

    delete query;
    transactionCommit();

    return result;
  
}

//------------------------------------------------
/// Get value from PVSS archive
//------------------------------------------------
double PixCoralClient::get_values_from_PVSSarch(std::string element_name, const coral::TimeStamp &since, const coral::TimeStamp &until){

    double result=0;

    transactionStartReadOnly();

  // Perform a query involving a subquery.

    coral::IQuery* query = m_session->nominalSchema().newQuery();
    query->addToOutputList( "A.ELEMENT_NAME", "NAME" );
    query->addToOutputList( "B.TS", "TS" );
    query->addToOutputList( "B.VALUE_NUMBER", "VALUE" );

    coral::IQueryDefinition& subQuery1 = query->defineSubQuery( "A" );
    subQuery1.addToOutputList( "ELEMENT_ID" );
    subQuery1.addToOutputList( "ELEMENT_NAME" );
    subQuery1.addToTableList( "ELEMENTS" );
    coral::AttributeList cond1;
    cond1.extend<std::string>( "el_name" );
    subQuery1.setCondition( "ELEMENT_NAME = :el_name", cond1 );
    query->addToTableList( "A" );

    coral::IQueryDefinition& subQuery2 = query->defineSubQuery( "B" );
    subQuery2.addToOutputList( "ELEMENT_ID" );
    subQuery2.addToOutputList( "TS" );
    subQuery2.addToOutputList( "VALUE_NUMBER");
    subQuery2.addToTableList( "EVENTHISTORY" );
    query->addToTableList( "B" );

    coral::AttributeList time;
    time.extend<coral::TimeStamp>( "since" );
    time.extend<coral::TimeStamp>( "until" );

    query->setCondition( "A.ELEMENT_ID = B.ELEMENT_ID AND B.TS > :since AND B.TS < :until" , time);  
    query ->addToOrderList("B.TS DESC");


    cond1[0].data<std::string>() = element_name;
    time[0].data<coral::TimeStamp>() = since;
    time[1].data<coral::TimeStamp>() = until;

    coral::ICursor& cursor = query->execute();
    coral::AttributeList sel;
    //    if ( cursor.next() ){ 
    while ( cursor.next() ){ 
      sel = cursor.currentRow();
      result = sel[2].data<double>();
      //      cursor.currentRow().toOutputStream( std::cout ) << std::endl;
      std::cout << "Result = " << result << std::endl;
    }

    delete query;
    transactionCommit();

    return result;
  } 

//------------------------------------------------
/// Get value from PVSS archive
//------------------------------------------------
void PixCoralClient::get_alias_from_PVSSarch(){

  transactionStartReadOnly();

  // Perform a query involving a subquery.

    coral::IQuery* query = m_session->nominalSchema().tableHandle("ELEMENTS").newQuery();
    query->addToOutputList( "ELEMENT_NAME");
    query->addToOutputList( "ALIAS");

    //    query->setCondition( "A.ELEMENT_ID = B.ELEMENT_ID AND B.TS > :since AND B.TS < :until" , time);  
    //    query ->addToOrderList("B.TS DESC");


//     cond1[0].data<std::string>() = element_name;
//     time[0].data<coral::TimeStamp>() = since;
//     time[1].data<coral::TimeStamp>() = until;

    coral::ICursor& cursor = query->execute();
    coral::AttributeList sel;
    //    if ( cursor.next() ){ 
    std::cout << "NAME\t\t\t ALIAS" << std::endl;
    while ( cursor.next() ){ 
      sel = cursor.currentRow();
      std::string elname = sel[0].data<std::string>();
      std::string elalias = sel[1].data<std::string>();


      std::cout << elname << "\t\t\t " << elalias << std::endl;
    }

    delete query;
  transactionCommit();

    //    return result;
} 
