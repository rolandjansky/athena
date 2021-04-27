/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BookkeeperDumperTool.cxx
 * @brief Implementation of class BookkeeperDumperTool
 */

#include <xAODCutFlow/CutBookkeeperContainer.h>

#include <EventBookkeeperTools/BookkeeperDumperTool.h>


BookkeeperDumperTool::BookkeeperDumperTool(const std::string &name)
  : asg::AsgMetadataTool(name)
{
#ifndef XAOD_STANDALONE
  declareInterface< ::IMetaDataTool >( this );
#endif // XAOD_STANDALONE
}


StatusCode BookkeeperDumperTool::initialize()
{
  if (m_standaloneMode.value()) {
    ATH_CHECK(beginInputFile());
  }

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperDumperTool::beginInputFile()
{
  // Complete CutBookkeepers
  size_t index{};
  while (true)
  {
    std::string name{"CutBookkeepers"};
    if (index > 0) {
      name.append("_weight_");
      name.append(std::to_string(index));
    }

    if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(name)) {
      const xAOD::CutBookkeeperContainer* completeBookkeepers{};
      ATH_CHECK(inputMetaStore()->retrieve(completeBookkeepers, name));

      ATH_MSG_INFO("Complete CBK " << name << " size = " << completeBookkeepers->size());
      for (const xAOD::CutBookkeeper *cbk : *completeBookkeepers) {
        ATH_MSG_INFO("Complete CBK"
                    << " name= " << cbk->name()
                    << " cycle=" << cbk->cycle()
                    << " stream=" << cbk->inputStream()
                    << " N=" << cbk->nAcceptedEvents()
                    << " W=" << cbk->sumOfEventWeights()
                    << " W2=" << cbk->sumOfEventWeightsSquared()
                    << " nc=" << cbk->nChildren());
      }
      index++;
    } else {
      if (index == 0) {
        ATH_MSG_INFO("No complete CutBookkeepers found");
      }
      break;
    }

    if (!m_allVariations) {
      break;
    }
  }

  // Incomplete CutBookkeepers
  index = 0;
  while (true)
  {
    std::string name{"IncompleteCutBookkeepers"};
    if (index > 0) {
      name.append("_weight_");
      name.append(std::to_string(index));
    }

    if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(name)) {
      const xAOD::CutBookkeeperContainer* incompleteBookkeepers{};
      ATH_CHECK(inputMetaStore()->retrieve(incompleteBookkeepers, name));

      ATH_MSG_INFO("Incomplete CBK " << name << " size = " << incompleteBookkeepers->size());
      for (const xAOD::CutBookkeeper *cbk : *incompleteBookkeepers) {
        ATH_MSG_INFO("Incomplete CBK"
                    << " name= " << cbk->name()
                    << " cycle=" << cbk->cycle()
                    << " stream=" << cbk->inputStream()
                    << " N=" << cbk->nAcceptedEvents()
                    << " W=" << cbk->sumOfEventWeights()
                    << " W2=" << cbk->sumOfEventWeightsSquared()
                    << " nc=" << cbk->nChildren());
      }
      index++;
    } else {
      if (index == 0) {
        ATH_MSG_INFO("No incomplete CutBookkeepers found");
      }
      break;
    }

    if (!m_allVariations) {
      break;
    }
  }

  // Complete PDF CutBookkeepers
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>("PDFSumOfWeights")) {
    const xAOD::CutBookkeeperContainer* pdfBookkeepers = nullptr;
    ATH_CHECK(inputMetaStore()->retrieve(pdfBookkeepers, "PDFSumOfWeights"));

    ATH_MSG_INFO("PDF CBK size = " << pdfBookkeepers->size());
    for (const xAOD::CutBookkeeper *cbk : *pdfBookkeepers) {
      ATH_MSG_INFO("PDF CBK"
                   << " name= " << cbk->name()
                   << " cycle=" << cbk->cycle()
                   << " stream=" << cbk->inputStream()
                   << " N=" << cbk->nAcceptedEvents()
                   << " W=" << cbk->sumOfEventWeights()
                   << " W2=" << cbk->sumOfEventWeightsSquared()
                   << " nc=" << cbk->nChildren());
    }
  } else {
    ATH_MSG_INFO("No PDF CutBookkeepers found");
  }

  // Incomplete PDF CutBookkeepers
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>("IncompletePDFSumOfWeights")) {
    const xAOD::CutBookkeeperContainer* pdfBookkeepers = nullptr;
    ATH_CHECK(inputMetaStore()->retrieve(pdfBookkeepers, "IncompletePDFSumOfWeights"));

    ATH_MSG_INFO("Incomplete PDF CBK size = " << pdfBookkeepers->size());
    for (const xAOD::CutBookkeeper *cbk : *pdfBookkeepers) {
      ATH_MSG_INFO("Incomplete PDF CBK"
                   << " name= " << cbk->name()
                   << " cycle=" << cbk->cycle()
                   << " stream=" << cbk->inputStream()
                   << " N=" << cbk->nAcceptedEvents()
                   << " W=" << cbk->sumOfEventWeights()
                   << " W2=" << cbk->sumOfEventWeightsSquared()
                   << " nc=" << cbk->nChildren());
    }
  } else {
    ATH_MSG_INFO("No incomplete PDF CutBookkeepers found");
  }

  return StatusCode::SUCCESS;
}
