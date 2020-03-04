/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2MINBIAS_MBTS_FEX_MT_H
#define TRIGT2MINBIAS_T2_MBTS_FEX_MT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainerVector.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "CaloEvent/CaloCell.h"

#include "GaudiKernel/StatusCode.h"
#include "TileByteStream/TileCellCont.h"
#include "TileEvent/TileCellCollection.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "CaloIdentifier/TileID.h"
#include "TileEvent/TileCell.h"

#include <string>

class TrigCaloDataAccessSvc;

class MbtsFexMT : public AthReentrantAlgorithm {
public:
  MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MbtsFexMT() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  SG::WriteHandleKey<xAOD::TrigT2MbtsBitsContainer> m_t2MbtsBitsKey{this,"t2MbtsBitsKey","Undefined",""};
  SG::ReadHandleKey<TileTBID> m_TileHelperKey{this,"m_TileHelperKey", "DetectorStore+TileTBID"," "};

  ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc;
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };


};
#endif
