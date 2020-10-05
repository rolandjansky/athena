/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitterUtils/TrackToVertexIPEstimator.h"
#include "TrkTrack/Track.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"
#include "TrkTrackLink/ITrackLink.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"

namespace Trk
{

 TrackToVertexIPEstimator::TrackToVertexIPEstimator(const std::string& t, const std::string& n, const IInterface*  p):
  AthAlgTool(t,n,p), m_extrapolator("Trk::Extrapolator",this),m_Updator("Trk::KalmanVertexUpdator",this),
  m_linFactory("Trk::FullLinearizedTrackFactory",this)
 {
  declareProperty("Extrapolator",            m_extrapolator);
  declareProperty("VertexUpdator",           m_Updator);
  declareProperty("LinearizedTrackFactory",  m_linFactory);
  declareInterface<ITrackToVertexIPEstimator>(this);    
 }

 TrackToVertexIPEstimator::~TrackToVertexIPEstimator()= default;

 StatusCode TrackToVertexIPEstimator::initialize()
 {

//uploading the corresponding tools
//extrapolator    
  if ( m_extrapolator.retrieve().isFailure() ) 
  {
   msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endmsg;
   return StatusCode::FAILURE;
  } 
   msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
  

//updator 
  if ( m_Updator.retrieve().isFailure() ) 
  {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_Updator << endmsg;
    return StatusCode::FAILURE;
  } 
    msg(MSG::INFO) << "Retrieved tool " << m_Updator << endmsg;
  
  
//linearized track factory
  if ( m_linFactory.retrieve().isFailure() ) 
  {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_linFactory << endmsg;
    return StatusCode::FAILURE;
  } 
    msg(MSG::INFO) << "Retrieved tool " << m_linFactory << endmsg;
  
  
   return StatusCode::SUCCESS;  
 }//end of initialize method
    
 StatusCode TrackToVertexIPEstimator::finalize()
 { 
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
 }

 const  ImpactParametersAndSigma * TrackToVertexIPEstimator::estimate(const xAOD::TrackParticle * track, const xAOD::Vertex * vtx, bool doRemoval) const
 {
  if(track && vtx)
  {
   return estimate(&(track->perigeeParameters()),&(track->perigeeParameters()),vtx,doRemoval); 
  }
   msg(MSG::INFO) << "Empty TrackParticle or Vertex pointer passed. Returning zero " << endmsg;
   return nullptr;
  //end of track particle validity check
 }//end of method using track particles
 
 const  ImpactParametersAndSigma * TrackToVertexIPEstimator::estimate(const xAOD::TrackParticle * track, const xAOD::TrackParticle * newtrack, const xAOD::Vertex * vtx, bool doRemoval) const
 {
  if(track && vtx)
  {
    return estimate(&(track->perigeeParameters()),&(newtrack->perigeeParameters()),vtx,doRemoval); 
  }
   msg(MSG::INFO) << "Empty TrackParticle or Vertex pointer passed. Returning zero " << endmsg;
   return nullptr;
  //end of track particle validity check
 }//end of method using track particles
 



 const  ImpactParametersAndSigma * TrackToVertexIPEstimator::estimate(const TrackParameters * track, const xAOD::Vertex * vtx, bool doRemoval) const
 {
   if(track && vtx){
     return estimate(track,track,vtx,doRemoval); 
   }
     msg(MSG::INFO) << "Empty TrackParticle or Vertex pointer passed. Returning zero " << endmsg;
     return nullptr;
   //end of track particle validity check
 
 }//end of parameterBase estimate method     
   
