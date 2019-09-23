// -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"
#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/IUpdator.h"
#include <TNtuple.h>
#include <TFile.h>
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"

class AtlasDetectorID;
class TRT_ID;

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
  ToolHandle<ITRT_CalDbTool> m_trtcaldbTool ;
  ServiceHandle<ITRT_StrawNeighbourSvc> m_neighbourSvc ;
  ToolHandle<ITRT_StrawStatusSummaryTool> m_TRTStrawSummaryTool; //!< The ConditionsSummaryTool

  Trk::IUpdator* m_updator; //!< updator for unbiased states

  float m_maxDistance ;
  float m_maxTimeResidual ;
  int m_minTimebinsOverThreshold ;
  float m_maxTrackChisquarePerDof ;
  unsigned int m_numOfHitsTotal ;
  unsigned int m_numOfHitsAccepted ;
  unsigned int m_numOfProcessedTracks ;
  float m_DoMCCosmicTimeShift;

  TFile* m_f;
  TNtuple* m_ntuple;
};

#endif //  TRT_CALIBTOOLS__FILLALIGNTRTHITS_H
