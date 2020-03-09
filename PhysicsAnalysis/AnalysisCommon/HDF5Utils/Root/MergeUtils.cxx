/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HDF5Utils/MergeUtils.h"

#include <vector>
#include <stdexcept>

namespace {
  struct SmartMalloc {
    SmartMalloc() : data(nullptr) {}
    ~SmartMalloc() { this->freeData(); }
    operator bool() { return data != nullptr; }

    void* allocate(std::size_t size);
    void freeData();
    void* data;
  };


  void* SmartMalloc::allocate(std::size_t size) {
    // If we are already looking at memory, reallocate it
    if (data) {
      void* newData = realloc(data, size);
      if (!newData)
        // Note that we don't free 'data' here. That will still be taken care of
        // by the destructor. This means that a user can catch the exception if
        // they like and the old memory will still be available.
        throw std::bad_alloc{};
      else
        data = newData;
    }
    else {
      // We aren't looking at memory - use malloc
      data = malloc(size);
      if (!data)
        throw std::bad_alloc{};
    }
    return data;
  }

  void SmartMalloc::freeData() {
    // free does nothing to the nullptr so it's safe to call without a check
    free(data);
    // Make sure we know that we don't own anything
    data = nullptr;
  }

}

namespace H5Utils {
  bool checkDatasetsToMerge(
      const H5::DataSet& target,
      const H5::DataSet& source,
      hsize_t mergeAxis)
  {
    std::string sink;
    return checkDatasetsToMerge(target, source, mergeAxis, sink);
  }

  bool checkDatasetsToMerge(
      const H5::DataSet& target,
      const H5::DataSet& source,
      hsize_t mergeAxis,
      std::string& errMsg)
  {
    // Check that the datasets hold the same types
    // Note that H5 *can* do type comparisons but this function assumes that we
    // should only merge the same types
    if (target.getDataType() != source.getDataType() ) {
      errMsg = "Target and source datasets hold different types.";
      return false;
    }

    // Get the dataspaces
    H5::DataSpace targetSpace = target.getSpace();
    H5::DataSpace sourceSpace = source.getSpace();
    if (!targetSpace.isSimple() || !sourceSpace.isSimple() ) {
      errMsg = "Only simple dataspaces are understood.";
      return false;
    }

    // Make sure that the dataspaces have the same dimensions
    int nDims = targetSpace.getSimpleExtentNdims();
    if (nDims != sourceSpace.getSimpleExtentNdims() ) {
      errMsg = "Target and source dataspaces have different dimensions, " + 
          std::to_string(nDims) + " and " +
          std::to_string(sourceSpace.getSimpleExtentNdims() ) + " respectively";
      return false;
    }

    // Make sure that the merge axis fits in the dimension
    if (nDims <= static_cast<int>(mergeAxis)) {
      errMsg = "Dataset dimension " + std::to_string(nDims) +
        " is not compatible with the merge axis " +
        std::to_string(mergeAxis);
      return false;
    }

    // Now make sure that the extent matches
    std::vector<hsize_t> targetDims(nDims, 0);
    std::vector<hsize_t> maxTargetDims(nDims, 0);
    targetSpace.getSimpleExtentDims(targetDims.data(), maxTargetDims.data() );
    std::vector<hsize_t> sourceDims(nDims, 0);
    sourceSpace.getSimpleExtentDims(sourceDims.data() );
    
    for (int ii = 0; ii < nDims; ++ii) {
      // Skip the merge axis in this check
      if (ii == static_cast<int>(mergeAxis) )
        continue;
      if (targetDims.at(ii) != sourceDims.at(ii) ) {
        errMsg = "Target and source databases dimensions differ on axis " + 
          std::to_string(ii) + ", " + std::to_string(targetDims.at(ii) ) + 
          " and " + std::to_string(sourceDims.at(ii) ) + " respectively";
        return false;
      }
    }

    // Check the maximum extent is sufficient
    if (maxTargetDims.at(mergeAxis) < (
          targetDims.at(mergeAxis) + sourceDims.at(mergeAxis) ) ) {
      errMsg = "Merged dataset will not fit into target dataset";
      return false;
    }

    return true;
  } //> end function checkDatasetsToMerge

