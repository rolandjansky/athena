/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONINDEX_H
#define COLLECTIONBASE_COLLECTIONINDEX_H

#include "CollectionBase/ICollectionIndex.h"


namespace pool {

  /** 
   * @class CollectionIndex CollectionIndex.h CollectionBase/CollectionIndex.h
   *
   * An implementation of the ICollectionIndex interface for retrieving a description of an
   * index on one or more columns of a collection.
   */
  class CollectionIndex : virtual public ICollectionIndex
  {
  public:
    /// Default constructor.
    CollectionIndex()
      : m_name( "" ),
        m_columnNames(),
        m_isUnique( false ) {}

    /**
     * Constructor that takes the index properties as input.
     *
     * @param indexName Name of index.
     * @param columnNames Names of all columns on which index is created.
     * @param isUnique Flag indicating whether index is unique.
     */
    CollectionIndex( const std::string& indexName,
                     const std::vector< std::string >& columnNames,
                     bool isUnique = false )
      : m_name( indexName ),
        m_columnNames( columnNames ),
        m_isUnique( isUnique ) {}

    /// Copy constructor.
    CollectionIndex( const pool::CollectionIndex& rhs )
	  : ICollectionIndex(),
	    m_name( rhs.m_name ),
	    m_columnNames( rhs.m_columnNames ),
	    m_isUnique( rhs.m_isUnique )
    {}

    /// Default destructor.
    ~CollectionIndex() {}

    /// Assignment operator.
    CollectionIndex& operator=( const CollectionIndex& rhs )
    {
      m_name = rhs.m_name;
      m_columnNames = rhs.m_columnNames;
      m_isUnique = rhs.m_isUnique;
   
      return *this;
    }

    /// Equality operator.
    bool operator==( const CollectionIndex& rhs ) const
    {
      if ( m_name == rhs.m_name &&
           m_columnNames == rhs.m_columnNames &&
           m_isUnique == rhs.m_isUnique )
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    /// Inequality operator.
    bool operator!=( const CollectionIndex& rhs ) const
    {
      return ( ! ( *this == rhs ) );
    }

    /**
     * Sets the name of the index.
     *
     * @param name Name of index.
     */
    virtual void setName( const std::string& name ) { m_name = name; }

    /**
     * Returns the names of all columns on which the index is created, with possibility 
     * for direct modification.
     */
    virtual std::vector<std::string>& columnNames() { return m_columnNames; }

    /**
     * Sets the uniqueness of the index.
     *
     * @param isUnique Flag indicating whether index is unique.
     */
    virtual void setIsUnique( bool isUnique ) { m_isUnique = isUnique; }

    /// Returns the name of the index.
    virtual const std::string& name() const { return m_name; }

    /// Returns the names of all columns on which the index is created.
    virtual const std::vector<std::string>& columnNames() const { return m_columnNames; }

    /// Indicates whether the combination of indexed column values must be different for each row. 
    virtual bool isUnique() const { return m_isUnique; }

  private:
    /// Name of index.
    std::string m_name;
  
    /// Names of all columns on which index is created.
    std::vector< std::string > m_columnNames;

    /// Flag indicating whether index is unique.
    bool m_isUnique;
  };

}

#endif


