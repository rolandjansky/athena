/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"

#include <iostream>

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"

const unsigned int L1CaloPprConditionsContainerRun2::s_vectorSize;

L1CaloPprConditionsContainerRun2::L1CaloPprConditionsContainerRun2()
    : AbstractL1CaloPersistentCondition("CondAttrListCollection"),
      m_coolFoldersKeysMap(
          {{L1CaloPprConditionsContainerRun2::ePprChanCalib,
            "/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"},
           {L1CaloPprConditionsContainerRun2::ePprChanDefaults,
            "/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"}}) {
  this->addSpecification(eBcidDecision1, std::string("CR12_BcidDecision1"),
                         std::string("Int32"));
  this->addSpecification(eSatOverride1, std::string("CR12_SatOverride1"),
                         std::string("Int32"));
  this->addSpecification(eBcidDecision2, std::string("CR13_BcidDecision2"),
                         std::string("Int32"));
  this->addSpecification(eSatOverride2, std::string("CR13_SatOverride2"),
                         std::string("Int32"));
  this->addSpecification(eBcidDecision3, std::string("CR14_BcidDecision3"),
                         std::string("Int32"));
  this->addSpecification(eSatOverride3, std::string("CR14_SatOverride3"),
                         std::string("Int32"));
  this->addSpecification(ePeakFinderCond, std::string("CR15_PeakFinderCond"),
                         std::string("Int32"));
  this->addSpecification(eDecisionSource, std::string("CR15_DecisionSource"),
                         std::string("Int32"));

  // ------------------------------------------------------------------------------------------------------------------
  // Common
  // ------------------------------------------------------------------------------------------------------------------
  // Define DB rows names and types in order to construct the
  // AttributeListSpecification object
  this->addSpecification(eExtBcidThreshold, std::string("ExtBcidThreshold"),
                         std::string("UInt16"));
  this->addSpecification(eSatBcidThreshLow, std::string("SatBcidThreshLow"),
                         std::string("UInt16"));
  this->addSpecification(eSatBcidThreshHigh, std::string("SatBcidThreshHigh"),
                         std::string("UInt16"));
  this->addSpecification(eSatBcidLevel, std::string("SatBcidLevel"),
                         std::string("UInt16"));
  this->addSpecification(eBcidEnergyRangeLow, std::string("BcidEnergyRangeLow"),
                         std::string("UInt16"));
  this->addSpecification(eBcidEnergyRangeHigh,
                         std::string("BcidEnergyRangeHigh"),
                         std::string("UInt16"));

  this->addSpecification(ePedValue, std::string("PedValue"),
                         std::string("UInt32"));
  this->addSpecification(ePedMean, std::string("PedMean"),
                         std::string("Double"));

  this->addSpecification(eLutCpStrategy, std::string("LutCpStrategy"),
                         std::string("UInt16"));

  this->addSpecification(eLutCpScale, std::string("LutCpScale"),
                         std::string("UInt16"));
  this->addSpecification(eLutCpPar1, std::string("LutCpPar1"),
                         std::string("UInt16"));
  this->addSpecification(eLutCpPar2, std::string("LutCpPar2"),
                         std::string("UInt16"));
  this->addSpecification(eLutCpPar3, std::string("LutCpPar3"),
                         std::string("UInt16"));
  this->addSpecification(eLutCpPar4, std::string("LutCpPar4"),
                         std::string("UInt16"));

  this->addSpecification(eLutJepStrategy, std::string("LutJepStrategy"),
                         std::string("UInt16"));

  this->addSpecification(eLutJepScale, std::string("LutJepScale"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepPar1, std::string("LutJepPar1"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepPar2, std::string("LutJepPar2"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepPar3, std::string("LutJepPar3"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepPar4, std::string("LutJepPar4"),
                         std::string("UInt16"));

  // Missing attributes in model with strategy
  this->addSpecification(ePedFirSum, std::string("PedFirSum"),
                         std::string("UInt32"));
  this->addSpecification(eLutCpOffset, std::string("LutCpOffset"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepOffset, std::string("LutJepOffset"),
                         std::string("UInt16"));
  // ------------------------------------------------------------------------------------------------------------------
  // Strategy
  // ------------------------------------------------------------------------------------------------------------------
  this->addSpecification(eFirStartBit, std::string("FirStartBit"),
                         std::string("UInt16"));
  this->addSpecification(eFirCoeff1, std::string("FirCoeff1"),
                         std::string("short"));
  this->addSpecification(eFirCoeff2, std::string("FirCoeff2"),
                         std::string("short"));
  this->addSpecification(eFirCoeff3, std::string("FirCoeff3"),
                         std::string("short"));
  this->addSpecification(eFirCoeff4, std::string("FirCoeff4"),
                         std::string("short"));
  this->addSpecification(eFirCoeff5, std::string("FirCoeff5"),
                         std::string("short"));
  this->addSpecification(eLutCpNoiseCut, std::string("LutCpNoiseCut"),
                         std::string("UInt16"));
  this->addSpecification(eLutCpSlope, std::string("LutCpSlope"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepNoiseCut, std::string("LutJepNoiseCut"),
                         std::string("UInt16"));
  this->addSpecification(eLutJepSlope, std::string("LutJepSlope"),
                         std::string("UInt16"));
  // ------------------------------------------------------------------------------------------------------------------

  m_pprConditionsVec.reserve(s_vectorSize);
  this->clear();
}

L1CaloPprConditionsContainerRun2::L1CaloPprConditionsContainerRun2(
    const std::map<L1CaloPprConditionsContainerRun2::eCoolFolders, std::string>&
        folderKeysMap)
    : L1CaloPprConditionsContainerRun2()  // delegating constructor
{
  m_coolFoldersKeysMap = folderKeysMap;
}

L1CaloPprConditionsContainerRun2::~L1CaloPprConditionsContainerRun2() {
  this->clear();
}

std::string L1CaloPprConditionsContainerRun2::coolFolderKey(
    L1CaloPprConditionsContainerRun2::eCoolFolders efolder) const {
  auto it = m_coolFoldersKeysMap.find(efolder);
  if (it != m_coolFoldersKeysMap.end()) {
    return it->second;
  } else {
    return std::string("");
  }
}

std::vector<std::string> L1CaloPprConditionsContainerRun2::coolInputKeys()
    const {
  std::vector<std::string> result{
      this->coolFolderKey(L1CaloPprConditionsContainerRun2::ePprChanDefaults)};
  std::string calibKey =
      this->coolFolderKey(L1CaloPprConditionsContainerRun2::ePprChanCalib);

  if (!calibKey.empty()) {
    result.push_back(calibKey);
  } else {
    result.push_back(this->coolFolderKey(
        L1CaloPprConditionsContainerRun2::ePprChanCalibCommon));
    result.push_back(this->coolFolderKey(
        L1CaloPprConditionsContainerRun2::ePprChanCalibStrategy));
  }

  return result;
}

std::string L1CaloPprConditionsContainerRun2::coolOutputKey() const {
  return this->coolFolderKey(L1CaloPprConditionsContainerRun2::ePprChanCalib);
}

DataObject* L1CaloPprConditionsContainerRun2::makePersistent() const {
  // this container is not supposed to be saved to COOL
  return 0;
}

void L1CaloPprConditionsContainerRun2::makeTransient(const std::map<
    std::string, CondAttrListCollection*> condAttrListCollectionMap) {
  this->clear();
  // --------------------------------------------------------------------------
  // Folder names
  std::string chanCalibFolderKey =
      this->coolFolderKey(L1CaloPprConditionsContainerRun2::ePprChanCalib);
  std::string chanCalibCommonFolderKey = this->coolFolderKey(
      L1CaloPprConditionsContainerRun2::ePprChanCalibCommon);
  std::string chanCalibStrategyFolderKey = this->coolFolderKey(
      L1CaloPprConditionsContainerRun2::ePprChanCalibStrategy);

  decltype(condAttrListCollectionMap)::const_iterator
      it_pprChanCalibAttrListCollection = condAttrListCollectionMap.end();
  
  decltype(condAttrListCollectionMap)::const_iterator
      it_pprChanCalibStrategyAttrListCollection =
          condAttrListCollectionMap.end();
  // --------------------------------------------------------------------------
  bool isUseStrategy = false;
  // Check that folders exist
  if (!chanCalibFolderKey.empty()) {
    it_pprChanCalibAttrListCollection =
        condAttrListCollectionMap.find(chanCalibFolderKey);
  } else {
    isUseStrategy = true;
    it_pprChanCalibAttrListCollection =
        condAttrListCollectionMap.find(chanCalibCommonFolderKey);
    
    it_pprChanCalibStrategyAttrListCollection =
        condAttrListCollectionMap.find(chanCalibStrategyFolderKey);
  }
  // --------------------------------------------------------------------------
  if (isUseStrategy) {
    // Check that strategy folder exists
    if (it_pprChanCalibStrategyAttrListCollection ==
        condAttrListCollectionMap.end()) {
      std::cout << "L1CaloPprConditionsContainerRun2 : Could not find "
                   "requested CondAttrListCollection "
                << chanCalibStrategyFolderKey << std::endl;
      return;
    }

    // Check that common folder exists
    if (it_pprChanCalibAttrListCollection == condAttrListCollectionMap.end()) {
      std::cout << "L1CaloPprConditionsContainerRun2 : Could not find "
                   "requested CondAttrListCollection "
                << chanCalibCommonFolderKey << std::endl;
      return;
    }
  } else {
    if (it_pprChanCalibAttrListCollection == condAttrListCollectionMap.end()) {
      std::cout << "L1CaloPprConditionsContainerRun2 : Could not find "
                   "requested CondAttrListCollection "
                << chanCalibCommonFolderKey << std::endl;
      return;
    }
  }
  // --------------------------------------------------------------------------
  CondAttrListCollection* chanCalibAttrListCollection =
      it_pprChanCalibAttrListCollection->second;

  std::string chanDefaultsFolderKey(
      this->coolFolderKey(L1CaloPprConditionsContainerRun2::ePprChanDefaults));
  auto it_pprChanDefaultsAttrListCollection =
      condAttrListCollectionMap.find(chanDefaultsFolderKey);
  if (it_pprChanDefaultsAttrListCollection == condAttrListCollectionMap.end()) {
    std::cout << "L1CaloPprConditionsContainerRun2 : Could not find requested "
                 "CondAttrListCollection " << chanDefaultsFolderKey
              << std::endl;
    return;
  }
  // --------------------------------------------------------------------------
  CondAttrListCollection* chanDefaultsAttrListCollection =
      it_pprChanDefaultsAttrListCollection->second;

  // There should be only one channel (channel#1) in the Default folder
  // we just retrieve that one, waiting for a better method to retrieve that
  // information.
  const int defaultChannel = 1;
  const AthenaAttributeList& chanDefaultAttrList(
      chanDefaultsAttrListCollection->attributeList(defaultChannel));

  m_bcidDecision1 =
      chanDefaultAttrList[this->specificationName(eBcidDecision1)].data<int>();
  m_satOverride1 =
      chanDefaultAttrList[this->specificationName(eSatOverride1)].data<int>();
  m_bcidDecision2 =
      chanDefaultAttrList[this->specificationName(eBcidDecision2)].data<int>();
  m_satOverride2 =
      chanDefaultAttrList[this->specificationName(eSatOverride2)].data<int>();
  m_bcidDecision3 =
      chanDefaultAttrList[this->specificationName(eBcidDecision3)].data<int>();
  m_satOverride3 =
      chanDefaultAttrList[this->specificationName(eSatOverride3)].data<int>();
  m_peakFinderCond =
      chanDefaultAttrList[this->specificationName(ePeakFinderCond)].data<int>();
  m_decisionSource =
      chanDefaultAttrList[this->specificationName(eDecisionSource)].data<int>();
  // ------------------------------------------------------------------------
  // loop over CondAttrListCollection
  auto it_AttrListColl = chanCalibAttrListCollection->begin();
  auto it_AttrListCollE = chanCalibAttrListCollection->end();
  // --------------------------------------------------------------------------
  for (; it_AttrListColl != it_AttrListCollE; ++it_AttrListColl) {
    // ------------------------------------------------------------------------
    L1CaloCoolChannelId coolId(it_AttrListColl->first);
    unsigned int index = (coolId.crate() << 10) + (coolId.module() << 6) +
                         (coolId.subModule() << 2) + coolId.channel();
    // ------------------------------------------------------------------------                     
    if (index >= s_vectorSize) {
      continue;
    }
    // ------------------------------------------------------------------------
    const AthenaAttributeList& chanCalibAttrList(it_AttrListColl->second);
    // ------------------------------------------------------------------------
    unsigned short extBcidThreshold =
        chanCalibAttrList[this->specificationName(eExtBcidThreshold)]
            .data<unsigned short>();
    unsigned short satBcidThreshLow =
        chanCalibAttrList[this->specificationName(eSatBcidThreshLow)]
            .data<unsigned short>();
    unsigned short satBcidThreshHigh =
        chanCalibAttrList[this->specificationName(eSatBcidThreshHigh)]
            .data<unsigned short>();
    unsigned short satBcidLevel =
        chanCalibAttrList[this->specificationName(eSatBcidLevel)]
            .data<unsigned short>();

    unsigned short bcidEnergyRangeLow =
        chanCalibAttrList[this->specificationName(eBcidEnergyRangeLow)]
            .data<unsigned short>();
    unsigned short bcidEnergyRangeHigh =
        chanCalibAttrList[this->specificationName(eBcidEnergyRangeHigh)]
            .data<unsigned short>();
    unsigned short lutCpStrategy =
        chanCalibAttrList[this->specificationName(eLutCpStrategy)]
            .data<unsigned short>();

    short lutCpPar1 =
        chanCalibAttrList[this->specificationName(eLutCpPar1)].data<short>();
    short lutCpPar2 =
        chanCalibAttrList[this->specificationName(eLutCpPar2)].data<short>();
    short lutCpPar3 =
        chanCalibAttrList[this->specificationName(eLutCpPar3)].data<short>();
    short lutCpPar4 =
        chanCalibAttrList[this->specificationName(eLutCpPar4)].data<short>();
    unsigned short lutJepStrategy =
        chanCalibAttrList[this->specificationName(eLutJepStrategy)]
            .data<unsigned short>();

    unsigned short lutCpScale =
        chanCalibAttrList[this->specificationName(eLutCpScale)]
            .data<unsigned short>();
    unsigned short lutJepScale =
        chanCalibAttrList[this->specificationName(eLutJepScale)]
            .data<unsigned short>();
    short lutJepPar1 =
        chanCalibAttrList[this->specificationName(eLutJepPar1)].data<short>();
    short lutJepPar2 =
        chanCalibAttrList[this->specificationName(eLutJepPar2)].data<short>();
    short lutJepPar3 =
        chanCalibAttrList[this->specificationName(eLutJepPar3)].data<short>();
    short lutJepPar4 =
        chanCalibAttrList[this->specificationName(eLutJepPar4)].data<short>();
    unsigned int pedValue =
        chanCalibAttrList[this->specificationName(ePedValue)]
            .data<unsigned int>();
    float pedMean =
        (float)
        chanCalibAttrList[this->specificationName(ePedMean)].data<double>();

    unsigned short lutJepOffset =
        isUseStrategy
            ? 0
            : chanCalibAttrList[this->specificationName(eLutJepOffset)]
                  .data<unsigned short>();
    unsigned short lutCpOffset =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eLutCpOffset)]
                            .data<unsigned short>();
    unsigned int pedFirSum =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(ePedFirSum)]
                            .data<unsigned int>();

    unsigned short firStartBit =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eFirStartBit)]
                            .data<unsigned short>();
    short int firCoeff1 =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eFirCoeff1)]
                            .data<short>();
    short int firCoeff2 =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eFirCoeff2)]
                            .data<short>();
    short int firCoeff3 =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eFirCoeff3)]
                            .data<short>();
    short int firCoeff4 =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eFirCoeff4)]
                            .data<short>();
    short int firCoeff5 =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eFirCoeff5)]
                            .data<short>();

    unsigned short lutCpSlope =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eLutCpSlope)]
                            .data<unsigned short>();
    unsigned short lutCpNoiseCut =
        isUseStrategy
            ? 0
            : chanCalibAttrList[this->specificationName(eLutCpNoiseCut)]
                  .data<unsigned short>();
    unsigned short lutJepSlope =
        isUseStrategy ? 0
                      : chanCalibAttrList[this->specificationName(eLutJepSlope)]
                            .data<unsigned short>();
    unsigned short lutJepNoiseCut =
        isUseStrategy
            ? 0
            : chanCalibAttrList[this->specificationName(eLutJepNoiseCut)]
                  .data<unsigned short>();
    // ------------------------------------------------------------------------
    m_pprConditionsVec[index] = new L1CaloPprConditionsRun2(
        extBcidThreshold, satBcidThreshLow, satBcidThreshHigh, satBcidLevel,
        bcidEnergyRangeLow, bcidEnergyRangeHigh, firStartBit, firCoeff1,
        firCoeff2, firCoeff3, firCoeff4, firCoeff5, lutCpStrategy, lutCpOffset,
        lutCpSlope, lutCpNoiseCut, lutCpPar1, lutCpPar2, lutCpPar3, lutCpPar4,
        lutCpScale, lutJepStrategy, lutJepOffset, lutJepSlope, lutJepNoiseCut,
        lutJepPar1, lutJepPar2, lutJepPar3, lutJepPar4, lutJepScale, pedValue,
        pedMean, pedFirSum);
  }
  // --------------------------------------------------------------------------
  if (isUseStrategy){
    // ------------------------------------------------------------------------
    CondAttrListCollection* chanCalibAttrListCollection =
      it_pprChanCalibStrategyAttrListCollection->second; 
    auto it_AttrListColl = chanCalibAttrListCollection->begin();
    auto it_AttrListCollE = chanCalibAttrListCollection->end();
    // ------------------------------------------------------------------------
    for (; it_AttrListColl != it_AttrListCollE; ++it_AttrListColl) {
      L1CaloCoolChannelId coolId(it_AttrListColl->first);
      unsigned int index = (coolId.crate() << 10) + (coolId.module() << 6) +
                           (coolId.subModule() << 2) + coolId.channel();

      if (index >= s_vectorSize) {
        continue;
      }

      const AthenaAttributeList& chanCalibAttrList(it_AttrListColl->second);

      if (m_pprConditionsVec[index] == nullptr){
          std::cout << "L1CaloPprConditionsContainerRun2 : Could not find channel "
                 << " with index " << index << std::endl;
          return;
      }

      unsigned short firStartBit =
          chanCalibAttrList[this->specificationName(eFirStartBit)]
              .data<unsigned short>();
      short int firCoeff1 =
          chanCalibAttrList[this->specificationName(eFirCoeff1)].data<short>();
      short int firCoeff2 =
          chanCalibAttrList[this->specificationName(eFirCoeff2)].data<short>();
      short int firCoeff3 =
          chanCalibAttrList[this->specificationName(eFirCoeff3)].data<short>();
      short int firCoeff4 =
          chanCalibAttrList[this->specificationName(eFirCoeff4)].data<short>();
      short int firCoeff5 =
          chanCalibAttrList[this->specificationName(eFirCoeff5)].data<short>();

      unsigned short lutCpSlope =
          chanCalibAttrList[this->specificationName(eLutCpSlope)]
              .data<unsigned short>();
      unsigned short lutCpNoiseCut =
          chanCalibAttrList[this->specificationName(eLutCpNoiseCut)]
              .data<unsigned short>();
      unsigned short lutJepSlope =
          chanCalibAttrList[this->specificationName(eLutJepSlope)]
              .data<unsigned short>();
      unsigned short lutJepNoiseCut =
          chanCalibAttrList[this->specificationName(eLutJepNoiseCut)]
              .data<unsigned short>();
      m_pprConditionsVec[index]->initializeByStrategy(
          firStartBit, firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5,
          lutCpSlope, lutCpNoiseCut, lutJepSlope, lutJepNoiseCut);

    } // end for
    // ------------------------------------------------------------------------
  } // end isUseStrategy
  // --------------------------------------------------------------------------
 
}

