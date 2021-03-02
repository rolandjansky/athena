/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_MBTS_FEX_MT_H
#define TRIGMINBIAS_MBTS_FEX_MT_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TileIdentifier/TileTBID.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "TileConditions/TileEMScale.h"
#include "TileConditions/TileBadChannels.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"

class MbtsFexMT : public AthReentrantAlgorithm {
public:
  MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  SG::WriteHandleKey<xAOD::TrigT2MbtsBitsContainer> m_MbtsBitsKey{this, "MbtsBitsKey", "unspecified"};
  SG::ReadHandleKey<TileTBID> m_TileHelperKey{this,"TileHelperKey", "DetectorStore+TileTBID"};
  SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this, "TileEMScale", "TileEMScale", "Input Tile EMS calibration constants"};
  SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this, "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};
  ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc{ this, "TrigDataAccessMT", "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", "Data Access for LVL2 Calo Algorithms in MT" };
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};
#endif
