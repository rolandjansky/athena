/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMonitorAlgorithm.h"

#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/TileID.h"

StatusCode TileMonitorAlgorithm::initialize() {

  int triggerIdx(0);
  for (const std::string& triggerName : m_fillHistogramsForL1Triggers) {
    L1TriggerTypeBit trigger = getL1TriggerTypeBitFromName(triggerName);
    m_l1Triggers.push_back(trigger);
    m_l1TriggerIndices[trigger] = triggerIdx;
    ++triggerIdx;
  }

  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Will fill histograms for L1 triggers:";
    for (int l1TriggerIdx : m_l1TriggerIndices) {
      if (l1TriggerIdx >= 0) {
        msg(MSG::INFO) << " " << m_l1TriggerBitNames[l1TriggerIdx] << "[" << l1TriggerIdx << "]";
      }
    }
    msg(MSG::INFO) << endmsg;
  }

  return AthMonitorAlgorithm::initialize();
}


TileMonitorAlgorithm::L1TriggerTypeBit
TileMonitorAlgorithm::getL1TriggerTypeBitFromName(const std::string& triggerBitName) const {

  // Convert the triger name to lower case
  std::string loCaseTriggerBitName = triggerBitName;
  std::transform(triggerBitName.begin(), triggerBitName.end(), loCaseTriggerBitName.begin(), ::tolower);

  if( loCaseTriggerBitName == "bit0_rndm" ) {
    return L1TriggerTypeBit::BIT0_RNDM;
  } else if( loCaseTriggerBitName == "bit1_zerobias" ) {
    return L1TriggerTypeBit::BIT1_ZEROBIAS;
  } else if( loCaseTriggerBitName == "bit2_l1cal" ) {
    return L1TriggerTypeBit::BIT2_L1CAL;
  } else if( loCaseTriggerBitName == "bit3_muon" ) {
    return L1TriggerTypeBit::BIT3_MUON;
  } else if( loCaseTriggerBitName == "bit4_rpc" ) {
    return L1TriggerTypeBit::BIT4_RPC;
  } else if( loCaseTriggerBitName == "bit5_ftk" ) {
    return L1TriggerTypeBit::BIT5_FTK;
  } else if( loCaseTriggerBitName == "bit6_ctp" ) {
    return L1TriggerTypeBit::BIT6_CTP;
  } else if( loCaseTriggerBitName == "bit7_calib" ) {
    return L1TriggerTypeBit::BIT7_CALIB;
  } else if( loCaseTriggerBitName == "anyphystrig" ) {
    return L1TriggerTypeBit::ANY_PHYSICS;
  } else { // Otherwise, warn the user and return "AnyPhysTrig"
    ATH_MSG_WARNING("::getL1TriggerTypeBitFromName(): Unknown L1 trigger type bit name: "
                    << triggerBitName << ", returning AnyPhysTrig.");
    return L1TriggerTypeBit::ANY_PHYSICS;
  }
}


std::vector<int> TileMonitorAlgorithm::getL1TriggerIndices(uint32_t lvl1TriggerType) const {

  std::vector<int> triggerIndices;
  int triggerIdx{-1};

  if (lvl1TriggerType != 0) {
    // First bit tells if physics (=1) or calibration (=0) event
    if ((lvl1TriggerType >> BIT7_CALIB) & 1) { // format is 0x1AAAAAAA
      // Always try store at least AnyPhysTrig (=8)
      triggerIdx = m_l1TriggerIndices[ANY_PHYSICS];
      if (triggerIdx >= 0) triggerIndices.push_back(triggerIdx);
      // Adding the phys triggers one by one
      for (int bitTrigger = 0; bitTrigger < BIT7_CALIB; ++bitTrigger) {
        if ((lvl1TriggerType >> bitTrigger) & 1) {
          triggerIdx = m_l1TriggerIndices[bitTrigger];
          if (triggerIdx >= 0) triggerIndices.push_back(triggerIdx);
        }
      }
    } else { // Calibration event foramt is 0x0AAAAAAA
      triggerIdx = m_l1TriggerIndices[BIT7_CALIB];
      if (triggerIdx >= 0) triggerIndices.push_back(triggerIdx);
    }
  } else {
    // Always try store at least AnyPhysTrig (=8)
    triggerIdx = m_l1TriggerIndices[ANY_PHYSICS];
    if (triggerIdx >= 0) triggerIndices.push_back(triggerIdx);
  }

  return triggerIndices;
}

bool TileMonitorAlgorithm::isPhysicsEvent(uint32_t lvl1TriggerType) const {
  // First bit tells if physics (=1) or calibration (=0) event
  return (lvl1TriggerType == 0) || (((lvl1TriggerType >> BIT7_CALIB) & 1) == 1);
}

TileMonitorAlgorithm::L1TriggerTypeBit
TileMonitorAlgorithm::getL1TriggerTypeBit(int lvl1TriggerIdx) const {
  return m_l1Triggers.at(lvl1TriggerIdx);
}

TileMonitorAlgorithm::Partition
TileMonitorAlgorithm::getPartition(const CaloCell* cell, const TileID* tileID) const {
  return cell ? getPartition(cell->ID(), tileID) : MAX_PART;
}

TileMonitorAlgorithm::Partition
TileMonitorAlgorithm::getPartition(IdentifierHash hash, const TileID* tileID) const {
  return getPartition(tileID->cell_id(hash), tileID);
}


TileMonitorAlgorithm::Partition
TileMonitorAlgorithm::getPartition(Identifier id, const TileID* tileID) const {

  Partition partition = MAX_PART; // by default for non Tile cell

  if (tileID->is_tile(id)) {
    int section = tileID->section(id);
    int side = tileID->side(id);

    if (section == 1) {
      partition = (side == 1) ? PART_LBA : PART_LBC;
    } else if (section == 2 || section == 3) {
      partition = (side == 1) ? PART_EBA : PART_EBC;
    }
  }

  return partition;
}

