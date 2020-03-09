/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HDF5Utils/DefaultMerger.h"
#include "HDF5Utils/MergeUtils.h"
#include <exception>
#include <iostream>

namespace H5Utils {

  DefaultMerger::DefaultMerger(
      hsize_t mergeAxis,
      int chunkSize,
      bool requireSameFormat,
      std::size_t bufferSize,
      bool bufferInRows) :
    m_mergeAxis(mergeAxis),
    m_chunkSize(chunkSize),
    m_requireSameFormat(requireSameFormat),
    m_bufferSize(bufferSize),
    m_measureBufferInRows(bufferInRows) {}

  DefaultMerger::~DefaultMerger() {}

  void DefaultMerger::merge(
      H5::Group& target,
      const H5::Group& source)
  {
    // Check if this group was empty before we started
    bool isEmpty = target.getNumObjs() == 0;

    // Iterate through each child of the source group
    for (hsize_t ii = 0; ii < source.getNumObjs(); ++ii) {
      H5G_obj_t childType = source.getObjTypeByIdx(ii);
      std::string childName = source.getObjnameByIdx(ii);
      // Find the correct index in the target
      hsize_t targetIdx = 0;
      for (; targetIdx < target.getNumObjs(); ++targetIdx) 
        if (target.getObjnameByIdx(targetIdx) == childName)
          break;
      bool found = targetIdx != target.getNumObjs();
      if (found) {
        // Make sure these are the same type!
        if (target.getObjTypeByIdx(targetIdx) != childType)
          throw std::invalid_argument(
              "Both target and source contain " + childName +
              " but they have different types!");
      }
      else if (m_requireSameFormat && !isEmpty) {
        throw std::invalid_argument(
            "Target and source have different formats!");
      }
      switch (childType) {
        case H5G_GROUP:
          {
            H5::Group sg = source.openGroup(childName);
            H5::Group tg = found ? 
              target.openGroup(childName) : 
              createFrom(target, sg);
            try {
              merge(tg, sg);
            }
            catch (...) {
              std::cerr << "Encountered an error merging child " << childName << std::endl;
              throw;
            }
          }
          break;
        case H5G_DATASET:
          {
            H5::DataSet sd = source.openDataSet(childName);
            if (sd.getSpace().getSimpleExtentNdims() == 0) {
              std::cerr << "WARNING: skipping scalar '"
                        << childName << "'" << std::endl;
              break;
            }
            H5::DataSet td = found ? 
              target.openDataSet(childName) : 
              createFrom(target, sd);
            try {
              merge(td, sd);
            }
            catch (...) {
              std::cerr << "Encountered an error merging child " << childName << std::endl;
              throw;
            }
          }
          break;
        default:
          break; 
      }
    } //> end loop over children
    // TODO - this did no check to see if target contained something source
    // didn't, this is probably fine though.
  } //> end function merge(group)

  void DefaultMerger::merge(
      H5::DataSet& target,
      const H5::DataSet& source)
  {
    std::size_t bufferSize = m_bufferSize;
    if (m_measureBufferInRows) {
      // Need to calculate the actual buffer size
      std::size_t rowSize = getRowSize(source, m_mergeAxis);
      if (std::size_t(-1) / m_bufferSize < rowSize)
        std::overflow_error("Requested buffer would overflow the register!");
      bufferSize = rowSize * m_bufferSize;
    }
    mergeDatasets(target, source, m_mergeAxis, bufferSize);
  }

  H5::DataSet DefaultMerger::createFrom(
      H5::H5Location& targetLocation,
      const H5::DataSet& source)
  {
    return createDataSet(targetLocation, source, m_mergeAxis, m_chunkSize);
  }
} //> end namespace H5Utils
