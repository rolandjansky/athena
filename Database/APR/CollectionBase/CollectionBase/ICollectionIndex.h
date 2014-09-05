/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONINDEX_H
#define COLLECTIONBASE_ICOLLECTIONINDEX_H

#include <string>
#include <vector>


namespace pool {

  /** 
   * @class ICollectionIndex ICollectionIndex.h CollectionBase/ICollectionIndex.h
   *
   * An interface for retrieving a description of an index on one or more columns of a collection.
   */
  class ICollectionIndex
  {
  public:
    /// Returns the name of the index.
    virtual const std::string& name() const = 0;

    /// Returns the names of all columns on which the index is created.
    virtual const std::vector<std::string>& columnNames() const = 0;

    /// Indicates whether the combination of indexed column values must be different for each row. 
    virtual bool isUnique() const = 0;

  protected:
    /// Empty destructor.
    virtual ~ICollectionIndex() {}
  };

}

#endif


