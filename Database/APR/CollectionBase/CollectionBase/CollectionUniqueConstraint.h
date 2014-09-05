/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONUNIQUECONSTRAINT_H
#define COLLECTIONBASE_COLLECTIONUNIQUECONSTRAINT_H

#include "CollectionBase/ICollectionUniqueConstraint.h"


namespace pool {

  /** 
   * @class CollectionUniqueConstraint CollectionUniqueConstraint.h CollectionBase/CollectionUniqueConstraint.h
   *
   * An implementation of the ICollectionUniqueConstraint interface for retrieving a description of a 
   * unique constraint on one or more columns of a collection.
   */
  class CollectionUniqueConstraint : virtual public ICollectionUniqueConstraint
  {
  public:
    /// Default constructor.
    CollectionUniqueConstraint()
      : m_name( "" ),
        m_columnNames() {}

    /**
     * Constructor that takes the constraint properties as input.
     * 
     * @param constraintName Name of constraint.
     * @param columnNames Names of columns on which constraint is set.
     */
    CollectionUniqueConstraint( const std::string& name, const std::vector< std::string >& columnNames )
      : m_name( name ),
        m_columnNames( columnNames ) {}

    /// Copy constructor.
    CollectionUniqueConstraint( const pool::CollectionUniqueConstraint& rhs )
	  : ICollectionUniqueConstraint(),
	    m_name( rhs.m_name ),
	    m_columnNames( rhs.m_columnNames )
    {}

    /// Default destructor.
    ~CollectionUniqueConstraint() {}

    /// Assignment operator.
    CollectionUniqueConstraint& operator=( const CollectionUniqueConstraint& rhs )
    {
      m_name = rhs.m_name;
      m_columnNames = rhs.m_columnNames;
   
      return *this;
    }

    /// Equality operator.
    bool operator==( const CollectionUniqueConstraint& rhs ) const
    {
      if ( m_name == rhs.m_name &&
           m_columnNames == rhs.m_columnNames )
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    /// Inequality operator.
    bool operator!=( const CollectionUniqueConstraint& rhs ) const
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
     * Returns the names of all columns on which the constraint is set,
     * with possibility for modification.
     */
    virtual std::vector<std::string>& columnNames() { return m_columnNames; }

    /// Returns the name of the constraint.
    virtual const std::string& name() const { return m_name; }

    /// Returns the names of all columns on which the constraint is set.
    virtual const std::vector<std::string>& columnNames() const { return m_columnNames; }

  private:
    /// Name of constraint.
    std::string m_name;

    /// Names of columns on which constraint is set.
    std::vector< std::string > m_columnNames;
  };

}

#endif


