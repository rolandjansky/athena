/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertex.cxx, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

// Reco & Rec
#include "TrackToVertex/TrackToVertex.h"
#include "Particle/TrackParticle.h"
// GaudiKernel
#include "GaudiKernel/IIncidentSvc.h"
// Trk
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"


const Amg::Vector3D Reco::TrackToVertex::s_origin(0.,0.,0.);

// constructor
Reco::TrackToVertex::TrackToVertex(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator", this)
{
   declareInterface<ITrackToVertex>(this);
   // the extrapolatr and the BeamCondSvc 
   declareProperty("Extrapolator", m_extrapolator);
   declareProperty("ForceBeamSpotZero", m_ForceBeamSpotZero);
}

// destructor
Reco::TrackToVertex::~TrackToVertex()
{
}



// Athena standard methods

// initialize
StatusCode Reco::TrackToVertex::initialize()
{
    // Get the GeometryBuilder AlgTool
    if ( m_extrapolator.retrieve().isFailure() ) {
      ATH_MSG_WARNING( "Failed to retrieve tool " << m_extrapolator << " - extrapolations will not be performed." );
    } else 
      ATH_MSG_INFO( "Retrieved tool " << m_extrapolator);
    if(!m_ForceBeamSpotZero){
      ATH_CHECK(m_beamSpotKey.initialize());
    }else{
//      m_beamSpotKey = "";
    }
    ATH_MSG_DEBUG( name() << " initialize() successful");
    return StatusCode::SUCCESS;
}

const InDet::BeamSpotData* Reco::TrackToVertex::GetBeamSpotData(const EventContext &ctx) const {
   SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
   return beamSpotHandle.isValid() ? beamSpotHandle.retrieve() : nullptr;
}

// incident listener waiting for BeginEvent
std::unique_ptr<Trk::StraightLineSurface> Reco::TrackToVertex::GetBeamLine(const InDet::BeamSpotData* beamSpotHandle) const {
    // get the transform
    Amg::Transform3D* beamTransform = new Amg::Transform3D(Amg::AngleAxis3D(beamSpotHandle->beamTilt(0),Amg::Vector3D(0.,1.,0.)));
    (*beamTransform) *= Amg::AngleAxis3D(beamSpotHandle->beamTilt(1),Amg::Vector3D(1.,0.,0.));
    beamTransform->pretranslate(beamSpotHandle->beamPos());
    // create the new beam line
    return std::make_unique< Trk::StraightLineSurface >(beamTransform);
}

// finalize
StatusCode Reco::TrackToVertex::finalize()
{
    ATH_MSG_DEBUG( name() << " finalize() successful");
    return StatusCode::SUCCESS;
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtVertex(const Rec::TrackParticle& tp) const {
  
  // retrieve the reconstructed Vertex from the TrackParticle  
  const Trk::VxCandidate* vxCandidate = tp.reconstructedVertex();
  if (vxCandidate!=0) {
     // create a global position from this
     const Trk::RecVertex& reconVertex = vxCandidate->recVertex();
     const Amg::Vector3D& vertexPosition = reconVertex.position();
     Amg::Vector3D persfPosition(vertexPosition.x(), vertexPosition.y(), vertexPosition.z());
     return(this->perigeeAtVertex(tp, persfPosition));
  }
  ATH_MSG_DEBUG("No reconstructed vertex found in TrackParticle, perigee will be expressed to (0.,0.,0.).");
  return (perigeeAtVertex(tp,Trk::s_origin));
}

const Trk::Perigee* Reco::TrackToVertex::perigeeAtVertex(const xAOD::TrackParticle& tp) const {
    return perigeeAtVertex(tp, Amg::Vector3D(tp.vx(),tp.vy(),tp.vz()));
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtVertex(const xAOD::TrackParticle& tp, const Amg::Vector3D& gp) const {

  // preparation
  Trk::PerigeeSurface persf(gp); 
  const Trk::Perigee* vertexPerigee = 0;
  // retrieve the Perigee from the track particle
  const Trk::Perigee& trackparPerigee = tp.perigeeParameters();
  if ( trackparPerigee.associatedSurface() == persf) {
       ATH_MSG_DEBUG("Perigee of TrackParticle is already expressed to given vertex, a copy is returned.");
       return(trackparPerigee.clone());
  } else 
      vertexPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(trackparPerigee, persf));
  if (!vertexPerigee)
     ATH_MSG_DEBUG("Extrapolation to Perigee failed, a NULL pointer is returned.");
  return vertexPerigee;    
}

const Trk::Perigee* Reco::TrackToVertex::perigeeAtVertex(const Rec::TrackParticle& tp, const Amg::Vector3D& gp) const {

  // preparation
  Trk::PerigeeSurface persf(gp); 
  const Trk::Perigee* vertexPerigee = 0;
  // retrieve the Perigee from the track particle
  const Trk::Perigee* trackparPerigee = tp.measuredPerigee();
  if (trackparPerigee){
     if ( trackparPerigee->associatedSurface() == persf)
     {
       ATH_MSG_DEBUG("Perigee of TrackParticle is already expressed to given vertex, a copy is returned.");
       return(trackparPerigee->clone());
     } else 
      vertexPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(*trackparPerigee, persf));
  } else {
      ATH_MSG_DEBUG("No Perigee found in  TrackParticle, a NULL pointer is returned.");
      return 0;      
  }
  if (!vertexPerigee)
     ATH_MSG_DEBUG("Extrapolation to Perigee failed, a NULL pointer is returned.");
  return vertexPerigee;    
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtVertex(const Trk::Track& track, const Amg::Vector3D& gp) const {

  Trk::PerigeeSurface persf(gp); 
  const Trk::Perigee* vertexPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(track, persf));
    if (!vertexPerigee){
        const Trk::Perigee* trackPerigee = track.perigeeParameters();
        if ( trackPerigee && trackPerigee->associatedSurface() == persf )
        {
           ATH_MSG_DEBUG("Perigee of Track is already expressed to given vertex, a copy is returned.");
           vertexPerigee = trackPerigee->clone();
        } else
           ATH_MSG_DEBUG("Extrapolation to Perigee failed, NULL pointer is returned.");
     }
  return (vertexPerigee);
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtBeamspot(const Rec::TrackParticle& tp, const InDet::BeamSpotData* beamspot) const
{  
  return perigeeAtVertex(tp, beamspot ? beamspot->beamVtx().position() : s_origin);
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtBeamspot(const xAOD::TrackParticle& tp, const InDet::BeamSpotData* beamspot) const
{  
  return perigeeAtVertex(tp, beamspot ? beamspot->beamVtx().position() : s_origin);
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtBeamspot(const Trk::Track& track, const InDet::BeamSpotData* beamspot) const
{
  return perigeeAtVertex(track, beamspot ? beamspot->beamVtx().position() : s_origin);
}


const Trk::Perigee* Reco::TrackToVertex::perigeeAtBeamline(const Trk::Track& track, const InDet::BeamSpotData* beamspotptr) const
{
  
  Amg::Vector3D beamspot(s_origin); 
  float tiltx = 0.0;
  float tilty = 0.0;
  if (beamspotptr) {
    beamspot = Amg::Vector3D(beamspotptr->beamVtx().position());
    tiltx =  beamspotptr->beamTilt(0);
    tilty =  beamspotptr->beamTilt(1);
  }
  Amg::Transform3D * amgTransf = new Amg::Transform3D();
  Amg::Translation3D amgtranslation(beamspot);
  *amgTransf = amgtranslation * Amg::RotationMatrix3D::Identity();
  *amgTransf *= Amg::AngleAxis3D(tilty, Amg::Vector3D(0.,1.,0.));
  *amgTransf *= Amg::AngleAxis3D(tiltx, Amg::Vector3D(1.,0.,0.));
  // preparation
  Trk::PerigeeSurface persf(amgTransf); 
  
  const Trk::Perigee* vertexPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(track, persf));
  if (!vertexPerigee) {
    // workaround.
    // try again using the first track parameter set, since the current extrapolator will
    // use "the closest" track parameterset which is not necessarily the mostuseful one to
    // start the extrapolation with.
    // @TODO should try to improve the extrapolator to pick the correct start parameters.
    const DataVector<const Trk::TrackParameters> *track_parameter_list= track.trackParameters();
    if (track_parameter_list) {
      for(const Trk::TrackParameters *trk_params: *track_parameter_list) {
        if (!trk_params) continue;
        vertexPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*trk_params, persf));
        break;
      }
    }
  }
  if (!vertexPerigee){
    const Trk::Perigee* trackPerigee = track.perigeeParameters();
    if ( trackPerigee && trackPerigee->associatedSurface() == persf )
      {
	ATH_MSG_DEBUG("Perigee of Track is already expressed to given vertex, a copy is returned.");
	vertexPerigee = trackPerigee->clone();
      } else{
      ATH_MSG_DEBUG("Extrapolation to Beamline Perigee failed, NULL pointer is returned.");
    }
    }
  return (vertexPerigee); 
    
}

const Trk::TrackParameters* Reco::TrackToVertex::trackAtBeamline(const Rec::TrackParticle& /*tp*/) const
{
 ATH_MSG_WARNING(" Method not implemented!! ");
  return 0;
  //return m_extrapolator->extrapolate(tp, *m_beamLine);
}
 
const Trk::TrackParameters* Reco::TrackToVertex::trackAtBeamline(const xAOD::TrackParticle& tp,
                const InDet::BeamSpotData* beamspotptr) const
{
 
  Amg::Vector3D beamspot(s_origin); 
  float tiltx = 0.0;
  float tilty = 0.0;
  if (beamspotptr) {
    beamspot = Amg::Vector3D(beamspotptr->beamVtx().position());
    tiltx =  beamspotptr->beamTilt(0);
    tilty =  beamspotptr->beamTilt(1);
  }
  Amg::Transform3D * amgTransf = new Amg::Transform3D();
  Amg::Translation3D amgtranslation(beamspot);
  *amgTransf = amgtranslation * Amg::RotationMatrix3D::Identity();
  *amgTransf *= Amg::AngleAxis3D(tilty, Amg::Vector3D(0.,1.,0.));
  *amgTransf *= Amg::AngleAxis3D(tiltx, Amg::Vector3D(1.,0.,0.));
 // preparation
  Trk::PerigeeSurface persf(amgTransf); 
  const Trk::TrackParameters* vertexPerigee = 0;
  // retrieve the Perigee from the track particle
  const Trk::Perigee& trackparPerigee = tp.perigeeParameters();
  if ( trackparPerigee.associatedSurface() == persf) {
       ATH_MSG_DEBUG("Perigee of TrackParticle is already expressed to given vertex, a copy is returned.");
       delete amgTransf;
       return(trackparPerigee.clone());
  } else 
      vertexPerigee = m_extrapolator->extrapolateDirectly(trackparPerigee, persf);
  if (!vertexPerigee){
     ATH_MSG_DEBUG("Extrapolation to Beam Line failed, a NULL pointer is returned.");
     delete amgTransf;
  }
  return vertexPerigee;

}

const Trk::TrackParameters* Reco::TrackToVertex::trackAtBeamline(const Trk::Track& trk,
                                const Trk::StraightLineSurface* beamline) const
{
  return m_extrapolator->extrapolate(trk, *beamline);
}

const Trk::TrackParameters* Reco::TrackToVertex::trackAtBeamline(const Trk::TrackParameters& tpars,
                                const Trk::StraightLineSurface* beamline) const
{
  return m_extrapolator->extrapolate(tpars, *beamline);
}


