/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class BookkeeperTool
// Authors: Tadej Novak <tadej@cern.ch>
//          Jack Cranshaw <Jack.Cranshaw@cern.ch>

#include <xAODCutFlow/xAODCutFlowHelpers.h>

#ifndef GENERATIONBASE
#include <xAODMetaData/FileMetaData.h>
#include <xAODTruth/TruthMetaDataContainer.h>
#endif

#ifndef XAOD_STANDALONE
#include <AthenaPoolUtilities/CondAttrListCollection.h>
#include <IOVDbDataModel/IOVMetaDataContainer.h>
#endif

#include <EventBookkeeperTools/BookkeeperTool.h>
#include <EventBookkeeperTools/CutFlowSvc.h>


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

#ifndef XAOD_STANDALONE
  ATH_CHECK(m_cutFlowSvc.retrieve());
  // Access "private" CutFlowSvc methods
  m_cutFlowSvcPrivate = dynamic_cast<CutFlowSvc *>(&*(m_cutFlowSvc));
#endif

  return StatusCode::SUCCESS;
}


#ifdef XAOD_STANDALONE
StatusCode BookkeeperTool::beginInputFile()
#else
StatusCode BookkeeperTool::beginInputFile(const SG::SourceID &source)
#endif
{
  // OPENING NEW INPUT FILE
  // Things to do:
  // 1) Load CutBookkeepers from input file
  //    a) if incomplete from input, directly propagate to output
  //    b) if complete from input, wait for EndInputFile to decide what to do in output

  // Get number of event weight variations if needed
  if (m_numberOfWeightVariations == 0) {
    ATH_MSG_DEBUG("Determining number of weight variations");
#ifndef GENERATIONBASE
    if (inputMetaStore()->contains<xAOD::TruthMetaDataContainer>("TruthMetaData")) {
      StatusCode status = loadXAODMetaData();
      if (!status.isSuccess()) {
        if (status.isRecoverable()) {
          ATH_CHECK(loadPOOLMetaData());
        } else {
          return StatusCode::FAILURE;
        }
      }
    } else {
      ATH_CHECK(loadPOOLMetaData());
    }
#else
    ATH_CHECK(loadPOOLMetaData());
#endif

    if (m_numberOfWeightVariations == 0) {
      ATH_MSG_ERROR("Could not determine the number of weight variations");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Using number of event weight variations " << m_numberOfWeightVariations);

#ifndef XAOD_STANDALONE
    ATH_CHECK(m_cutFlowSvcPrivate->setNumberOfWeightVariations(m_numberOfWeightVariations));
#endif
  }

#ifdef XAOD_STANDALONE
  SG::SourceID source{"file"};
#endif

  // Prepare local cache
  CutBookkeepersLocalCache::prepareContainers(m_completeContainers, m_numberOfWeightVariations);
  CutBookkeepersLocalCache::prepareContainers(m_incompleteContainers, m_numberOfWeightVariations);

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
      xAOD::CutFlowHelpers::updateContainer(m_incompleteContainers.at(0), inIncomplete);
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
      auto [it, status] = m_inputContainers.emplace(source, CutBookkeepersLocalCache());
      CutBookkeepersLocalCache::prepareContainers(it->second, m_numberOfWeightVariations);

      xAOD::CutFlowHelpers::updateContainer(it->second.at(0), inComplete);
      ATH_MSG_DEBUG("Successfully copied complete bookkeepers to temp container");
    } else {
      ATH_MSG_INFO("No complete bookkeepers in this file with name " << m_inputCollName);
    }
  }

  return StatusCode::SUCCESS;
}

