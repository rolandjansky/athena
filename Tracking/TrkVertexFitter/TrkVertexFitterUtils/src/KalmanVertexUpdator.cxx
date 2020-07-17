/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "xAODTracking/Vertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkVertexFitterUtils/KalmanVertexUpdator.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Trk{

 KalmanVertexUpdator::KalmanVertexUpdator(const std::string& t, const std::string& n, const IInterface*  p):
                                                                       AthAlgTool(t,n,p)
 {
  declareInterface<IVertexUpdator>(this);
 }

 KalmanVertexUpdator::~KalmanVertexUpdator()
 = default;
 
 StatusCode KalmanVertexUpdator::initialize()
 {
  StatusCode sc = AlgTool::initialize();
  if(sc.isFailure())
  {
   msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endmsg;
   return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
 }

 StatusCode KalmanVertexUpdator::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
 }

 
 xAOD::Vertex * KalmanVertexUpdator:: add(xAOD::Vertex& vtx, VxTrackAtVertex& trk) const
 {
   return update(vtx,trk,IVertexUpdator::addTrack);
 }

 
 xAOD::Vertex * KalmanVertexUpdator::remove(xAOD::Vertex& vtx,  VxTrackAtVertex& trk) const
 {
  return update(vtx,trk,IVertexUpdator::removeTrack);
 }
 

 xAOD::Vertex * KalmanVertexUpdator::update(xAOD::Vertex& vtx, const VxTrackAtVertex& trk, IVertexUpdator::updateMode mode) const
 {

   //getting tracks at vertex
   std::vector<Trk::VxTrackAtVertex> & tracksAtVertex = vtx.vxTrackAtVertex();

   //in case one wants to remove the track, one should be sure that this track
   //is already fitted to the vertex
   auto righttrack = std::find(tracksAtVertex.begin(), tracksAtVertex.end(), trk); 
   // if it is not part of the vertex (which includes a possible empty vertex), we either add it or give up
   // in case we are removing tracks
   if (righttrack == tracksAtVertex.end())
   {
     ATH_MSG_VERBOSE ("The track requested for removal or adding is not found in the vector of tracks");
     // removal - can not proceed
     if (mode == IVertexUpdator::removeTrack) {
       ATH_MSG_ERROR ("During remove track has to be already attached to the vertex");
       msg(MSG::ERROR)  << "The copy of initial xAOD::Vertex returned" << endmsg;
       return &vtx;
     }
     // addition - here this is expected behaviour 
     
       righttrack = tracksAtVertex.insert(tracksAtVertex.end(),trk); 
       ATH_MSG_VERBOSE ("Updating vertex with new track which is still not attached to vertex. Adding it before updating...");
     
   }

   //all safe, call the position update
   // chi and ndf
   // track weight and old vertex

   double trkWeight = trk.weight();

   const IVertexUpdator::positionUpdateOutcome & fit_vrt = positionUpdate( vtx, trk.linState(), trkWeight, mode);

   double chi2 = vtx.chiSquared();
   double trk_chi = trackParametersChi2( fit_vrt, trk.linState() );

   const int sign = (mode == IVertexUpdator::addTrack ? 1 : -1); 
   chi2 += sign * (vertexPositionChi2(vtx, fit_vrt) + trkWeight * trk_chi);

   //number of degrees of freedom
   double ndf = vtx.numberDoF();
   ndf +=  sign * trkWeight * (2.0);

   //forming a final xAOD::Vertex
   vtx.setPosition( fit_vrt.position );
   vtx.setCovariancePosition( fit_vrt.covariancePosition );
   vtx.setFitQuality( chi2, ndf );

   //playing with vector of fitted tracks and compatibility
   if( mode == IVertexUpdator::addTrack )
   {
     //changes towards fit quality
     righttrack->setWeight( trkWeight );
     righttrack->setTrackQuality( Trk::FitQuality(trk_chi, 2 * trkWeight) );
   }
   else
   {
     tracksAtVertex.erase( righttrack );
   }

   return &vtx;
 }//end of update method

 //actual method where the position update happens
 IVertexUpdator::positionUpdateOutcome KalmanVertexUpdator::positionUpdate(const xAOD::Vertex& vtx, const LinearizedTrack * trk, double trackWeight,IVertexUpdator::updateMode mode) const
 {
   // linearized track information
   const AmgMatrix(5,3)& A = trk->positionJacobian();
   const AmgMatrix(5,3)& B = trk->momentumJacobian();
   const AmgVector(5) & trackParameters = trk->expectedParametersAtPCA();
   const AmgVector(5) & constantTerm = trk->constantTerm();
   const AmgSymMatrix(5) & trackParametersWeight  = trk->expectedWeightAtPCA();

   int sign = (mode == IVertexUpdator::addTrack ? 1 : -1); 

   //vertex to be updated
   const Amg::Vector3D & old_pos = vtx.position();
   const AmgSymMatrix(3)& old_vrt_weight = vtx.covariancePosition().inverse();
   ATH_MSG_VERBOSE ("Old vertex weight " << old_vrt_weight);

   //making the intermediate quantities:
   //W_k = (B^T*G*B)^(-1)

   AmgSymMatrix(3) S = B.transpose()*(trackParametersWeight*B);
   S = S.inverse().eval();

   //G_b = G_k - G_k*B_k*W_k*B_k^(T)*G_k
   AmgSymMatrix(5) gB = trackParametersWeight - trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose();

   //new vertex weight matrix
   AmgSymMatrix(3) new_vrt_weight_later_cov = old_vrt_weight + trackWeight * sign * A.transpose()*(gB*A);
   
   new_vrt_weight_later_cov = new_vrt_weight_later_cov.inverse().eval();

   //new vertex position
   Amg::Vector3D new_vrt_position =  new_vrt_weight_later_cov*(old_vrt_weight * old_pos + trackWeight * sign * A.transpose() * gB *(trackParameters - constantTerm) );

   return IVertexUpdator::positionUpdateOutcome{new_vrt_position, new_vrt_weight_later_cov,trackParametersWeight}; 
 }//end of position update method


 //xAOD chi2 increment method 
 float KalmanVertexUpdator::trackParametersChi2(const xAOD::Vertex& new_vtx, const LinearizedTrack * trk) const
 {
    return trackParametersChi2(new_vtx.position(), trk, trk->expectedWeightAtPCA()); 
 }//end of chi2 increment method

 float KalmanVertexUpdator::vertexPositionChi2(const xAOD::Vertex& old_vtx, const xAOD::Vertex& new_vtx) const
 {
    return vertexPositionChi2(old_vtx, new_vtx.position());
 }//end of vertex position chi2


 float KalmanVertexUpdator::trackParametersChi2(const Amg::Vector3D & new_position, const LinearizedTrack * trk,const AmgSymMatrix(5) & trkParametersWeight) const{

   // track information
     const AmgMatrix(5,3)& A = trk->positionJacobian();
     const AmgMatrix(5,3) & B = trk->momentumJacobian();
     const AmgVector(5) & trkParameters = trk->expectedParametersAtPCA();

   //calculation of S matrix
     AmgSymMatrix(3) Sm = B.transpose()*(trkParametersWeight*B);

     Sm = Sm.inverse().eval();
     const AmgVector(5)& theResidual = trk->constantTerm();

   //refitted track momentum
     Amg::Vector3D newTrackMomentum = Sm*B.transpose()*trkParametersWeight*(trkParameters - theResidual - A*new_position);

   //refitted track parameters
     AmgVector(5) refTrackParameters = theResidual + A * new_position + B * newTrackMomentum;

   //parameters difference
     AmgVector(5) paramDifference = trkParameters - refTrackParameters;

     return paramDifference.transpose() * ( trkParametersWeight * paramDifference );;

 }
 float KalmanVertexUpdator::vertexPositionChi2(const xAOD::Vertex& old_vtx, const Amg::Vector3D & new_position) const{
     AmgSymMatrix(3)  old_wrt_weight = old_vtx.covariancePosition().inverse();
     Amg::Vector3D posDifference = new_position - old_vtx.position();
     return posDifference.transpose()*(old_wrt_weight*posDifference);
 }

 float KalmanVertexUpdator::trackParametersChi2(const IVertexUpdator::positionUpdateOutcome& new_vtx, const LinearizedTrack * trk) const {
    return trackParametersChi2(new_vtx.position, trk, new_vtx.trkParametersWeight); 
 }//end of chi2 increment method

 float KalmanVertexUpdator::vertexPositionChi2(const xAOD::Vertex& old_vtx, const IVertexUpdator::positionUpdateOutcome& new_vtx) const {
    return vertexPositionChi2(old_vtx, new_vtx.position); 
 }//end of vertex position chi2
 
}//end of namespace definition
