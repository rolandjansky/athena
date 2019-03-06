//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTIMETUNINGNTUPLE
#define LARTIMETUNINGNTUPLE

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/ReadHandleKey.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArCellTimeOffset.h"
#include "TBEvent/TBPhase.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "LArIdentifier/LArOnlineID.h"

#include <string>
#include <map>

class LArTimeTuningNtuple : public AthAlgorithm
{
 public:
  LArTimeTuningNtuple(const std::string & name, ISvcLocator * pSvcLocator);
  virtual ~LArTimeTuningNtuple();

  //standard algorithm methods
  virtual StatusCode initialize() override; 
  virtual StatusCode execute() override;
  virtual StatusCode stop() override;
 private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};
  SG::ReadHandleKey<TBPhase> m_TBPhaseKey
    { this, "TBPhaseKey", "", "" };
  SG::ReadHandleKey<LArGlobalTimeOffset> m_GlobalTimeKey
    { this, "GlobalTimeOffsetKey", "", "" };
  SG::ReadHandleKey<LArFEBTimeOffset> m_FebTimeKey
    { this, "FebTimeOffsetKey", "", "" };

  std::string m_CellTimeOffsetKey;
  
  NTuple::Item<float> m_phaseNt;

  NTuple::Item<float> m_globalTimeNt;

  NTuple::Item<unsigned long> m_nFebNt;

  NTuple::Array<float> m_febTimeNt;
  NTuple::Array<long>  m_febSlotNt;
  NTuple::Array<long>  m_febFTNt;
  NTuple::Array<long>  m_febIDNt;

  NTuple::Item<unsigned long> m_nCellNt;

  NTuple::Array<float> m_cellTimeNt;
  NTuple::Array<long>  m_cellSlotNt;
  NTuple::Array<long>  m_cellFTNt;
  NTuple::Array<long>  m_cellIDNt;
  NTuple::Array<long>  m_cellRegionNt;
  NTuple::Array<long>  m_cellLayerNt; 
  NTuple::Array<long>  m_cellEtaNt; 
  NTuple::Array<long>  m_cellPhiNt; 
  NTuple::Array<long>  m_cellGainNt;  
  NTuple::Array<long>  m_cellChannelNt;
  NTuple::Array<long>  m_cellCalibLineNt;

  //NTuplePtr nt;
  NTuple::Tuple* m_ntuplePtr;

  const LArOnlineID* m_larOnlineHelper;
};

#endif