#ifdef XAOD_STANDALONE
StatusCode BookkeeperTool::endInputFile()
#else
StatusCode BookkeeperTool::endInputFile(const SG::SourceID &source)
#endif
{
#ifdef XAOD_STANDALONE
  SG::SourceID source{"file"};
#endif
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
  for (std::size_t i = 0; i < m_completeContainers.size(); ++i) {
    std::string name = m_outputCollName;
    if (i != 0) {
      name.append("_weight_");
      name.append(std::to_string(i));
    }
    std::string incompleteName = "Incomplete" + name;

    // In MP we might already have them written out
    if (outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(name)) {
      xAOD::CutBookkeeperContainer *complete{};
      if (!outputMetaStore()->retrieve(complete, name).isSuccess()) {
        ATH_MSG_ERROR("Could not get " << name << " CutBookkeepers from output MetaDataStore");
        return StatusCode::FAILURE;
      }
      xAOD::CutFlowHelpers::updateContainer(complete, m_completeContainers.at(i));
    } else {
      ATH_CHECK(outputMetaStore()->record(std::move(m_completeContainers.cont[i]), name));
      ATH_CHECK(outputMetaStore()->record(std::move(m_completeContainers.aux[i]), name + "Aux."));
    }

    if (outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(incompleteName)) {    
      xAOD::CutBookkeeperContainer *incomplete{};
      if (!outputMetaStore()->retrieve(incomplete, incompleteName).isSuccess()) {
        ATH_MSG_ERROR("Could not get " << incompleteName << " CutBookkeepers from output MetaDataStore");
        return StatusCode::FAILURE;
      }
      xAOD::CutFlowHelpers::updateContainer(incomplete, m_incompleteContainers.at(i));
    } else {
      // Only write non-empty incomplete containers
      if (i > 0 && !m_incompleteContainers.at(i)->empty()) {
        ATH_CHECK(outputMetaStore()->record(std::move(m_incompleteContainers.cont[i]), incompleteName));
        ATH_CHECK(outputMetaStore()->record(std::move(m_incompleteContainers.aux[i]), incompleteName + "Aux."));
      }
    }
  }

  m_incompleteContainers.clear();
  m_completeContainers.clear();

  ATH_MSG_DEBUG("Successfully copied CutBookkeepers to the output MetaDataStore");

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::copyInputContainersToOutput(CutBookkeepersLocalCache &target,
                                                       const SG::SourceID &source)
{
  CutBookkeepersLocalCache::prepareContainers(target, m_numberOfWeightVariations);

  if (!source.empty()) {
    auto it = m_inputContainers.find(source);
    if (it == m_inputContainers.end()) {
      ATH_MSG_DEBUG("No input containers for this file");
      return StatusCode::SUCCESS;
    }

    for (std::size_t i = 0; i < it->second.size(); ++i) {
      xAOD::CutFlowHelpers::updateContainer(target.at(i), it->second.at(i));
    }
    m_inputContainers.erase(it);

    return StatusCode::SUCCESS;
  }

  for (auto &[s, list] : m_inputContainers) {
    for (std::size_t i = 0; i < list.size(); ++i) {
      xAOD::CutFlowHelpers::updateContainer(target.at(i), list.at(i));
    }
  }
  m_inputContainers.clear();

  return StatusCode::SUCCESS;
}

StatusCode BookkeeperTool::copyCutflowFromService()
{
#ifndef XAOD_STANDALONE
  // Get the bookkeeper from the current processing
  const CutBookkeepersLocalCache &cache = m_cutFlowSvcPrivate->getCutBookkeepers();
  if (!cache.empty()) {
    CutBookkeepersLocalCache::prepareContainers(m_completeContainers, std::max(cache.size(), m_numberOfWeightVariations));

    for (std::size_t i = 0; i < cache.size(); ++i) {
      xAOD::CutFlowHelpers::updateContainer(m_completeContainers.at(i), cache.at(i));
    }
  } else {
    ATH_MSG_ERROR("No cutflow container in the CutFlowSvc");
    return StatusCode::FAILURE;
  }
#endif

  return StatusCode::SUCCESS;
}


StatusCode BookkeeperTool::loadXAODMetaData()
{
#ifdef GENERATIONBASE
  return StatusCode::RECOVERABLE;
#else

  // Try to load MC channel number from file metadata
  ATH_MSG_DEBUG("Attempting to retrieve MC channel number...");
  const xAOD::FileMetaData *fileMetaData{};
  uint32_t mcChannelNumber{uint32_t(-1)};
  if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
    ATH_CHECK(inputMetaStore()->retrieve(fileMetaData, "FileMetaData"));
    float fltChannelNumber{-1};
    if (fileMetaData->value(xAOD::FileMetaData::mcProcID, fltChannelNumber)) {
      mcChannelNumber = static_cast<uint32_t>(mcChannelNumber);
    }
  }
  if (mcChannelNumber == uint32_t(-1)) {
    ATH_MSG_WARNING("... MC channel number could not be loaded from FileMetaData");
#ifdef XAOD_STANDALONE
    mcChannelNumber = 0;
#else
    return StatusCode::RECOVERABLE;
#endif
  }

  // Find the correct truth meta data object
  ATH_MSG_DEBUG("Attempting to load weight meta data from xAOD TruthMetaData for channel " << mcChannelNumber);
  const xAOD::TruthMetaDataContainer *metaDataContainer{};
  ATH_CHECK(inputMetaStore()->retrieve(metaDataContainer, "TruthMetaData"));

  // If we only have one metadata item take MC channel from there if needed
  if (mcChannelNumber == 0 && metaDataContainer->size() == 1) {
    mcChannelNumber = metaDataContainer->at(0)->mcChannelNumber();
    ATH_MSG_WARNING("... MC channel number taken from the metadata as " << mcChannelNumber);
  }

  auto itTruthMetaDataPtr = std::find_if(metaDataContainer->begin(), metaDataContainer->end(),
    [mcChannelNumber] (const auto& it) { return it->mcChannelNumber() == mcChannelNumber; }
  );

  // If no such object is found then return
  if (itTruthMetaDataPtr == metaDataContainer->end()) {
#ifdef XAOD_STANDALONE
    m_numberOfWeightVariations = 1;
    ATH_MSG_DEBUG("Could not load weight meta data! Assumming 1 variation.");
    return StatusCode::SUCCESS;
#else
    ATH_MSG_DEBUG("Could not load weight meta data from TruthMetaData!");
    return StatusCode::RECOVERABLE;
#endif
  }

  // Update cached weight data
  const std::vector<std::string> &truthWeightNames = (*itTruthMetaDataPtr)->weightNames();

  m_numberOfWeightVariations = truthWeightNames.size();
  if (m_numberOfWeightVariations == 0) {
    ATH_MSG_DEBUG("No variations present, setting to 1.");
    m_numberOfWeightVariations = 1;
  }

  return StatusCode::SUCCESS;
#endif
}


