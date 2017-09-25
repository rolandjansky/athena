/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "PMGTools/PMGTruthWeightIndexRetriever.h"

namespace PMGTools
{
  PMGTruthWeightIndexRetriever::PMGTruthWeightIndexRetriever(const std::string& weightName) :
    m_isValid(false),
    m_weightName(weightName)
  {}


  void PMGTruthWeightIndexRetriever::update(const xAOD::TruthMetaData* const truthMetaData) {
    if (!truthMetaData) {
      m_isValid = false;
      return;
    }
    update(truthMetaData->weightNames());
  }


  void PMGTruthWeightIndexRetriever::update(const std::vector<std::string>& weightNameVec) {
    if (!weightNameVec.size()) {
      m_isValid = false;
      return;
    }

    auto weightPosItr = std::find(weightNameVec.begin(), weightNameVec.end(), m_weightName);
    if (weightPosItr != weightNameVec.end()) {
      m_isValid = true;
      m_currentIndex = static_cast<size_t>(weightPosItr - weightNameVec.begin());
    } else {
      m_isValid = false;
    }
  }


  size_t PMGTruthWeightIndexRetriever::getIndex() {
    if (!m_isValid) {
      throw std::runtime_error("Weight \"" + m_weightName + "\" not found in event");
    }
    return m_currentIndex;
  }


  bool PMGTruthWeightIndexRetriever::isValid() {
    return m_isValid;
  }

} // namespace PMGTools
