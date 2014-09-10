/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRACKTRACKSUMMARYFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKTRACKSUMMARYFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>

namespace Trk{
class TrackSummary;
}

namespace D3PD {

class TrackTrackSummaryFillerTool
  : public D3PD::BlockFillerTool<Trk::TrackSummary>
{
public:
  typedef D3PD::BlockFillerTool<Trk::TrackSummary> Base;

  TrackTrackSummaryFillerTool (const std::string& type,
			       const std::string& name,
			       const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const Trk::TrackSummary& p);

private:

  void clearData();

  // flag to store full info
  bool m_storeFullSummary;

  // track summary flags
  bool m_doIDHits;
  bool m_doIDHoles;
  bool m_doIDSharedHits;
  bool m_doIDOutliers;
  bool m_doPixelInfoPlus;
  bool m_doSCTInfoPlus;
  bool m_doTRTInfoPlus;
  bool m_doInfoPlus;
  bool m_doMuonHits;
  bool m_doMuonHoles;
  bool m_doExpectBLayer;
  // extra info flags
  bool m_doHitSum;
  bool m_doHoleSum;
  bool m_doHitPattern;
  bool m_doSiHits;
  bool m_doTRTRatio;
  bool m_doPixeldEdx;
  bool m_doElectronPID;

  unsigned int m_typeMask;

  /* NTuple variables: **/
  // hits on track information
  int   *m_nHits;
  int   *m_nHoles;
  int   *m_hitPattern;
  int   *m_nSiHits;
  float *m_TRTHighTHitsRatio;
  float *m_TRTHighTOutliersRatio;
  float *m_pixeldEdx;
  int   *m_nGoodHitsPixeldEdx;
  float *m_massPixeldEdx;
  std::vector<float> *m_likelihoodsPixeldEdx;
  float *m_eProbabilityComb;
  float *m_eProbabilityHT;
  float *m_eProbabilityToT;
  float *m_eProbabilityBrem;

  std::vector<int*> m_vars;

}; // class TrackSummaryFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRACKTRACKSUMMARYFILLERTOOL_H
