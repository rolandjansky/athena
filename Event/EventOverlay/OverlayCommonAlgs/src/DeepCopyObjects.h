/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//// To deep copy various transient objects from one storegate to another
//// Piyali.Banerjee@cern.ch; September 2010
////////////////////////////////////////////////////////////////

#ifndef OVERLAYCOMMONALGS_DEEPCOPYOBJECTS_H
#define OVERLAYCOMMONALGS_DEEPCOPYOBJECTS_H

#include <string>
#include <iostream>
#include <typeinfo>
#include <typeinfo>

#include "OverlayAlgBase/OverlayAlgBase.h"

#include "StoreGate/DataHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileL2Container.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include "InDetSimData/InDetSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include "LArRawEvent/LArTTL1Container.h"
#include "TileEvent/TileTTL1Container.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/CPMTower.h"

class DeepCopyObjects : public OverlayAlgBase  {
public:

  DeepCopyObjects(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();


protected:

  ServiceHandle<StoreGateSvc> m_storeGateTo, m_storeGateFrom;

private:

  /*
    template<class TypeToBeCopied> void
    copyInDetObject(StoreGateSvc* to, StoreGateSvc *from);

    template<class TypeToBeCopied> void
    copyTruthObject(StoreGateSvc* to, StoreGateSvc *from);

    template<class TypeToBeCopied> void
    copyLArObject(StoreGateSvc* to, StoreGateSvc *from);

    template<class TypeToBeCopied> void
    copyTileObject(StoreGateSvc* to, StoreGateSvc *from);

    template<class TypeToBeCopied> void
    copyLevel1Object(StoreGateSvc* to, StoreGateSvc *from);

    template<class TypeToBeCopied, class Datum> void
    copyMuonObjectDigit(StoreGateSvc* from, StoreGateSvc *to);
  */

  template<class TypeToBeCopied> void
  copyMuonObjectRDO(StoreGateSvc* to, StoreGateSvc *from);

  template <class TypeToBeCopied> void
  printEveryObjectOfType(StoreGateSvc* stg);

  bool m_evtinfo;
  bool m_indet;
  bool m_muon;
  bool m_truth;
  bool m_lar;
  bool m_tile;
  bool m_level1;
};

#endif/*OVERLAYCOMMONALGS_DEEPCOPYOBJECTS_H*/
