/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_TRACKSELECTOR_H
#define JETTAGTOOLS_TRACKSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetRecToolInterfaces/IInDetEtaDependentCutsSvc.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"

#include <string>
#include <bitset>

namespace Reco { class ITrackToVertex; }

namespace Analysis {

  /**
  @class TrackSelector
  This class selects tracks for b-taggers
  @author Laurent Vacavant (vacavant@in2p3.fr)
  */

class TrackSelector : public AthAlgTool {

public:
  TrackSelector (const std::string& type,
            const std::string& name, const IInterface* parent);
  virtual ~TrackSelector();

  static const InterfaceID& interfaceID();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  //  inline void setSumTrkPt(double sum) { m_SumTrkPt = sum; }

  /** Returns true if the argument track fulfills the selection
   */
  bool selectTrack(const Amg::Vector3D& pv,
                   const xAOD::TrackParticle* track,
                   double refPt = 0) const;

  /// Also return individual cuts.
  bool selectTrack(const Amg::Vector3D& pv,
                   const xAOD::TrackParticle* track,
                   std::bitset<17>& failedCuts,
                   double refPt = 0) const;

  inline const std::bitset<17> currentTrackpassedCuts() const { return m_passedCuts; }

private:
  /** Properties for V0 finding:
      not yet implemented */

  /** Properties for track selection: 
      all cuts are ANDed */
  bool m_usePerigeeParameters; /// if true use perigee parameters instead of parameters w.r.t. primary vertex
  double m_pTMin;     /// min. pT: |pT|>pTMin
  bool m_usepTDepTrackSel;
  double m_pTMinOffset;
  double m_pTMinSlope;
  double m_d0Max;     /// max. d0: |d0|<d0Max
  double m_z0Max;     /// max. z0:
  double m_sigd0Max; // max sig d0:
  double m_sigz0Max; // max sig z0:
  double m_etaMax;    /// max. pseudo-rapidity
  bool m_useTrackSummaryInfo; /// if false the following cuts are ignored
  bool m_useBLayerHitPrediction; // use b-layer tool tied to conddb
  int m_nHitBLayer;   /// at least n hits in Blayer
  int m_nHitPix;      /// at least n hits in pixels
  int m_nHitSct;      /// at least n hits in SCT
  int m_nHitSi;       /// at least n hits in pixels+SCT
  int m_nHitTrt;      /// at least n hits in TRT
  int m_nHitTrtHighE; /// at least n transition hits in TRT 
  bool m_useDeadPixInfo;  /// if true uses dead pixel sensors from conddb (except b-layer) to compute nPix
  bool m_useDeadSctInfo;  /// if true uses dead SCT sensors to compute nSct
  bool m_useTrackQualityInfo; /// if false the following cuts are ignored
  double m_fitChi2;   /// max. fit chi2
  double m_fitProb;   /// min. fit chi2 probability
  double m_fitChi2OnNdfMax; //max. fitchi2/ndf
  std::string m_inputTrackCollection; /// location of inputTracks in StoreGate
  std::string m_outputTrackCollection; /// location of outputTracks in StoreGate
  std::string m_sharedHitMapLocation; /// location of shared map in StoreGate
  bool m_useAntiPileUpCuts; /// activate antiPU cut d0%z0
  double m_antiPileUpSigD0Cut;
  double m_antiPileUpSigZ0Cut;
  int m_antiPileUpNHitSiCut;
  int m_antiPileUpNHolePixCut;
  bool m_useTrackingTightDefinition;
  // for debugging:
  enum m_Cuts { pTMin, d0Max, z0Max, sigd0Max, sigz0Max, etaMax,
                nHitBLayer, deadBLayer, nHitPix, nHitSct, nHitSi, nHitTrt, nHitTrtHighE,
                fitChi2, fitProb,fitChi2OnNdfMax, trackingTightDef,
                numCuts };

  int m_ntri;
  int m_ntrf;
  mutable std::atomic<int> m_ntrc[numCuts];

  ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool{this, "trackToVertexTool", "Reco::TrackToVertex"};

  std::bitset<numCuts> m_passedCuts;

  bool m_useEtaDependentCuts;
  /** service to get cut values depending on different variable */
  ServiceHandle<InDet::IInDetEtaDependentCutsSvc> m_etaDependentCutsSvc{this, "InDetEtaDependentCutsSvc", ""};

};


}
#endif // JETTAGTOOLS_TRACKSELECTOR_H
