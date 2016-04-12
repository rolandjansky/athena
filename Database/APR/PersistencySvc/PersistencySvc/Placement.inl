inline
pool::Placement::Placement():
  m_technology( 0 ),
  m_dbID(""),
  m_contID(""),
  m_dbNameType( pool::DatabaseSpecification::UNDEFINED )
{}

inline
pool::Placement::Placement( const std::string& dbName,
                            pool::DatabaseSpecification::NameType dbNameType,
                            const std::string& container,
                            long technology ):
  m_technology( technology ),
  m_dbID( dbName ),
  m_contID( container ),
  m_dbNameType( dbNameType )
{}

inline void
pool::Placement::setDatabase( const std::string& dbName,
                              pool::DatabaseSpecification::NameType nameType )
{
  m_dbID = dbName;
  m_dbNameType = nameType;
}

inline const std::string&
pool::Placement::databaseName() const
{
  return m_dbID;
}

inline
pool::DatabaseSpecification::NameType
pool::Placement::databaseNameType() const
{
  return pool::DatabaseSpecification::PFN;
}

inline void
pool::Placement::setContainerName( const std::string& containerName )
{
  m_contID = containerName;
}

inline const std::string&
pool::Placement::containerName() const
{
  return m_contID;
}

inline void
pool::Placement::setTechnology( long technology )
{
  m_technology = technology;
}
 
inline long
pool::Placement::technology() const
{
  return m_technology;
}
