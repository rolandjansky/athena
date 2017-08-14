/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParametersKVU.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkSUSY/TrackParametersKVU.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TrkVertexFitterInterfaces/IVertexTrackUpdator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TrackParametersKVU::TrackParametersKVU( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_collTrackName("InDetPixelPrdAssociationTrackParticles"),
  m_collVertexName("PrimaryVertices"),
  m_vertexTrackUpdator("Trk::KalmanVertexTrackUpdator"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_LinearizedTrackFactory("Trk::FullLinearizedTrackFactory/FullLinearizedTrackFactory"),
  m_IPEstimator("Trk::TrackToVertexIPEstimator"),
  m_sgName(""),
  m_sgKey1("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackParticleContainerName", m_collTrackName);
    declareProperty("VertexContainerName", m_collVertexName);
    declareProperty("VertexTrackUpdator",m_vertexTrackUpdator);
    declareProperty("TrackExtrapolator", m_extrapolator);
    declareProperty("LinearizedTrackFactory",m_LinearizedTrackFactory);
    declareProperty("DecorationPrefix", m_sgName);
    declareProperty("KVUSGEntryName", m_sgKey1); 
  }
 
// Destructor
DerivationFramework::TrackParametersKVU::~TrackParametersKVU() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::TrackParametersKVU::initialize()
{

  if (m_collTrackName == "" || m_collVertexName == "") {
    ATH_MSG_ERROR("No selection variables for the TrackParametersKVU tool!");
    return StatusCode::FAILURE;
  }
  
  if (m_sgKey1 == "") {
    ATH_MSG_ERROR("No Store Gate Keys for the TrackParametersKVU tool!");
    return StatusCode::FAILURE;
  }
  
  CHECK(m_vertexTrackUpdator.retrieve());

  CHECK(m_extrapolator.retrieve());

  CHECK(m_LinearizedTrackFactory.retrieve());

  ATH_MSG_VERBOSE("initialize() ...");
  ATH_MSG_VERBOSE("Successfully retrived the TrackParametersKVU tool" );
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TrackParametersKVU::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::TrackParametersKVU::addBranches() const
{

  // --- Get the tracks
  const xAOD::TrackParticleContainer* tracks = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_collTrackName) ) {
    CHECK( evtStore()->retrieve(tracks,m_collTrackName ) );
  } else {
    ATH_MSG_WARNING("No track collection found. Skipping event.");
    return StatusCode::SUCCESS;
  }


  //-- for each track, update track params with vtx considered as extra measurement (choose the closest vtx)
  if(tracks->size() !=0) { 
    for (const auto& track : *tracks) {
      if(track){
	const Trk::TrackParameters* trackParams = 0;
	float minIP = 1000.;
	//--- retrieve closest vertex to track
	const xAOD::Vertex* closestVertex= 0;
	int nVtx = 0;
	if ( evtStore()->contains<xAOD::VertexContainer>(m_collVertexName) ) {
	  const xAOD::VertexContainer* vxContainer =  0;
	  CHECK( evtStore()->retrieve(vxContainer, m_collVertexName) );
	  if (vxContainer) {
	    for (const xAOD::Vertex* vtx: *vxContainer) {
	      if ( (vtx->vertexType() == xAOD::VxType::PriVtx) ||
	  	   (vtx->vertexType() == xAOD::VxType::PileUp) ) {
	  	Amg::Vector3D  vtxPos(vtx->position());
	  	const Trk::PerigeeSurface* vtxSurface = new Trk::PerigeeSurface(vtxPos);
	  	trackParams = m_extrapolator->extrapolate(*track,*vtxSurface);
		const Trk::ImpactParametersAndSigma* iPandSigma(NULL);
	  	iPandSigma = m_IPEstimator->estimate(trackParams, vtx);
	  	if(sqrt(iPandSigma->IPd0*iPandSigma->IPd0+iPandSigma->IPz0*iPandSigma->IPz0) < minIP){
	  	  minIP = sqrt(iPandSigma->IPd0*iPandSigma->IPd0+iPandSigma->IPz0*iPandSigma->IPz0);
	  	  closestVertex = vtx;
	  	}
	  	ATH_MSG_VERBOSE("n vtx pos(x, y, z) IPd0 IPz0 : " << nVtx << " " << vtx->position().x() << " "<<  vtx->position().y() << " "<<  vtx->position().z() << " " << iPandSigma->IPd0 << " " << iPandSigma->IPz0);
		ATH_MSG_VERBOSE("                   d0,Delta_z0-TrackParticle:" << track->d0() <<", "<< track->z0() - vtx->z() + track->vz());
	  	nVtx++;
	      }
	    }
	  }else{
	    ATH_MSG_ERROR ("Couldn't retrieve vxContainer  with key: " << m_collVertexName );
	    return StatusCode::FAILURE;
	  }
	} // --- end retrieve closest vertex to track

	// --- list of new variables that will decorate the track
	AmgSymMatrix(5) *updateTrackCov = 0;
	float updatephi = -999;
	float updatetheta = -999;
	float updated0 = -999;
	float updatez0 = -999;
	float updateqOverP = -999;
	float updateChi2 = -999; 

	SG::AuxElement::Decorator< float > decoratorKVUphi(m_sgName+"KVUphi");
	SG::AuxElement::Decorator< float > decoratorKVUtheta(m_sgName+"KVUtheta");
	SG::AuxElement::Decorator< float > decoratorKVUd0(m_sgName+"KVUd0");
	SG::AuxElement::Decorator< float > decoratorKVUz0(m_sgName+"KVUz0");
	SG::AuxElement::Decorator< float > decoratorKVUqOverP(m_sgName+"KVUqOverP");
	SG::AuxElement::Decorator< float > decoratorKVUChi2(m_sgName+"KVUChi2");
	SG::AuxElement::Decorator< std::vector<float> > decoratorKVUCovMat(m_sgName+"KVUCovMat");
	
	// update the track params with vtx info after linearization of track around it
	if(closestVertex){
	  ATH_MSG_VERBOSE("Vertex selected position (x y z): " << closestVertex->position().x()<<" "<<closestVertex->position().y()<<" "<<closestVertex->position().z());
	  ATH_MSG_VERBOSE("                type: " << closestVertex->vertexType());
	  ATH_MSG_VERBOSE("                nParticles: " << closestVertex->nTrackParticles());	  
	  ATH_MSG_VERBOSE("                quality chi2: " << closestVertex->chiSquared() << " " << closestVertex->numberDoF());
	  Amg::Vector3D  globPos(closestVertex->position()); 

	  xAOD::Vertex *recVtx = new xAOD::Vertex();
	  recVtx->makePrivateStore(*closestVertex);

	  const Trk::PerigeeSurface* surface = new Trk::PerigeeSurface(globPos);
	  trackParams = m_extrapolator->extrapolate(*track,*surface);
	  Trk::VxTrackAtVertex* linearTrack = new Trk::VxTrackAtVertex(0.,0,NULL,trackParams,NULL);
	  if(linearTrack){
	    ATH_MSG_VERBOSE("Linearizing track");
	    m_LinearizedTrackFactory->linearize(*linearTrack,globPos);
	    ATH_MSG_VERBOSE("Updating linearized track parameters after vertex fit. Track weight = " << linearTrack->weight());
	    m_vertexTrackUpdator->update((*linearTrack), (*recVtx));
	    ATH_MSG_VERBOSE ("track info after vertex track updator !"<<*linearTrack );
	    //retrieve & store updated track param qOverP,d0, z0,theta,phi after KVU
	    updateqOverP = linearTrack->perigeeAtVertex()->parameters()[Trk::qOverP];
	    updated0 = linearTrack->perigeeAtVertex()->parameters()[Trk::d0];
	    updatez0 = linearTrack->perigeeAtVertex()->parameters()[Trk::z0];
	    updatephi = linearTrack->perigeeAtVertex()->parameters()[Trk::phi0];
	    updatetheta = linearTrack->perigeeAtVertex()->parameters()[Trk::theta];
	    //retrieve & store updated track cov matrix plus save the KVU Chi2 
	    updateTrackCov = new AmgSymMatrix(5)(*linearTrack->perigeeAtVertex()->covariance());
	    updateChi2 = linearTrack->trackQuality().chiSquared();
	  }
	  delete recVtx;
	}// --- end if closest vertex

	//decorate tracks with new updated track parameters:
	decoratorKVUqOverP(*track) = updateqOverP;
	decoratorKVUd0(*track) = updated0;
	decoratorKVUz0(*track) = updatez0;
	decoratorKVUphi(*track) = updatephi;
	decoratorKVUtheta(*track) = updatetheta;
	decoratorKVUChi2(*track) = updateChi2;
	std::vector<float> vec;
	if (updateTrackCov)
	  Amg::compress(*updateTrackCov, vec);
	else
	  vec.assign(5, 0.0);
	decoratorKVUCovMat(*track) =  vec;
	ATH_MSG_VERBOSE("track updated.");
      } // --- end if(track)
    } // --- end loop tracks
    ATH_MSG_VERBOSE("All tracks updated.");
  }	
  
  return StatusCode::SUCCESS;
}