 const  ImpactParametersAndSigma * TrackToVertexIPEstimator::estimate(const TrackParameters * track, const TrackParameters * newtrack, const xAOD::Vertex * vtx, bool doRemoval) const
 {
 
   if (vtx==nullptr) 
   {
     ATH_MSG_WARNING("Vertex is zero pointer. Will not estimate IP of track.");
     return nullptr;
   }

   const xAOD::Vertex *newVertex = vtx;
   if (doRemoval)
   {
     newVertex = getUnbiasedVertex(track,vtx);
     if (newVertex == nullptr) {
       ATH_MSG_WARNING("Unbiasing of vertex failed. Will not estimate IP of track.");
       return nullptr;
     }
   }

   const ImpactParametersAndSigma  *  IPandSigma=calculate(newtrack,*newVertex);

   if (doRemoval)
   {
     delete newVertex;
     newVertex=nullptr;
   }
   
   return IPandSigma;

 }//end of parameterBase estimate method   

   
 const  ImpactParametersAndSigma * TrackToVertexIPEstimator::calculate(const TrackParameters * track, const xAOD::Vertex& vtx) const
 { 
  //estimating the d0 and its significance by propagating the trajectory state towards 
  //the vertex position. By this time the vertex should NOT contain this trajectory anymore
  
  //estrapolating to the  perigee of the reconstructed vertex  
  const Amg::Vector3D & lp = vtx.position();
  PerigeeSurface perigeeSurface(lp);
  const  Trk::Perigee * extrapolatedParameters =dynamic_cast<const Trk::Perigee *>(m_extrapolator->extrapolate(*track,perigeeSurface));
  if (extrapolatedParameters && extrapolatedParameters->covariance()) {
  
    //actual calculation of d0 and sigma.  
    const AmgVector(5) & par = extrapolatedParameters->parameters();
    const double d0  = par[Trk::d0];
    const double z0  = par[Trk::z0];
    const double phi = par[Trk::phi];
    const double theta = par[Trk::theta];

    AmgSymMatrix(2) vrtXYCov = vtx.covariancePosition().block<2,2>(0,0);
   
    //   std::cout<<"Vertex covariance: "<<vtx.errorPosition().covariance()<<std::endl;
    //   std::cout<<"Vertex covariance sub: "<<vrtXYCov<<std::endl;
   
    const AmgSymMatrix(5) & perigeeCov = *(extrapolatedParameters->covariance());
    //   std::cout<<"Perigee covariance: "<<perigeeCov<<std::endl;
   
    //d0phi->cartesian Jacobian  
    Amg::Vector2D d0JacXY(-sin(phi), cos(phi));
   
    //  std::cout<<"To cartesian jacobian "<<d0PhiCart<<std::endl;
    //  std::cout<<" - d0*cos(phi)"<< - d0*cos(phi)<<std::endl;

    ImpactParametersAndSigma* newIPandSigma=new ImpactParametersAndSigma;
    newIPandSigma->IPd0=d0;
    double d0_PVcontrib=d0JacXY.transpose()*(vrtXYCov*d0JacXY);
    if (d0_PVcontrib>=0)
    {
      newIPandSigma->sigmad0=sqrt(d0_PVcontrib+perigeeCov(Trk::d0,Trk::d0));
      newIPandSigma->PVsigmad0=sqrt(d0_PVcontrib);
    }
    else
    {
      msg(MSG::WARNING) << " The contribution to d0_err: " << d0_PVcontrib << " from PV is negative: critical error in PV error matrix! Removing contribution from PV ... "  << endmsg;
      newIPandSigma->sigmad0=sqrt(perigeeCov(Trk::d0,Trk::d0));
      newIPandSigma->PVsigmad0=0;
    }

    AmgSymMatrix(2) covPerigeeZ0Theta;
   covPerigeeZ0Theta(0,0)=perigeeCov(Trk::z0,Trk::z0);
   covPerigeeZ0Theta(0,1)=perigeeCov(Trk::z0,Trk::theta);
   covPerigeeZ0Theta(1,0)=perigeeCov(Trk::theta,Trk::z0);
   covPerigeeZ0Theta(1,1)=perigeeCov(Trk::theta,Trk::theta);

   double vrtZZCov = vtx.covariancePosition()(Trk::z,Trk::z);

   Amg::Vector2D IPz0JacZ0Theta (sin(theta), z0*cos(theta));
   if (vrtZZCov>=0)
   {
     newIPandSigma->IPz0SinTheta=z0*sin(theta);
     newIPandSigma->sigmaz0SinTheta=
         sqrt(IPz0JacZ0Theta.transpose()*(covPerigeeZ0Theta*IPz0JacZ0Theta)+sin(theta)*vrtZZCov*sin(theta));
     newIPandSigma->PVsigmaz0SinTheta=sqrt(sin(theta)*vrtZZCov*sin(theta));
     newIPandSigma->IPz0 = z0;
     newIPandSigma->sigmaz0 = std::sqrt( vrtZZCov + perigeeCov(Trk::z0,Trk::z0) );
     newIPandSigma->PVsigmaz0 = std::sqrt( vrtZZCov );
   }
   else
   {
     msg(MSG::WARNING) << " The contribution to z0_err: " << vrtZZCov << " from PV is negative: critical error in PV error matrix! Removing contribution from PV ... "  << endmsg;     
     newIPandSigma->IPz0SinTheta=z0*sin(theta);
     double temp = (IPz0JacZ0Theta.transpose()*(covPerigeeZ0Theta*IPz0JacZ0Theta));
     newIPandSigma->sigmaz0SinTheta=sqrt(temp);
     newIPandSigma->PVsigmaz0SinTheta=0;
     
     newIPandSigma->IPz0 = z0;
     newIPandSigma->sigmaz0 = std::sqrt( perigeeCov(Trk::z0,Trk::z0) );
     newIPandSigma->PVsigmaz0 = 0;
   }

   //   std::cout<<"Calculated sigma: "<<sqrt(sigmaM)<<std::endl;
  
   //checking the other way of calculating the errors
   // Calculation using the projection
   //   double  s_d0_test = extrapolatedParameters->localErrorMatrix().covValue(Trk::d0)+
   //   sin(phi) * sin(phi) * vtx.errorPosition().covValue(Trk::x) +
   //   cos(phi) * cos(phi) * vtx.errorPosition().covValue(Trk::y) - 
   //   2.* sin(phi) * cos(phi) * vtx.errorPosition().covValue(Trk::x, Trk::y);
   //   std::cout<<"new sigma: "<<sqrt(s_d0_test)<<std::endl; 
   
   delete extrapolatedParameters;
   return newIPandSigma;
  } 
    ATH_MSG_DEBUG ("Cannot extrapolate the trajectory state. Returning null. ");
    return nullptr;
  //end of successfull extrapolation check
 }//end of actual calculation method






