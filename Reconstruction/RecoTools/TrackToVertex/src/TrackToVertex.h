/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertex.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////


#ifndef RECOTOOLS_TRACKTOVERTEX_H
#define RECOTOOLS_TRACKTOVERTEX_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrkExInterfaces/IExtrapolator.h"

namespace Rec {
  class TrackParticle;
}

namespace Trk {
  class Track;
  class StraightLineSurface;
}

  /** @class TrackToVertex
    
     Standard Tool to extrapolate Track/TrackParticleBase to Vertex
     and BeamSpot/BeamLine.    
    
    @author Andreas.Salzburger@cern.ch
    
    */
    
namespace Reco {
                        
  class TrackToVertex : public extends <AthAlgTool, ITrackToVertex>
  {
    public:
           
      /** AlgTool like constructor */
      TrackToVertex(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~TrackToVertex() = default;

      /** AlgTool initailize method.*/
      virtual StatusCode initialize() override final;
      /** AlgTool finalize method */
      virtual StatusCode finalize() override final;

      /** Use this for MT Coding */
      virtual std::unique_ptr<Trk::StraightLineSurface> GetBeamLine(
        const InDet::BeamSpotData*)
        const override final; // In C++17 make this [[nodiscard]]

      virtual const InDet::BeamSpotData* GetBeamSpotData(
        const EventContext& ctx)
        const override final; // In C++17 make this [[nodiscard]]

      /** Interface method for use with TrackParticle and given vertex position
       * - AOD */
      virtual const Trk::Perigee* perigeeAtVertex(
        const Rec::TrackParticle& tp,
        const Amg::Vector3D& gp) const override final;

      /** Interface method for use with xAOD::Trackparticle and given vertex
       * position - xAOD */
      virtual const Trk::Perigee* perigeeAtVertex(
        const xAOD::TrackParticle& tp,
        const Amg::Vector3D& gp) const override final;

      /** Interface method for use with TrackParticle and default primary vertex
       * from TrackParticle  - AOD */
      virtual const Trk::Perigee* perigeeAtVertex(
        const Rec::TrackParticle& tp) const override final;

      /** Interface method for use with TrackParticle and default primary vertex
       * from TrackParticle  - xAOD */
      virtual const Trk::Perigee* perigeeAtVertex(
        const xAOD::TrackParticle& tp) const override final;

      /** Interface method for use with Track and given vertex position - ESD */
      virtual const Trk::Perigee* perigeeAtVertex(
        const Trk::Track& trk,
        const Amg::Vector3D& gp) const override final;

      /** Interface method for use with TrackParticle and the beamspot from the
       * BeamSpotSvc - AOD*/
      virtual const Trk::Perigee* perigeeAtBeamspot(
        const Rec::TrackParticle& tp,
        const InDet::BeamSpotData*) const override final;

      /** Interface method for use with TrackParticle and the beamspot from the
       * BeamSpotSvc - xAOD*/
      virtual const Trk::Perigee* perigeeAtBeamspot(
        const xAOD::TrackParticle& tp,
        const InDet::BeamSpotData*) const override final;

      /** Interface method for use with Track and the beamspot from the
       * BeamSpotSvc - ESD */
      virtual const Trk::Perigee* perigeeAtBeamspot(
        const Trk::Track& trk,
        const InDet::BeamSpotData*) const override final;

      /** Interface method for use with Track and the beamline from the
       * BeamSpotSvc - ESD */
      virtual const Trk::Perigee* perigeeAtBeamline(
        const EventContext& ctx,
        const Trk::Track& trk,
        const InDet::BeamSpotData*) const override final;

      /** Interface method for use with TrackParticle and the beamline from the
       * BeamSpotSvc - AOD*/
      virtual const Trk::TrackParameters* trackAtBeamline(
        const Rec::TrackParticle& tp) const override final;

      /** Interface method for use with TrackParticle and the beamline from the
       * BeamSpotSvc - xAOD*/
      virtual const Trk::TrackParameters* trackAtBeamline(
        const xAOD::TrackParticle& tp,
        const InDet::BeamSpotData*) const override final;

      /** Interface method for use with Track and the beamline from the
       * BeamSpotSvc - ESD */
      virtual const Trk::TrackParameters* trackAtBeamline(
        const Trk::Track& trk,
        const Trk::StraightLineSurface* beamline) const override final;

      /** Interface method for use with Track and the beamline from the
       * BeamSpotSvc - TrackParameters  */
      virtual const Trk::TrackParameters* trackAtBeamline(
        const Trk::TrackParameters& tpars,
        const Trk::StraightLineSurface* beamline) const override final;

    private:
      ToolHandle<Trk::IExtrapolator>
        m_extrapolator {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"}; //!< ToolHandle for Extrapolator
      
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{
        this,
        "BeamSpotKey",
        "BeamSpotData",
        "SG key for beam spot"
      };
      Gaudi::Property<bool> m_ForceBeamSpotZero {  "ForceBeamSpotZero", false};
      const static Amg::Vector3D s_origin; //!< static origin
  };

} // end of namespace


#endif // RECOTOOLS_TRACKTOVERTEX_H

