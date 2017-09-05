#include "TruthWeightTools/IndexRetriever.h"

IndexRetriever::IndexRetriever(std::string weightName): 
m_WeightName(weightName) {}

void IndexRetriever::update(xAOD::TruthMetaData const * const truthMetaData) {
 
  if( !truthMetaData) {
    m_isValid = false;
    return;
  }
  
  update(truthMetaData->weightNames());
}

void IndexRetriever::update(const std::vector<std::string>& weightNameVec) {
 
  if( !weightNameVec.size()) {
    m_isValid = false;
    return;
  }

  auto weightPosItr = std::find(weightNameVec.begin(), weightNameVec.end(), m_WeightName);
  if( weightPosItr != weightNameVec.end() ) {
    m_isValid = true;
    m_currentIndex = static_cast<size_t>(weightPosItr-weightNameVec.begin());
  } else {
    m_isValid = false;
  }
}
