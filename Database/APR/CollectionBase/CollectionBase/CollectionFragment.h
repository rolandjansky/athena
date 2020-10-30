/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONFRAGMENT_H
#define COLLECTIONBASE_COLLECTIONFRAGMENT_H

#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/CollectionColumn.h"


namespace pool {

  /** 
   * @class CollectionFragment CollectionFragment.h CollectionBase/CollectionFragment.h
   *
   * An implementation of the ICollectionFragment interface for retrieving a description of a
   * collection fragment of a collection.
   */
  class CollectionFragment : virtual public ICollectionFragment
  {
  public:
    /// Default constructor.
    CollectionFragment()
      : m_name( "" ),
        m_parentFragmentName( "" ),
        m_usesForeignKey( true ),
	m_foreignKeyName( "" ) ,
	m_childFragmentName( "" ),
        m_id( 0 )
    {}

    /**
     * Constructor that takes the collection fragment properties as input.
     * 
     * @param name Name of collection fragment to add to collection.
     * @param parentFragmentName Name of parent collection fragment.
     * @param usesForeignKey Flag indicating whether fragment uses foreign key to reference parent fragment.
     */
    CollectionFragment( const std::string& name,
                        const std::string& parentFragmentName = "",
                        bool usesForeignKey = true )
      : m_name( name ),
        m_parentFragmentName( parentFragmentName ),
        m_usesForeignKey( usesForeignKey ),
        m_foreignKeyName( "" ),
        m_childFragmentName( "" ),
        m_id( 0 )
    {}

    /// Copy constructor.
    CollectionFragment( const pool::CollectionFragment& rhs )
	  : ICollectionFragment(),
	    m_name( rhs.m_name ),
	    m_parentFragmentName( rhs.m_parentFragmentName ),
	    m_usesForeignKey( rhs.m_usesForeignKey ),
	    m_foreignKeyName( rhs.m_foreignKeyName ),
	    m_childFragmentName( rhs.m_childFragmentName ),
	    m_tokenColumns( rhs.m_tokenColumns ),
	    m_attributeColumns( rhs.m_attributeColumns ),
	    m_id( rhs.m_id )
    {}

    /// Default destructor.
    ~CollectionFragment() {}

    /// Assignment operator.
    CollectionFragment& operator=( const CollectionFragment& rhs )
    {
      m_name = rhs.m_name;
      m_parentFragmentName = rhs.m_parentFragmentName;
      m_usesForeignKey = rhs.m_usesForeignKey;
      m_foreignKeyName = rhs.m_foreignKeyName;
      m_childFragmentName = rhs.m_childFragmentName;
      m_tokenColumns = rhs.m_tokenColumns;
      m_attributeColumns = rhs.m_attributeColumns;
      m_id = rhs.m_id;
   
      return *this;
    }

    /// Equality operator.
    bool operator==( const CollectionFragment& rhs ) const
    {
       return  m_name == rhs.m_name &&
	  m_parentFragmentName == rhs.m_parentFragmentName &&
	  m_usesForeignKey == rhs.m_usesForeignKey &&
	  m_foreignKeyName == rhs.m_foreignKeyName &&
	  m_childFragmentName == rhs.m_childFragmentName &&
	  m_tokenColumns == rhs.m_tokenColumns &&
	  m_attributeColumns == rhs.m_attributeColumns &&
	  m_id == rhs.m_id;
    }

    /// Inequality operator.
    bool operator!=( const CollectionFragment& rhs ) const
    {
      return !( *this == rhs );
    }
    
    /**
     * Sets the name of the collection fragment.
     *
     * @param name Name of collection fragment.
     */
    virtual void setName( const std::string& name ) { m_name = name; }

    /**
     * Sets the name of the parent collection fragment of the collection fragment. If a foreign
     * key constraint was being used for a former parent collection fragment of the colletion 
     * fragment a new name is generated for the key.
     *
     * @param parentFragmentName Name of parent collection fragment.
     */
    virtual void setParentCollectionFragmentName( const std::string& parentFragmentName ) 
    { 
      m_parentFragmentName = parentFragmentName;
      if ( m_usesForeignKey ) {
        m_foreignKeyName = parentFragmentName + "_FK";
      }
    }

    /**
     * Sets a flag indicating whether the collection fragment references its parent collection fragment
     * via a foreign key constraint and assigns a unique name to this constraint if the flag is set to true.
     *
     * @param usesForeignKey Flag indicating whether fragment references its parent fragment via a foreign key.
     */
    virtual void setUsesForeignKey( bool usesForeignKey )
    { 
      m_usesForeignKey = usesForeignKey;
      if ( usesForeignKey ) {
        m_foreignKeyName = m_parentFragmentName + "_FK";
      }
    }

    /**
     * Sets the name of the child collection fragment of the collection fragment.
     *
     * @param childFragmentName Name of child collection fragment.
     */
    virtual void setChildCollectionFragmentName( const std::string& childFragmentName ) 
    { m_childFragmentName = childFragmentName; }

    /// Returns the Token column description objects of the collection fragment for performing updates.
    virtual std::vector< pool::CollectionColumn* >& tokenColumns() { return m_tokenColumns; }

    /// Returns the Attribute column description objects of the collection fragment for performing updates.
    virtual std::vector< pool::CollectionColumn* >& attributeColumns() { return m_attributeColumns; }

    /**
     * Sets the position of the collection fragment in the collection fragment chain.
     *
     * @param position Position of collection fragment in collection fragment chain.
     */
    virtual void setId( int id ) { m_id = id; }

    /// Returns the name of the collection fragment.
    virtual const std::string& name() const { return m_name; }

    /// Returns the name of the parent collection fragment of the collection fragment.
    virtual const std::string& parentCollectionFragmentName() const { return m_parentFragmentName; }

    /** 
     * Returns a flag indicating whether the collection fragment references its parent collection
     * fragment via a foreign key.
     */
    virtual bool usesForeignKey() const { return m_usesForeignKey; }

    /**
     * Returns the name of the foreign key used by the collection fragment to reference its
     * parent collection fragment. Returns an empty string if a foreign key is not being used.
     */
    virtual const std::string& foreignKeyName() const { return m_foreignKeyName; }

    /// Returns the name of the child collection fragment of the collection fragment.
    virtual const std::string& childCollectionFragmentName() const { return m_childFragmentName; }

    /// Returns the Token column description objects of the collection fragment.
    virtual const std::vector< pool::CollectionColumn* >& tokenColumns() const { return m_tokenColumns; }

    /// Returns the Attribute column description objects of the collection fragment.
    virtual const std::vector< pool::CollectionColumn* >& attributeColumns() const { return m_attributeColumns; }

    /// Returns the position of the collection fragment in the collection fragment chain.
    virtual int id() const { return m_id; }

  private:
    /// Name of collection fragment.
    std::string m_name;

    /// Name of parent collection fragment of collection fragment.
    std::string m_parentFragmentName;

    /// Flag indicating whether collection fragment references its parent collection fragment via a foreign key.
    bool m_usesForeignKey;

    /// Name of foreign key used by collection fragment to reference its parent collection fragment.
    std::string m_foreignKeyName;

    /// Name of child collection fragment of collection fragment.
    std::string m_childFragmentName;

    /// Token column description objects of collection fragment.
    std::vector< pool::CollectionColumn* >	m_tokenColumns;

    /// Attribute column description objects of collection fragment.
    std::vector< pool::CollectionColumn* >	m_attributeColumns;

    /// Position of collection fragment in collection fragment chain.
    int m_id;
  };

}

#endif


