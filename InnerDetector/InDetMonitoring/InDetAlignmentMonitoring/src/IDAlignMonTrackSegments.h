/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDAlignMonTrackSegments_H
#define IDAlignMonTrackSegments_H

// **********************************************************************
// IDAlignMonTrackSegments.cxx
// AUTHORS: John Alison (john.alison@cern.ch) johnda
//          Ben Cooper
//          Tobias Golling
// **********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/TrackCollection.h"

class TH1F_LW;
class TH2F_LW;

class AtlasDetectorID;
class PixelID;

namespace InDetAlignMon {
  class TrackSelectionTool;
}

namespace Trk {
//  class ITrackSummaryTool;
  class Track;
  class TrackStateOnSurface;
}

namespace InDet {
  class IInDetTrackSplitterTool;
}

class IDAlignMonTrackSegments: public ManagedMonitorToolBase
{
public:
  IDAlignMonTrackSegments(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~IDAlignMonTrackSegments();

  virtual StatusCode initialize();

  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  //virtual StatusCode procHistograms();
  void RegisterHisto(MonGroup& mon, TH1F_LW* histo);
  void RegisterHisto(MonGroup& mon, TH2F_LW* histo);
protected:
private:
  StatusCode setupTools();
  static TH1F_LW* MakeHist(const std::string& name, const std::string& title, int nBins, float xLow, float xUp,
                           const std::string& xAxisTitle = "", const std::string& yAxisTitle = "");
  static TH2F_LW* MakeHist(const std::string& name, const std::string& title, int nXBins, float xLow, float xUp,
                           int nYBins, float yLow, float yUp, const std::string& xAxisTitle = "",
                           const std::string& yAxisTitle = "");
  void InitializeHistograms();
  std::string m_stream;
  SG::ReadHandleKey<TrackCollection> m_inputTracksName {
    this, "InputTracksName", "InDetCosmic_Tracks"
  };
  SG::ReadHandleKey<TrackCollection> m_upperTracksName {
    this, "UpperTracksName", "InDetCosmic_Tracks_Upper"
  };
  SG::ReadHandleKey<TrackCollection> m_lowerTracksName {
    this, "LowerTracksName", "InDetCosmic_Tracks_Lower"
  };

  float m_matchedRcut;
  int m_events;
  int m_histosBooked;

  //tools
  ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool;
  ToolHandle<InDet::IInDetTrackSplitterTool> m_trackSplitter;
  ToolHandle<InDetAlignMon::TrackSelectionTool> m_trackSelectionUpper;
  ToolHandle<InDetAlignMon::TrackSelectionTool> m_trackSelectionLower;

  bool m_useCTBSplitTracks;
  float m_deltaD0Range;
  float m_deltaD0Range2D;
  float m_deltaPhiRange;
  float m_deltaPhiRange2D;
  float m_deltaQoverPtRange;
  float m_deltaQoverPtRange2D;
  float m_deltaPtQoverPtRange;
  float m_deltaPtQoverPtRange2D;
  float m_deltaZ0Range;
  float m_d0Range;
  float m_upperPhi;
  std::string m_triggerChainName;

  // Histograms
  //=============

  // Upper And Lower Histograms
  struct UpperLowerHistograms;
  UpperLowerHistograms* m_upper_hist;
  UpperLowerHistograms* m_lower_hist;

  // Difference in Track Parameters
  struct DeltaTrackParamHistograms;
  DeltaTrackParamHistograms* m_delta_d0;
  DeltaTrackParamHistograms* m_delta_z0;
  DeltaTrackParamHistograms* m_delta_phi0;
  DeltaTrackParamHistograms* m_delta_eta0;
  DeltaTrackParamHistograms* m_delta_qOverPt;
  DeltaTrackParamHistograms* m_delta_PtqOverPt;
  DeltaTrackParamHistograms* m_delta_nHits;
  DeltaTrackParamHistograms* m_delta_charge;

  // Debuging
  TH1F_LW* m_debug_phi0 {};
  TH1F_LW* m_debug_eta0 {};

  // pt
  TH1F_LW* m_delta_pt {};
  TH1F_LW* m_reldelta_pt {};
};

#endif
