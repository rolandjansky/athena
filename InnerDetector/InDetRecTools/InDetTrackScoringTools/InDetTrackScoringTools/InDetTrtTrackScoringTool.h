/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRTTRACKSCORINGTOOL_H
#define INDETTRTTRACKSCORINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
// Access to mu
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "TrkParameters/TrackParameters.h"
#include <string>
#include <vector>

namespace Trk {
class Track;
class TrackSummary;
}

class TRT_ID;

namespace InDet {

class ITrtDriftCircleCutTool;

/**Concrete implementation of the ITrackScoringTool pABC*/
class InDetTrtTrackScoringTool
  : virtual public Trk::ITrackScoringTool
  , public AthAlgTool
{

public:
  InDetTrtTrackScoringTool(const std::string&,
                           const std::string&,
                           const IInterface*);
  virtual ~InDetTrtTrackScoringTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  /** create a score based on how good the passed track is*/
  virtual Trk::TrackScore score(const Trk::Track& track,
                                const bool suppressHoleSearch) const override;

  /** create a score based on how good the passed TrackSummary is*/
  virtual Trk::TrackScore simpleScore(
    const Trk::Track& track,
    const Trk::TrackSummary& trackSum) const override;

  Trk::TrackScore TRT_ambigScore(const Trk::Track& track,
                                 const Trk::TrackSummary& trackSum) const;

private:
  void setupTRT_ScoreModifiers();

  /** Decide whether standalone TRT tracks pass the minimum hit requirement. */
  bool isGoodTRT(const Trk::Track&) const;

  // Get Eta bin for eta-dependent TRT-track cuts
  unsigned int getEtaBin(const Trk::Perigee& perigee) const;

  // Get eta- and mu-dependent nTRT cut for TRT track
  double getMuDependentNtrtMinCut(unsigned int eta_bin) const;

  /**ID TRT helper*/
  const TRT_ID* m_trtId;

  // these are used for ScoreModifiers
  int m_maxSigmaChi2, m_maxTrtRatio, m_maxTrtFittedRatio;

  std::vector<double> m_factorSigmaChi2, m_factorTrtRatio,
    m_factorTrtFittedRatio;

  std::vector<double> m_boundsSigmaChi2, m_boundsTrtRatio,
    m_boundsTrtFittedRatio;

  /** use the scoring tuned to Ambiguity processing or not */
  bool m_useAmbigFcn;
  bool m_useSigmaChi2;

  /**holds the scores assigned to each Trk::SummaryType from the track's
   * Trk::TrackSummary*/
  std::vector<Trk::TrackScore> m_summaryTypeScore;

  /** Returns minimum number of expected TRT drift circles depending on eta. */
  ToolHandle<ITrtDriftCircleCutTool> m_selectortool;

  // Read handle for conditions object to get the field cache
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{
    this,
    "AtlasFieldCacheCondObj",
    "fieldCondObj",
    "Name of the Magnetic Field conditions object key"
  };

  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool{
    this,
    "LuminosityTool",
    "LumiBlockMuTool/LumiBlockMuTool",
    "Luminosity Tool"
  };

  /** cuts for selecting good tracks*/
  int m_minTRTonTrk; //!< minimum number of TRT hits
  double m_maxEta;   //!< maximal Eta cut
  double m_ptmin;    //!< Minimum Pt
  bool
    m_parameterization; //!< use parameterization to cut instead of custom cut
  bool m_oldLogic;      //!< use old transition hit logic
  double m_minTRTprecision; //!< minimum fraction of TRT precision hits

  std::vector<double>
    m_TRTTrksEtaBins; //!< Eta bins (10 expected) for TRT-only track cuts
  std::vector<double>
    m_TRTTrksMinTRTHitsThresholds; //!< Eta-binned nTRT cut for TRT-only cuts
  std::vector<double>
    m_TRTTrksMinTRTHitsMuDependencies; //!< Eta-bined Mu-dependent component for
                                       //!< nTRT cut
};

}
#endif
