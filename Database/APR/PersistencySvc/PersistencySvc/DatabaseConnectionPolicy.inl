inline
pool::DatabaseConnectionPolicy::DatabaseConnectionPolicy():
  m_writeModeForExisting( pool::DatabaseConnectionPolicy::UPDATE ),
  m_writeModeForNonExisting( pool::DatabaseConnectionPolicy::RAISE_ERROR ),
  m_readMode( pool::DatabaseConnectionPolicy::READ )
{}

inline
pool::DatabaseConnectionPolicy::DatabaseConnectionPolicy( const pool::DatabaseConnectionPolicy& rhs ):
  m_writeModeForExisting( rhs.m_writeModeForExisting ),
  m_writeModeForNonExisting( rhs.m_writeModeForNonExisting ),
  m_readMode( rhs.m_readMode )
{}

inline pool::DatabaseConnectionPolicy&
pool::DatabaseConnectionPolicy::operator=( const pool::DatabaseConnectionPolicy& rhs )
{
  if (&rhs != this) {
    m_writeModeForExisting = rhs.m_writeModeForExisting;
    m_writeModeForNonExisting = rhs.m_writeModeForNonExisting;
    m_readMode = rhs.m_readMode;
  }
  return *this;
}

inline pool::DatabaseConnectionPolicy::Mode
pool::DatabaseConnectionPolicy::writeModeForExisting() const
{
  return m_writeModeForExisting;
}

inline pool::DatabaseConnectionPolicy::Mode
pool::DatabaseConnectionPolicy::writeModeForNonExisting() const
{
  return m_writeModeForNonExisting;
}

inline pool::DatabaseConnectionPolicy::Mode
pool::DatabaseConnectionPolicy::readMode() const
{
  return m_readMode;
}
