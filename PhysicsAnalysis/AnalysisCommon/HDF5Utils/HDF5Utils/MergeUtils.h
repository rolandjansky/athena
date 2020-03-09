/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HDF5Utils_MergeUtils_H
#define HDF5Utils_MergeUtils_H

#include "H5Cpp.h"
#include <string>

/**
 * @file MergeUtils
 *
 * Provides several helper functions for doing common parts of file merging.
 */

namespace H5Utils {
  /**
   * @brief Make sure that two datasets can be merged.
   * @param target The dataset to merge into
   * @param source The dataset to merge from
   * @param mergeAxis The axis to merged along.
   * @return False if the datasets cannot be merged
   */
  bool checkDatasetsToMerge(
      const H5::DataSet& target,
      const H5::DataSet& source,
      hsize_t mergeAxis);

  /**
   * @brief Make sure that two datasets can be merged.
   * @param target The dataset to merge into
   * @param source The dataset to merge from
   * @param mergeAxis The axis to merged along.
   * @param[out] errMsg If the datasets cannot be merged, fill this string with
   * an explanation
   * @return False if the datasets cannot be merged
   */
  bool checkDatasetsToMerge(
      const H5::DataSet& target,
      const H5::DataSet& source,
      hsize_t mergeAxis,
      std::string& errMsg);

  /**
   * @brief Merge two datasets
   * @param target The dataset to merge into
   * @param source The dataset to merge from
   * @param mergeAxis The axis to merged along.
   * @param bufferSize The maximum size of the buffer to use. Take care when
   * setting this, if it is too large then the job may run into memory issues!
   * This size is measured in bytes.
   *
   * Note that this does nothing to dataset attributes. This function ignores
   * the chunking of the source and target datasets, only splitting up the
   * source dataset along the merge axis.
   */
  void mergeDatasets(
      H5::DataSet& target,
      const H5::DataSet& source,
      hsize_t mergeAxis,
      std::size_t bufferSize = -1);

  /**
   * @brief Make a new dataset using the properties of another
   * @param targetLocation The location to place the new dataset
   * @param source The dataset to create from
   * @param mergeAxis The axis to merge along
   * @param chunkSize The chunk size to use. If negative then the chunk size
   * from the source is used.
   * @param mergeExtent The maximum extent to allow along the merge axis. -1
   * means unlimited.
   *
   * This will not merge the source dataset into the new one!
   */
  H5::DataSet createDataSet(
      H5::H5Location& targetLocation,
      const H5::DataSet& source,
      hsize_t mergeAxis,
      int chunkSize = -1,
      int mergeExtent = -1);

  /**
   * @brief Calculate the size of a row of a dataset in bytes
   * @param ds The dataset to use
   * @param axis The axis that the row is orthogonal to
   *
   * A row is the hyperplane orthogonal to the axis.
   * This will throw an overflow error if the row size overflows a std::size_t.
   * This is rather unlikely because that means that there wouldn't be enough
   * memory addresses to hold a single row in memory!
   */
  std::size_t getRowSize(const H5::DataSet& ds, hsize_t axis);

} //> end namespace H5Utils

#endif //> !HDF5Utils_MergeUtils_H
