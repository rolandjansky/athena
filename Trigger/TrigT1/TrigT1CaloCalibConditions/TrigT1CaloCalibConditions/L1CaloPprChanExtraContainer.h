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
#include "TrigT1CaloCalibConditions/L1CaloPprChanExtra.h"

class CondAttrListCollection;

/***
* Container of L1CaloPprChanCalib objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude ModuleId,PprDacScanResultsTimeStamp,PprPedestalRunResultsTimeStamp,PprNoiseRunResultsTimeStamp,PprTimingResultsTimeStamp,PprSatBcidResultsTimeStamp,PprFirFilterResultsTimeStamp,PprLutValuesResultsTimeStamp,DacOffset,DacSlope,FullDelayData,SyncDelayBcid,InBcidNegedge,rateCpEtThresh,rateJepEtThresh /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib
*/
class L1CaloPprChanExtraContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eChannelId, eErrorCode, ePedCorrEnable, eSat80BcidThreshHigh, eSat80BcidThreshLow, eThreshNumSamplesPeak, eThreshNumSamplesSat, eSatOverride1, eSatOverride2, eSatOverride3, eLutCpPar5, eLutCpPar6, eLutCpPar7, eLutJepPar5, eLutJepPar6, eLutJepPar7};
public:
  L1CaloPprChanExtraContainer();
  L1CaloPprChanExtraContainer(const std::string& folderKey);
  virtual ~L1CaloPprChanExtraContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_pprChanExtras.clear(); }

  // getters
  const L1CaloPprChanExtra* pprChanExtra(unsigned int channelId) const;
  const L1CaloPprChanExtra* pprChanExtra(const L1CaloCoolChannelId& channelId) const {
    return pprChanExtra(channelId.id());
  }

  using iterator = std::vector<L1CaloPprChanExtra>::iterator;
  iterator begin() { return m_pprChanExtras.begin(); }
  iterator end() { return m_pprChanExtras.end(); }
  using const_iterator = std::vector<L1CaloPprChanExtra>::const_iterator;
  const_iterator begin() const { return m_pprChanExtras.begin(); }
  const_iterator end() const { return m_pprChanExtras.end(); }

  // setters
  void addPprChanExtra(const L1CaloPprChanExtra& pprChanExtra);

private:
  std::vector<L1CaloPprChanExtra> m_pprChanExtras;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanExtra";
};

CLASS_DEF( L1CaloPprChanExtraContainer, 1262985908, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBCONTAINER_H
