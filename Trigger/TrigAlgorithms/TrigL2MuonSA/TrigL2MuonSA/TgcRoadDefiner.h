/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCROADDEFINER_H
#define  TRIGL2MUONSA_TGCROADDEFINER_H

#include <string>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"
#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "TrigL2MuonSA/PtEndcapLUT.h"

#include "MDTcabling/IMDTcablingSvc.h"

#include "TrigL2MuonSA/TgcFit.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/MdtRegion.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigMuonEvent/MuonFeature.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "RegionSelector/IRegSelSvc.h"

class StoreGateSvc;

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TgcRoadDefiner
{
 public:
  TgcRoadDefiner(MsgStream* msg);
  ~TgcRoadDefiner(void);
  
  inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }

  bool defineRoad(const LVL1::RecMuonRoI*    p_roi,
		  const TrigL2MuonSA::TgcHits& tgcHits,
		  TrigL2MuonSA::MuonRoad&      muonRoad,
		  TrigL2MuonSA::TgcFitResult&  tgcFitResult);

  void setMsgStream(MsgStream* msg) { m_msg = msg; };
  void setMdtGeometry(const MDTGeometry* mdtGeometry);
  void setMdtGeometry(IRegSelSvc* regionSelector, const MdtIdHelper* mdtIdHelper);
  void setPtLUT(const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc);
  void setRoadWidthForFailure(double rWidth_TGC_Failed);
  void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator);

  inline MsgStream& msg() const { return *m_msg; }
  bool prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits);
  
 private:
  MsgStream* m_msg;
  const MDTGeometry*     m_mdtGeometry;
  ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool;
  const TrigL2MuonSA::PtEndcapLUT*       m_ptEndcapLUT;

  TgcFit     m_tgcFit;

  TrigL2MuonSA::TgcFit::PointArray m_tgcStripMidPoints;  // List of TGC strip middle station points.
  TrigL2MuonSA::TgcFit::PointArray m_tgcWireMidPoints;   // List of TGC wire middle station points.
  TrigL2MuonSA::TgcFit::PointArray m_tgcStripInnPoints;  // List of TGC strip inner station points.
  TrigL2MuonSA::TgcFit::PointArray m_tgcWireInnPoints;   // List of TGC wire inner station points.

  double m_rWidth_TGC_Failed;

  bool m_use_new_geometry;

  IRegSelSvc* m_regionSelector;
  const MdtIdHelper* m_mdtIdHelper;

};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_TGCROADDEFINER_H
