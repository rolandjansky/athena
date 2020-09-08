/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerSegmentMatchingTool.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Muon {

  MuonLayerSegmentMatchingTool::MuonLayerSegmentMatchingTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent) {
    declareInterface<IMuonLayerSegmentMatchingTool>(this);
  }

  StatusCode MuonLayerSegmentMatchingTool::initialize() {
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_matchingTool.retrieve());
    return StatusCode::SUCCESS;
  }

  bool MuonLayerSegmentMatchingTool::match( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment ) const {

    if( msgLvl(MSG::VERBOSE) ){
      const Trk::TrackParameters* pars = intersection.trackParameters.get();
      msg(MSG::VERBOSE) << " startPars: phi "  << pars->position().phi() 
                        << " r " << pars->position().perp() << " z " << pars->position().z() 
                        << " local " << pars->parameters()[Trk::locX] << " " << pars->parameters()[Trk::locY];
      if( pars->covariance() ) msg(MSG::VERBOSE) << " err " << Amg::error(*pars->covariance(),Trk::locX) << " " << Amg::error(*pars->covariance(),Trk::locY);
      msg(MSG::VERBOSE) << endmsg;
    }
    
    std::unique_ptr<const Trk::TrackParameters> exPars(m_extrapolator->extrapolate(*intersection.trackParameters,segment.associatedSurface(),Trk::anyDirection,false,Trk::muon));
    if( !exPars ){
      ATH_MSG_VERBOSE(" extrapolation failed ");
      return false;
    }

    const Trk::AtaPlane* ataPlane = dynamic_cast<const Trk::AtaPlane*>(exPars.get());
    if(!ataPlane){
      ATH_MSG_WARNING(" dynamic_cast<> failed ");
      return false;
    }
    if( msgLvl(MSG::VERBOSE) ){
      msg(MSG::VERBOSE) << " Segment r " << segment.globalPosition().perp() << " z " << segment.globalPosition().z()
                        << " local " << segment.localParameters()[Trk::locX] << " " << segment.localParameters()[Trk::locY] 
                        << " err " << Amg::error(segment.localCovariance(),Trk::locX) << " " << Amg::error(segment.localCovariance(),Trk::locY)
                        << " extrapolation  r " << ataPlane->position().perp() << " z " << ataPlane->position().z() 
                        << " local " << ataPlane->parameters()[Trk::locX] << " " << ataPlane->parameters()[Trk::locY];
      if( ataPlane->covariance() ) msg(MSG::VERBOSE) << " err " << Amg::error(*ataPlane->covariance(),Trk::locX) << " " << Amg::error(*ataPlane->covariance(),Trk::locY);
      msg(MSG::VERBOSE) << endmsg;
    }

    MuonCombined::MuonSegmentInfo segmentInfo = m_matchingTool->muTagSegmentInfo( nullptr, &segment, ataPlane );
    if( !m_matchingTool->matchSegmentPosition(&segmentInfo,true) ){
      ATH_MSG_DEBUG(" position match failed ");
      return false;
    }
    if( !m_matchingTool->matchSegmentDirection(&segmentInfo,true) ){
      ATH_MSG_DEBUG(" direction match failed ");
      return false;
    }
    if( !m_matchingTool->matchDistance(&segmentInfo) ){
      ATH_MSG_DEBUG(" distance match failed ");
      return false;
    }
    if( !m_matchingTool->matchCombinedPull(&segmentInfo) ){
      ATH_MSG_DEBUG(" combined pull match failed ");
      return false;
    }
    return true;
  }

  void MuonLayerSegmentMatchingTool::select( const MuonSystemExtension::Intersection& intersection, const std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments, 
                                             std::vector< std::shared_ptr<const Muon::MuonSegment> >& selectedSegments ) const {
    
    // loop over segments and match them to the intersection
    for( auto segment : segments ){
      
      if( match(intersection,*segment) ){
        selectedSegments.push_back(segment);
      }
    }
    ATH_MSG_DEBUG("Selected segments: " << selectedSegments.size() );
  }

}
 
