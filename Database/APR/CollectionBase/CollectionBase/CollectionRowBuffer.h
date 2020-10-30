/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONROWBUFFER_H
#define COLLECTIONBASE_COLLECTIONROWBUFFER_H

#include "CollectionBase/TokenList.h"

#include "CoralBase/AttributeList.h"
#include "CxxUtils/checker_macros.h"

#include <string>


namespace pool {

  /** 
   * @class CollectionRowBuffer CollectionRowBuffer.h CollectionBase/CollectionRowBuffer.h
   *
   * A class that acts as a buffer to be filled with the event reference and associated metadata
   * values of a given row to be inserted into a collection or collection fragment by its 
   * data editor object. The reference type row values are provided by the user in the form 
   * of POOL Token objects which are then internally converted to a database storable type. 
   * Note that this interface is only intended for data changes, not schema changes. 
   * To perform the latter operations one should use the schema editor object of the collection 
   * or collection fragment. The CollectionRowBuffer class may also act as a buffer to contain 
   * the rows of event references and associated metadata resulting from a query.
   */
  class CollectionRowBuffer
  {
  public:
    /// Default Constructor.
    CollectionRowBuffer();

    /**
     * Constructor taking Token list and Attribute list schema as input.
     *
     * @param tokenList List of Tokens
     * @param attributeList List of Attributes.
     */
    CollectionRowBuffer( const TokenList& tokenList, coral::AttributeList& attributeList );

    /**
     * Copy Constructor.
     *
     * @param rhs Object to be copied.
     */
    CollectionRowBuffer( const CollectionRowBuffer& rhs );

    /// Default destructor.
    ~CollectionRowBuffer();

    /**
     * Assignment operator.
     *
     * @param rhs Collection row buffer to copy.
     */
    CollectionRowBuffer& operator=( const CollectionRowBuffer& rhs );

    /**
     * Equality operator.
     *
     * @param rhs Collection row buffer to compare to.
     */
    bool operator==( const CollectionRowBuffer& rhs ) const;

    /**
     * Inequality operator.
     *
     * @param rhs Collection row buffer to compare to.
     */
    bool operator!=( const CollectionRowBuffer& rhs ) const;

    /**
     * Sets the Token list schema.
     *
     * @param tokenList List of Tokens.
     */
    void setTokenList( const TokenList& tokenList );

    /**
     * Sets the Attribute list schema.
     *
     * @param attributeList List of Attributes.
     */
    void setAttributeList( const coral::AttributeList& attributeList );

    /// Returns a reference to the list of Tokens.
    TokenList& tokenList();

    /// Returns a reference to the list of Attributes.
    coral::AttributeList& attributeList();

    /// Returns a constant reference to the list of Tokens.
    const TokenList& tokenList() const;

    /// Returns a constant reference to the list of Attributes.
    const coral::AttributeList& attributeList() const;

  private:
    /// List of Tokens.
    TokenList 				m_tokenList;

    /// List of Attributes.
    // Changed to a pointed to be able to avoid thread-safety checker
    // warnings about AttributeList.  We can change back to holding
    // this by value once those warnings are removed.
    coral::AttributeList*		m_attributeList;

    bool deleteAL ATLAS_NOT_THREAD_SAFE ();
  };
}

#endif