StatusCode BookkeeperTool::loadPOOLMetaData()
{
  // AnalysisBase can only use the xAOD::TruthMetaDataContainer, so skip this
#ifdef XAOD_STANDALONE
  return StatusCode::SUCCESS;
#else

  const IOVMetaDataContainer *container
    = inputMetaStore()->tryConstRetrieve<IOVMetaDataContainer>("/Generation/Parameters");
  if (container == nullptr) {
    // set variations to 1 and exit quietly...
    m_numberOfWeightVariations = 1;
    return StatusCode::SUCCESS;
  }
  // payload is a collection of CondAttrListCollections
  // only look a the first one, assuming it exists, and within that only look at the first channel;
  if (!(container->payloadContainer()->size() > 0
        && container->payloadContainer()->at(0)->size() > 0)) {
    return StatusCode::FAILURE;
  }

  // we take the first collection of weights
  // TODO: improve this
  CondAttrListCollection::ChanNum chanNum{container->payloadContainer()->at(0)->chanNum(0)};
  const coral::Attribute& attr
    = container->payloadContainer()->at(0)->attributeList(chanNum)["HepMCWeightNames"];

  ATH_MSG_DEBUG("Attempting to load weight meta data from /Generation/Parameters");
  std::map<std::string, int> truthWeightMap;
  ATH_CHECK(Gaudi::Parsers::parse(truthWeightMap, attr.data<std::string>()));

  m_numberOfWeightVariations = truthWeightMap.size();

  return StatusCode::SUCCESS;
#endif // XAOD_STANDALONE
}
