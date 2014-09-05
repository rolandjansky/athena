/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONCURSOR_H
#define COLLECTIONBASE_ICOLLECTIONCURSOR_H


class Token;

namespace pool {

  class CollectionRowBuffer;

  /** 
   * @class ICollectionCursor ICollectionCursor.h CollectionBase/ICollectionCursor.h
   *
   * An interface used to navigate the result of a query on a collection.
   */
  class ICollectionCursor
  {
  public:
    /// Advances the cursor to the next row of the query result set.
    virtual bool next() = 0;

    /// Returns the selected Tokens and Attributes for the current row of the query result set.
    virtual const pool::CollectionRowBuffer& currentRow() const = 0;

    /// Returns the event reference Token for the current row.
    virtual const Token& eventRef() const = 0;

    /// Cleanup.
    virtual void close() = 0;

//  protected:   // MN: why protected?
    /// Empty destructor.
    virtual ~ICollectionCursor() {}
  };

}

#endif


