/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TokenIterator.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/FileDescriptor.h"
#include "PersistencySvc/PersistencySvcException.h"


pool::PersistencySvc::TokenIterator::TokenIterator( const FileDescriptor& fileDescriptor,
                                                    const std::string& containerName,
                                                    IStorageExplorer& storageExplorer,
                                                    const std::string& selection ):
  m_storageExplorer( storageExplorer ),
  m_selection( new pool::DbSelect( selection ) )
{
   DbStatus sc = m_storageExplorer.select( fileDescriptor, containerName, *m_selection );
   if( sc.isError() )
      throw PersistencySvcException( std::string("Selection from ") + fileDescriptor.PFN()
				     + "(" + containerName + ") failed",
				     "TokenIterator::TokenIterator()" );
}

pool::PersistencySvc::TokenIterator::~TokenIterator()
{
  delete m_selection;
}


Token*
pool::PersistencySvc::TokenIterator::next()
{
  Token* objectToken = 0;
  if ( ! m_storageExplorer.next( *m_selection, objectToken ).isSuccess() ) return 0;
  else return objectToken;
}


bool
pool::PersistencySvc::TokenIterator::seek (long long int position)
{
  m_selection->link().second = int(position);
  return true;
}


int
pool::PersistencySvc::TokenIterator::size ()
{
  return m_selection->container().size();
}