  double TrackToVertexIPEstimator::get3DLifetimeSignOfTrack(const TrackParameters & track,
                                                            const CLHEP::Hep3Vector & jetMomentum,
                                                            const xAOD::Vertex & primaryVertex) const
  {
    Amg::Vector3D eigenJetMomentum(jetMomentum.x(), jetMomentum.y(), jetMomentum.z());
    return get3DLifetimeSignOfTrack(track, eigenJetMomentum, primaryVertex);
  }

  double TrackToVertexIPEstimator::get3DLifetimeSignOfTrack(const TrackParameters & track,
                                                            const Amg::Vector3D & jetMomentum,
                                                            const xAOD::Vertex & primaryVertex) const
  {
    const Amg::Vector3D & lp = primaryVertex.position();
    PerigeeSurface perigeeSurface(lp);
    
    const  Trk::TrackParameters * extrapolatedParameters = m_extrapolator->extrapolate(track,perigeeSurface);
    
    if (!extrapolatedParameters) return 0.;

    const Amg::Vector3D & primaryPos=primaryVertex.position();
    const Amg::Vector3D & trackPos=extrapolatedParameters->position();
    const Amg::Vector3D & trackMom=extrapolatedParameters->momentum();
    
    double sign=(jetMomentum.cross(trackMom)).dot(trackMom.cross(primaryPos-trackPos));
    delete extrapolatedParameters;
    
    return sign>=0.?1.:-1;
  }

  

  double TrackToVertexIPEstimator::get2DLifetimeSignOfTrack(const TrackParameters & track,
                                                            const CLHEP::Hep3Vector & jetMomentum,
                                                            const xAOD::Vertex & primaryVertex) const
  {
    Amg::Vector3D eigenJetMomentum(jetMomentum.x(), jetMomentum.y(), jetMomentum.z());
    return get2DLifetimeSignOfTrack(track, eigenJetMomentum, primaryVertex);
  }

