/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TruncationAnalysisAlg.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace {
  struct CollectionDebugInfo {
    std::string_view name;
    uint32_t size;
    bool isRecorded;
  };
  bool cmpCollections(const CollectionDebugInfo& a, const CollectionDebugInfo& b) {
    return a.size > b.size;
  }
}

TruncationAnalysisAlg::TruncationAnalysisAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode TruncationAnalysisAlg::initialize() {
  ATH_CHECK( m_truncationInfoKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TruncationAnalysisAlg::execute(const EventContext& context) const {
  ATH_MSG_DEBUG("Retrieving truncation info object " << m_truncationInfoKey.fullKey());
  auto truncationInfo = SG::makeHandle(m_truncationInfoKey, context);
  ATH_MSG_DEBUG("Retrieved container with size: " << truncationInfo->size());
  const xAOD::TrigComposite::ConstAccessor<uint16_t> moduleId("moduleId");
  const xAOD::TrigComposite::ConstAccessor<uint32_t> totalSize("totalSize");
  const xAOD::TrigComposite::ConstAccessor<std::vector<std::string>> typeNameVec("typeName");
  const xAOD::TrigComposite::ConstAccessor<std::vector<uint32_t>> sizeVec("size");
  const xAOD::TrigComposite::ConstAccessor<std::vector<char>> isRecordedVec("isRecorded");
  size_t counter = 0;
  const size_t nInfos = truncationInfo->size();
  for (const xAOD::TrigComposite* info : *truncationInfo) {
    ++counter;
    std::ostringstream ss;
    ss << std::endl << "================================================================================" << std::endl;

    //Print general information
    ss << "Info number " << counter << " / " << nInfos << std::endl;
    ss << "Module ID: " << moduleId(*info) << std::endl;
    ss << "Total size: " << totalSize(*info)/1024. << " kB" << std::endl;

    // Collect name+size+isRecorded in one structure and sort by descending size
    std::vector<CollectionDebugInfo> collections;
    for (size_t i=0; i<typeNameVec(*info).size(); ++i) {
      collections.push_back({typeNameVec(*info).at(i),
                             sizeVec(*info).at(i),
                             static_cast<bool>(isRecordedVec(*info).at(i))});
    }
    std::sort(collections.begin(), collections.end(), cmpCollections);

    // Print information about all collections
    ss << "Found " << collections.size() << " collections in this module" << std::endl;
    ss << "--------------------------------------------------------------------------------" << std::endl;
    ss << "| Size [kB] | Recorded? | Name |" << std::endl;
    for (const CollectionDebugInfo& coll : collections) {
      ss << "| " << std::setw(9) << std::setprecision(2) << std::fixed << coll.size/1024.;
      ss << (coll.isRecorded ? " |     Y     | " : " |     N     | ");
      ss << coll.name << " |" << std::endl;
    }
    ss << "--------------------------------------------------------------------------------" << std::endl;
    ss << "================================================================================" << std::endl;
    ATH_MSG_INFO(ss.str());
  }
  ATH_MSG_DEBUG("Execution finished");
  return StatusCode::SUCCESS;
}
