/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbType.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================

#include "StorageSvc/pool.h"
#include "StorageSvc/DbType.h"
#include "POOLCore/DbPrint.h"
#include <cstdio>
using namespace pool;

DbType DbType::getType(const std::string& name)  {
  if ( "ROOT_Key" == name )
    return ROOTKEY_StorageType;
  else if ( "ROOT_Tree" == name )
    return ROOTTREE_StorageType;
  else if ( "ROOT_TreeIndex" == name )
    return ROOTTREEINDEX_StorageType;
  else if ( "ROOT_All" == name )
    return ROOT_StorageType;
  else if ( "POOL_RDBMS" == name )
    return POOL_RDBMS_StorageType;
  else if ( "POOL_RDBMS_HOMOGENEOUS" == name )
    return POOL_RDBMS_HOMOGENEOUS_StorageType;
  else if ( "POOL_RDBMS_POLYMORPHIC" == name )
    return POOL_RDBMS_POLYMORPHIC_StorageType;
  else if ( "Objy/DB" == name )
    return OBJY_StorageType;
  else if ( "POOL" == name )
    return POOL_StorageType;
  else if ( "MS Access" == name )
    return ACCESS_StorageType;
  else if ( "MS Excel" == name )
    return EXCEL_StorageType;
  else if ( "JET(Text)" == name )
    return TEXTJET_StorageType;
  else if ( "SQL Server" == name )
    return SQLSERVER_StorageType;
  else if ( "MySQL" == name )
    return MYSQL_StorageType;
  else if ( "Oracle" == name )
    return ORACLE_StorageType;
  else if ( "XML" == name )
    return XML_StorageType;
  else if ( "Test" == name )
    return TEST_StorageType;

  debugBreak("POOL", "Unknown storage type requested:"+name, true);  
  // Nothing real found; default is test
  return TEST_StorageType;
}

const std::string DbType::storageName()  const {
  if ( *this == TEST_StorageType          )
    return "Test";
  else if ( match(ROOTKEY_StorageType)    )
    return "ROOT_Key";
  else if ( match(ROOTTREE_StorageType)   )
    return "ROOT_Tree";
  else if ( match(ROOTTREEINDEX_StorageType)   )
    return "ROOT_TreeIndex";
  else if ( *this == ROOT_StorageType     )
    return "ROOT_All";
  else if ( match( POOL_RDBMS_HOMOGENEOUS_StorageType ) )
    return "POOL_RDBMS_HOMOGENEOUS";
  else if ( match( POOL_RDBMS_POLYMORPHIC_StorageType ) )
    return "POOL_RDBMS_POLYMORPHIC";
  else if ( *this == POOL_RDBMS_StorageType )
    return "POOL_RDBMS";
  else if ( *this == OBJY_StorageType     )
    return "Objy/DB";
  else if ( *this == POOL_StorageType     )
    return "POOL";
  else if ( *this == ACCESS_StorageType   )
    return "MS Access";
  else if ( *this == EXCEL_StorageType    )
    return "MS Excel";
  else if ( *this == TEXTJET_StorageType  )
    return "JET(Text)";
  else if ( *this == SQLSERVER_StorageType)
    return "SQL Server";
  else if ( *this == MYSQL_StorageType    )
    return "MySQL";
  else if ( *this == ORACLE_StorageType   )
    return "Oracle";
  else if ( *this == XML_StorageType   )
    return "XML";

  char nam[64];
  std::sprintf(nam,"%08X", type());
  return nam;
}

/// Error message on missing back-end driver implementation
void DbType::missingDriver( DbPrint& str) const  {
  str << DbPrintLvl::Fatal 
      << "The requested persistent backend implementation" << DbPrint::endmsg
      << "for the storage type:" << storageName()
      << " cannot be loaded.\n"
      << "Are you sure you loaded you loaded the correct DLLs?" 
      << DbPrint::endmsg;
  debugBreak("POOL", "Missing driver DLL.", true);
}

/// Check storage type
void DbType::badStorageType() const     {
  if ( doTrace() )    {
    DbPrint log( "DbType" );
    log << DbPrintLvl::Error
        << "--> Found bad or unknown storage TYPE [" 
        << int(type()) << "] !!!!" << DbPrint::endmsg;
  }
}
