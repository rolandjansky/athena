/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCROADDEFINER_H
#define  TRIGL2MUONSA_TGCROADDEFINER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"
#include "PtEndcapLUTSvc.h"
#include "PtEndcapLUT.h"
#include "TgcFit.h"
#include "TgcData.h"
#include "MuonRoad.h"
#include "MdtRegion.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "RegionSelector/IRegSelSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TgcRoadDefiner: public AthAlgTool
{
 public:
  TgcRoadDefiner(const std::string& type, 
		 const std::string& name,
		 const IInterface*  parent);
  
  virtual StatusCode initialize() override;

  StatusCode defineRoad(const LVL1::RecMuonRoI*      p_roi,
                        const bool                   insideOut,
                        const TrigL2MuonSA::TgcHits& tgcHits,
                        TrigL2MuonSA::MuonRoad&      muonRoad,
                        TrigL2MuonSA::TgcFitResult&  tgcFitResult);

  void setMdtGeometry(const ServiceHandle<IRegSelSvc>& regionSelector) { m_regionSelector = regionSelector; };
  void setPtLUT(const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc) { m_ptEndcapLUT = ptEndcapLUTSvc->ptEndcapLUT(); };
  void setRoadWidthForFailure(double rWidth_TGC_Failed) { m_rWidth_TGC_Failed = rWidth_TGC_Failed; };
  void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator) { m_backExtrapolatorTool = backExtrapolator; };

  bool prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits);
  
 private:
  // setted in MuFastSteering::hltInitialize, setExtrapolatorTool
  ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool {nullptr};
  // setted in MuFastSteering::hltInitialize, setMCFlag  
  const ToolHandle<PtEndcapLUT>*         m_ptEndcapLUT {nullptr};

  ToolHandle<TgcFit>                     m_tgcFit {"TrigL2MuonSA::TgcFit"};

  TrigL2MuonSA::TgcFit::PointArray m_tgcStripMidPoints;  // List of TGC strip middle station points.
  TrigL2MuonSA::TgcFit::PointArray m_tgcWireMidPoints;   // List of TGC wire middle station points.
  TrigL2MuonSA::TgcFit::PointArray m_tgcStripInnPoints;  // List of TGC strip inner station points.
  TrigL2MuonSA::TgcFit::PointArray m_tgcWireInnPoints;   // List of TGC wire inner station points.

  double m_rWidth_TGC_Failed {0};
  
  ServiceHandle<IRegSelSvc> m_regionSelector;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_TGCROADDEFINER_H
