// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__FILLALIGNTRTHITS_H
#define TRT_CALIBTOOLS__FILLALIGNTRTHITS_H
/********************************************************************

NAME:     FillAlignTRTHits
PACKAGE:  TRT_AlignTOOLS

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Tool to fill Hit info into the TRT::Hit messenger class

********************************************************************/

// INCLUDES:

#include <string>
#include "TRT_CalibTools/IFillAlignTrkInfo.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/IUpdator.h"
#include <TNtuple.h>
#include <TFile.h>
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"

class ITRT_StrawSummarySvc;
class AtlasDetectorID;
class TRT_ID;
class ITRT_CalDbSvc;

namespace Trk {
  class IPropagator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;
  class ITruthToTrack;
  class IResidualPullCalculator;
 }

class FillAlignTRTHits :  virtual public IFillAlignTrkInfo, public AthAlgTool {
public:

  FillAlignTRTHits(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~FillAlignTRTHits() {}

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output) const;
  virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output,
                    const ComTime* theComTime, const xAOD::EventInfo& eventInfo,
                    const xAOD::VertexContainer& vertices);

private:
  const AtlasDetectorID* m_DetID;
  const TRT_ID* m_TRTID;

  ToolHandle< ITRT_DriftFunctionTool > m_driftFunctionTool{this, "TRTDriftFunctionTool", "TRT_DriftFunctionTool", "Drift function tool name"};
  ToolHandle<Trk::IUpdator> m_updatorHandle{this, "UpdatorTool", "Trk::KalmanUpdator/TrkKalmanUpdator", "Measurement updator to calculate unbiased track states"};
 // ToolHandle<ITRTCalDbTool> m_trtcaldbtool ;
 // ToolHandle<ITRTStrawNeighbourTool> m_neighbourtool;
  ServiceHandle<ITRT_CalDbSvc> m_trtcaldbSvc ;
  ServiceHandle<ITRT_StrawNeighbourSvc> m_neighbourSvc ;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; //!< The ConditionsSummaryTool

  Trk::IUpdator* m_updator; //!< updator for unbiased states

  float m_maxDistance ;
  float m_maxTimeResidual ;
  int m_minTimebinsOverThreshold ;
  float m_maxTrackChisquarePerDof ;
  unsigned int m_numOfHitsTotal ;
  unsigned int m_numOfHitsAccepted ;
  unsigned int m_numOfProcessedTracks ;
  float m_DoMCCosmicTimeShift;
//  StoreGateSvc     * m_store_gate;

  TFile* m_f;
  TNtuple* m_ntuple;
};

#endif //  TRT_CALIBTOOLS__FILLALIGNTRTHITS_H
