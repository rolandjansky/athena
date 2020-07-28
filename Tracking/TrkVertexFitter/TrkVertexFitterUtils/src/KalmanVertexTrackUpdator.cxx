/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitterUtils/KalmanVertexTrackUpdator.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Trk
{
 StatusCode KalmanVertexTrackUpdator::initialize()
 {
//uploading the corresponding tools
//updator 
  if ( m_Updator.retrieve().isFailure() ) 
  {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_Updator);
    return StatusCode::FAILURE;
  } 
    ATH_MSG_INFO("Retrieved tool " << m_Updator);
  
  
  return StatusCode::SUCCESS;
 }
 
 StatusCode KalmanVertexTrackUpdator::finalize()
 {
  ATH_MSG_INFO("Finalize successful");
  return StatusCode::SUCCESS;
 }
 
 KalmanVertexTrackUpdator::KalmanVertexTrackUpdator(const std::string& t, const std::string& n, const IInterface*  p):
 AthAlgTool(t,n,p),m_Updator("Trk::KalmanVertexUpdator", this),m_maxWeight(0.001)
 { 
  declareProperty("MaximalWeight",m_maxWeight);

//updator-related stuff  
  declareProperty("VertexUpdator",m_Updator);

  declareInterface<IVertexTrackUpdator>(this);
 }
 
 KalmanVertexTrackUpdator::~KalmanVertexTrackUpdator()
 = default;
 
 void KalmanVertexTrackUpdator::update(VxTrackAtVertex& trk, const xAOD::Vertex& vtx) const
 {
   const Amg::Vector3D & fit_vrt_pos  = vtx.position();
   Trk::LinearizedTrack * linTrack = trk.linState();
 
   //protection check
   if(linTrack == nullptr)
   {
     if(trk.weight() > m_maxWeight)
     {
       //actual error case
       msg(MSG::INFO)  << "The VxTrackAtVertex passed does not have a Linearized Track" << endmsg;
       msg(MSG::INFO)  << "Please produce one with corresponding LinearizedTrackFactory"<< endmsg;
       msg(MSG::INFO)  << "The VxTrackAtVertex returned not refitted"<< endmsg;
       //    std::cout<<"Weight of the track: "<<trk.weight()<<std::endl;
     }else{
       if (msgLvl(MSG::DEBUG))
       {
         msg(MSG::DEBUG) << "The VxTrackAtVertex passed does not have a Linearized Track" << endmsg;
         msg(MSG::DEBUG) << "However the weight of this track is less than " << m_maxWeight << endmsg;
         msg(MSG::DEBUG)  << "The VxTrackAtVertex returned not refitted"<< endmsg;
       }
     }//end of absent linTrack case

     return;
   }//end of no lintrack check

   //getting Jacobians and track information
   const AmgMatrix(5,3) & A = linTrack->positionJacobian();
   const AmgMatrix(5,3) & B = linTrack->momentumJacobian();
   const AmgVector(5) & trkParameters = linTrack->expectedParametersAtPCA();
   //  std::cout << "Perigee before refit " << trkParameters << std::endl;
   //  std::cout << "Track covariance before refit " << linTrack->expectedPerigeeCovarianceAtPCA() << std::endl;

   const AmgSymMatrix(5) & trkParametersWeight = linTrack->expectedWeightAtPCA();
   
   //calculation of S matrix
   AmgSymMatrix(3) Sm = B.transpose()*(trkParametersWeight*B);

   if (Sm.determinant() == 0.0) {
     ATH_MSG_WARNING("Matrix can not be inverted, new type of check as part of Eigen, please monitor."
                     << endmsg << "Matrix Sm = " << Sm);
     return;
   }
   Sm = Sm.inverse().eval();
   const AmgVector(5) & theResidual = linTrack->constantTerm();

   //refitted track momentum
   Amg::Vector3D newTrackMomentum = Sm*B.transpose() * trkParametersWeight
    * (trkParameters - theResidual - A*fit_vrt_pos);

   //refitted track parameters
   AmgVector(5) refTrkPar; refTrkPar.setZero();
   refTrkPar(Trk::phi)   = newTrackMomentum(0);//phi
   refTrkPar(Trk::theta) = newTrackMomentum(1);//theta
   refTrkPar(Trk::qOverP)= newTrackMomentum(2);//qoverP

   //bring parameter phi again in the limits, if 
   //phi is slightly outside (as can be expected 
   //due to refitting)
   //  if (refTrkPar[2]<-M_PI) {
   //    refTrkPar[2] = refTrkPar[2]+M_PI;
   //  } else if (refTrkPar[2]>M_PI) {
   //    refTrkPar[2] = refTrkPar[2]-M_PI;
   //  }

   if (refTrkPar[Trk::phi0] > M_PI)
   {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "-U- phi = " << refTrkPar[Trk::phi0];
    refTrkPar[Trk::phi0] = fmod(refTrkPar[Trk::phi0]+M_PI,2*M_PI)-M_PI;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " out of range, now "
        << "corrected to " << refTrkPar[Trk::phi0] << endmsg;
   } else if(refTrkPar[Trk::phi0]<-M_PI) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "-U- phi = " << refTrkPar[Trk::phi0];
    refTrkPar[Trk::phi0] = fmod(refTrkPar[Trk::phi0]-M_PI,2*M_PI)+M_PI;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " out of range, now "
        << "corrected to " << refTrkPar[Trk::phi0] << endmsg;
   }

   if (refTrkPar[Trk::theta] > M_PI)
   {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " Theta out of range: correcting theta: " << refTrkPar[Trk::theta];
     refTrkPar[Trk::theta]=fmod(refTrkPar[Trk::theta]+M_PI,2*M_PI)-M_PI;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " to: " << refTrkPar[Trk::theta] << endmsg;
   }
   else if (refTrkPar[Trk::theta]<0)
   {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " Theta out of range: correcting theta: " << refTrkPar[Trk::theta];
     refTrkPar[Trk::theta]=fmod(refTrkPar[Trk::theta]-M_PI,2*M_PI)+M_PI;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " to: " << refTrkPar[Trk::theta] << endmsg;
   }

   if (refTrkPar[Trk::theta] < 0)
   {
     refTrkPar[Trk::theta] =
       - refTrkPar[Trk::theta];
     refTrkPar[Trk::phi0] +=
       (refTrkPar[Trk::phi0] > 0 ? -M_PI : M_PI);
   } else if (refTrkPar[Trk::theta] > M_PI) {//this in principle should not be needed anymore
     refTrkPar[Trk::theta] =
       2*M_PI - refTrkPar[Trk::theta];
     refTrkPar[Trk::phi0] +=
       (refTrkPar[Trk::phi0] > 0 ? -M_PI : M_PI);
   }

   //temporary hack to check the consistence of the frame
   //  std::cout<<"New track parameters "<<refTrkPar<<std::endl;

   //vertex covariance and weight
   const AmgSymMatrix(3)& vrt_cov = vtx.covariancePosition();
   if (vrt_cov.determinant() == 0.0) {
     ATH_MSG_WARNING("Matrix can not be inverted, new type of check as part of Eigen, please monitor."
                     << endmsg << "Matrix vrt_cov = " << vrt_cov);
     return;
   }
   const AmgSymMatrix(3) vrt_weight = vtx.covariancePosition().inverse();

   //making a new track covariance matrix
   AmgSymMatrix(3) posMomentumCovariance =  -vrt_cov * A.transpose() * trkParametersWeight * B *Sm;

   //--------------------------------------------------------------------------------------------------------
   //correction to what was forgotten before: making the smoothed chi2 of track
   //this operation invokes removing of the track from the vertex
   const IVertexUpdator::positionUpdateOutcome & reducedVrt = m_Updator->positionUpdate( vtx, linTrack, trk.weight(), IVertexUpdator::removeTrack );

   const AmgSymMatrix(3) reduced_vrt_weight = reducedVrt.covariancePosition.inverse();
   //  const CLHEP::HepSymMatrix & reduced_vrt_cov = reducedVrt.covariancePosition();

   Amg::Vector3D posDifference = vtx.position() - reducedVrt.position;

   //now making the  second part of the chi2 calculation: smoothed parameters
   AmgVector(5) smRes = trkParameters - (theResidual + A*vtx.position() + B*newTrackMomentum);

   double chi2 = posDifference.dot(reduced_vrt_weight*posDifference) + smRes.dot(trkParametersWeight*smRes);

   //  std::cout<<"Smoothed chi2 in the fit "<<chi2<<std::endl;
   //-------------------------------------------------------------------------------------
   //new momentum covariance
   AmgSymMatrix(3) momentumCovariance = Sm + posMomentumCovariance.transpose()*(vrt_weight*posMomentumCovariance);

   //full (x,y,z,phi, theta, q/p) covariance matrix
   AmgSymMatrix (6) fullTrkCov;
   fullTrkCov.setZero();

   fullTrkCov.block<3,3>(0,0) = vrt_cov;
   fullTrkCov.block<3,3>(0,3) = posMomentumCovariance;
   fullTrkCov.block<3,3>(3,0) = posMomentumCovariance.transpose();
   fullTrkCov.block<3,3>(3,3) = momentumCovariance;

   //debug hacking   
   AmgMatrix(5,6) trJac;
   trJac.setZero();
   trJac(4,5) = 1.;
   trJac(3,4) = 1.;
   trJac(2,3) = 1.;

   //first row
   trJac(0,0) = - sin(refTrkPar[2]);
   trJac(0,1) =   cos(refTrkPar[2]);

   //second row
   trJac(1,0) = -cos(refTrkPar[2])/tan(refTrkPar[3]);
   trJac(1,1) = -sin(refTrkPar[2])/tan(refTrkPar[3]);
   trJac(1,2) = 1.;

   AmgSymMatrix(5)* perFullTrkCov = new AmgSymMatrix(5)(trJac*(fullTrkCov*trJac.transpose()));

   //temporary hack: returning the state with incorrect covariance matrix, to see
   //how general infrastructure works.


   Trk::Perigee  * refittedPerigee = new Trk::Perigee(refTrkPar[0],
                                                      refTrkPar[1],
                                                      refTrkPar[2],
                                                      refTrkPar[3],
                                                      refTrkPar[4],
                                                      fit_vrt_pos,
                                                      perFullTrkCov);

   trk.setPerigeeAtVertex(refittedPerigee);
   const FitQuality trkQuality(chi2,2*trk.weight());
   //  std::cout<<"Tracks at vertex are assigned with chi2 "<<trkQuality.chiSquared()<<std::endl;
   //  std::cout<<"Tracks at vertex are assigned with  ndf "<<trkQuality.numberDoF()<<std::endl;
   trk.setTrackQuality(trkQuality);

 }//end of update method

}//end of namespace definitions 
