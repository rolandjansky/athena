/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
        InDetConversionFinderTools.h  -  Description
        -------------------
        begin   : 28-08-2006
        authors : Tatjana Lenz
        email   : tatjana.lenz@cern.ch
        changes :
***************************************************************************/
#ifndef INDETCONVERSIONFINDERTOOLS_INDETCONVERSIONFINDERTOOLS_H
#define INDETCONVERSIONFINDERTOOLS_INDETCONVERSIONFINDERTOOLS_H

#include "InDetRecToolInterfaces/IVertexFinder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

/* Tools*/
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "InDetConversionFinderTools/TrackPairsSelector.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrack/TrackCollection.h"
/*xAOD collections */
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include <vector>

/**
   @class InDetConversionFinderTools

   InDet::InDetConversionFinderTools is a tool which reconstructs conversion
   vertex candidates in the form of xAOD::Vertex using Trk::Track (no longer
   available) or xAOD::TrackParticle  (default) as an input
*/

namespace InDet {

class InDetConversionFinderTools
  : public AthAlgTool
  , virtual public IVertexFinder
{
public:
  InDetConversionFinderTools(const std::string& t,
                             const std::string& n,
                             const IInterface* p);
  ~InDetConversionFinderTools();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  //!< Remove standalone TRT tracks
  bool m_removeTrt;
  //!< Conversions or V0s
  bool m_isConversion;
  //!< Decorate vertices with values used for vertex selection
  bool m_decorateVertices;

  using IVertexFinder::findVertex;
  //!< Conversion candidate reconstruction for Trk::Tracks.
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx,
             const TrackCollection* trk_coll) const override;

  /** Conversion candidate reconstruction for Trk::TrackParticle (default)  */
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx,
             const xAOD::TrackParticleContainer* trk_coll) const override;

protected:
 
  /** Vertex fitter interface.   */
  /** Track pair selection tool. */
  ToolHandle<Trk::IVertexFitter> m_iVertexFitter{ this,
                                                  "VertexFitterTool",
                                                  "Trk::TrkVKalVrtFitter",
                                                  "Vertex fitter Tool" };
  /** Initial conversion vertex estimator tool. */
  ToolHandle<InDet::TrackPairsSelector> m_trackPairsSelector{
    this,
    "TrackPairsSelector",
    "InDet::TrackPairsSelector",
    "Track Pair Selector Tool"
  };
  /** Initial conversion vertex estimator tool. */
  ToolHandle<InDet::VertexPointEstimator> m_vertexEstimator{
    this,
    "VertexPointEstimator",
    "InDet::VertexPointEstimator",
    "Vertex point estimator"
  };
  /** Conversion post-fit selector tool. */
  ToolHandle<InDet::ConversionPostSelector> m_postSelector{
    this,
    "PostSelector",
    "InDet::ConversionPostSelector",
    "Tool for post selection of conversion candidates"
  };
  /** Single track conversion tool. */
  ToolHandle<InDet::SingleTrackConversionTool> m_singleTrkConvTool{
    this,
    "SingleTrackConversionTool",
    "InDet::SingleTrackConversionTool",
    "Tool for single track conversions"
  };

  /** Track extrapolator tool. */
  ToolHandle<Trk::IExtrapolator> m_extrapolator{ this,
                                                 "Extrapolator",
                                                 "",
                                                 "Extrapolation Tool" };
  /** Track Selector Tool. */
  ToolHandle<Trk::ITrackSelectorTool> m_trkSelector{
    this,
    "TrackSelectorTool",
    "InDet::TrackSelectorTool",
    "Tool for track Selection"
  };

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrkParticleCollectionKey{
    this,
    "TrackParticleCollection",
    "",
    "Name of the input track particle container"
  };

  bool passPreSelection(TrackPairsSelector::Cache& cache,
                        const xAOD::TrackParticle* track_pos,
                        const xAOD::TrackParticle* track_neg,
                        std::vector<Amg::Vector3D>& trackList,
                        Amg::Vector3D& initPos,
                        int& flag,
                        std::map<std::string, float>& intersectionDecors) const;

  /** Cuts.  */
  //!< Minimum allwoed radial distance beteeen guess vertex and closest 1st hit
  //!< of participating track.
  double m_mindR;
  //!< Maximum allowed radial distance beteeen guess vertex and closest 1st hit
  //!< of participating track.
  double m_maxdR;
  //!< Minimum allowed radial position for initial guess vertex. Used only in V0
  //!< reconstruction.
  double m_MinInitVtxR;
  //!< Minimum allowed angular difference between V0 and children direction.
  //!< Used only in V0 reconstruction.
  double m_MinFlightAngle;
};

}

#endif // INDETCONVERSIONFINDERTOOLS_INDETCONVERSIONFINDERTOOL_H