  double TrackToVertexIPEstimator::get2DLifetimeSignOfTrack(const TrackParameters & track,
                                                            const Amg::Vector3D & jetMomentum,
                                                            const xAOD::Vertex & primaryVertex) const
  {
    const Amg::Vector3D & lp = primaryVertex.position();
    PerigeeSurface perigeeSurface(lp);
    
    const  Trk::TrackParameters * extrapolatedParameters =  m_extrapolator->extrapolate(track,perigeeSurface);
    
    if (!extrapolatedParameters) return 0.;

    double trackD0  = extrapolatedParameters->parameters()[Trk::d0];
    double trackPhi = extrapolatedParameters->parameters()[Trk::phi];
    double vs = sinf( atan2(jetMomentum.y(),jetMomentum.x()) - trackPhi )*trackD0;
   
    delete extrapolatedParameters;
    return (vs>=0. ? 1. : -1.);
  }




  double TrackToVertexIPEstimator::getZLifetimeSignOfTrack(const TrackParameters & track,
                                                           const CLHEP::Hep3Vector & jetMomentum,
                                                           const xAOD::Vertex & primaryVertex) const
  {
    Amg::Vector3D eigenJetMomentum(jetMomentum.x(), jetMomentum.y(), jetMomentum.z());
    return getZLifetimeSignOfTrack(track, eigenJetMomentum, primaryVertex);
  }

