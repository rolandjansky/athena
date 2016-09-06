/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"


#include <vector>

/* Forward declarations */
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

namespace Trk
{
  class IVertexFitter;
  class IExtrapolator;
  class ITrackSelectorTool;
}

namespace InDet{
  class ConversionFinderUtils;
  class VertexPointEstimator;
  class ConversionPostSelector;
  class SingleTrackConversionTool;
  class TrackPairsSelector;
  
  /**
     @class InDetConversionFinderTools
     
     InDet::InDetConversionFinderTools is a tool which reconstructs conversion
     vertex candidates in the form of xAOD::Vertex using Trk::Track (no longer available) or
     Trk::TrackParticleBase (default) as an input
  */
  
  class InDetConversionFinderTools : public AthAlgTool, virtual public IVertexFinder
  {	
  public: 
    InDetConversionFinderTools(const std::string& t, const std::string& n, const IInterface* p);
    ~InDetConversionFinderTools();
    
    StatusCode initialize();
    StatusCode finalize();
    
    std::string                       m_TrkParticleCollection ;  //!< Name of input track particle collection
    bool                              m_removeTrt             ;  //!< Remove standalone TRT tracks
    bool                              m_isConversion          ;  //!< Conversions or V0s
    bool                              m_decorateVertices      ;  //!< Decorate vertices with values used for vertex selection
    
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const TrackCollection* trk_coll);    //!< Conversion candidate reconstruction for Trk::Tracks.
  
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const Trk::TrackParticleBaseCollection* trk_coll);
  
    /** Conversion candidate reconstruction for Trk::TrackParticle (default)  */
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trk_coll); 

    
  protected:
    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;                  /** Vertex fitter interface.   */
    ToolHandle <InDet::TrackPairsSelector> m_trackPairsSelector;        /** Track pair selection tool. */
    ToolHandle <InDet::ConversionFinderUtils> m_helpertool;             /** Conversion helper tool. */
    ToolHandle <InDet::VertexPointEstimator> m_vertexEstimator;         /** Initial conversion vertex estimator tool. */
    ToolHandle <InDet::ConversionPostSelector> m_postSelector;          /** Conversion post-fit selector tool. */
    ToolHandle <InDet::SingleTrackConversionTool> m_singleTrkConvTool;  /** Single track conversion tool. */		
    ToolHandle<Trk::IExtrapolator> m_extrapolator;  /** Track extrapolator tool. */
    ToolHandle<Trk::ITrackSelectorTool> m_trkSelector; /** Track Selector Tool. */
    
    
    bool passPreSelection(const xAOD::TrackParticle* track_pos, const xAOD::TrackParticle* track_neg, std::vector<Amg::Vector3D>&  trackList, Amg::Vector3D& initPos, int& flag);

    
    static double s_innerDetectorR;
    
    /** Cuts.  */
    double m_mindR; //!< Minimum allwoed radial distance beteeen guess vertex and closest 1st hit of participating track.
    double m_maxdR; //!< Maximum allowed radial distance beteeen guess vertex and closest 1st hit of participating track.
    double m_MinInitVtxR; //!< Minimum allowed radial position for initial guess vertex. Used only in V0 reconstruction.
    double m_MinFlightAngle; //!< Minimum allowed angular difference between V0 and children direction. Used only in V0 reconstruction.
  };
  
}

#endif // INDETCONVERSIONFINDERTOOLS_INDETCONVERSIONFINDERTOOL_H
