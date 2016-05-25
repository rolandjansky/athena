/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>

#include <mysql++.h>

int main( int argc, const char* argv[] )
{
  try
  {
    if( argc < 6 )
    {
      std::cerr << "Usage: MySQLppWriteTest "
                << "<table name> "
                << "<database name> "
                << "<database server name> "
                << "<database username> "
                << "<database password> "
                << "[number of rows DEFAULT 10]"
	        << std::endl;
      return 1;
    }

    std::string tableName = std::string( argv[1] );
    const char* dbName = argv[2];
    const char* dbServerName = argv[3];
    const char* dbUserName = argv[4];
    const char* dbPassWord = argv[5];
    unsigned int numRows = 10;
    if ( argc > 6 )
    {
      numRows = atoi( argv[6] );
    }

    mysqlpp::Connection* connect = new mysqlpp::Connection(true);

    connect->real_connect(dbName, 
                          dbServerName, 
                          dbUserName,
                          dbPassWord, 
                          3306, 
                          0, 
                          60, 
                          "/tmp/mysql.sock", 
                          0);
    mysqlpp::Query query = connect->query();
    query.exec("SET AUTOCOMMIT=0");

    std::string queryStrDrop = "";
    queryStrDrop += "DROP TABLE IF EXISTS " + tableName;
    query.exec( queryStrDrop );

    std::string queryStrCreate( "" );
    queryStrCreate += "CREATE TABLE " + tableName + " (";
    queryStrCreate += "test_bool BOOL, ";
    queryStrCreate += "test_smallint SMALLINT(6), ";
    queryStrCreate += "test_int INT(11), ";
    queryStrCreate += "test_bigint BIGINT(20), ";
    queryStrCreate += "test_smalluint SMALLINT(5) UNSIGNED, ";
    queryStrCreate += "test_uint INT(10) UNSIGNED, ";
    queryStrCreate += "test_biguint BIGINT(20) UNSIGNED, ";
    queryStrCreate += "test_float FLOAT, ";
    queryStrCreate += "test_double DOUBLE, ";
    queryStrCreate += "test_text TEXT ";
    queryStrCreate += ") TYPE = InnoDB";

    query.exec( queryStrCreate );

    std::string columns = "";
    columns += "test_bool, ";
    columns += "test_smallint, ";
    columns += "test_int, ";
    columns += "test_bigint, ";
    columns += "test_smalluint, ";
    columns += "test_uint, ";
    columns += "test_biguint, ";
    columns += "test_float, ";
    columns += "test_double, ";
    columns += "test_text";

    std::string values = "";
    char countStr[128] = "";
    char boolStr[128] = "";
    char smallIntStr[128] = "";
    char intStr[128] = "";
    char bigIntStr[128] = "";
    char smallUIntStr[128] = "";
    char uIntStr[128] = "";
    char bigUIntStr[128] = "";
    char floatStr[128] = "";
    char doubleStr[128] = "";
    std::string testStr = "";
    std::string queryStrInsert = "";
    for (unsigned int i=1; i<=numRows; i++)
    {
      std::sprintf(countStr, "%u", i);
      std::sprintf(boolStr, "%i", i%2);
      std::sprintf(smallIntStr, "%i", i+1);
      std::sprintf(intStr, "%i", i+2);
      std::sprintf(bigIntStr, "%i", i+3);
      std::sprintf(smallUIntStr, "%i", i+4);
      std::sprintf(uIntStr, "%i", i+5);
      std::sprintf(bigUIntStr, "%i", i+6);
      std::sprintf(floatStr, "%d", i+7);
      std::sprintf(doubleStr, "%d", i+8);
      testStr = "TestString" + std::string(countStr);

      values = "";
      values += std::string(boolStr) + ", ";
      values += std::string(smallIntStr) + ", ";
      values += std::string(intStr) + ", ";
      values += std::string(bigIntStr) + ", ";
      values += std::string(smallUIntStr) + ", ";
      values += std::string(uIntStr) + ", ";
      values += std::string(bigUIntStr) + ", ";
      values += std::string(floatStr) + ", ";
      values += std::string(doubleStr) + ", ";
      values += "'" + testStr + "'";

      queryStrInsert = "";
      queryStrInsert += "INSERT INTO " + tableName + " (";
      queryStrInsert += columns;
      queryStrInsert += ") VALUES (";
      queryStrInsert += values;
      queryStrInsert += ")\n";

      query.exec( queryStrInsert );  
    }
    
    query.exec( "COMMIT" );

    delete connect;
  }
  catch ( mysqlpp::BadQuery& badQueryException )
  {
    std::cerr << "mysqlpp::BadQuery " << badQueryException.what() << std::endl;
    return 1;
  }
}




