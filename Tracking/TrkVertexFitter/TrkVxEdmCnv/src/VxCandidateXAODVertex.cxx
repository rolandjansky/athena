/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



///////////////////////// -*- C++ -*- /////////////////////////////
// VxCandidateXAODVertex.cxx
// Implementation file for class VxCandidateXAODVertex
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// VxEdmXAODCnv includes
#include "TrkVxEdmCnv/VxCandidateXAODVertex.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkLinks/LinkToXAODNeutralParticle.h"



namespace Trk{
///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
VxCandidateXAODVertex::VxCandidateXAODVertex( const std::string& type,
		      const std::string& name,
		      const IInterface* parent ) :
  ::AthAlgTool  ( type, name, parent   ),
  m_LinearizedTrackFactory("Trk::InDetFullLinearizedTrackFactory", this)
{
  declareInterface< IVxCandidateXAODVertex >(this);
  //
  // Property declaration
  //
  declareProperty("LinearizedTrackFactory",m_LinearizedTrackFactory);

}


// Athena algtool's Hooks
////////////////////////////
StatusCode VxCandidateXAODVertex::initialize()
{
  ATH_MSG_VERBOSE ("Initializing " << name() << "...");

  // initialize Linearized Track Factory for VxTrackAtVertex creation
  if ( m_LinearizedTrackFactory.retrieve().isFailure() ) {
    ATH_MSG_WARNING("Failed to retrieve tool " << m_LinearizedTrackFactory << ". Creation of VxCandidate from xAOD::Vertex only partially possible.");
  }
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////

  StatusCode VxCandidateXAODVertex::createVxCandidate(const xAOD::Vertex &xAODVx, Trk::VxCandidate* &vxVertex) const
  {
    if (vxVertex) {
      ATH_MSG_WARNING("Passed a valid pointer as return object. It will be overwritten: potential memory leak!");
    }
    //create vxTrackAtVertex linearizing with respect to vertex
    std::vector <Trk::VxTrackAtVertex*> tavCollection;
    bool xaod_no_tracks = true;
//check whether there are already VxTracksAtVertex here
#ifndef XAOD_STANDALONE
     if(xAODVx.vxTrackAtVertexAvailable())
     {
//ok  there are some, let us convert them
// remember that the VxCandidate takes over the ownership, so it needs a copy..

      const std::vector< Trk::VxTrackAtVertex >& vtv = xAODVx.vxTrackAtVertex();
      unsigned int vtv_size = vtv.size();
      for(unsigned int i = 0;i<vtv_size;++i) tavCollection.push_back(vtv.at(i).clone());
      xaod_no_tracks = false;
     }//end of the VxTrackAtVertex conversion

#endif


 //if they were not coming from xAOD, let us create them on the spot
    if(xaod_no_tracks)
    {
     // This is really ugly! This will make a copy of each ElementLink object
     // during the loop. But because Trk::LinkToXAODTrackParticle expects a
     // non-const reference (why???), we can't use "const auto&" as the type.
     for (auto t : xAODVx.trackParticleLinks())
     {
       Trk::VxTrackAtVertex *tav = new VxTrackAtVertex();

      //create ITrackLink to xAOD::TrackParticle
       Trk::LinkToXAODTrackParticle *elTrackParticle = new Trk::LinkToXAODTrackParticle( t );
       tav->setOrigTrack(elTrackParticle);

      //linearize with respect to given vertex (do we need/want it here?)
       Trk::LinearizedTrack* myLinearizedTrack=m_LinearizedTrackFactory->linearizedTrack(elTrackParticle->parameters(),xAODVx.position());
       tav->setLinTrack(myLinearizedTrack);

      //TODO: do we need/want to calculate other quantities?
      // - *m_initialPerigee
      // - m_perigeeAtVertex
      // - m_ImpactPoint3dAtaPlane
      // - m_VertexCompatibility
      // - m_trkWeight
      // - *m_fitQuality

      //push back into collection
      tavCollection.push_back(tav);
     }//end of loop over all tracks
     //Same for neutrals
     for (auto t : xAODVx.neutralParticleLinks())      {
       Trk::VxTrackAtVertex *tav = new VxTrackAtVertex();
       //create ITrackLink to xAOD::NeutralParticle
       Trk::LinkToXAODNeutralParticle *elNeutralParticle = new Trk::LinkToXAODNeutralParticle( t );
       tav->setOrigTrack(elNeutralParticle);
       //linearize with respect to given vertex (do we need/want it here?)
       Trk::LinearizedTrack* myLinearizedTrack=m_LinearizedTrackFactory->linearizedTrack(elNeutralParticle->neutralParameters(),xAODVx.position());
       tav->setLinTrack(myLinearizedTrack);
       //push back into collection
       tavCollection.push_back(tav);
     }//end of loop over all neutrals
    }//end of the xod check

    RecVertex recVtx(xAODVx.position(), xAODVx.covariancePosition(), xAODVx.numberDoF(), xAODVx.chiSquared());
    vxVertex = new VxCandidate(recVtx, tavCollection);
    //TODO: only one enum should survive!
    vxVertex->setVertexType((Trk::VertexType)xAODVx.vertexType());

    return StatusCode::SUCCESS;
  }

  StatusCode VxCandidateXAODVertex::createXAODVertex(const Trk::VxCandidate &vxVertex, xAOD::Vertex* &xAODVx) const
  {

    ATH_MSG_DEBUG("Creating new xAOD vertex from VxCandidate ");
    ATH_MSG_VERBOSE(vxVertex);
    if (!xAODVx) {
      xAODVx = new xAOD::Vertex();
      xAODVx->makePrivateStore();
      ATH_MSG_VERBOSE("Internal Aux store implicitly associated to the new VxVertex object. Pass a valid xAODVx pointer for output for optimal performance.");
    }

    // set type, position, covariance and fit quality
    xAODVx->setVertexType((xAOD::VxType::VertexType)vxVertex.vertexType());
    xAODVx->setPosition(vxVertex.recVertex().position());
    xAODVx->setCovariancePosition(vxVertex.recVertex().covariancePosition());
    xAODVx->setFitQuality(vxVertex.recVertex().fitQuality().chiSquared(), static_cast<float>(vxVertex.recVertex().fitQuality().doubleNumberDoF ()));

    //now set links to xAOD::TrackParticles
    unsigned int VTAVsize = vxVertex.vxTrackAtVertex()->size();
    for (unsigned int i = 0 ; i < VTAVsize ; ++i)
    {
      Trk::VxTrackAtVertex* VTAV = vxVertex.vxTrackAtVertex()->at(i);
      if (not VTAV){ //added check on VTAV here, instead of inside the ifndef sroe 4 March 2015
        ATH_MSG_WARNING (" Trying to convert to xAOD::Vertex a Trk::VxCandidate. The VxTrackAtVertex is not found");
        continue;
      }
#ifndef XAOD_STANDALONE
      xAODVx->vxTrackAtVertex().push_back(*VTAV);
#endif

      Trk::ITrackLink*      trklink = VTAV->trackOrParticleLink();
      Trk::LinkToXAODTrackParticle* linkToXAODTP = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trklink);
      if (linkToXAODTP)
      {
				//Now set the newlink to the new xAOD vertex
				xAODVx->addTrackAtVertex(*linkToXAODTP, VTAV->weight());
      } else {
				Trk::LinkToXAODNeutralParticle* linkToXAODTPneutral = dynamic_cast<Trk::LinkToXAODNeutralParticle*>(trklink);
				if (!linkToXAODTPneutral)
					ATH_MSG_WARNING ("Skipping track. Trying to convert to xAOD::Vertex a Trk::VxCandidate with links to something else than xAOD::TrackParticle or xAOD::NeutralParticle.");
				else
					//Now set the newlink to the new xAOD vertex
					xAODVx->addNeutralAtVertex(*linkToXAODTPneutral, VTAV->weight());
      }
    }//end of loop

    ATH_MSG_DEBUG("Successfully created new xAOD::Vertex  " << xAODVx);
    return StatusCode::SUCCESS;
  }

} //end  namespace Trk
