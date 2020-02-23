/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2MINBIAS_MBTS_FEX_MT_H
#define TRIGT2MINBIAS_T2_MBTS_FEX_MT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "CaloEvent/CaloCellContainerVector.h"
#include "AthContainers/ConstDataVector.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TileConditions/TileEMScale.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"
#include "TileByteStream/TileROD_Decoder.h"
#include <iostream>
#include <random>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <mutex>
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "LArByteStream/LArRodDecoder.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "LArRawUtils/LArTT_Selector.h"
#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileL2Container.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "StoreGate/ReadHandleKey.h"
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
  virtual StatusCode finalize() override;

private:

  ServiceHandle<TileROD_Decoder> m_tileDecoder { this, "tileDecoder", "Tool to decode Tile raw data" };
  SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey ;
  SG::WriteHandleKey<CaloConstCellContainer > m_cellContainerKey;
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_MbtsKey{this,"MbtsKey","Undefined",""};
  SG::ReadHandleKey<TileTBID> m_TileHelperKey{this,"TileHelperKey", "DetectorStore+TileTBID"," "};
  ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc;
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  SG::ReadHandleKey<CaloCellContainer> m_calocellcollectionKey ;

};
#endif
