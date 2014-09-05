/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONFRAGMENT_H
#define COLLECTIONBASE_ICOLLECTIONFRAGMENT_H

#include <string>
#include <vector>


namespace pool {

  class ICollectionColumn;
  class CollectionColumn;

  /** 
   * @class ICollectionFragment ICollectionFragment.h CollectionBase/ICollectionFragment.h
   *
   * An interface for retrieving a description of a collection fragment of a collection.
   */
  class ICollectionFragment
  {
  public:
    /// Returns the name of the collection fragment.
    virtual const std::string& name() const = 0;

    /// Returns the name of the parent collection fragment of the collection fragment.
    virtual const std::string& parentCollectionFragmentName() const = 0;

    /** 
     * Returns a flag indicating whether the collection fragment references its parent collection 
     * fragment via foreign key.
     */
    virtual bool usesForeignKey() const = 0;

    /**
     * Returns the name of the foreign key used by the collection fragment to reference its
     * parent collection fragment. Returns an empty string if a foreign key is not being used.
     */
    virtual const std::string& foreignKeyName() const = 0;

    /// Returns the name of the child collection fragment of the collection fragment.
    virtual const std::string& childCollectionFragmentName() const = 0;

    /// Returns the Token column description objects of the collection fragment.
    virtual const std::vector< pool::CollectionColumn* >& tokenColumns() const = 0;

    /// Returns the Attribute column description objects of the collection fragment.
    virtual const std::vector< pool::CollectionColumn* >& attributeColumns() const = 0;

    /// Returns the position of the collection fragment in the collection fragment chain.
    virtual int id() const = 0;

  protected:
    /// Empty destructor.
    virtual ~ICollectionFragment() {}
  };

}

#endif


