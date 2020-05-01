/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BookkeeperDumperTool.cxx
 * @brief Implementation of class BookkeeperDumperTool
 */
 
#include "BookkeeperDumperTool.h"

#include <xAODCutFlow/CutBookkeeperContainer.h>



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
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>("CutBookkeepers")) {
    const xAOD::CutBookkeeperContainer* completeBookkeepers = nullptr;
    ATH_CHECK(inputMetaStore()->retrieve(completeBookkeepers, "CutBookkeepers"));

    ATH_MSG_INFO("Complete CBK size = " << completeBookkeepers->size());
    for (const xAOD::CutBookkeeper *cbk : *completeBookkeepers) {
      ATH_MSG_INFO("Complete CBK"
                   << " name= " << cbk->name()
                   << " cycle=" << cbk->cycle()
                   << " stream=" << cbk->inputStream()
                   << " N=" << cbk->nAcceptedEvents()
                   << " W=" << cbk->sumOfEventWeights()
                   << " nc=" << cbk->nChildren());
    }
  } else {
    ATH_MSG_INFO("No complete CutBookkeepers found");
  }

  // Incomplete CutBookkeepers
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>("IncompleteCutBookkeepers")) {
    const xAOD::CutBookkeeperContainer* incompleteBookkeepers = nullptr;
    ATH_CHECK(inputMetaStore()->retrieve(incompleteBookkeepers, "IncompleteCutBookkeepers"));

    ATH_MSG_INFO("Incomplete CBK size = " << incompleteBookkeepers->size());
    for (const xAOD::CutBookkeeper *cbk : *incompleteBookkeepers) {
      ATH_MSG_INFO("Incomplete CBK"
                   << " name= " << cbk->name()
                   << " cycle=" << cbk->cycle()
                   << " stream=" << cbk->inputStream()
                   << " N=" << cbk->nAcceptedEvents()
                   << " W=" << cbk->sumOfEventWeights()
                   << " nc=" << cbk->nChildren());
    }
  } else {
    ATH_MSG_INFO("No incomplete CutBookkeepers found");
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
                   << " nc=" << cbk->nChildren());
    }
  } else {
    ATH_MSG_INFO("No incomplete PDF CutBookkeepers found");
  }

  return StatusCode::SUCCESS;
}
