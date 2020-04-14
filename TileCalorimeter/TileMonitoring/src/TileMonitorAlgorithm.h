/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEMONITORALGORITHM_H
#define TILEMONITORING_TILEMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

class CaloCell;
class TileID;

/** @class TileMonitorAlgorithm
 *  @brief Base class for Tile monitoring per L1 trigger type
 */

class TileMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    TileMonitorAlgorithm(const std::string& name, ISvcLocator* svcLocator)
      :AthMonitorAlgorithm(name, svcLocator), m_l1TriggerIndices(9, -1) {}

    using AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override = 0;

   /**
    * @enum L1TriggerTypeBit
    * @brief Describes L1 trigger type bits
    */
    enum L1TriggerTypeBit {BIT0_RNDM, BIT1_ZEROBIAS, BIT2_L1CAL, BIT3_MUON,
                           BIT4_RPC, BIT5_FTK, BIT6_CTP, BIT7_CALIB, ANY_PHYSICS};

   /**
    * @enum AuxiliarySampling
    * @brief Describes Tile auxiliary sampling
    */
    enum AuxiliarySampling {SAMP_ALL = 4, MAX_SAMP = 5};

   /**
    * @enum Partition
    * @brief Describes Tile partitions (ROS - 1)
    */
    enum Partition {PART_LBA, PART_LBC, PART_EBA,
                    PART_EBC, PART_ALL, MAX_PART}; // ROS - 1

   /**
    * @brief Return indices of histograms to be filled according fired L1 trigger type
    * @param lvl1TriggerType Level1 Trigger Type
    * @return vector of indices of histograms to be filled
    */
    std::vector<int> getL1TriggerIndices(uint32_t lvl1TriggerType) const;

   /**
    * @brief Return Level1 Trigger type bit according trigger index
    * @param lvl1TriggerIdx Level1 Trigger index
    * @return level1 trigger type bit according trigger index
    */
    L1TriggerTypeBit getL1TriggerTypeBit(int lvl1TriggerIdx) const;

   /**
    * @brief Return number of L1 triggers for which histograms should be filled
    */
    int getNumberOfL1Triggers(void) const {return m_fillHistogramsForL1Triggers.size();};

   /**
    * @brief Return true if it is physics event or false for calibration event
    * @param lvl1TriggerType Level1 Trigger Type
    * @return true if it is physics event according L1 trigger type
    */
    bool isPhysicsEvent(uint32_t lvl1TriggerType) const;

  /**
    * @brief Return Partition for Tile cell or MAX_PART otherwise
    * @param cell Calo cell
    */
    Partition getPartition(const CaloCell* cell, const TileID* tileID) const;

   /**
    * @brief Return Partition for Tile cell identifier or MAX_PART otherwise
    * @param id Calo cell identifier
    */
    Partition getPartition(Identifier id, const TileID* tileID) const;

    /**
    * @brief Return Partition for Tile cell identifier hash or MAX_PART otherwise
    * @param hash Calo cell identifier hash
    */
    Partition getPartition(IdentifierHash hash, const TileID* tileID) const;


  private:

    L1TriggerTypeBit getL1TriggerTypeBitFromName(const std::string& triggerBitName) const;

    Gaudi::Property<std::vector<std::string>> m_fillHistogramsForL1Triggers{this,
         "fillHistogramsForL1Triggers", {}, "Fill histograms per given L1 trigger types"};

    std::vector<L1TriggerTypeBit> m_l1Triggers;
    std::vector<int> m_l1TriggerIndices;
    std::vector<std::string> m_l1TriggerBitNames{"bit0_RNDM", "bit1_ZeroBias", "bit2_L1CAL", "bit3_MUON",
                                                 "bit4_RPC", "bit5_FTK", "bti6_CTP", "bit7_Calib", "AnyPhysTrig"};
};


#endif // TILEMONITORING_TILEMONITORALGORITHM_H
