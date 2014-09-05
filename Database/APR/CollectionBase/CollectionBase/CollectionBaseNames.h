/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONBASENAMES_H
#define COLLECTIONBASE_COLLECTIONBASENAMES_H

#include <string>


namespace pool {

  /**
   * @class CollectionBaseNames CollectionBaseNames.h CollectionBase/CollectionBaseNames.h
   *
   * Utility class to define string literals for the CollectionBase package.
   */
  class CollectionBaseNames 
  {
  public:
    /// The type name for objects of type pool::Token.
    static std::string tokenTypeName();

    /// The default name assigned to the event reference Token column of a collection.
    static std::string defaultEventReferenceColumnName();

    /// name of the key that holds CollectionID in the metadata
    static std::string CollIDMdataKey();
  };

}

#endif
