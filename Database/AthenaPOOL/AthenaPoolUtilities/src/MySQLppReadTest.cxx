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
      std::cerr << "Usage: MySQLppReadTest "
                << " <table name> "
                << " <database name> "
                << " <database server name> "
                << " <database username> "
                << " <database password> "
                << " [predicates for WHERE clause]"
	        << std::endl;
      return 1;
    }

    std::string tableName = std::string( argv[1] );
    const char* dbName = argv[2];
    const char* dbServerName = argv[3];
    const char* dbUserName = argv[4];
    const char* dbPassWord = argv[5];
    std::string predicates = "";
    if ( argc > 6 )
    {
      predicates = std::string( argv[6] );
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

    query << "SELECT * FROM " << tableName;
    if ( predicates != "" )
    {
      query << " WHERE " << predicates;
    }

    mysqlpp::ResUse* resUse = new mysqlpp::ResUse( query.use() );
    mysqlpp::Row row;
    unsigned int rowCounter = 0;

    for (;;)
    {
      if ( row = resUse->fetch_row() )
      {
        short testBool =  row.lookup_by_name("test_bool");
        short testSmallInt = row.lookup_by_name("test_smallint");
        short testInt = row.lookup_by_name("test_int");
        long testBigInt = row.lookup_by_name("test_bigint");
        short testSmallUInt = row.lookup_by_name("test_smalluint");
        short testUInt = row.lookup_by_name("test_uint");
        long testBigUInt = row.lookup_by_name("test_biguint");
        float testFloat = row.lookup_by_name("test_float");
        double testDouble = row.lookup_by_name("test_double");
	std::string testText = std::string( row.lookup_by_name("test_text") );
       
        std::cout << "Row number is " << ++rowCounter << std::endl; 
	std::cout << "test_bool is " << testBool << std::endl;
        std::cout << "test_smallint is " << testSmallInt << std::endl;
        std::cout << "test_int is " << testInt << std::endl;
        std::cout << "test_bigint is " << testBigInt << std::endl;
        std::cout << "test_smalluint is " << testSmallUInt << std::endl;
        std::cout << "test_uint is " << testUInt << std::endl;
        std::cout << "test_biguint is " << testBigUInt << std::endl;
        std::cout << "test_float is " << testFloat << std::endl;
        std::cout << "test_double is " << testDouble << std::endl;
	std::cout << "test_text is " << testText << std::endl;
        std::cout << std::endl;
      }
      else
      {
        break;
      }
    }

    delete resUse;
    delete connect;
  }
  catch ( mysqlpp::BadQuery& badQueryException )
  {
    std::cerr << "mysqlpp::BadQuery " << badQueryException.what() << std::endl;
    return 1;
  }
}




