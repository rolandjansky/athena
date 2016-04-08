/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
 NeutralParticleParameterCalculator.cxx - Description in header file
*********************************************************************/

#include "NeutralParticleParameterCalculator.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkEventPrimitives/JacobianPxyzToPhiThetaQoverPspherical.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"


// #define IMPACTPOINT3DESTIMATOR_DEBUG

namespace Trk
{

  NeutralParticleParameterCalculator::NeutralParticleParameterCalculator(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_LinearizedTrackFactory("Trk::FullLinearizedTrackFactory/FullLinearizedTrackFactory"),
    m_linearizedTrackFactoryIsAvailable(false)
  {   
    declareProperty("LinearizedTrackFactory",m_LinearizedTrackFactory);
    declareInterface<INeutralParticleParameterCalculator>(this);
  }
  
  NeutralParticleParameterCalculator::~NeutralParticleParameterCalculator() {
  }
  
  StatusCode NeutralParticleParameterCalculator::initialize() 
  { 
    if (!m_LinearizedTrackFactory.empty()) {
      StatusCode sc=m_LinearizedTrackFactory.retrieve();
      if (sc.isFailure()) {
        msg(MSG::WARNING) << "Could not find TrackLinearizer tool." << endreq;
        m_linearizedTrackFactoryIsAvailable=false;
      } else {
        m_linearizedTrackFactoryIsAvailable=true;
      }
    }

    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode NeutralParticleParameterCalculator::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }


  NeutralPerigee* NeutralParticleParameterCalculator::createNeutralTrackFromVertex
    (const Trk::VxCandidate & myVxCandidate) const
  { 
    const std::vector<Trk::VxTrackAtVertex*> * myTrackVectorPtr=myVxCandidate.vxTrackAtVertex();

    if (myTrackVectorPtr==0) 
    {
      msg(MSG::WARNING) << " 0 pointer vector of Tracks in VxCandidate. No Neutral Perigee could be created" << endreq;
      return 0;
    }

    if (myTrackVectorPtr->size()!=2) 
    {
      msg(MSG::WARNING) <<  " More or less than 2 tracks in VxCandidate.  No Neutral Perigee could be created" << endreq;
      return 0;
    }

    const Trk::VxTrackAtVertex * myFirstTrack=*(myTrackVectorPtr->begin());
    const Trk::VxTrackAtVertex * mySecondTrack=*(++myTrackVectorPtr->begin());

    const Trk::Perigee * myFirstPerigee  = dynamic_cast<const Trk::Perigee*>(myFirstTrack->perigeeAtVertex());
    
    const Trk::Perigee * mySecondPerigee = dynamic_cast<const Trk::Perigee*>(mySecondTrack->perigeeAtVertex());

    if (myFirstPerigee==0||mySecondPerigee==0) {
      msg(MSG::WARNING) <<  " some parameters at vertex are null pointers - malformed object!" << endreq;
      return 0;
    }

    const Trk::RecVertex & myVertex=myVxCandidate.recVertex();
    AmgMatrix(3,3) vrt_cov;
    AmgMatrix(3,3) vrt_weight;

    const Trk::ExtendedVxCandidate* extendedCandidate=
        dynamic_cast<const Trk::ExtendedVxCandidate*>(&myVxCandidate);

    if (extendedCandidate==0)
    {
      vrt_cov=myVertex.covariancePosition();
      vrt_weight=myVertex.covariancePosition().inverse().eval();
    }
    else
    {
      vrt_cov=extendedCandidate->recVertex().covariancePosition();
      vrt_weight=extendedCandidate->recVertex().covariancePosition().inverse().eval(); // not used in have-ext-vtx case
    }
    //need to recalculate the refitted covariance matrix of the tracks (yes, we don't have it yet***REMOVED***)
    std::pair<AmgMatrix(3,3),AmgMatrix(3,3)> PosMomAndMomCovFirstTrack;
    std::pair<AmgMatrix(3,3),AmgMatrix(3,3)> PosMomAndMomCovSecondTrack;

    if (extendedCandidate==0) {

      //check if the linearizedTrackFactory is available...
      if (m_linearizedTrackFactoryIsAvailable==false) {
        msg(MSG::ERROR) << " No LinearizedTrackFactory is defined and no ExtendedVxCandidate is provided. Cannot obtain covariance matrix of neutral particle. Failed... " << endreq;
        return 0;
      }
      //don't use extended VxCandidate
      try
      {
        const Trk::LinearizedTrack * myFirstLinearizedTrack=myFirstTrack->linState();
        const Trk::LinearizedTrack * mySecondLinearizedTrack=mySecondTrack->linState();
        if (myFirstLinearizedTrack==0||mySecondLinearizedTrack==0)
        {
          msg(MSG::WARNING) <<  " Could not find one of the linearized tracks.  No Neutral Perigee could be created" << endreq;
          return 0;
        }

        PosMomAndMomCovFirstTrack=getPosMomentumAndMomentumCovMatrix(myFirstLinearizedTrack,
                                                                     vrt_cov,
                                                                     vrt_weight);

        PosMomAndMomCovSecondTrack=getPosMomentumAndMomentumCovMatrix(mySecondLinearizedTrack,
                                                                      vrt_cov,
                                                                      vrt_weight);
      }
      catch (...)
      {
        msg(MSG::WARNING) <<  " getPosMomentumAndMomentumCovMatrix method failed " << endreq;
        return 0;
      }
    } else {

      if (extendedCandidate->fullCovariance()==0)
      {
        msg(MSG::WARNING) << "Extended VxCandidate has a NULL full covariance matrix. Cannot proceed... " << endreq;
        return 0;
      }
      //use extended VxCandidate
      AmgMatrix(3,3) firstMixed; firstMixed.setZero();
      AmgMatrix(3,3) secondMixed; secondMixed.setZero();

      for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++)
          firstMixed(i,j)=(*extendedCandidate->fullCovariance())(i,j+3);
      }

      for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++)
          secondMixed(i,j)=(*extendedCandidate->fullCovariance())(i,j+6);
      }

      PosMomAndMomCovFirstTrack.first=firstMixed;
      PosMomAndMomCovFirstTrack.second=
          extendedCandidate->fullCovariance()->block<3,3>(3,3); // was ClHEP::SymMatrix.sub(4,6);
      PosMomAndMomCovSecondTrack.first=secondMixed;
      PosMomAndMomCovSecondTrack.second=
          extendedCandidate->fullCovariance()->block<3,3>(6,6); // was CLHEP::SymMatrix.sub(7,9);

    }

    const AmgVector(5) & myFirstPerigeeParameters=myFirstPerigee->parameters();
    const AmgVector(5) & mySecondPerigeeParameters=mySecondPerigee->parameters();

    AmgMatrix(3,3) FirstJacobianToPxPyPz=getPhiThetaQOverPToPxPyPzJacobian(myFirstPerigeeParameters[Trk::qOverP],
                                                                           myFirstPerigeeParameters[Trk::theta],
                                                                           myFirstPerigeeParameters[Trk::phi0]);

    AmgMatrix(3,3) SecondJacobianToPxPyPz=getPhiThetaQOverPToPxPyPzJacobian(mySecondPerigeeParameters[Trk::qOverP],
                                                                            mySecondPerigeeParameters[Trk::theta],
                                                                            mySecondPerigeeParameters[Trk::phi0]);

   /*
    if (msgLvl(MSG::VERBOSE)) msg() << std::setprecision(10) << "1overp 1st track: " << 1./myFirstPerigeeParameters[Trk::qOverP] <<
        " error " <<
        1./myFirstPerigeeParameters[Trk::qOverP]
        /myFirstPerigeeParameters[Trk::qOverP]
        *sqrt((*myFirstPerigee->covariance())(Trk::qOverP,Trk::qOverP) << endreq;
    if (msgLvl(MSG::VERBOSE)) msg() << "1overp 2nd track: " << 1./mySecondPerigeeParameters[Trk::qOverP] << " error " <<
        1./mySecondPerigeeParameters[Trk::qOverP]
        /mySecondPerigeeParameters[Trk::qOverP]
        *sqrt((*mySecondPerigee->covariance())(Trk::qOverP,Trk::qOverP)) << endreq;
    */


    AmgMatrix(3,3) posMomentumCovFirst=PosMomAndMomCovFirstTrack.first*FirstJacobianToPxPyPz.transpose();
    AmgMatrix(3,3) posMomentumCovSecond=PosMomAndMomCovSecondTrack.first*SecondJacobianToPxPyPz.transpose();

    AmgMatrix(3,3) momentumCovFirst=PosMomAndMomCovFirstTrack.second.similarity(FirstJacobianToPxPyPz);
    AmgMatrix(3,3) momentumCovSecond=PosMomAndMomCovSecondTrack.second.similarity(SecondJacobianToPxPyPz);

    //P1 P2 covariance matrix
    AmgMatrix(3,3) covP1P2; covP1P2.setZero();

    // For the case of having no ExtendedVxCandidate (and no constrained fit used)
    // now you need to reconstruct the covariance between p1 and p2
    // you can do it, since you know that the element of the weight matrix between p1 and p2 is 0!
    // (in this case p1 and p2 are correlated only due to x)
    // For the extended candidate case, get the same information from the extended covariance matrix.
    // In this case constraints can be used in the fit...

    if (extendedCandidate==0) {
      covP1P2=posMomentumCovFirst.transpose()*vrt_weight*posMomentumCovSecond;
    } else {
      for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++)
          covP1P2(i,j)=(*extendedCandidate->fullCovariance())(i+3,j+6);
      }

      covP1P2=FirstJacobianToPxPyPz*covP1P2*SecondJacobianToPxPyPz.transpose();
    }

    // no distinction between symmetric and non-symmetric square matrices in new EDM, drop checks.
    // AmgMatrix(3,3) nonSymCovNewP = momentumCovFirst+momentumCovSecond+covP1P2+covP1P2.transpose();
    AmgMatrix(3,3) covNewP = momentumCovFirst+momentumCovSecond+covP1P2+covP1P2.transpose();

    AmgMatrix(3,3) covNewXP=posMomentumCovFirst+posMomentumCovSecond;

    //now  covNewP and covNewXP are the new covariance matrices with the new momentum
    //calculate also new momentum
    Amg::Vector3D newMomentum=myFirstPerigee->momentum()+mySecondPerigee->momentum();

    ATH_MSG_VERBOSE(" new momentum " << newMomentum << " new cov Momentum " << covNewP);
    ATH_MSG_VERBOSE(" position " << myVertex.position() << " cov Pos - Momentum " << covNewXP);
    ATH_MSG_VERBOSE(" old cov position " << vrt_cov);

    //Now you need to create the new neutral tracks
    //NOT trivial, since no global representation exists...
    //now you need first to go back with the momentum (px,py,pz -> phi,theta,q/p)

    Trk::JacobianPxyzToPhiThetaQoverPspherical JacobianToPhiThetaQOverP(newMomentum.phi(),
                                                                        newMomentum.theta(),
                                                                        1./newMomentum.mag());

    AmgMatrix(3,3) finalCovNewP=covNewP.similarity(JacobianToPhiThetaQOverP);
    AmgMatrix(3,3) finalCovNewXP=covNewXP*JacobianToPhiThetaQOverP.transpose();

    double phi=newMomentum.phi();
    double theta=newMomentum.theta();
    double qOverP=1./newMomentum.mag();

    ATH_MSG_VERBOSE(" new phi: " << phi << " +/- " << sqrt(finalCovNewP(0,0)));
    ATH_MSG_VERBOSE(" new theta: " << theta << " +/- " << sqrt(finalCovNewP(1,1)));
    ATH_MSG_VERBOSE(" new P: " << 1./qOverP << " +/- " << 1./qOverP/qOverP*sqrt(finalCovNewP(2,2)));

    //now you still have 6 parameters! need to reduce them to 5!
    //
    //create the jacobian for a neutral particle

    double sin_phi_v=sin(phi);
    double cos_phi_v=cos(phi);
    double tan_th=tan(theta);

    //jacobian elements
    AmgMatrix(5,6) globalNeutralJacobian; globalNeutralJacobian.setZero();
    globalNeutralJacobian(0,0) = -sin_phi_v;
    globalNeutralJacobian(0,1) = +cos_phi_v;
    globalNeutralJacobian(1,0) = -cos_phi_v/tan_th;
    globalNeutralJacobian(1,1) = -sin_phi_v/tan_th;
    globalNeutralJacobian(1,2) = 1.;
    globalNeutralJacobian(2,3) = 1.;
    globalNeutralJacobian(3,4) = 1.;
    globalNeutralJacobian(4,5) = 1.;

    AmgMatrix(6,6) fullTrkCov; fullTrkCov.setZero();
    fullTrkCov.block<3,3>(0,3) = finalCovNewXP;             // was .sub(1,4, finalCovNewXP);
    fullTrkCov.block<3,3>(3,0) = finalCovNewXP.transpose(); // was .sub(4,1, finalCovNewXP.T());
    fullTrkCov.block<3,3>(0,0) = vrt_cov;                   // was .sub(1,1, vrt_cov);
    fullTrkCov.block<3,3>(3,3) = finalCovNewP;              // was .sub(4,4, finalCovNewP);

    // no mechanism in new EDM: CLHEP::HepSymMatrix symFullTrkCov;
    // no mechanism in new EDM: symFullTrkCov.assign(fullTrkCov);

    Trk::PerigeeSurface perSurface(myVertex.position());
    AmgSymMatrix(5)* covNeutral = new AmgSymMatrix(5)(globalNeutralJacobian*fullTrkCov*globalNeutralJacobian.transpose());

    return new Trk::NeutralPerigee(0,0,phi,theta,qOverP,perSurface,covNeutral);
  }
  
  NeutralPerigee* NeutralParticleParameterCalculator::createNeutralTrackFromVertex
    (const xAOD::Vertex & myVxCandidate) const
  { 

    if (myVxCandidate.nTrackParticles()!=2) 
    {
      msg(MSG::WARNING) <<  " More or less than 2 tracks in VxCandidate.  No Neutral Perigee could be created" << endreq;
      return 0;
    }

    if ( !myVxCandidate.trackParticleLinks()[0].isValid() || !myVxCandidate.trackParticleLinks()[1].isValid() )
    {
      msg(MSG::WARNING) <<  " Track Particle Element link is not valid" << endreq;
      return 0;
  
    }
    const xAOD::TrackParticle * myFirstTrack= myVxCandidate.trackParticle( 0 );
    const xAOD::TrackParticle * mySecondTrack=myVxCandidate.trackParticle( 1 );

    const Trk::Perigee& myFirstPerigee  = myFirstTrack->perigeeParameters();
    const Trk::Perigee& mySecondPerigee = mySecondTrack->perigeeParameters();

    AmgMatrix(3,3) vrt_cov = myVxCandidate.covariancePosition();
    AmgMatrix(3,3) vrt_weight =  myVxCandidate.covariancePosition().inverse().eval();

    //need to recalculate the refitted covariance matrix of the tracks (yes, we don't have it yet***REMOVED***)
    std::pair<AmgMatrix(3,3),AmgMatrix(3,3)> PosMomAndMomCovFirstTrack;
    std::pair<AmgMatrix(3,3),AmgMatrix(3,3)> PosMomAndMomCovSecondTrack;


    //check if the linearizedTrackFactory is available...
    if (m_linearizedTrackFactoryIsAvailable==false) {
      msg(MSG::ERROR) << " No LinearizedTrackFactory is defined and no ExtendedVxCandidate is provided. Cannot obtain covariance matrix of neutral particle. Failed... " << endreq;
      return 0;
    }


    if(m_LinearizedTrackFactory){
    
      const Trk::LinearizedTrack * myFirstLinearizedTrack = m_LinearizedTrackFactory->linearizedTrack(&myFirstPerigee, Trk::Vertex(myVxCandidate.position() ) );
      const Trk::LinearizedTrack * mySecondLinearizedTrack= m_LinearizedTrackFactory->linearizedTrack(&mySecondPerigee, Trk::Vertex(myVxCandidate.position() ) );

      if (myFirstLinearizedTrack==0||mySecondLinearizedTrack==0)
      {
        msg(MSG::WARNING) <<  " Could not find one of the linearized tracks.  No Neutral Perigee could be created" << endreq;
        if (myFirstLinearizedTrack) delete myFirstLinearizedTrack;
        if (mySecondLinearizedTrack) delete mySecondLinearizedTrack;
        return 0;
      }

      PosMomAndMomCovFirstTrack=getPosMomentumAndMomentumCovMatrix(myFirstLinearizedTrack,
                                                                   vrt_cov,
                                                                   vrt_weight);

      PosMomAndMomCovSecondTrack=getPosMomentumAndMomentumCovMatrix(mySecondLinearizedTrack,
                                                                    vrt_cov,
                                                                    vrt_weight);
                                                                    
      delete myFirstLinearizedTrack;
      delete mySecondLinearizedTrack;                                                                    
    } else {
      msg(MSG::WARNING) <<  " getPosMomentumAndMomentumCovMatrix method failed " << endreq;
      return 0;
    }
    
    const AmgVector(5) & myFirstPerigeeParameters=myFirstPerigee.parameters();
    const AmgVector(5) & mySecondPerigeeParameters=mySecondPerigee.parameters();

    AmgMatrix(3,3) FirstJacobianToPxPyPz=getPhiThetaQOverPToPxPyPzJacobian(myFirstPerigeeParameters[Trk::qOverP],
                                                                           myFirstPerigeeParameters[Trk::theta],
                                                                           myFirstPerigeeParameters[Trk::phi0]);

    AmgMatrix(3,3) SecondJacobianToPxPyPz=getPhiThetaQOverPToPxPyPzJacobian(mySecondPerigeeParameters[Trk::qOverP],
                                                                            mySecondPerigeeParameters[Trk::theta],
                                                                            mySecondPerigeeParameters[Trk::phi0]);
    
    
    
    AmgMatrix(3,3) posMomentumCovFirst=PosMomAndMomCovFirstTrack.first*FirstJacobianToPxPyPz.transpose();
    AmgMatrix(3,3) posMomentumCovSecond=PosMomAndMomCovSecondTrack.first*SecondJacobianToPxPyPz.transpose();

    AmgMatrix(3,3) momentumCovFirst=PosMomAndMomCovFirstTrack.second.similarity(FirstJacobianToPxPyPz);
    AmgMatrix(3,3) momentumCovSecond=PosMomAndMomCovSecondTrack.second.similarity(SecondJacobianToPxPyPz);

    
    //P1 P2 covariance matrix
    AmgMatrix(3,3) covP1P2; covP1P2.setZero();

    
    covP1P2=posMomentumCovFirst.transpose()*vrt_weight*posMomentumCovSecond;
  
    // no distinction between symmetric and non-symmetric square matrices in new EDM, drop checks.
    // AmgMatrix(3,3) nonSymCovNewP = momentumCovFirst+momentumCovSecond+covP1P2+covP1P2.transpose();
    AmgMatrix(3,3) covNewP = momentumCovFirst+momentumCovSecond+covP1P2+covP1P2.transpose();

    AmgMatrix(3,3) covNewXP=posMomentumCovFirst+posMomentumCovSecond;

    //now  covNewP and covNewXP are the new covariance matrices with the new momentum
    //calculate also new momentum
    Amg::Vector3D newMomentum=myFirstPerigee.momentum()+mySecondPerigee.momentum();

    ATH_MSG_VERBOSE(" new momentum " << newMomentum << " new cov Momentum " << covNewP);
    ATH_MSG_VERBOSE(" position " << myVxCandidate.position() << " cov Pos - Momentum " << covNewXP);
    ATH_MSG_VERBOSE(" old cov position " << vrt_cov);

    //Now you need to create the new neutral tracks
    //NOT trivial, since no global representation exists...
    //now you need first to go back with the momentum (px,py,pz -> phi,theta,q/p)

    Trk::JacobianPxyzToPhiThetaQoverPspherical JacobianToPhiThetaQOverP(newMomentum.phi(),
                                                                        newMomentum.theta(),
                                                                        1./newMomentum.mag());

    AmgMatrix(3,3) finalCovNewP=covNewP.similarity(JacobianToPhiThetaQOverP);
    AmgMatrix(3,3) finalCovNewXP=covNewXP*JacobianToPhiThetaQOverP.transpose();

    double phi=newMomentum.phi();
    double theta=newMomentum.theta();
    double qOverP=1./newMomentum.mag();

    ATH_MSG_VERBOSE(" new phi: " << phi << " +/- " << sqrt(finalCovNewP(0,0)));
    ATH_MSG_VERBOSE(" new theta: " << theta << " +/- " << sqrt(finalCovNewP(1,1)));
    ATH_MSG_VERBOSE(" new P: " << 1./qOverP << " +/- " << 1./qOverP/qOverP*sqrt(finalCovNewP(2,2)));

    //now you still have 6 parameters! need to reduce them to 5!
    //
    //create the jacobian for a neutral particle

    double sin_phi_v=sin(phi);
    double cos_phi_v=cos(phi);
    double tan_th=tan(theta);

    //jacobian elements
    AmgMatrix(5,6) globalNeutralJacobian; globalNeutralJacobian.setZero();
    globalNeutralJacobian(0,0) = -sin_phi_v;
    globalNeutralJacobian(0,1) = +cos_phi_v;
    globalNeutralJacobian(1,0) = -cos_phi_v/tan_th;
    globalNeutralJacobian(1,1) = -sin_phi_v/tan_th;
    globalNeutralJacobian(1,2) = 1.;
    globalNeutralJacobian(2,3) = 1.;
    globalNeutralJacobian(3,4) = 1.;
    globalNeutralJacobian(4,5) = 1.;

    AmgMatrix(6,6) fullTrkCov; fullTrkCov.setZero();
    fullTrkCov.block<3,3>(0,3) = finalCovNewXP;             // was .sub(1,4, finalCovNewXP);
    fullTrkCov.block<3,3>(3,0) = finalCovNewXP.transpose(); // was .sub(4,1, finalCovNewXP.T());
    fullTrkCov.block<3,3>(0,0) = vrt_cov;                   // was .sub(1,1, vrt_cov);
    fullTrkCov.block<3,3>(3,3) = finalCovNewP;              // was .sub(4,4, finalCovNewP);

    // no mechanism in new EDM: CLHEP::HepSymMatrix symFullTrkCov;
    // no mechanism in new EDM: symFullTrkCov.assign(fullTrkCov);

    Trk::PerigeeSurface perSurface(myVxCandidate.position());
    AmgSymMatrix(5)* covNeutral = new AmgSymMatrix(5)(globalNeutralJacobian*fullTrkCov*globalNeutralJacobian.transpose());

    return new Trk::NeutralPerigee(0,0,phi,theta,qOverP,perSurface,covNeutral);
  }
  
  
  

  std::pair<AmgMatrix(3,3),AmgMatrix(3,3)> NeutralParticleParameterCalculator::getPosMomentumAndMomentumCovMatrix
    (const Trk::LinearizedTrack* linTrack,
     const AmgMatrix(3,3) & vrt_cov,
     const AmgMatrix(3,3) & vrt_weight) const
  {

    const AmgMatrix(5,3) & A = linTrack->positionJacobian();
    const AmgMatrix(5,3) & B = linTrack->momentumJacobian();
    const AmgSymMatrix(5) & trkParametersWeight = linTrack->expectedWeightAtPCA();

    AmgSymMatrix(3) Sm = B.transpose()*(trkParametersWeight*B);
    // same maths, but doesn't compile: AmgSymMatrix(3) Sm = trkParametersWeight.similarityT(B);
    if (Sm.determinant() == 0.0) {
      ATH_MSG_WARNING("S matrix can not be inverted, new type of check as part of Eigen, please monitor."
                      << endreq << "Matrix Sm = " << Sm);
      ATH_MSG_WARNING("This track is returned not refitted");
      // throw std::string("Inversion of S matrix fails in track parameters refit");
    }
    Sm = Sm.inverse().eval();
    AmgMatrix(3,3) posMomentumCovariance = -vrt_cov * A.transpose() * trkParametersWeight * B *Sm;
    AmgMatrix(3,3) momentumCovariance = Sm + vrt_weight.similarityT(posMomentumCovariance);

    return std::pair<AmgMatrix(3,3),AmgMatrix(3,3)>(posMomentumCovariance,momentumCovariance);
  }


  AmgMatrix(3,3) NeutralParticleParameterCalculator::getPhiThetaQOverPToPxPyPzJacobian
      (double qOverP,double theta,double phi) const {
    AmgMatrix(3,3) transform; transform.setZero();
    transform(0,0)=-1./fabs(qOverP)*sin(theta)*sin(phi);
    transform(0,1)=1./fabs(qOverP)*sin(theta)*cos(phi);
    transform(0,2)=0.;
    transform(1,0)=1./fabs(qOverP)*cos(theta)*cos(phi);
    transform(1,1)=1./fabs(qOverP)*cos(theta)*sin(phi);
    transform(1,2)=-1./fabs(qOverP)*sin(theta);
    transform(2,0)=-1./qOverP/fabs(qOverP)*sin(theta)*cos(phi);
    transform(2,1)=-1./qOverP/fabs(qOverP)*sin(theta)*sin(phi);
    transform(2,2)=-1./qOverP/fabs(qOverP)*cos(theta);
    return transform.transpose().eval();
  }

}
