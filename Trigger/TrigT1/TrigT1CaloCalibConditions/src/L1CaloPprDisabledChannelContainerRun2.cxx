/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"

#include <string>
#include <iostream>


L1CaloPprDisabledChannelContainerRun2::L1CaloPprDisabledChannelContainerRun2()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") {

  m_coolFoldersKeysMap[L1CaloPprDisabledChannelContainer::ePpmDeadChannels] = std::string("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels");
  m_coolFoldersKeysMap[L1CaloPprDisabledChannelContainer::ePprChanCalib] = std::string("/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib");
  m_coolFoldersKeysMap[L1CaloPprDisabledChannelContainer::eDisabledTowers] = std::string("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers");
}

L1CaloPprDisabledChannelContainerRun2::L1CaloPprDisabledChannelContainerRun2(const std::map<L1CaloPprDisabledChannelContainerRun2::eCoolFolders, std::string>& folderKeysMap)
  : AbstractL1CaloPersistentCondition("CondAttrListCollection")
  , m_coolFoldersKeysMap(folderKeysMap)
{
}

std::string L1CaloPprDisabledChannelContainerRun2::coolFolderKey(L1CaloPprDisabledChannelContainerRun2::eCoolFolders efolder) const {

  std::map<L1CaloPprDisabledChannelContainerRun2::eCoolFolders, std::string>::const_iterator it = m_coolFoldersKeysMap.find(efolder);
  if(it!=m_coolFoldersKeysMap.end()) {
    return it->second;
  } else {
    return std::string("");
  }
}

std::vector<std::string> L1CaloPprDisabledChannelContainerRun2::coolInputKeys() const {
  std::vector<std::string> v;
  v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainerRun2::ePpmDeadChannels));
  v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainerRun2::ePprChanCalib));
  v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainerRun2::eDisabledTowers));
  return v;
}

std::string L1CaloPprDisabledChannelContainerRun2::coolOutputKey() const {
  return std::string("");
}

