/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JJETSEEDHELPER_H
#define TRIGT1CALOFEXPERF_JJETSEEDHELPER_H

#include <set>

/**
 * @brief Helper class mapping JTowers to jets and jet seeds
 */
class JJetSeedHelper
{
public:
  JJetSeedHelper(std::size_t centreIdx, float eta, float phi)
      : m_centreIdx(centreIdx), m_eta(eta), m_phi(phi) {}
  /// The index of the central JTower
  std::size_t centreIdx() const { return m_centreIdx; }
  /// eta coordinate of the centre of the seed
  float eta() const { return m_eta; }
  /// phi coordinate of the centre of the seed
  float phi() const { return m_phi; }
  /// Indices of all towers in the seed (includes the central tower)
  const std::set<std::size_t> &seedIndices() const { return m_seedIndices; }
  std::set<std::size_t> &seedIndices() { return m_seedIndices; }
  /// Indices of all towers in the 1st ring (small-R jet)
  const std::set<std::size_t> &firstRingIndices() const { return m_firstRingIndices; }
  std::set<std::size_t> &firstRingIndices() { return m_firstRingIndices; }
  /// Indices of all towers in the 2nd ring (large-R jet)
  const std::set<std::size_t> &secondRingIndices() const { return m_secondRingIndices; }
  std::set<std::size_t> &secondRingIndices() { return m_secondRingIndices; }
  /**
   * @brief Indices of all other seeds whose energies have to be less than this one's
   * 
   * This list only includes the seeds where our energy has to be >= theirs.
   * This is only half the total list of seeds which we have to compare against,
   * but the other half will all include this seed in their comparisonIndices.
   * Therefore all comparisons will actually be performed.
   */
  const std::set<std::size_t> &comparisonIndices() const { return m_comparisonIndices; }
  std::set<std::size_t> &comparisonIndices() { return m_comparisonIndices; }

private:
  std::size_t m_centreIdx;
  float m_eta;
  float m_phi;
  std::set<std::size_t> m_seedIndices;
  std::set<std::size_t> m_firstRingIndices;
  std::set<std::size_t> m_secondRingIndices;
  std::set<std::size_t> m_comparisonIndices;

}; //> end class JJetSeedHelper

#endif //> !TRIGT1CALOFEXPERF_JJETSEEDHELPER_H