  double TrackToVertexIPEstimator::getZLifetimeSignOfTrack(const TrackParameters & track,
                                                           const Amg::Vector3D & jetMomentum,
                                                           const xAOD::Vertex & primaryVertex) const
  {

    const Amg::Vector3D & lp = primaryVertex.position();
    PerigeeSurface perigeeSurface(lp);
    
    const  Trk::TrackParameters * extrapolatedParameters = m_extrapolator->extrapolate(track,perigeeSurface);
    
    if (!extrapolatedParameters) return 0.;

    double trackTheta  = extrapolatedParameters->parameters()[Trk::theta];
    double trackZ0 = extrapolatedParameters->parameters()[Trk::z0];
    double trackEta = -logf(tanf(trackTheta/2.));
    double jetEta = jetMomentum.eta();
    double zs = (jetEta - trackEta)*trackZ0;
    delete extrapolatedParameters;
    return (zs>=0. ? 1. : -1.);
  }


/*
  StatusCode TrackToVertexIPEstimator::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
  
  
    if(interfaceID() == riid){
      *ppvIf = dynamic_cast< TrackToVertexIPEstimator* > (this);
    } 
    else if(ITrackToVertexIPEstimator::interfaceID() == riid){
      *ppvIf = dynamic_cast<ITrackToVertexIPEstimator*> (this);
    } 
    else{
      return AthAlgTool::queryInterface(riid, ppvIf);
    }
    
    addRef();
    return StatusCode::SUCCESS;
     
  }
*/

///// ANTHONY ADDITION

const xAOD::Vertex * TrackToVertexIPEstimator::getUnbiasedVertex(const xAOD::TrackParticle * track, const xAOD::Vertex * vtx ) const
 {
  if(track)
  {
   return getUnbiasedVertex(&(track->perigeeParameters()),vtx);
  }
   msg(MSG::INFO) << "Empty xAOD::TrackParticle pointer passed. Returning zero " << endmsg;
   return nullptr;
  //end of track particle validity check   
 }

const xAOD::Vertex * TrackToVertexIPEstimator::getUnbiasedVertex(const TrackParameters * track, const xAOD::Vertex * vtx ) const 
 {
   if (!track) {
     msg(MSG::INFO) << "Empty Trk::TrackParameter pointer passed. Returning zero " << endmsg;
     return nullptr;
   }
   if (!vtx) {
     msg(MSG::INFO) << "Empty xAOD::Vertex pointer passed. Returning zero " << endmsg;
     return nullptr;
   }

   if (vtx->nTrackParticles() == 0)
    {
      ATH_MSG_DEBUG("This vertex has no associated tracks. Normal if beam spot is used. Vertex already unbiased");
      return new xAOD::Vertex(*vtx);
    }
   
   //create new vertex for output
   xAOD::Vertex *outputVertex = new xAOD::Vertex(*vtx);
   outputVertex->clearTracks(); //remove all tracks -> will add them back one by one
   if (outputVertex->vxTrackAtVertexAvailable()) outputVertex->vxTrackAtVertex().clear(); //remove all VxTrackAtVertex

   bool tmpLinTrack = false; //do we created a new linearised track?

   //loop over tracks
   const Amg::Vector3D & pos = track->position();
   const Amg::Vector3D & mom = track->momentum();
   for (unsigned int itrk=0; itrk < vtx->nTrackParticles(); ++itrk) {
     const Perigee& testTP = vtx->trackParticle(itrk)->perigeeParameters();
     if ((testTP.position() == pos) and (testTP.momentum() == mom)) {
       //track found, now unbias the vertex using linearized track
       const LinearizedTrack *linTrack = nullptr;
       double trackWeight(0.0);
       //first check if a VxTrackAtVertex is already available with linearized track
       if (vtx->vxTrackAtVertexAvailable()) {
	 if (vtx->vxTrackAtVertex().size() > itrk) {
	   linTrack = vtx->vxTrackAtVertex()[itrk].linState();
	   trackWeight = vtx->vxTrackAtVertex()[itrk].weight();
	 }
       }
       //if linearized track is not available, create it
       if (!linTrack) {
	 linTrack = m_linFactory->linearizedTrack(track, vtx->position());
	 trackWeight = vtx->trackWeight(itrk);
	 if (!linTrack) {
	   ATH_MSG_INFO("Failing to linearized track. Returning biased vertex.");
	   outputVertex->addTrackAtVertex(vtx->trackParticleLinks()[itrk], vtx->trackWeight(itrk));
	   if (vtx->vxTrackAtVertexAvailable()) {
	     outputVertex->vxTrackAtVertex().push_back(vtx->vxTrackAtVertex()[itrk]);//will clone everything inside -> output vertex owns all the memory
	   }
	   continue;
	 }
	 tmpLinTrack = true;
       }
       //now update vertex position removing the linearized track, and do not add the track back to the output vertex
       const IVertexUpdator::positionUpdateOutcome & reducedVertex = m_Updator->positionUpdate(*vtx, linTrack, trackWeight,IVertexUpdator::removeTrack);
       
       //calculate updated chi2
       double chi2 = vtx->chiSquared();
       double trk_chi = m_Updator->trackParametersChi2( reducedVertex, linTrack );
       chi2 += -1 * (m_Updator->vertexPositionChi2(*vtx, reducedVertex) + trackWeight * trk_chi);

       //calculate updated ndf
       double ndf = vtx->numberDoF();
       ndf += -1 * trackWeight * (2.0);

       //reducedVertex has the updated position and covariance
       outputVertex->setPosition(reducedVertex.position);
       outputVertex->setCovariancePosition(reducedVertex.covariancePosition);

       //chi2 and ndf now store the updated FitQuality
       outputVertex->setFitQuality( chi2, ndf );

       if (tmpLinTrack) delete linTrack; //only delete if it was created new, and doesn't belong to a vertex
     } else {
       //track not to be removed. Add back the track to the vertex collection
       outputVertex->addTrackAtVertex(vtx->trackParticleLinks()[itrk], vtx->trackWeight(itrk));
       if (vtx->vxTrackAtVertexAvailable()) {
	 outputVertex->vxTrackAtVertex().push_back(vtx->vxTrackAtVertex()[itrk]);//will clone everything inside -> output vertex owns all the memory
       }
     }
   }  
   return outputVertex;

 }



 const ImpactParametersAndSigma  * TrackToVertexIPEstimator::estimate(const xAOD::TrackParticle * track,
                                                                      const xAOD::Vertex* vtx)const
 {

   if(track && vtx ){
     return estimate( &(track->perigeeParameters()), vtx);
   }
   return nullptr;

 }

 const ImpactParametersAndSigma  * TrackToVertexIPEstimator::estimate(const TrackParameters * track,
                                                                      const xAOD::Vertex* vtx)const
 {

   if(track && vtx ){
     return calculate( track , *vtx);
   }
   return nullptr;

 }

}//end of namespace definitions
