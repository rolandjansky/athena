/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONDESCRIPTION_H
#define COLLECTIONBASE_ICOLLECTIONDESCRIPTION_H

#include <string>
#include <vector>
#include <typeinfo>


namespace pool {

  class ICollectionColumn;
  class ICollectionIndex;
  class ICollectionUniqueConstraint;
  class ICollectionFragment;

  /** 
   * @class ICollectionDescription ICollectionDescription.h CollectionBase/ICollectionDescription.h
   *
   * An interface used to define the properties of a collection to be constructed and to retrieve 
   * these properties after construction. The schema editor of the collection should be used for 
   * any modifications to these properties after construction. Only the top level collection 
   * fragment can contain the event reference column. The collection may also be constructed 
   * without this column but in that case it will be defined as a collection fragment which 
   * means, for instance, that it cannot be registered in a collection catalog. However, in that 
   * case another collection can add it to its collecton fragment list.
   */
  class ICollectionDescription
  {
  public:
    /// Returns the name of the collection and the top level collection fragment.
    virtual const std::string& name() const = 0;

    /// Returns the storage technology type of the collection.
    virtual const std::string& type() const = 0;

    /// Returns the connection to the database containing the collection.
    virtual const std::string& connection() const = 0;

    /** 
     * Returns the name reserved for the event reference Token column. If the name has not
     * been set by the user a default name is returned.
     */
    virtual const std::string& eventReferenceColumnName() const = 0;

    /**
     * Indicates whether the top level collection fragment contains the event reference column
     * and is therefore defined as a collection.
     */
    virtual bool hasEventReferenceColumn() const = 0;

    /**
     * Returns the number of columns (including the event reference column if it is used) in 
     * the collection fragment specified as input. If no collection fragment name is specified 
     * then the total number of columns is returned.
     *
     * @param fragmentName Name of collection fragment for which to return column count.
     */
    virtual int numberOfColumns( std::string fragmentName = "" ) const = 0;

    /**
     * Returns a description object for a column of the collection, given the name of
     * the column.
     *
     * @param columnName Name of column.
     */
    virtual const ICollectionColumn& column( const std::string& columnName ) const = 0;

    /// return pointer to Column or NULL if it's not found (will not throw exceptions)
    virtual const ICollectionColumn* columnPtr( const std::string& columnName ) const = 0;

    
    /**
     * Returns the number of Token columns (including the event reference column if it is used) in 
     * the collection fragment specified as input. If no collection fragment name is specified 
     * then the total number of Token columns in the collection is returned.
     *
     * @param fragmentName Name of collection fragment for which to return column count.
     */
    virtual int numberOfTokenColumns( std::string fragmentName = "" ) const = 0;

    /**
     * Returns the number of Token columns (including the event reference column if it is used) in 
     * the collection fragment specified as input.
     *
     * @param fragmentId Position of collection fragment in collection fragment chain.
     */
    virtual int numberOfTokenColumns( int fragmentId ) const = 0;


    /**
     * Returns a description object for a Token column of the collection, given the name of 
     * the column.
     *
     * @param columnName Name of column.
     */
    virtual const ICollectionColumn& tokenColumn( const std::string& columnName ) const = 0; 

    /**
     * Returns a description object for a Token column of the collection, given the position
     * of the column in its associated collection fragment and given the position of the
     * this fragment in the collection fragment chain.
     *
     * @param columnId Position of column in associated collection fragment.
     * @param fragmentId Position of associated collection fragment in fragment chain.
     */
    virtual const ICollectionColumn& tokenColumn( int columnId, int fragmentId ) const = 0; 

    /**
     * Returns a description object for a Token column of the collection, given the position
     * of the column in its associated collection fragment and given the name of the
     * this fragment.
     *
     * @param columnId Position of column in associated collection fragment.
     * @param fragmentName Name of associated collection fragment.
     */
    virtual const ICollectionColumn& tokenColumn( int columnId, const std::string& fragmentName ) const = 0; 

    /** 
     * Returns the number of Attribute columns in the collection fragment specified as input.
     * If no collection fragment is specified then the total number of Attribute columns in 
     * the collection is returned.
     * 
     * @param fragmentName Name of collection fragment.
     */
    virtual int numberOfAttributeColumns( std::string fragmentName = "" ) const = 0;

    /**
     * Returns the number of Attribute columns in the collection fragment specified as input. 
     *
     * @param fragmentId Position of collection fragment in collection fragment chain.
     */
    virtual int numberOfAttributeColumns( int fragmentId ) const = 0;

