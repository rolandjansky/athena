/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_COMBOHYPOCOMBINATION_H
#define DECISIONHANDLING_COMBOHYPOCOMBINATION_H

// STL includes
#include <map>
#include <utility> // std::pair  
#include <vector>  

/**
* @brief Utility class to assist with the bookkeeping when counting Decision objects which pass 
* specific chain-leg requirements. Here Decision nodes are identified by abstract key and index pairs, and these
* node's "Feature" and "initialRoI" by hash values.
* 
* This suffices to perform Decision node multiplicity counting, and "Feature"/"initialRoI" uniqueness checks.
**/
class ComboHypoCombination {
public:

  /**
  * @brief Compresses the Feature and ROI type-less ID data into single uint32_t hash values
  * and stores this along with the Decision node's key & index in two parallel vectors.
  **/
  void add(const uint32_t inputContainerID, const uint16_t decisionElementID, 
    const uint32_t featureContainerKey, const uint16_t featureIndex,
    const uint32_t roiContainerKey, const uint16_t roiIndex);
  
  /**
  * @brief Get the node vector. Each entry corresponds to a Decision node (first: key, second: index)
  **/
  const std::vector< std::pair<uint32_t,uint16_t> >& getCombinations() const;

  /**
  * @brief Get the features vector. Each entry corresponds to a Decision node's Feature & ROI (first: feature, second: roi)
  **/
  const std::vector< std::pair<uint32_t,uint32_t> >& getFeatures() const;

private:

  /**
  * @brief key is a SG hash, so is already well distributed. The hash is incremented by the index 
  * to create a new per-object hash
  **/
  uint32_t combineKeyAndIndex(const uint32_t key, const uint16_t index);

  /**
  * @brief Each entry corresponds to a Decision node.
  * The first element in the pair is the SG key of the node's collection.
  * The second element in the pair is the index inside the collection.
  **/
  std::vector< std::pair<uint32_t,uint16_t> > m_combinations;
  /**
  * @brief Each entry corresponds to the Feature of the node in the parallel m_combinations vector.
  * The first element in the pair is a combined hash of the SG key & index of the node's Feature.
  * The second element in the pair is a combined hash of the SG key & index of the node's initialRoI
  **/
  std::vector< std::pair<uint32_t,uint32_t> > m_features;

};


#endif // DECISIONHANDLING_COMBOHYPOCOMBINATION_H
