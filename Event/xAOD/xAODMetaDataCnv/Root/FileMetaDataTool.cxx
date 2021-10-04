/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODMetaDataCnv/FileMetaDataTool.h"

// standard library includes
#include <memory>
#include <utility>

// EDM include(s):
#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"


namespace xAODMaker {

FileMetaDataTool::FileMetaDataTool(const std::string& name)
    : asg::AsgMetadataTool(name) {
       declareProperty( "Keys", m_keys = {},
             "List of keys to propogate from input to output - all if empty "
             "(default: empty)");
#ifndef XAOD_STANDALONE
      declareInterface< ::IMetaDataTool >(this);
#endif  // XAOD_STANDALONE
    }

StatusCode
    FileMetaDataTool::initialize() {
#ifndef XAOD_STANDALONE
      ASG_CHECK(m_metaDataSvc.retrieve());
#endif  // XAOD_STANDALONE

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::beginInputFile() {
      // Previous input file has been processed
      std::lock_guard lock(m_toolMutex);

      // get the keys for all metadata in input
      if (m_keys.empty()) {
         inputMetaStore()->keys<xAOD::FileMetaData>(m_keys);
      }

      // Now copy all object to MetaDataStore
      for(const std::string& key : m_keys) {
         ASG_CHECK(copy(key));
      }

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::copy(const std::string& key) {
      ATH_MSG_DEBUG("Copying \"" << key << "\" from InputMetaDataStore");
      // Quit gracefully if there is nothing to do
      if (!inputMetaStore()->contains< xAOD::FileMetaData >(key)) {
        ATH_MSG_INFO("No \"" << key << "\" in the input file");
        return StatusCode::SUCCESS;
      }

      // Get the FileMetaData object from the input file
      const xAOD::FileMetaData * input = nullptr;
      ASG_CHECK(inputMetaStore()->retrieve(input, key));

      // Emit a warning if the FileMetaData from previous files does not
      // match that of the new input file
#ifdef XAOD_STANDALONE
      if (outputMetaStore()->contains< xAOD::FileMetaData >(key)) {
        xAOD::FileMetaData * output = nullptr;
        ASG_CHECK(outputMetaStore()->retrieve(output, key));
#else
      if (m_metaDataSvc->contains< xAOD::FileMetaData >(key)) {
        const auto *output = m_metaDataSvc->tryConstRetrieve< xAOD::FileMetaData >(key);
        if (!output) return StatusCode::FAILURE;
#endif  // XAOD_STANDALONE

        if (*input != *output)
          ATH_MSG_WARNING("Inconsistent input file MetaData");

        return StatusCode::SUCCESS;

      }

      ATH_MSG_DEBUG("Creating output objects");
      auto output = std::make_unique< xAOD::FileMetaData >();
      auto outputAux = std::make_unique< xAOD::FileMetaDataAuxInfo >();
      output->setStore(outputAux.get());

      // Copy input object
      *output = *input;


#ifdef XAOD_STANDALONE
      ASG_CHECK(
          outputMetaStore()->record< xAOD::FileMetaData >(
              std::move(output), key));

      ASG_CHECK(
          outputMetaStore()->record< xAOD::FileMetaDataAuxInfo >(
              std::move(outputAux), key + "Aux."));
#else
      ASG_CHECK(
          m_metaDataSvc->record< xAOD::FileMetaData >(
              std::move(output), key));

      ASG_CHECK(
          m_metaDataSvc->record< xAOD::FileMetaDataAuxInfo >(
              std::move(outputAux), key + "Aux."));
#endif  // XAOD_STANDALONE

      ATH_MSG_INFO("Copied \"" << key << "\" to MetaDataStore");

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

}  // namespace xAODMaker
