/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONCOLUMN_H
#define COLLECTIONBASE_COLLECTIONCOLUMN_H

#include "CollectionBase/ICollectionColumn.h"

#include "CoralBase/AttributeSpecification.h"

#include <typeinfo>


namespace pool {

  /** 
   * @class CollectionColumn CollectionColumn.h CollectionBase/CollectionColumn.h
   *
   * An implementation of the ICollectionColumn interface for retrieving a description
   * of a column of a collection.
   */
  class CollectionColumn : virtual public pool::ICollectionColumn
  {
  public:
    /// Default constructor.
    CollectionColumn()
      : m_name( "" ),
        m_type( "" ),
        m_fragmentName( "" ),
        m_maxSize( 0 ),
        m_sizeIsFixed( true ),
        m_isUnique( false ),
        m_id( 0 ) {}

    /**
     * Constructor that takes the column properties as input.
     * 
     * @param name Name of column.
     * @param type Data type of column.
     * @param fragmentName Name of collection fragment that contains column.
     * @param maxSize Maximum size of column data type.
     * @param sizeIsFixed Flag to allow column data type to vary in size.
     */
    CollectionColumn( const std::string& name,
                      const std::string& type,
                      const std::string& fragmentName = "",
                      int maxSize = 0,
                      bool sizeIsFixed = true )
      : m_name( name ),
        m_type( type ),
        m_fragmentName( fragmentName ),
        m_maxSize( maxSize ),
        m_sizeIsFixed( sizeIsFixed ),
        m_isUnique( false ),
        m_id( 0 ) {}

    /// Copy constructor.
    CollectionColumn( const CollectionColumn& rhs )
	  : ICollectionColumn(),
	    m_name( rhs.m_name ),
	    m_type( rhs.m_type ),
	    m_fragmentName( rhs.m_fragmentName ),
	    m_maxSize( rhs.m_maxSize ),
	    m_sizeIsFixed( rhs.m_sizeIsFixed ),
	    m_isUnique( rhs.m_isUnique ),
	    m_id( rhs.m_id ) 
    {}
     
    /// Default destructor.
    ~CollectionColumn() {}

    /// Assignment operator.
    CollectionColumn& operator=( const CollectionColumn& rhs )
    {
      m_name = rhs.m_name;
      m_type = rhs.m_type;
      m_fragmentName = rhs.m_fragmentName;
      m_maxSize = rhs.m_maxSize;
      m_sizeIsFixed = rhs.m_sizeIsFixed;
      m_isUnique = rhs.m_isUnique;
      m_id = rhs.m_id; 
   
      return *this;
    }

    /// Equality operator.
    bool operator==( const CollectionColumn& rhs ) const
    {
      return
	 m_name == rhs.m_name &&
	 m_type == rhs.m_type &&
	 // m_fragmentName == rhs.m_fragmentName &&  //MN - does not matter?
	 m_maxSize == rhs.m_maxSize &&
	 m_sizeIsFixed == rhs.m_sizeIsFixed &&
	 m_isUnique == rhs.m_isUnique
	 // && m_id == rhs.m_id   //MN - does not matter?
	 ;
    }

    /// Inequality operator.
    bool operator!=( const CollectionColumn& rhs ) const
    {
      return !( *this == rhs );
    }
    
    /**
     * Sets the name of the column.
     *
     * @param name Name of column.
     */
    virtual void setName( const std::string& name ) { m_name = name; }

    /**
     * Sets the data type of the column.
     *
     * @param type Data type of column.
     */
    virtual void setType( const std::string& type ) { m_type = type; }

    /**
     * Sets the data type of the column.
     *
     * @param type Data type of column.
     */
    virtual void setType( const std::type_info& type )
    { m_type = coral::AttributeSpecification::typeNameForId( type ); }

    /**
     * Sets the name of the collection fragment that contains the column.
     *
     * @param name Name of collection fragment that contains column.
     */
    virtual void setCollectionFragmentName( const std::string& fragmentName )
    { m_fragmentName = fragmentName; }

    /**
     * Sets the maximum size of the column data type. This method is useful for data
     * of type string or blob.
     *
     * @param maxSize Maximum size of column data type.
     */
    virtual void setMaxSize( int maxSize ) { m_maxSize = maxSize; }

    /**
     * Sets a flag indicating whether the size of the column data type may vary.
     * This method is useful for data of type string or blob.
     *
     * @param sizeIsFixed Flag to allow column data type to vary in size.
     */
    virtual void setSizeIsFixed( bool sizeIsFixed ) { m_sizeIsFixed = sizeIsFixed; }

    /**
     * If the column has an index, sets the uniqueness of index.
     *
     * @param isUnique Flag indicating whether index is unique.
     */
    virtual void setIsUnique( bool isUnique ) { m_isUnique = isUnique; }

    /// Sets the position of the column in its associated collection fragment.
    virtual void setId( int id ) { m_id = id; }
  
    /// Returns the user annotation for this column
    virtual void setAnnotation( const std::string& anno )  { m_annotation = anno; }

    /// Returns the name of the column.
    virtual const std::string& name() const { return m_name; }

    /// Returns the data type of the column.
    virtual const std::string& type() const { return m_type; }

    /// Returns the user annotation for this column
    virtual const std::string& annotation() const { return m_annotation; }

    /// Returns the name of the collection fragment that contains the column.
    virtual const std::string& collectionFragmentName() const { return m_fragmentName; }

    /**
     * Returns the maximum size of the column data type. This information is useful for 
     * data of type string or blob.
     */
    virtual int maxSize() const { return m_maxSize; }

    /**
     * Indicates whether the data type of the column can vary in size. This information 
     * is useful for data of type string or blob.
     */
    virtual bool sizeIsFixed() const { return m_sizeIsFixed; }

    /// If the column has an index, returns a flag indicating whether the index is unique.
    virtual bool isUnique() const { return m_isUnique; }

    /// Returns the position of the column in its associated collection fragment.
    virtual int id() const { return m_id; }

  private:
    /// Name of column.
    std::string m_name;

    /// Data type of column.
    std::string m_type;

    /// User annotation
    std::string m_annotation;
    
    /// Name of collection fragment that contains column.
    std::string m_fragmentName;

    /// Maximum size of column data type.
    int m_maxSize;

    /// Flag indicating whether data type of column can vary in size.
    bool m_sizeIsFixed;

    /// Flag indicating whether index on column is unique.
    bool m_isUnique;

    /// Position of column in associated collection fragment.
    int m_id;
  };

}

#endif


