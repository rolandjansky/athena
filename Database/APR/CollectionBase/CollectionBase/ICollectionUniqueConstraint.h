/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONUNIQUECONSTRAINT_H
#define COLLECTIONBASE_ICOLLECTIONUNIQUECONSTRAINT_H

#include <string>
#include <vector>


namespace pool {

  /** 
   * @class ICollectionUniqueConstraint ICollectionUniqueConstraint.h CollectionBase/ICollectionUniqueConstraint.h
   *
   * An interface for retrieving a description of a unique constraint on one or more columns of a collection.
   */
  class ICollectionUniqueConstraint
  {
  public:
    /// Returns the name of the constraint.
    virtual const std::string& name() const = 0;

    /// Returns the names of all columns on which the constraint is set.
    virtual const std::vector<std::string>& columnNames() const = 0;

  protected:
    /// Empty destructor.
    virtual ~ICollectionUniqueConstraint() {}
  };

}

#endif


