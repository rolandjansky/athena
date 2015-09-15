/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistencySvcConfiguration.h"
#include "FileCatalog/IFileCatalog.h"

pool::PersistencySvc::PersistencySvcConfiguration::PersistencySvcConfiguration():
  m_fileCatalog( 0 )
{}
    
pool::PersistencySvc::PersistencySvcConfiguration::~PersistencySvcConfiguration()
{
}
      
void
pool::PersistencySvc::PersistencySvcConfiguration::setFileCatalog( pool::IFileCatalog& catalog )
{
  m_fileCatalog = &catalog;
}

pool::IFileCatalog&
pool::PersistencySvc::PersistencySvcConfiguration::fileCatalog()
{
  return *m_fileCatalog;
}

const pool::IFileCatalog&
pool::PersistencySvc::PersistencySvcConfiguration::fileCatalog() const
{
  return *m_fileCatalog;
}

void
pool::PersistencySvc::PersistencySvcConfiguration::setDatabaseConnectionPolicy( const pool::DatabaseConnectionPolicy& policy )
{
  m_policy = policy;
}

const pool::DatabaseConnectionPolicy&
pool::PersistencySvc::PersistencySvcConfiguration::databaseConnectionPolicy() const
{
  return m_policy;
}
