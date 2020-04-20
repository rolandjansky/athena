/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HDF5Utils_IH5Merger_H
#define HDF5Utils_IH5Merger_H

#include "H5Cpp.h"

/**
 * @file IH5Merger.h
 * @author Jon Burr
 *
 * Provides a base class for H5Mergers
 */

namespace H5Utils {
  /**
   * @class Base class for H5Mergers
   *
   * A merger is responsible for merging two H5 objects.
   *
   * This class could be extended to allow for links, etc
   */
  class IH5Merger {
    public:
      virtual ~IH5Merger() = 0;

      /**
       * @brief Merge a source file into a target file
       * @param target The file to merge into
       * @param source The file to merge from
       *
       * The default implementation provided here just forwards this to the
       * group function.
       */
      virtual void merge(H5::H5File& target, const H5::H5File& source);

      /**
       * @brief Merge a source group into a target group
       * @param target The group to merge into
       * @param source The group to merge from
       */
      virtual void merge(H5::Group& target, const H5::Group& source) = 0;

      /**
       * @brief Merge a source dataset into a target dataset
       * @param target The dataset to merge into
       * @param source The dataset to merge from
       */
      virtual void merge(H5::DataSet& target, const H5::DataSet& source) = 0;

      /**
       * @brief Make a new group from information in a source group
       * @param targetLocation Where the new group will be created
       * @param source The group to use to create the new group
       *
       * The default implementation provided here just copies the source group's
       * name then uses the merge function.
       */
      virtual H5::Group createFrom(
          H5::H5Location& targetLocation,
          const H5::Group& source);

      /**
       * @brief Make a new dataset from information in a source dataset
       * @param targetLocation Where the new dataset will be created
       * @param source The dataset to use to create the new dataset
       */
      virtual H5::DataSet createFrom(
          H5::H5Location& targetLocation,
          const H5::DataSet& source) = 0;
  }; //> end class
} //> end namespace H5Utils

#endif //> !HDF5Utils_IH5Merger_H