const L1CaloPprConditionsRun2* L1CaloPprConditionsContainerRun2::pprConditions(
    unsigned int channelId) const {
  L1CaloCoolChannelId coolId(channelId);
  return pprConditions(coolId);
}

const L1CaloPprConditionsRun2* L1CaloPprConditionsContainerRun2::pprConditions(
    const L1CaloCoolChannelId& channelId) const {
  unsigned int index = (channelId.crate() << 10) + (channelId.module() << 6) +
                       (channelId.subModule() << 2) + channelId.channel();
  if (index < s_vectorSize)
    return m_pprConditionsVec[index];
  else
    return 0;
}

void L1CaloPprConditionsContainerRun2::dump() const {
  std::cout << "bcidDecision1: " << m_bcidDecision1 << ", "
            << "satOverride1: " << m_satOverride1 << ", "
            << "bcidDecision2: " << m_bcidDecision2 << ", "
            << "satOverride2: " << m_satOverride2 << ", "
            << "bcidDecision3: " << m_bcidDecision3 << ", "
            << "satOverride3: " << m_satOverride3 << ", "
            << "peakFinderCond: " << m_peakFinderCond << ", "
            << "decisionSource: " << m_decisionSource << std::endl;
  std::size_t index(0);
  for (auto* C : m_pprConditionsVec) {
    if (C) std::cout << "index " << index++ << " * item: " << *C << std::endl;
  }
}

void L1CaloPprConditionsContainerRun2::clear() {
  for (L1CaloPprConditionsRun2* p : m_pprConditionsVec) {
    if (p) delete p;
  }
  m_pprConditionsVec.assign(s_vectorSize, nullptr);
}
