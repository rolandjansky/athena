/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetailedTrackSelectorTool_InDetDetailedTrackSelectorTool_H
#define InDetDetailedTrackSelectorTool_InDetDetailedTrackSelectorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexFwd.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

/**
 * @file InDetDetailedTrackSelectorTool.h
 * @class InDetDetailedTrackSelectorTool
 *
 * @brief TrackSelector for general use
 *
 * The option for anyDirection:
 * Required for cosmic tracks:  mixture of tracks with different reo logic.
 * Default is the extrapolation oppositeToMomentum, which is required for collision tracks.
 *
 * @author Giacinto Piacquadio <giacinto.piacquadio@physik.uni-freiburg.de>
 * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * @author Daniel Kollar <daniel.kollar@cern.ch>
 */



namespace Trk
{
  class ITrackSummaryTool;
  class IExtrapolator;
  class Vertex;
  class RecVertex;
  class FitQuality;
  class TrackSummary;
  class Track;
  class TrackParticleBase;
}

namespace InDet
{
  class ITrtDriftCircleCutTool;
  class IInDetTestBLayerTool;


  class InDetDetailedTrackSelectorTool : virtual public Trk::ITrackSelectorTool, public AthAlgTool
  {

    public:

      enum Grade { Undefined, Good, Shared, nbGrades };

      StatusCode initialize();

      StatusCode finalize();

      InDetDetailedTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p);

      ~InDetDetailedTrackSelectorTool();

      bool decision(const Trk::Track& track,const Trk::Vertex* vertex) const;

      bool decision(const Trk::TrackParticleBase& track,const Trk::Vertex* vertex) const;

      bool decision(const xAOD::TrackParticle& track,const xAOD::Vertex* vertex) const;

    private:
      int getCount( const xAOD::TrackParticle& tp, xAOD::SummaryType type ) const {
	uint8_t val;
	if( !tp.summaryValue(val,type) ) return 0;
	return val > 0 ? val : 0;
      }
      bool decision(const Trk::Perigee* track,const AmgSymMatrix(3)* covariancePosition) const;
      bool decision(const Trk::FitQuality*  TrkQuality) const;
      bool decision(double chi2, int ndf ) const;
      bool decision(const Trk::TrackSummary* summary,bool useSharedHitInfo,bool useTrtHitInfo, 
                    const Trk::Perigee* track,
                    const int nHitTrt, const int nHitTrtPlusOutliers) const;

      bool preselectionBeforeExtrapolation(const Trk::Perigee & myPerigee) const;
      Amg::Vector3D getPosOrBeamSpot(const xAOD::Vertex*) const;

      double m_pTMin;       //<! min. pT: |pT|>pTMin
      double m_pMin;        //<! min. p = pT/cos(theta): |p| > pMin
      double m_IPd0Max;     //<! max. d0: |d0|<d0Max
      double m_IPz0Max;     //<! max. z0: |z0*sin(theta)|<z0Max
      double m_z0Max;       //<! max. z0: |z0|<z0Max
      double m_sigIPd0Max; //<! max sig IP d0:
      double m_sigIPz0Max; //<! max sig IP z0 (error only due to z0)*sin(theta)
      double m_d0significanceMax; //<! max IP significance d0 (-1 switches it off)
      double m_z0significanceMax; //<! max IP significance z0 (-1 switches it off)
      double m_etaMax;    //<! max. pseudo-rapidity

      bool m_useTrackSummaryInfo; //<! if false the following cuts are ignored

      int m_nHitBLayer;   //<! at least n hits in Blayer
      int m_nHitPix;      //<! at least n hits in pixels
      int m_nHitBLayerPlusPix; //<! at least n hits in blayer+pixel
      int m_nHitSct;      //<! at least n hits in SCT
      int m_nHitSi;       //<! at least n hits in pixels+SCT

      int m_nHitPixPhysical; //!< at least n physical hits in pixel
      int m_nHitSiPhysical;  //!< at least n physical hits in pixel+SCT

      int m_nHitTrt;      //<! at least n hits in TRT
      int m_nHitTrtPlusOutliers; //<! at least n hits in TRT (including outliers)

      //<! for selecting electrons (soft E-ID)
      int m_nHitTrtHighE; //<! at least n high threshold hits in TRT
      int m_nHitTrtPlusOutliersHighE; //<! at least n high threshold hits in TRT (including outliers)
      //<! for rejecting electrons (tau-ID)
      double m_nHitTrtHighEFraction; //<! maximum x fraction of transition hits in TRT
      double m_nHitTrtHighEFractionWithOutliers; //<! maximum x fraction of transition hits in TRT (including outliers)

      double m_TrtMaxEtaAcceptance;//<! limit of eta regions where trt hits are expected
      bool m_useSharedHitInfo; //<! if false the following cuts are ignored
      int m_nSharedBLayer;//<! max. number of shared hits in B layer
      int m_nSharedPix;   //<! max. number of shared hits in pixels
      int m_nSharedSct;   //<! max. number of shared hits in SCT
      int m_nSharedSi;    //<! max. number of shared hits in pixels+SCT

      int m_nHoles; //<! max. number of holes in pixel+SCT
      int m_nDoubleHoles; //<! max number of double-holes in SCT
      int m_nHolesPix; //<! max. number of holes in pixels
      int m_nHolesSct; //<! max. number of holes in SCT

      bool m_useTrackQualityInfo; //<! if false the following cuts are ignored
      double m_fitChi2;   //<! max. fit chi2
      double m_fitProb;   //<! min. fit chi2 probability
      double m_fitChi2OnNdfMax; //<! max. fitchi2/ndf

      double m_scaleMinHitTrt; //<! scale the eta dependent minimum number of TRT hits; scaling is only applied if m_addToMinHitTrt==0
      int    m_addToMinHitTrt; //<! add to/subtract from eta dependent minimum nimber of TRT hits
      double m_scaleMinHitTrtWithOutliers; //<! scale the eta dependent minimum number of TRT hits + outliers; scaling is only applied if m_addToMinHitTrtWithOutliers==0
      int    m_addToMinHitTrtWithOutliers; //<! add to/subtract from eta dependent minimum nimber of TRT hits + outliers

      bool m_usePreselectionCuts;
      double m_d0MaxPreselection;

      bool m_useEtaDepententMinHitTrt;
      bool m_useEtaDepententMinHitTrtWithOutliers;

      ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool; //!< Track summary tool
      ToolHandle<Trk::IExtrapolator> m_extrapolator; //!< Extrapolator tool
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      ToolHandle<ITrtDriftCircleCutTool> m_trtDCTool; //!< Tool to get eta dependent cut on number of TRT hits

      ToolHandle< InDet::IInDetTestBLayerTool > m_inDetTestBLayerTool; //Tool to test if the track crosses a dead module on the b-layer
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

      bool m_trackSumToolAvailable;
            
// chnages for the pt-dependent sct cut
      bool m_usePtDependentCuts;
      
      std::vector<float> m_ptBenchmarks;
      
      std::vector<int> m_nSCTValues;      

  }; //end of class definitions

} //end of namespace definitions

#endif //TrkMultipleVertexSeedFinders_PVFindingTrackSelectoTool_H
