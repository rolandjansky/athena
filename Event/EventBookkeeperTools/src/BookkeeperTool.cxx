/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class BookkeeperTool
// Authors: Tadej Novak <tadej@cern.ch>
//          Jack Cranshaw <Jack.Cranshaw@cern.ch>

#include "BookkeeperTool.h"
#include "CutBookkeeperUtils.h"
#include "CutFlowSvc.h"


BookkeeperTool::BookkeeperTool(const std::string& name)
  : asg::AsgMetadataTool(name)
{
#ifndef XAOD_STANDALONE
  declareInterface< ::IMetaDataTool >( this );
#endif // XAOD_STANDALONE
}


StatusCode BookkeeperTool::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name());

  ATH_MSG_DEBUG("InputCollName = " << m_inputCollName);
  ATH_MSG_DEBUG("OutputCollName = " << m_outputCollName);

  ATH_CHECK(m_cutFlowSvc.retrieve());
  // Access "private" CutFlowSvc methods
  m_cutFlowSvcPrivate = dynamic_cast<CutFlowSvc *>(&*(m_cutFlowSvc));

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::beginInputFile(const SG::SourceID &source)
{
  // OPENING NEW INPUT FILE
  // Things to do:
  // 1) Load CutBookkeepers from input file
  //    a) if incomplete from input, directly propagate to output
  //    b) if complete from input, wait for EndInputFile to decide what to do in output

  // Prepare local cache
  ATH_CHECK(prepareContainers(m_completeContainers));
  ATH_CHECK(prepareContainers(m_incompleteContainers));

  // Check if a local container is there. IT SHOULD NOT BE
  auto sit = m_inputContainers.find(source);
  if (sit != m_inputContainers.end()) {
    ATH_MSG_ERROR("Undefined behaviour: this file has already been processed");
    return StatusCode::FAILURE;
  }

  // Get the incomplete bookkeeper collection of the input metadata store
  const xAOD::CutBookkeeperContainer *inIncomplete{};
  // Construct input and output incomplete names
  std::string incompleteCollName = "Incomplete" + m_inputCollName;
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(incompleteCollName)) {
    if (inputMetaStore()->retrieve(inIncomplete, incompleteCollName).isSuccess()) {
      // update incomplete output with any incomplete input
      ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(m_incompleteContainers.at(0), inIncomplete));
      ATH_MSG_DEBUG("Successfully merged input incomplete bookkeepers with output");
    }
  } else {
    ATH_MSG_INFO("No incomplete bookkeepers in this file with name " << incompleteCollName);
  }

  // Get the complete bookkeeper collection of the input metadata store
  const xAOD::CutBookkeeperContainer *inComplete{};
  if (inputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_inputCollName)) {
    if (inputMetaStore()->retrieve(inComplete, m_inputCollName).isSuccess()) {
      // Create the temporary container
      auto [it, status] = m_inputContainers.emplace(source, LocalContainers());
      ATH_CHECK(prepareContainers(it->second));

      ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(it->second.at(0), inComplete));
      ATH_MSG_DEBUG("Successfully copied complete bookkeepers to temp container");
    } else {
      ATH_MSG_INFO("No complete bookkeepers in this file with name " << m_inputCollName);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::endInputFile(const SG::SourceID &source)
{
  ATH_CHECK(copyInputContainersToOutput(m_completeContainers, source));

  return StatusCode::SUCCESS;
}

StatusCode BookkeeperTool::metaDataStop()
{
  // TERMINATING THE JOB (EndRun)
  // Things to do:
  // 1) Create new incomplete CutBookkeepers if relevant
  // 2) Copy cutflow from the service
  // 3) Copy containers to output

  // All files that have not been fully processed are incomplete
  ATH_CHECK(copyInputContainersToOutput(m_incompleteContainers));

  // Copy cutflow from the service
  ATH_CHECK(copyCutflowFromService());

  // Write out the containers
  for (std::size_t i = 0; i < m_completeContainers.cont.size(); ++i) {
    std::string name = m_outputCollName;
    if (i != 0) {
      name.append("_weight_");
      name.append(std::to_string(i));
    }

    // In MP we might already have them written out
    if (outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(name)) {
      xAOD::CutBookkeeperContainer *complete{};
      if (!outputMetaStore()->retrieve(complete, name).isSuccess()) {
        ATH_MSG_ERROR("Could not get " << name << " CutBookkeepers from output MetaDataStore");
        return StatusCode::FAILURE;
      }
      xAOD::CutBookkeeperContainer *incomplete{};
      if (!outputMetaStore()->retrieve(incomplete, "Incomplete" + name).isSuccess()) {
        ATH_MSG_ERROR("Could not get " << "Incomplete" + name << " CutBookkeepers from output MetaDataStore");
        return StatusCode::FAILURE;
      }
      ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(complete, m_completeContainers.at(i)));
      ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(incomplete, m_incompleteContainers.at(i)));
    } else {
      ATH_CHECK(outputMetaStore()->record(std::move(m_completeContainers.cont[i]), name));
      ATH_CHECK(outputMetaStore()->record(std::move(m_completeContainers.aux[i]), name + "Aux."));
      ATH_CHECK(outputMetaStore()->record(std::move(m_incompleteContainers.cont[i]), "Incomplete" + name));
      ATH_CHECK(outputMetaStore()->record(std::move(m_incompleteContainers.aux[i]), "Incomplete" + name + "Aux."));
    }
  }

  m_incompleteContainers.clear();
  m_completeContainers.clear();

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::copyInputContainersToOutput(LocalContainers &target,
                                                       const SG::SourceID &source)
{
  ATH_CHECK(prepareContainers(target));

  if (!source.empty()) {
    auto it = m_inputContainers.find(source);
    if (it == m_inputContainers.end()) {
      ATH_MSG_DEBUG("No input containers for this file");
      return StatusCode::SUCCESS;
    }

    for (std::size_t i = 0; i < it->second.size(); ++i) {
      ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(target.at(i), it->second.at(i)));
    }
    m_inputContainers.erase(it);

    return StatusCode::SUCCESS;
  }

  for (auto &[s, list] : m_inputContainers) {
    for (std::size_t i = 0; i < list.size(); ++i) {
      ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(target.at(i), list.at(i)));
    }
  }
  m_inputContainers.clear();

  return StatusCode::SUCCESS;
}

StatusCode BookkeeperTool::copyCutflowFromService()
{
  ATH_CHECK(prepareContainers(m_completeContainers));

  // Get the bookkeeper from the current processing
  const xAOD::CutBookkeeperContainer* cbkCont = m_cutFlowSvcPrivate->getCutBookkeepers();
  if (cbkCont != nullptr) {
    ATH_CHECK(xAOD::CutBookkeeperUtils::updateContainer(m_completeContainers.at(0), cbkCont));
  } else {
    ATH_MSG_ERROR("No cutflow container in the CutFlowSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::prepareContainers(LocalContainers &target)
{
  if (!target.empty()) {
    return StatusCode::SUCCESS;
  }

  std::size_t size{1}; // TODO: determine actual size
  for (std::size_t i = 0; i < size; ++i) {
    auto container = std::make_unique<xAOD::CutBookkeeperContainer>();
    auto auxContainer = std::make_unique<xAOD::CutBookkeeperAuxContainer>();
    container->setStore(auxContainer.get());
    target.cont.push_back(std::move(container));
    target.aux.push_back(std::move(auxContainer));
  }

  return StatusCode::SUCCESS;
}