  void mergeDatasets(
      H5::DataSet& target,
      const H5::DataSet& source,
      hsize_t mergeAxis,
      std::size_t bufferSize)
  {
    std::string errMsg;
    if (!checkDatasetsToMerge(target, source, mergeAxis, errMsg) )
      throw std::invalid_argument(errMsg);

    // Get information about the target and source datasets
    H5::DataSpace targetSpace = target.getSpace();
    H5::DataSpace sourceSpace = source.getSpace();
    int nDims = targetSpace.getSimpleExtentNdims();

    // Now make sure that the extent matches
    std::vector<hsize_t> targetDims(nDims, 0);
    targetSpace.getSimpleExtentDims(targetDims.data() );
    std::vector<hsize_t> sourceDims(nDims, 0);
    sourceSpace.getSimpleExtentDims(sourceDims.data() );

    // Start by extending the target dataset
    std::vector<hsize_t> newDims = targetDims;
    newDims.at(mergeAxis) += sourceDims.at(mergeAxis);
    target.extend(newDims.data() );
    targetSpace.setExtentSimple(newDims.size(), newDims.data() );

    // Now we need to work out how far we need to subdivide the source dataset
    // to fit it inside the buffer. 
    std::size_t rowSize = getRowSize(source, mergeAxis);
    // How many rows can we fit into one buffer
    std::size_t nRowsBuffer = bufferSize / rowSize;
    if (nRowsBuffer == 0) 
      throw std::invalid_argument(
          "Allocated buffer is smaller than a single row! Merging is impossible.");

    // We have to allocate an area in memory for the buffer. Unlike normally in
    // C++ we aren't allocating a space for an object but a specific size. This
    // means that we have to use malloc.
    // Smart pointers require some annoying syntax to use with malloc, but we
    // can implement the same pattern with a simple struct.
    SmartMalloc buffer;

    // Keep track of the offset from the target dataset
    std::vector<hsize_t> targetOffset(nDims, 0);
    // Start it from its end point before we extended it
    targetOffset.at(mergeAxis) = targetDims.at(mergeAxis);

    // Step through the source dataset in increments equal to the number of
    // source rows that can fit into the buffer.
    std::size_t nSourceRows = sourceDims.at(mergeAxis);
    for (std::size_t iRow = 0; iRow < nSourceRows; iRow += nRowsBuffer) {
      // Construct the size and offset of the source slab
      std::vector<hsize_t> sourceOffset(nDims, 0);
      sourceOffset.at(mergeAxis) = iRow;
      // The number of rows to write
      std::size_t nRowsToWrite = std::min(nSourceRows-iRow, nRowsBuffer);
      std::vector<hsize_t> sourceSize(sourceDims);
      sourceSize.at(mergeAxis) = nRowsToWrite;
      // Create the source hyperslab
      sourceSpace.selectNone();
      sourceSpace.selectHyperslab(
          H5S_SELECT_SET,
          sourceSize.data(),
          sourceOffset.data() );

      // Create the target hyperslab
      targetSpace.selectNone();
      targetSpace.selectHyperslab(
          H5S_SELECT_SET,
          sourceSize.data(),
          targetOffset.data() );

      H5::DataSpace memorySpace(sourceSize.size(), sourceSize.data() );
      memorySpace.selectAll();

      // Prepare the buffer
      buffer.allocate(nRowsToWrite*rowSize);
      // Read into it
      source.read(buffer.data, source.getDataType(), memorySpace, sourceSpace);
      // Write from it
      target.write(buffer.data, target.getDataType(), memorySpace, targetSpace);
      // Increment the target offset
      targetOffset.at(mergeAxis) += nRowsToWrite;
    }
    // Sanity check - make sure that the final targetOffset is where we think it
    // should be
    if (targetOffset.at(mergeAxis) != newDims.at(mergeAxis) )
      throw std::logic_error(
          "Target dataset was not filled! This indicates a logic error in the code!");
  }

  H5::DataSet createDataSet(
      H5::H5Location& targetLocation,
      const H5::DataSet& source,
      hsize_t mergeAxis,
      int chunkSize,
      int mergeExtent)
  {
    H5::DataSpace sourceSpace = source.getSpace();
    // Get the new extent
    std::vector<hsize_t> DSExtent(sourceSpace.getSimpleExtentNdims(), 0);
    sourceSpace.getSimpleExtentDims(DSExtent.data() );
    // Set the merge axis to be 0 length to begin with
    DSExtent.at(mergeAxis) = 0;
    std::vector<hsize_t> maxDSExtent = DSExtent;
    maxDSExtent.at(mergeAxis) = mergeExtent;

    // Get the existing dataset creation properties
    H5::DSetCreatPropList cList = source.getCreatePlist();
    if (chunkSize > 0) {
      std::vector<hsize_t> chunks = DSExtent;
      chunks.at(mergeAxis) = chunkSize;
      cList.setChunk(chunks.size(), chunks.data() );
    }

    // Create the new space
    H5::DataSpace space(DSExtent.size(), DSExtent.data(), maxDSExtent.data());
    // This does nothing with the acc property list because I don't know
    // what it is
    return targetLocation.createDataSet(
        source.getObjName(), source.getDataType(), space, cList);
  }

  std::size_t getRowSize(const H5::DataSet& ds, hsize_t axis) {
    // The size of one element
    std::size_t eleSize = ds.getDataType().getSize();

    // The dimensions of the space
    H5::DataSpace space = ds.getSpace();
    std::vector<hsize_t> spaceDims(space.getSimpleExtentNdims(), 0);
    space.getSimpleExtentDims(spaceDims.data() );

    std::size_t nRowElements = 1;
    for (std::size_t ii = 0; ii < spaceDims.size(); ++ii)
      if (ii != axis)
        nRowElements *= spaceDims.at(ii);

    // Double check that this fits. This is probably over cautious but fine...
    if (std::size_t(-1) / nRowElements < eleSize)
      throw std::overflow_error("The size of one row would overflow the register!");

    return eleSize * nRowElements;
  }
} //> end namespace H5Utils
