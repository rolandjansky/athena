// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONSCONTAINERRUN2_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONSCONTAINERRUN2_H

#include "AthenaKernel/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"

#include <map>
#include <string>
#include <vector>

class L1CaloCoolChannelId;
class L1CaloPprConditionsRun2;
/**
 *  Container of L1CaloPprConditions objects, inherit
 *  from the abstract base class AbstractL1CaloConditionContainer.
 *  Implement the IL1CaloPersistenceCapable interface.
 *
 *  Implementation for the Run-2 database.
 * 
 *  @author Veit Scharf
 */
class L1CaloPprConditionsContainerRun2: public DataObject, public AbstractL1CaloPersistentCondition {

private:
  enum eAttrSpecification { eExtBcidThreshold, eSatBcidThreshLow, eSatBcidThreshHigh, eSatBcidLevel, eBcidEnergyRangeLow,
                            eBcidEnergyRangeHigh, eFirStartBit,
                            eBcidDecision1, eSatOverride1, eBcidDecision2, eSatOverride2, eBcidDecision3, eSatOverride3,
                            ePeakFinderCond, eDecisionSource,
                            eFirCoeff1, eFirCoeff2, eFirCoeff3, eFirCoeff4, eFirCoeff5,
                            ePedValue, ePedMean, ePedFirSum,
                            eLutCpStrategy, eLutCpOffset, eLutCpNoiseCut, eLutCpSlope, eLutCpPar1, eLutCpPar2, eLutCpPar3, eLutCpPar4, eLutCpScale,
                            eLutJepStrategy, eLutJepOffset, eLutJepNoiseCut, eLutJepSlope, eLutJepPar1, eLutJepPar2, eLutJepPar3, eLutJepPar4, eLutJepScale};

public:
  enum eCoolFolders { ePprChanDefaults, ePprChanCalib, ePprChanCalibCommon, ePprChanCalibStrategy };

  L1CaloPprConditionsContainerRun2();
  L1CaloPprConditionsContainerRun2(const std::map<L1CaloPprConditionsContainerRun2::eCoolFolders, std::string>& folderKeysMap);
  virtual ~L1CaloPprConditionsContainerRun2();

  // IL1CaloPersistenceCapable interface
  virtual DataObject* makePersistent() const;
  using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

  const L1CaloPprConditionsRun2* pprConditions(unsigned int channelId) const;
  const L1CaloPprConditionsRun2* pprConditions(const L1CaloCoolChannelId& channelId) const;

  inline int bcidDecision1()  const { return m_bcidDecision1; }
  inline int satOverride1()   const { return m_satOverride1; }
  inline int bcidDecision2()  const { return m_bcidDecision2; }
  inline int satOverride2()   const { return m_satOverride2; }
  inline int bcidDecision3()  const { return m_bcidDecision3; }
  inline int satOverride3()   const { return m_satOverride3; }
  inline int peakFinderCond() const { return m_peakFinderCond; }
  inline int decisionSource() const { return m_decisionSource; }

  virtual std::vector<std::string> coolInputKeys() const;
  virtual std::string coolOutputKey() const;

  virtual void clear();

  void dump() const;

private:
  std::string coolFolderKey(L1CaloPprConditionsContainerRun2::eCoolFolders efolder) const;

  typedef std::vector<L1CaloPprConditionsRun2*> L1CaloPprConditionsVec;
  L1CaloPprConditionsVec m_pprConditionsVec;

  std::map<L1CaloPprConditionsContainerRun2::eCoolFolders, std::string> m_coolFoldersKeysMap;

  int m_bcidDecision1 = 0;
  int m_satOverride1 = 0;
  int m_bcidDecision2 = 0;
  int m_satOverride2 = 0;
  int m_bcidDecision3 = 0;
  int m_satOverride3 = 0;
  int m_peakFinderCond = 0;
  int m_decisionSource = 0;

  static const unsigned int s_vectorSize = 8192;
};

CLASS_DEF( L1CaloPprConditionsContainerRun2 , 1152931916 , 1 )

#endif
