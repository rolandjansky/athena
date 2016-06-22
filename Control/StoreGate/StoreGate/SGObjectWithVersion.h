/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file SGObjectWithVersion.h
/// @brief associate a data object with its VersionedKey
#ifndef STOREGATE_SGOBJECTWITHVERSION_H
#define STOREGATE_SGOBJECTWITHVERSION_H 1

#include "SGTools/SGVersionedKey.h"
#include "StoreGate/DataHandle.h"
namespace SG {
  /// @class ObjectWithVersion
  /// @brief associate a data object with its VersionedKey
  /// The object is held by a DataHandle to delay its retrieval
  /// in case the user is interested only in the versionedKey
  template <typename T>
  class ObjectWithVersion {
  public:
    ObjectWithVersion(): versionedKey(), dataObject() {}
    ObjectWithVersion(const ObjectWithVersion& rhs):
      versionedKey(rhs.versionedKey), dataObject(rhs.dataObject) {}
    ObjectWithVersion(const VersionedKey& vk, const DataHandle<T>& dh):
      versionedKey(vk), dataObject(dh) {}
    ObjectWithVersion& operator= (const ObjectWithVersion& rhs)
    {
      if (this != &rhs) {
        versionedKey = rhs.versionedKey;
        dataObject = rhs.dataObject;
      }
      return *this;
    }
    SG::VersionedKey versionedKey;
    DataHandle<T> dataObject;
  };
}
/// sort according to highest key version
template <typename T>
bool operator < (const SG::ObjectWithVersion<T>& lhs,
                 const SG::ObjectWithVersion<T>& rhs) {
  return (lhs.versionedKey < rhs.versionedKey);
}

#endif
