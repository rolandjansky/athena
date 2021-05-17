/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "Container.h"
#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbOption.h"
#include "TokenIterator.h"

pool::PersistencySvc::Container::Container( FileDescriptor& fileDescriptor,
                                            IStorageExplorer& storageExplorer,
                                            long technology,
                                            const std::string& name ):
  pool::IContainer( name ),
  m_fileDescriptor( fileDescriptor ),
  m_storageExplorer( storageExplorer ),
  m_technology( technology )
{}

pool::PersistencySvc::Container::~Container()
{}

pool::ITokenIterator*
pool::PersistencySvc::Container::tokens( const std::string& selection )
{
  return new pool::PersistencySvc::TokenIterator( m_fileDescriptor,
                                                  this->name(),
                                                  m_storageExplorer,
                                                  selection );
}

const std::string&
pool::PersistencySvc::Container::parentDatabaseName() const
{
  return m_fileDescriptor.FID();
}

long
pool::PersistencySvc::Container::technology() const
{
  return m_technology;
}

const pool::ITechnologySpecificAttributes&
pool::PersistencySvc::Container::technologySpecificAttributes() const
{
  return static_cast< const pool::ITechnologySpecificAttributes& >( *this );
}

pool::ITechnologySpecificAttributes&
pool::PersistencySvc::Container::technologySpecificAttributes()
{
  return static_cast< pool::ITechnologySpecificAttributes& >( *this );
}

bool
pool::PersistencySvc::Container::attributeOfType( const std::string& attributeName,
                                                  void* data,
                                                  const std::type_info& typeInfo,
                                                  const std::string& option )
{
  pool::DbOption containerOption( attributeName, option );
  pool::DbStatus sc = m_storageExplorer.getContainerOption( m_fileDescriptor,
                                                            this->name(),
                                                            containerOption );
  if ( !sc.isSuccess() ) return false;
  if ( containerOption.i_getValue( typeInfo, data ).isSuccess() ) {
    return true;
  }
  else {
    return false;
  }
}


bool
pool::PersistencySvc::Container::setAttributeOfType( const std::string& attributeName,
                                                     const void* data,
                                                     const std::type_info& typeInfo,
                                                     const std::string& option )
{
  pool::DbOption containerOption( attributeName, option );
  pool::DbStatus sc = containerOption.i_setValue( typeInfo, const_cast<void*>( data ) );
  if ( !sc.isSuccess() ) return false;
  sc = m_storageExplorer.setContainerOption( m_fileDescriptor,
                                             this->name(),
                                             containerOption );
  if ( sc.isSuccess() ) {
    return true;
  }
  else {
    return false;
  }
}
