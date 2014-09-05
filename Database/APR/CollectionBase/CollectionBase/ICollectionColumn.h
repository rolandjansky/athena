/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONCOLUMN_H
#define COLLECTIONBASE_ICOLLECTIONCOLUMN_H

#include <string>


namespace pool {

  /** 
   * @class ICollectionColumn ICollectionColumn.h CollectionBase/ICollectionColumn.h
   *
   * An interface for retrieving a description of a column of a collection.
   */
  class ICollectionColumn
  {
  public:
    /// Returns the name of the column.
    virtual const std::string& name() const = 0;

    /// Returns the data type of the column.
    virtual const std::string& type() const = 0;

    /// Returns the user annotation for this column
    virtual const std::string& annotation() const = 0;

    /// Returns the name of the collection fragment that contains the column.
    virtual const std::string& collectionFragmentName() const = 0;

    /**
     * Returns the maximum size of the column data type. This information is useful for 
     * data of type string or blob.
     */
    virtual int maxSize() const = 0;

    /**
     * Indicates whether the data type of the column can vary in size. This information is 
     * useful for data of type string or blob.
     */
    virtual bool sizeIsFixed() const = 0;

    /// If the column has an index, returns a flag indicating whether the index is unique.
    virtual bool isUnique() const = 0;

    /// Returns the position of the column in its associated collection fragment.
    virtual int id() const = 0;

  protected:
    /// Empty destructor.
    virtual ~ICollectionColumn() {}
  };

}

#endif


