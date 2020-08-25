/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHWMAPCONDALG_H
#define TRTHWMAPCONDALG_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include "TRT_ConditionsData/HWMap.h"

class TRTHWMapCondAlg : public AthAlgorithm
{
 public:
  TRTHWMapCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTHWMapCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  StatusCode build_BarrelHVLinePadMaps(EventIDRange& range, TRTCond::HWMap* writeCdo) const;
  StatusCode build_EndcapHVLinePadMaps(EventIDRange& range, TRTCond::HWMap* writeCdo) const;
  int hashThisBarrelPad( int sector, int module, int padNum ) const;
  int hashThisEndcapCell( int sector, int wheel, int layer, int cellNum ) const;

 private:
  ServiceHandle<ICondSvc> m_condSvc;

  SG::ReadCondHandleKey<CondAttrListCollection> m_BarrelReadKey{this,"BarrelHWReadKey","TRT/DCS/HV/BARREL","Barrel HV in-key"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_EndAReadKey{this,"EndcapAHWReadKey","TRT/DCS/HV/ENDCAPA","EndcapA HV in-key"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_EndCReadKey{this,"EndcapCHWReadKey","TRT/DCS/HV/ENDCAPC","EndcapC HV in-key"};
  SG::WriteCondHandleKey<TRTCond::HWMap> m_WriteKey{this,"HWMapWriteKey","HWMap","HWMap out-key"};

};
#endif
