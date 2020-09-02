/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkVertexFitterUtils/Z0PVTrackCompatibilityEstimator.h"

#include "TrkTrack/Track.h"

#include "TrkParticleBase/TrackParticleBase.h"

#include "TrkParameters/TrackParameters.h"

#include "VxVertex/VxTrackAtVertex.h"



namespace Trk{

  Z0PVTrackCompatibilityEstimator::Z0PVTrackCompatibilityEstimator(const std::string& type, 
							       const std::string& name,
							       const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_ipEstimator("Trk::TrackToVertexIPEstimator"),
    m_z0SignificanceMax(30.)
  {
    declareInterface< Z0PVTrackCompatibilityEstimator >(this);
    declareProperty("IPEstimator", m_ipEstimator, "Impact parameter estimator tool");   
    declareProperty("Z0SignificanceMax", m_z0SignificanceMax);
  }

  Z0PVTrackCompatibilityEstimator::~Z0PVTrackCompatibilityEstimator()= default;


  StatusCode Z0PVTrackCompatibilityEstimator::initialize(){
    
    ATH_MSG_INFO( "Initializing Z0PVTrackCompatibilityEstimator" );

    StatusCode sc = m_ipEstimator.retrieve();
    if( !sc.isSuccess() ){
      msg(MSG::FATAL) << "Unable to retrieve impact parameter estimator tool" << endmsg;
      return sc;
    }

    return StatusCode::SUCCESS; 
  }


  StatusCode Z0PVTrackCompatibilityEstimator::finalize(){

    ATH_MSG_INFO( "Finalizing Z0PVTrackCompatibilityEstimator" );

    return StatusCode::SUCCESS; 
  } 



  bool Z0PVTrackCompatibilityEstimator::isCompatible(const xAOD::TrackParticle* track,
                                                     const xAOD::VertexContainer* primaryVertexContainer, unsigned int index) const{

    return isCompatible( &(track->perigeeParameters()), primaryVertexContainer, index );
  }

  bool Z0PVTrackCompatibilityEstimator::isCompatible(const TrackParameters* track,
						     const xAOD::VertexContainer* primaryVertexContainer, unsigned int index) const{
    
    // TODO: do we want to doRemoval here? (I have it as false for now) 
    const ImpactParametersAndSigma* ip =  m_ipEstimator->estimate(track, track, (*primaryVertexContainer)[index], false);
    
    bool pileup = false;
  
    double sumptPV = 0;
    
    /*
    for(std::vector<Trk::VxTrackAtVertex*>::const_iterator track = 
	  (*primaryVertexContainer)[index]->vxTrackAtVertex()->begin();
	track != (*primaryVertexContainer)[index]->vxTrackAtVertex()->end(); ++track){
      
      sumptPV += (*track)->initialPerigee()->momentum().perp();
    }
    */

    // TODO: perhaps calculate sumptPV using VxTracksAtVertex in xAOD::Vertex? -David S.
    for(unsigned int i = 0; i < (*primaryVertexContainer)[index]->nTrackParticles(); ++i){

      sumptPV += (*primaryVertexContainer)[index]->trackParticle(i)->perigeeParameters().momentum().perp();
    }

    for(unsigned int i = 0; i < primaryVertexContainer->size(); i++){
      
      double sumpt = 0.;
      
      if( i != index &&
	  (*primaryVertexContainer)[i]->vertexType() != xAOD::VxType::NoVtx ){
        /*
	for(std::vector<Trk::VxTrackAtVertex*>::const_iterator track = 
	      (*primaryVertexContainer)[i]->vxTrackAtVertex()->begin();
	    track != (*primaryVertexContainer)[i]->vxTrackAtVertex()->end(); ++track){
	
	  sumpt += (*track)->initialPerigee()->momentum().perp();
	}
	*/

        // TODO: perhaps calculate sumptPV using VxTracksAtVertex in xAOD::Vertex? -David S.
        for(unsigned int itrk = 0; itrk < (*primaryVertexContainer)[i]->nTrackParticles(); ++itrk){

          sumpt += (*primaryVertexContainer)[i]->trackParticle(itrk)->perigeeParameters().momentum().perp();
        }
	double vxReach = 
	  std::abs((*primaryVertexContainer)[i]->position().z()
		   - (*primaryVertexContainer)[index]->position().z()) * sumpt / (sumpt + sumptPV);
	
	if(std::abs(ip->IPz0 + (*primaryVertexContainer)[index]->position().z() -
		    (*primaryVertexContainer)[i]->position().z()) < vxReach){
	  
	  pileup = true;
	}
      }
    }
  
    if(std::abs(ip->IPz0) / ip->sigmaz0 < m_z0SignificanceMax && !pileup){

      delete ip;
      
      return true;
    }
  
    delete ip;
    
    return false;
  }


  StatusCode Z0PVTrackCompatibilityEstimator::queryInterface(const InterfaceID& riid, void** ppvIf){
    if(interfaceID() == riid){
      *ppvIf = dynamic_cast< Z0PVTrackCompatibilityEstimator* > (this);
    } 
    else if(IPVTrackCompatibilityEstimator::interfaceID() == riid){
      *ppvIf = dynamic_cast< IPVTrackCompatibilityEstimator* > (this);
    } 
    else{
      return AthAlgTool::queryInterface(riid, ppvIf);
    }
    
    addRef();
    return StatusCode::SUCCESS;
  }

}
