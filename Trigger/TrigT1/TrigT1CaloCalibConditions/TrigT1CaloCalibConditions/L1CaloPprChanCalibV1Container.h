// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBV1CONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBV1CONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class L1CaloPprChanCalibV1;

/***
* Container of L1CaloPprChanCalibV1 objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude ModuleId,PprDacScanResultsTimeStamp,PprPedestalRunResultsTimeStamp,PprNoiseRunResultsTimeStamp,PprTimingResultsTimeStamp,PprSatBcidResultsTimeStamp,PprFirFilterResultsTimeStamp,PprLutValuesResultsTimeStamp,DacOffset,DacSlope,FullDelayData,SyncDelayBcid,InBcidNegedge,rateCpEtThresh,rateJepEtThresh /TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib --objectName L1CaloPprChanCalibV1 --containerName L1CaloPprChanCalibV1Container
*/
class L1CaloPprChanCalibV1Container : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eErrorCode, ePedMean, ePedValue, eExtBcidThreshold, eSatBcidThreshLow, eSatBcidThreshHigh, eSatBcidLevel, eBcidEnergyRangeLow, eBcidEnergyRangeHigh, eFirStartBit, eFirCoeff1, eFirCoeff2, eFirCoeff3, eFirCoeff4, eFirCoeff5, eLutStrategy, eLutOffset, eLutNoiseCut, eLutSlope };
public:
  L1CaloPprChanCalibV1Container();
  L1CaloPprChanCalibV1Container(const std::string& folderKey);
  virtual ~L1CaloPprChanCalibV1Container() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_pprChanCalibs.clear(); }

  // getters
  const L1CaloPprChanCalibV1* pprChanCalibV1(unsigned int channelId) const;
  const L1CaloPprChanCalibV1* pprChanCalibV1(const L1CaloCoolChannelId& channelId) const {
    return pprChanCalibV1(channelId.id());
  }

  using const_iterator = std::vector<L1CaloPprChanCalibV1>::const_iterator;
  const_iterator begin() const;
  const_iterator end() const;

  // setters
  void addPprChanCalibV1(const L1CaloPprChanCalibV1& pprChanCalib);

private:
  std::vector<L1CaloPprChanCalibV1> m_pprChanCalibs;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib";
};

CLASS_DEF( L1CaloPprChanCalibV1Container, 1101882098, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBV1CONTAINER_H
