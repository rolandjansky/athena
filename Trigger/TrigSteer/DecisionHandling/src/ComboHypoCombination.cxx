/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypoCombination.h"

void ComboHypoCombination::add(const uint32_t inputContainerID, const uint16_t decisionElementID, 
    const uint32_t featureContainerKey, const uint16_t featureIndex,
    const uint32_t roiContainerKey, const uint16_t roiIndex)
{
  m_combinations.push_back( std::make_pair(inputContainerID, decisionElementID) );

  const uint32_t featureMapLocation = combineKeyAndIndex(featureContainerKey, featureIndex);
  const uint32_t roiPayload = combineKeyAndIndex(roiContainerKey, roiIndex);
  m_features.push_back( std::make_pair(featureMapLocation, roiPayload) );
}

uint32_t ComboHypoCombination::combineKeyAndIndex(const uint32_t key, const uint16_t index) {
  return (key + index);
}

const std::vector< std::pair<uint32_t,uint16_t> >& ComboHypoCombination::getCombinations() const {
  return m_combinations;
}

const std::vector< std::pair<uint32_t,uint32_t> >& ComboHypoCombination::getFeatures() const {
  return m_features;
}
