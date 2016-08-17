// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalib.h"

class CondAttrListCollection;

/***
* Container of L1CaloPprChanCalib objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude ModuleId,PprDacScanResultsTimeStamp,PprPedestalRunResultsTimeStamp,PprNoiseRunResultsTimeStamp,PprTimingResultsTimeStamp,PprSatBcidResultsTimeStamp,PprFirFilterResultsTimeStamp,PprLutValuesResultsTimeStamp,DacOffset,DacSlope,FullDelayData,SyncDelayBcid,InBcidNegedge,rateCpEtThresh,rateJepEtThresh /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib
*/
class L1CaloPprChanCalibContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eErrorCode, ePedMean, ePedValue, ePedFirSum, eExtBcidThreshold, eSatBcidThreshLow, eSatBcidThreshHigh, eSatBcidLevel, eBcidEnergyRangeLow, eBcidEnergyRangeHigh, eFirStartBit, eFirCoeff1, eFirCoeff2, eFirCoeff3, eFirCoeff4, eFirCoeff5, eLutCpStrategy, eLutCpOffset, eLutCpNoiseCut, eLutCpSlope, eLutCpPar1, eLutCpPar2, eLutCpPar3, eLutCpPar4, eLutCpScale, eLutJepStrategy, eLutJepOffset, eLutJepNoiseCut, eLutJepSlope, eLutJepPar1, eLutJepPar2, eLutJepPar3, eLutJepPar4, eLutJepScale };
public:
  L1CaloPprChanCalibContainer();
  L1CaloPprChanCalibContainer(const std::string& folderKey);
  virtual ~L1CaloPprChanCalibContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_pprChanCalibs.clear(); }

  // getters
  const L1CaloPprChanCalib* pprChanCalib(unsigned int channelId) const;
  const L1CaloPprChanCalib* pprChanCalib(const L1CaloCoolChannelId& channelId) const {
    return pprChanCalib(channelId.id());
  }

  using iterator = std::vector<L1CaloPprChanCalib>::iterator;
  iterator begin() { return m_pprChanCalibs.begin(); }
  iterator end() { return m_pprChanCalibs.end(); }
  using const_iterator = std::vector<L1CaloPprChanCalib>::const_iterator;
  const_iterator begin() const { return m_pprChanCalibs.begin(); }
  const_iterator end() const { return m_pprChanCalibs.end(); }

  // setters
  void addPprChanCalib(const L1CaloPprChanCalib& pprChanCalib);

private:
  std::vector<L1CaloPprChanCalib> m_pprChanCalibs;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib";
};

CLASS_DEF( L1CaloPprChanCalibContainer, 1220333813, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBCONTAINER_H
