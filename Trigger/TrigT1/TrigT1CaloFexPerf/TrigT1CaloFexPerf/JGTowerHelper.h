/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERHELPER_H
#define TRIGT1CALOFEXPERF_JGTOWERHELPER_H
/*
* Class   : JGTowerHelper
*
* Author  : Lin, Chiao-Ying (cylin@cern.ch)
*
* Date    : May 2017 - Created class
*
* This class is built for JGTower algorithm in topology
* including mapping to SC, eta/phi and granularity
* 
*/

#include <vector>
#include <string>
#include <cstdint>

namespace LVL1
{
  class JGTowerHelper
  {

  public:
    // constructors
    JGTowerHelper();
    JGTowerHelper(float eta, float deta, float phi, float dphi);

    // destructor
    virtual ~JGTowerHelper();

    float Eta() const { return m_eta; };
    float Phi() const { return m_phi; };
    float dEta() const { return m_deta; };
    float dPhi() const { return m_dphi; };
    float sampling() const { return m_sampling; };
    std::vector<int> GetSCIndices() const { return m_scellIndices; };
    std::vector<int> GetTileIndices() const { return m_tileIndices; };
    void SetSCIndices(int sc_id) { m_scellIndices.push_back(sc_id); };
    void SetTileIndices(int tile_id) { m_tileIndices.push_back(tile_id); };
    void SetSampling(int sampling) { m_sampling = sampling; };

    static bool inBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi);
    bool inBox(float eta, float phi) const;

    float area() const { return m_area; }
    void setArea(float value) { m_area = value; }

    // Notes on the tower relation indices:
    // - These all return indices relating to the vectors held in the mapping makers
    // - These are reliable in the central region where the towers are regular and grid-like.
    //   In the forward region this is much less reliable and should not be relied on
    // - These are different to those in the JGTowerBase_IDs as the 'next' eta step always
    //   goes in increasing eta, rather than increasing |eta|.
    // - These also stitch across region boundaries where the granularities allow

    /// The index of the tower before this one in eta (SIZE_MAX if not valid)
    std::size_t previousEtaIndex() const { return m_prevEtaIndex; }
    /// Whether there is a previous eta tower
    bool hasPreviousEta() const { return m_prevEtaIndex != SIZE_MAX; }
    /// The index of the tower after this one in eta (SIZE_MAX if not valid)
    std::size_t nextEtaIndex() const { return m_nextEtaIndex; }
    /// Whether there is a next eta tower
    bool hasNextEta() const { return m_nextEtaIndex != SIZE_MAX; }
    /// The index of the tower before this one in phi (SIZE_MAX if not valid)
    std::size_t previousPhiIndex() const { return m_prevPhiIndex; }
    /// Whether there is a previous phi tower
    bool hasPreviousPhi() const { return m_prevPhiIndex != SIZE_MAX; }
    /// The index of the tower after this one in phi (SIZE_MAX if not valid)
    std::size_t nextPhiIndex() const { return m_nextPhiIndex; }
    /// Whether there is a next phi tower
    bool hasNextPhi() const { return m_nextPhiIndex != SIZE_MAX; }
    /// The index of the tower in front of this one
    std::size_t indexInFront() const { return m_indexInFront; }
    /// Whether there is a tower in front of this one
    bool isFrontTower() const { return m_indexInFront == SIZE_MAX; }
    /// The index of the tower behind this one
    std::size_t indexBehind() const { return m_indexBehind; }
    /// Whether there is a tower behind this one
    bool isBackTower() const { return m_indexBehind == SIZE_MAX; }

    void setPreviousEtaIndex(std::size_t value) { m_prevEtaIndex = value; }
    void setNextEtaIndex(std::size_t value) { m_nextEtaIndex = value; }
    void setPreviousPhiIndex(std::size_t value) { m_prevPhiIndex = value; }
    void setNextPhiIndex(std::size_t value) { m_nextPhiIndex = value; }
    void setIndexInFront(std::size_t value) { m_indexInFront = value; }
    void setIndexBehind(std::size_t value) { m_indexBehind = value; }

  private:
    float m_eta;
    float m_deta;
    float m_phi;
    float m_dphi;
    int m_sampling;
    std::vector<int> m_scellIndices;
    std::vector<int> m_tileIndices;
    float m_area{-1};
    std::size_t m_prevEtaIndex{SIZE_MAX};
    std::size_t m_nextEtaIndex{SIZE_MAX};
    std::size_t m_prevPhiIndex{SIZE_MAX};
    std::size_t m_nextPhiIndex{SIZE_MAX};
    std::size_t m_indexInFront{SIZE_MAX};
    std::size_t m_indexBehind{SIZE_MAX};
  };
} // namespace LVL1
#endif
