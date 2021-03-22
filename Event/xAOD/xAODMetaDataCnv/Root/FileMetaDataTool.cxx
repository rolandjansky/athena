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
  declareProperty("InputKey", m_inputKey = "FileMetaData",
                  "Key of xAOD::FileMetaData object in input");

  declareProperty("OutputKey", m_outputKey = "FileMetaData",
                  "Key of xAOD::FileMetaData in MetaDataStore");
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

#ifndef XAOD_STANDALONE
StatusCode
    FileMetaDataTool::endInputFile(const SG::SourceID&) {
      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::beginInputFile(const SG::SourceID&) {
      return beginInputFile();
    }
#endif  // XAOD_STANDALONE

StatusCode
    FileMetaDataTool::endInputFile() {
      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::beginInputFile() {
      // Previous input file has been processed
      std::lock_guard lock(m_toolMutex);

      // Quit gracefully if there is nothing to do
      if (!inputMetaStore()->contains< xAOD::FileMetaData >(m_inputKey)) {
        ATH_MSG_INFO("No xAOD::FileMetaData in the input file");
        return StatusCode::SUCCESS;
      }

      // Get the FileMetaData object from the input file
      const xAOD::FileMetaData * input = nullptr;
      ASG_CHECK(inputMetaStore()->retrieve(input, m_inputKey));

      // Emit a warning if the FileMetaData from previous files does not
      // match that of the new input file
#ifdef XAOD_STANDALONE
      if (outputMetaStore()->contains< xAOD::FileMetaData >(m_outputKey)) {
        xAOD::FileMetaData * output = nullptr;
        ASG_CHECK(
            outputMetaStore()->retrieve(output, m_outputKey));
#else
      if (m_metaDataSvc->contains< xAOD::FileMetaData >(m_outputKey)) {
        const auto *output = m_metaDataSvc->tryConstRetrieve< xAOD::FileMetaData >(m_outputKey);
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
              std::move(output), m_outputKey));

      ASG_CHECK(
          outputMetaStore()->record< xAOD::FileMetaDataAuxInfo >(
              std::move(outputAux), m_outputKey + "Aux."));
#else
      ASG_CHECK(
          m_metaDataSvc->record< xAOD::FileMetaData >(
              std::move(output), m_outputKey));

      ASG_CHECK(
          m_metaDataSvc->record< xAOD::FileMetaDataAuxInfo >(
              std::move(outputAux), m_outputKey + "Aux."));
#endif  // XAOD_STANDALONE

      ATH_MSG_INFO("Copied xAOD::FileMetaData to MetaDataStore");

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::metaDataStop() {
      // Return gracefully:
      return StatusCode::SUCCESS;
    }
}  // namespace xAODMaker
