/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETOVERLAY_TRTOVERLAY_H
#define INDETOVERLAY_TRTOVERLAY_H

#include "IDC_OverlayBase/IDC_OverlayBase.h"
#include "InDetRawData/TRT_RDO_Container.h"

#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

class TRT_ID;

namespace CLHEP {
  class HepRandomEngine;
}

typedef InDetRawDataCollection<TRT_RDORawData> TRT_RDO_Collection;

class TRTOverlay : public IDC_OverlayBase
{
public:

  TRTOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  void overlayTRTContainers(const TRT_RDO_Container *bkgContainer,
                            const TRT_RDO_Container *signalContainer,
                            TRT_RDO_Container *outputContainer,
                            std::map<int, double> &occupancyMap,
                            const InDetSimDataCollection &SDO_Map);

  void mergeTRTCollections(TRT_RDO_Collection *bkgCollection,
                           TRT_RDO_Collection *signalCollection,
                           TRT_RDO_Collection *outputCollection,
                           double occupancy,
                           const InDetSimDataCollection &SDO_Map,
                           CLHEP::HepRandomEngine* rndmEngine);

  const TRT_ID *m_trtId{};

  SG::ReadHandleKey<TRT_RDO_Container> m_bkgInputKey{this, "BkgInputKey", "OriginalEvent_SG+TRT_RDOs"," ReadHandleKey for Background Input TRT_RDO_Container"};
  SG::ReadHandleKey<TRT_RDO_Container> m_signalInputKey{this, "SignalInputKey", "BkgEvent_0_SG+TRT_RDOs", "ReadHandleKey for Signal Input TRT_RDO_Container"};
  SG::WriteHandleKey<TRT_RDO_Container> m_outputKey{this, "OutputKey", "StoreGateSvc+TRT_RDOs", "WriteHandleKey for Output TRT_RDO_Container"};
  SG::ReadHandleKey<InDetSimDataCollection> m_signalInputSDOKey{this, "SignalInputSDOKey", "BkgEvent_0_SG+TRT_SDO_Map", "ReadHandleKey for Signal Input InDetSimDataCollection for TRT"};

  BooleanProperty m_includeBkg { this, "includeBkg", true, "Include Background RDO Container" };

  // Following tools, services and configurables are there only for the correct of HT hits
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service"};      // Random number service

  double                                 m_HTOccupancyCorrectionB;
  double                                 m_HTOccupancyCorrectionEC;
  double                                 m_HTOccupancyCorrectionB_noE;
  double                                 m_HTOccupancyCorrectionEC_noE;
  ToolHandle<InDet::ITRT_LocalOccupancy> m_TRT_LocalOccupancyTool{this, "TRT_LocalOccupancyTool", "TRT_LocalOccupancy", ""};
  ServiceHandle<ITRT_StrawStatusSummarySvc>   m_TRTStrawSummarySvc{this, "TRTStrawSummarySvc", "TRT_StrawStatusSummarySvc", ""};

};

#endif // INDETOVERLAY_TRTOVERLAY_H
