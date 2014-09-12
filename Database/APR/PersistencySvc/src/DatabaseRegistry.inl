inline pool::PersistencySvc::DatabaseHandler*
pool::PersistencySvc::DatabaseRegistry::lookupByFID( const std::string& fid )
{
  std::map< std::string, pool::PersistencySvc::DatabaseHandler* >::iterator idb = m_fidToDb.find( fid );
  if ( idb == m_fidToDb.end() ) return 0;
  else return idb->second;
}

inline pool::PersistencySvc::DatabaseHandler*
pool::PersistencySvc::DatabaseRegistry::lookupByPFN( const std::string& pfn )
{
  std::map< std::string, pool::PersistencySvc::DatabaseHandler* >::iterator idb = m_pfnToDb.find( pfn );
  if ( idb == m_pfnToDb.end() ) return 0;
  else return idb->second;
}

inline pool::PersistencySvc::DatabaseHandler*
pool::PersistencySvc::DatabaseRegistry::lookupByLFN( const std::string& lfn )
{
  std::map< std::string, pool::PersistencySvc::DatabaseHandler* >::iterator idb = m_lfnToDb.find( lfn );
  if ( idb == m_lfnToDb.end() ) return 0;
  else return idb->second;
}

inline std::size_t
pool::PersistencySvc::DatabaseRegistry::size() const
{
  return m_databases.size();
}

inline pool::PersistencySvc::DatabaseRegistry::iterator
pool::PersistencySvc::DatabaseRegistry::begin()
{
  return m_databases.begin();
}

inline pool::PersistencySvc::DatabaseRegistry::const_iterator
pool::PersistencySvc::DatabaseRegistry::begin() const
{
  return m_databases.begin();
}

inline pool::PersistencySvc::DatabaseRegistry::iterator
pool::PersistencySvc::DatabaseRegistry::end()
{
  return m_databases.end();
}

inline pool::PersistencySvc::DatabaseRegistry::const_iterator
pool::PersistencySvc::DatabaseRegistry::end() const
{
  return m_databases.end();
}
