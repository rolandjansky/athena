/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOMONITORING_TILECALMONALG_H
#define CALOMONITORING_TILECALMONALG_H

#include "CaloMonAlgBase.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

/** @class TileCalCellMonAlg
 *  @brief Class for TileCal Cell based monitoring
 */
class TileCalCellMonAlg : public CaloMonAlgBase {

  public:

    using CaloMonAlgBase::CaloMonAlgBase;
    virtual ~TileCalCellMonAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    Gaudi::Property<std::string> m_monGroupName{this, "MonGroupName", "TileCalGroup"};
    Gaudi::Property<bool> m_twoGaussianNoise{this, "TwoGaussianNoise", true, "Use 2-gaussian noise"};

    Gaudi::Property<float> m_energyThreshold{this,
       "EnergyThreshold", 300.0F, "Energy threshold in MeV"};

    SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey{this,
       "CaloCellContainer", "AllCalo", "Calo cell container name"};

    /**
     * @brief Key of the CaloNoise Conditions data object.
     * Typical values are '"electronicNoise', 'pileupNoise', or '"totalNoise' (default)
     */
    SG::ReadCondHandleKey<CaloNoise> m_caloNoiseKey{this,
       "CaloNoise", "totalNoise", "Calo noise object name"};

    const TileID* m_tileID{nullptr};
    static const int N_TILE_SAMPLES{4};

    std::vector<int> m_noiseEtaPhiGroups;
    std::vector<int> m_energyModuleGroups;
};


#endif // CALOMONITORING_TILECALMONALG_H