    /**
     * Returns a description object for an Attribute column of the collection, given the name of 
     * the column.
     *
     * @param columnName Name of column.
     */
    virtual const ICollectionColumn& attributeColumn( const std::string& columnName ) const = 0; 

    /**
     * Returns a description object for an Attribute column of the collection, given the position
     * of the column in its associated collection fragment and given the position of the this 
     * fragment in the collection fragment chain.
     *
     * @param columnId Position of column in associated collection fragment.
     * @param fragmentId Position of associated collection fragment in fragment chain.
     */
    virtual const ICollectionColumn& attributeColumn( int columnId, int fragmentId ) const = 0; 

    /**
     * Returns a description object for an Attribute column of the collection, given the position
     * of the column in its associated collection fragment and given the name of this fragment.
     *
     * @param columnId Position of column in associated collection fragment.
     * @param fragmentName Name of associated collection fragment.
     */
    virtual const ICollectionColumn& attributeColumn( int columnId, const std::string& fragmentName ) const = 0; 

/**
     * Returns the number of indices used by the collection.
     */
    virtual int numberOfIndices() const = 0;

    /**
     * Returns a description object for an index of the collection, given the name of the column on which
     * the index is applied.
     *
     * @param columnName Name of column on which index is applied.
     */
    virtual const ICollectionIndex& index( const std::string& columnName ) const = 0;

    /**
     * Returns a description object for an index of the collection, given the names of the columns on 
     * which the index is applied.
     *
     * @param columnNames Names of columns on which index is applied.
     */
    virtual const ICollectionIndex& index( const std::vector<std::string>& columnNames ) const = 0;

    /**
     * Returns a description object for an index of the collection, given the ID number of 
     * the index.
     *
     * @param indexId ID of index.
     */
    virtual const ICollectionIndex& index( int indexId ) const = 0;



/** 
     * Returns the number of unique constraints used by the collection.
     */
    virtual int numberOfUniqueConstraints() const = 0;

    /**
     * Returns a description object for a unique constraint on the collection, given the name 
     * of the column on which the unique constraint is set.
     *
     * @param columnName Name of column on which unique constraint is set.
     */
    virtual const ICollectionUniqueConstraint& uniqueConstraint( const std::string& columnName ) const = 0;

    /**
     * Returns a description object for a unique constraint on the collection, given the names 
     * of the columns on which the unique constraint is set.
     *
     * @param columnNames Names of columns on which unique constraint is set.
     */
    virtual const ICollectionUniqueConstraint& uniqueConstraint( const std::vector<std::string>& columnNames ) const = 0;

    /**
     * Returns a description object for a unique constraint of the collection, given the 
     * ID number of the unique constraint.
     *
     * @param constraintId ID number of unique constraint.
     */
    virtual const ICollectionUniqueConstraint& uniqueConstraint( int constraintId ) const = 0;

    /** 
     * Returns the number of collection fragments in the collection.
     */
    virtual int numberOfCollectionFragments() const = 0;

    /**
     * Returns a description object for a collection fragment of the collection,
     * given the name of the collection fragment.
     *
     * @param fragmentName Name of collection fragment.
     */ 
    virtual const pool::ICollectionFragment& collectionFragment( const std::string& fragmentName ) const = 0;

    /**
     * Returns a description object for a collection fragment of the collection,
     * given the position of the collection fragment in the collection fragment chain.
     *
     * @param fragmentId Position of collection fragment in fragment chain.
     */ 
    virtual const pool::ICollectionFragment& collectionFragment( int fragmentId ) const = 0;

    /**
     * Returns the name of a collection fragment of the collection, given the name of a column 
     * contained by the fragment.
     *
     * @param columnName Name of a column in collection fragment.
     */
    virtual const std::string& collectionFragmentName( const std::string& columnName ) const = 0;

    /**
     * Check if both Descriptions have the same columns
     *
     * @param rhs Collection description object to compare.
     */
    virtual bool equals( const ICollectionDescription& rhs ) const = 0;

    /**
     * Check if all columns from this Description are present in the rhs Description
     * and if they have the same type
     *
     * @param rhs Collection description object to compare.
     */
    virtual bool isSubsetOf( const ICollectionDescription& rhs ) const = 0;

  protected:
    /// Empty destructor.
    virtual ~ICollectionDescription() {}
  };

}

#endif

