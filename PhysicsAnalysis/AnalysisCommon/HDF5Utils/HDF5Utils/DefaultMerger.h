/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HDF5Utils_DefaultMerger_H
#define HDF5Utils_DefaultMerger_H

#include "HDF5Utils/IH5Merger.h"

/**
 * @file DefaultMerger
 * @author Jon Burr
 *
 * The default merging implementation
 */

namespace H5Utils {
  /**
   * @class Default H5 Merger
   */
  class DefaultMerger : public IH5Merger {
    public:
      /**
       * @brief Create the merger
       * @param mergeAxis The axis to merge along
       * @param chunkSize The chunk size to apply. If negative then the value
       * found in the input datasets will be used.
       * @param requireSameFormat Require all input files to have the same
       * groups and datasets.
       * @param bufferSize The maximum size of the buffer to use while merging
       * datasets
       * @param bufferInRows Whether the buffer size is specified in rows or
       * bytes
       */
      DefaultMerger(
          hsize_t mergeAxis = 0,
          int chunkSize = -1,
          bool requireSameFormat = true,
          std::size_t bufferSize = -1,
          bool bufferInRows = false);

      ~DefaultMerger();

      using IH5Merger::merge;
      using IH5Merger::createFrom;

      /**
       * @brief Merge a source group into a target group
       * @param target The group to merge into
       * @param source The group to merge from
       */
      void merge(H5::Group& target, const H5::Group& source) override;

      /**
       * @brief Merge a source dataset into a target dataset
       * @param target The dataset to merge into
       * @param source The dataset to merge from
       */
      void merge(H5::DataSet& target, const H5::DataSet& source) override;


      /**
       * @brief Make a new dataset from information in a source dataset
       * @param targetLocation Where the new dataset will be created
       * @param source The dataset to use to create the new dataset
       */
      H5::DataSet createFrom(
          H5::H5Location& targetLocation,
          const H5::DataSet& source) override;

    protected:
      /// The axis to merge along
      hsize_t m_mergeAxis;
      /// The chunk size to apply
      int m_chunkSize;
      /// Whether to require the same group structure
      bool m_requireSameFormat;
      /// The size of the buffer
      std::size_t m_bufferSize;
      /// Whether to measure the buffer in bytes or rows
      bool m_measureBufferInRows;
  }; //> end class DefaultMerger
} //> end namespace H5Utils

#endif //> !HDF5Utils_DefaultMerger_H
