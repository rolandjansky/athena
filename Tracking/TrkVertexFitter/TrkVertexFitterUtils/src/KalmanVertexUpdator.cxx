/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
 {}
 
 StatusCode KalmanVertexUpdator::initialize()
 {
  StatusCode sc = AlgTool::initialize();
  if(sc.isFailure())
  {
   msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endreq;
   return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
 }

 StatusCode KalmanVertexUpdator::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endreq;
  return StatusCode::SUCCESS;
 }

 
 xAOD::Vertex * KalmanVertexUpdator:: add(xAOD::Vertex& vtx, VxTrackAtVertex& trk) const
 {
   return update(vtx,trk,1);
 }

 
 xAOD::Vertex * KalmanVertexUpdator::remove(xAOD::Vertex& vtx,  VxTrackAtVertex& trk) const
 {
  return update(vtx,trk,-1);
 }
 

 xAOD::Vertex * KalmanVertexUpdator::update(xAOD::Vertex& vtx, const VxTrackAtVertex& trk, int sign) const
 {

   //getting tracks at vertex
   std::vector<Trk::VxTrackAtVertex>* tracksAtVertex( &vtx.vxTrackAtVertex() );
   int tr_size = tracksAtVertex->size();

   //in case one wants to remove the track, one should be sure that this track
   //is already fitted to the vertex and the number of tracks at vertex is not 0
   if( sign < 0 && tr_size == 0 )
   {
     msg(MSG::ERROR)  << "No tracks fitted to this vertex yet: can not remove any" << endreq;
     msg(MSG::ERROR)  << "The copy of initial VxCandidate returned" << endreq;
     return new xAOD::Vertex( vtx );
   }

   //trying to find requested track
   std::vector<Trk::VxTrackAtVertex>::iterator lintracksBegin  = tracksAtVertex->begin();
   std::vector<Trk::VxTrackAtVertex>::iterator lintracksEnd    = tracksAtVertex->end();

   std::vector<Trk::VxTrackAtVertex>::iterator righttrack = lintracksBegin;

   //to be replaced with faster/smarter search
   bool found(false);
   for (std::vector<Trk::VxTrackAtVertex>::iterator iter=lintracksBegin;iter!=lintracksEnd;++iter)
   {
     if (trk == *iter)
     {
       found = true;
       righttrack = iter;
       break;
     }
   }//end for loop

   if (!found)
   {
     ATH_MSG_VERBOSE ("The track requested for removal or adding is not found in the vector of tracks");
     if (sign<0) {
       ATH_MSG_ERROR ("During remove track has to be already attached to the vertex");
       msg(MSG::ERROR)  << "The copy of initial xAOD::Vertex returned" << endreq;
       return &vtx;
     }

     tracksAtVertex->push_back( trk ); // tracksAtVertex stores the objects themselves in xAOD EDM and not pointers to them - this simply calls the copy constructor
     righttrack = tracksAtVertex->end()-1; // vector::end() returns an iterator referring to the "past-the-end" element and thus does not point to any element!
     ATH_MSG_VERBOSE ("Updating vertex with new track which is still not attached to vertex. Adding it before updating...");
   }

   //all safe, call the position update
   // chi and ndf
   // track weight and old vertex

   double trkWeight = trk.weight();
   //  std::cout<< "Updator::update Weight is: " << trkWeight <<std::endl;

   //const xAOD::Vertex & old_vrt = xAOD::Vertex(vtx); // this also copies track info from vtx which old_vrt doesn't need...oh well

   // ATH_MSG_DEBUG ("old vertex is: " << old_vrt); //TODO: operator << not defined for xAOD::Vertex

   xAOD::Vertex fit_vrt = positionUpdate( vtx, trk.linState(), trkWeight, sign);

   // ATH_MSG_DEBUG ("updated vertex is: " << fit_vrt); //TODO: operator << not defined for xAOD::Vertex

   double chi2 = vtx.chiSquared();
   double trk_chi = trackParametersChi2( fit_vrt, trk.linState() );

   //  std::cout << "Track parameters chi2 " << trk_chi << std::endl;
   //  std::cout << "Vertex-related chi2 " << vertexPositionChi2( old_vrt, fit_vrt ) <<std::endl;

   chi2 += sign * (vertexPositionChi2(vtx, fit_vrt) + trkWeight * trk_chi);

   //  std::cout << "Resulting chi2 increment " << sign * (vertexPositionChi2(old_vrt, fit_vrt) + trkWeight * trk_chi) << std::endl;

   //number of degrees of freedom
   double ndf = vtx.numberDoF();
   ndf +=  sign * trkWeight * (2.0);

   //  std::cout << "Updator NDF in calculation is: " << ndf << std::endl;
   //  std::cout << "Updator Sign in calculation is: " << sign << std::endl;
   //  std::cout << "Updator TrkWeight in calculation is: " << trkWeight << std::endl;

   //forming a final xAOD::Vertex
   vtx.setPosition( fit_vrt.position() );
   vtx.setCovariancePosition( fit_vrt.covariancePosition() );
   vtx.setFitQuality( chi2, ndf );

   //playing with vector of fitted tracks and compatibility
   if( sign > 0 )
   {
     //changes towards fit quality
     righttrack->setWeight( trkWeight );
     righttrack->setTrackQuality( Trk::FitQuality(trk_chi, 2 * trkWeight) );
   }

   if( sign < 0 )
   {
     tracksAtVertex->erase( righttrack );
   }

   //ATH_MSG_VERBOSE ("final vtx "  <<  vtx ); //TODO: operator << not defined for xAOD::Vertex

   return &vtx;
 }//end of update method

 //actual method where the position update happens
 xAOD::Vertex KalmanVertexUpdator::positionUpdate(const xAOD::Vertex& vtx, const LinearizedTrack * trk, double trackWeight,int sign) const
 {
   //  std::cout<<"Position update called " <<std::endl;
   //  std::cout<<"Position update: position Jacobian: "<<trk->positionJacobian()<<std::endl;
   //  std::cout<<"Position update: momentum Jacobian: "<<trk->momentumJacobian()<<std::endl;
   //  std::cout<<"Expected parameters: "<<trk->expectedParametersAtPCA()<<std::endl; 

   // linearized track information
   const AmgMatrix(5,3)& A = trk->positionJacobian();
   const AmgMatrix(5,3)& B = trk->momentumJacobian();
   const AmgVector(5) & trackParameters = trk->expectedParametersAtPCA();
   const AmgVector(5) & constantTerm = trk->constantTerm();
   const AmgSymMatrix(5) & trackParametersWeight  = trk->expectedWeightAtPCA();
   // const double trackWeight = trk->weight();

   //vertex to be updated
   const Amg::Vector3D & old_pos = vtx.position();
   const AmgSymMatrix(3)& old_vrt_weight = vtx.covariancePosition().inverse();
   ATH_MSG_VERBOSE ("Old vertex weight " << old_vrt_weight);

   //making the intermediate quantities:
   //W_k = (B^T*G*B)^(-1)
   //  std::cout << "B matrix calculated: " << B << std::endl;

   AmgSymMatrix(3) S = B.transpose()*(trackParametersWeight*B);
   //  std::cout << "S matrix calculated " << S << std::endl;
   S = S.inverse().eval();

   //  std::cout << "Main inversion passed successfully" << std::endl;
   //  std::cout << "S matrix inverted" << S << std::endl;
   //G_b = G_k - G_k*B_k*W_k*B_k^(T)*G_k
   AmgSymMatrix(5) gB = trackParametersWeight - trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose();
   //  std::cout << "Gain factor obtained: " << trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose() << std::endl;
   //  std::cout << "Resulting Gain Matrix: " << gB << std::endl;

   //new vertex weight matrix
   AmgSymMatrix(3) new_vrt_weight_later_cov = old_vrt_weight + trackWeight * sign * A.transpose()*(gB*A);
   //  std::cout << "Track weight: " << trackWeight << std::endl;
   //  std::cout << "Gain similarity: " << A.transpose()*(gB*A) << std::endl;

   //  std::cout << "New vertex weight obtained: " << new_vrt_weight << std::endl;
   
   new_vrt_weight_later_cov = new_vrt_weight_later_cov.inverse().eval();

   //  std::cout << "New vertex covariance obtained: " << new_vrt_cov << std::endl;
   //new vertex position
   Amg::Vector3D new_vrt_position =  new_vrt_weight_later_cov*(old_vrt_weight * old_pos + trackWeight * sign * A.transpose() * gB *(trackParameters - constantTerm) );
   //  std::cout << "New vertex position obtained: " << new_vrt_position << std::endl;

   // return a vertex which has an updated position and covariance but with no tracks
   xAOD::Vertex r_vtx;
   r_vtx.makePrivateStore();
   // r_vtx was a RecVertex before vertex EDM migration and instantiated using RecVertex(pos,cov)
   r_vtx.setPosition( new_vrt_position );
   r_vtx.setCovariancePosition( new_vrt_weight_later_cov );
   r_vtx.setFitQuality( vtx.chiSquared(), vtx.numberDoF() );
   ATH_MSG_VERBOSE ( "Maths done, returning a valid xAOD::Vertex.");

   return r_vtx;
 }//end of position update method

 //xAOD chi2 increment method 
 float KalmanVertexUpdator::trackParametersChi2(const xAOD::Vertex& new_vtx, const LinearizedTrack * trk) const
 {

   const Amg::Vector3D & new_vrt_pos  = new_vtx.position();

   // track information
   const AmgMatrix(5,3)& A = trk->positionJacobian();
   const AmgMatrix(5,3) & B = trk->momentumJacobian();
   const AmgVector(5) & trkParameters = trk->expectedParametersAtPCA();
   const AmgSymMatrix(5) & trkParametersWeight = trk->expectedWeightAtPCA();

   //calculation of S matrix
   AmgSymMatrix(3) Sm = B.transpose()*(trkParametersWeight*B);

   Sm = Sm.inverse().eval();
   AmgVector(5) theResidual = trk->constantTerm();

   //refitted track momentum
   Amg::Vector3D newTrackMomentum = Sm*B.transpose()*trkParametersWeight*(trkParameters - theResidual - A*new_vrt_pos);

   //refitted track parameters
   AmgVector(5) refTrackParameters = theResidual + A * new_vrt_pos + B * newTrackMomentum;

   //parameters difference
   AmgVector(5) paramDifference = trkParameters - refTrackParameters;

   double chi2 = paramDifference.transpose() * ( trkParametersWeight * paramDifference );

   //here the momentum part of the chi2 is calculated;
   //making the vertex position chi2

   return chi2;

 }//end of chi2 increment method

 float KalmanVertexUpdator::vertexPositionChi2(const xAOD::Vertex& old_vtx, const xAOD::Vertex& new_vtx) const
 {
   AmgSymMatrix(3)  old_wrt_weight = old_vtx.covariancePosition().inverse();
   Amg::Vector3D posDifference = new_vtx.position() - old_vtx.position();
   float chi2 = posDifference.transpose()*(old_wrt_weight*posDifference);
   return chi2;
 }//end of vertex position chi2
 
}//end of namespace definition
