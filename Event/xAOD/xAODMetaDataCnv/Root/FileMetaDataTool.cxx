/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODMetaDataCnv/FileMetaDataTool.h"

namespace xAODMaker {

FileMetaDataTool::FileMetaDataTool(const std::string& name)
    : asg::AsgMetadataTool(name)
    , m_md()
    , m_mdAux()
    , m_beginFileIncidentSeen(false) {
      declareProperty("InputKey", m_inputKey = "FileMetaData");
      declareProperty("OutputKey", m_outputKey = "FileMetaData");

#ifndef XAOD_STANDALONE
      declareInterface< ::IMetaDataTool >(this);
#endif  // XAOD_STANDALONE
    }

StatusCode
    FileMetaDataTool::initialize() {
      // Greet the user:
      ATH_MSG_DEBUG("Initialising xAODMaker::FileMetaDataTool");
      ATH_MSG_DEBUG("  InputKey  = " << m_inputKey);
      ATH_MSG_DEBUG("  OutputKey = " << m_outputKey);

      // Reset the member variable(s):
      m_md.reset();
      m_mdAux.reset();
      m_beginFileIncidentSeen = false;

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::endInputFile() {
      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::beginInputFile() {
      // Whatever happens, we've seen the incident:
      m_beginFileIncidentSeen = true;

      // If the input file doesn't have any file-level metadata, then
      // finish right away:
      if (!inputMetaStore()->contains< xAOD::FileMetaData >(m_inputKey)) {
        return StatusCode::SUCCESS;
      }

      // Retrieve the input object:
      const xAOD::FileMetaData* input = 0;
      ATH_CHECK(inputMetaStore()->retrieve(input, m_inputKey));

      // Create the output objects if they don't exist yet:
      if ((!m_md.get()) && (!m_mdAux.get())) {
        ATH_MSG_DEBUG("Creating output objects");
        m_md.reset(new xAOD::FileMetaData());
        m_mdAux.reset(new xAOD::FileMetaDataAuxInfo());
        m_md->setStore(m_mdAux.get());

        // Copy the payload of the input object:
        *(m_md.get()) = *input;
      }

      // Make sure that the objects are compatible:
      if (*(m_md.get()) != *input) {
        ATH_MSG_ERROR("Processing input files with differing conditions");
        ATH_MSG_ERROR("Consistent xAOD::FileMetaData can't be provided for "
                      "the output");
        return StatusCode::FAILURE;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::beginEvent() {
      // In case we missed the BeginInputFile incident for the first input file,
      // make sure that we still run the appropriate function.
      if (!m_beginFileIncidentSeen) {
        ATH_CHECK(beginInputFile());
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataTool::metaDataStop() {
      // Don't be offended if the metadata already exists in the output:
      if (outputMetaStore()->contains< xAOD::FileMetaData >(m_outputKey)) {
        ATH_MSG_DEBUG("xAOD::FileMetaData already in the output");
        return StatusCode::SUCCESS;
      }

      // Record the metadata, if any was found on the input:
      if (m_md.get() && m_mdAux.get()) {
        ATH_MSG_DEBUG("Recoding file level metadata");
        ATH_CHECK(outputMetaStore()->record(m_md.release(), m_outputKey));
        ATH_CHECK(outputMetaStore()->record(m_mdAux.release(),
                                            m_outputKey + "Aux."));
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

}  // namespace xAODMaker
