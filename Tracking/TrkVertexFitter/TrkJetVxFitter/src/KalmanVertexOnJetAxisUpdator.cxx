/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkJetVxFitter/KalmanVertexOnJetAxisUpdator.h"
#include "VxJetVertex/RecVertexPositions.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/JetVtxParamDefs.h"
#include <algorithm>
#include <utility>


//#define Updator_DEBUG
#define KalmanVertexUpdate_NEW

namespace Trk{

  namespace {
    int numRow(int numVertex) {
      return numVertex+5;
    }
  }

 KalmanVertexOnJetAxisUpdator::KalmanVertexOnJetAxisUpdator(const std::string& t, const std::string& n, const IInterface*  p):
   AthAlgTool(t,n,p),
   m_initialMomentumError(1000.)
 {
  declareInterface<KalmanVertexOnJetAxisUpdator>(this);
  declareProperty("initialMomentumError",m_initialMomentumError);
 }

 KalmanVertexOnJetAxisUpdator::~KalmanVertexOnJetAxisUpdator()
 = default;
 
 StatusCode KalmanVertexOnJetAxisUpdator::initialize()
 {
  StatusCode sc = AthAlgTool::initialize();
  if(sc.isFailure())
  {
    ATH_MSG_ERROR (" Unable to initialize the AlgTool");
   return StatusCode::FAILURE;
  }
  
  
  return StatusCode::SUCCESS;
 }

 StatusCode KalmanVertexOnJetAxisUpdator::finalize()
 {
   ATH_MSG_INFO( "Finalize successful" );
   return StatusCode::SUCCESS;
 }


  void KalmanVertexOnJetAxisUpdator::add(VxTrackAtVertex* trackToAdd,
                                         const VxVertexOnJetAxis* vertexToUpdate,
                                         VxJetCandidate* candidateToUpdate) const {
    update(trackToAdd,vertexToUpdate,candidateToUpdate,1,false);
  }


  void KalmanVertexOnJetAxisUpdator::addWithFastUpdate(VxTrackAtVertex* trackToAdd,
                                                       const VxVertexOnJetAxis* vertexToUpdate,
                                                       VxJetCandidate* candidateToUpdate) const {
    update(trackToAdd,vertexToUpdate,candidateToUpdate,1,true);
  }


  void  KalmanVertexOnJetAxisUpdator::remove(VxTrackAtVertex* trackToRemove,
                                             const VxVertexOnJetAxis* vertexToUpdate,
                                             VxJetCandidate* candidateToUpdate) const {
    update(trackToRemove,vertexToUpdate,candidateToUpdate,-1,false);
  }

  
  void  KalmanVertexOnJetAxisUpdator::removeWithFastUpdate(VxTrackAtVertex* trackToRemove,
                                                           const VxVertexOnJetAxis* vertexToUpdate,
                                                           VxJetCandidate* candidateToUpdate) const {
    update(trackToRemove,vertexToUpdate,candidateToUpdate,-1,true);
  }


  void KalmanVertexOnJetAxisUpdator::update(VxTrackAtVertex* trackToUpdate,
                                            const VxVertexOnJetAxis* vertexToUpdate,
                                            VxJetCandidate* candidateToUpdate,
                                            int sign,bool doFastUpdate) const {
    
    //check that all pointers are there...
    if (trackToUpdate==nullptr||vertexToUpdate==nullptr||candidateToUpdate==nullptr||(sign!=1&&sign!=-1)) {
      ATH_MSG_WARNING (" Empty pointers then calling fit method update. No fit will be donw...");
      return;
    }

    //getting tracks at vertex and verify if your trackToUpdate is there
    bool found=false;
    const std::vector<Trk::VxTrackAtVertex*> & tracksAtVertex(vertexToUpdate->getTracksAtVertex()); 
    
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksBegin=tracksAtVertex.begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksEnd=tracksAtVertex.end();
    
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksIter=tracksBegin;tracksIter!=tracksEnd;++tracksIter) {
      if ((*tracksIter)==0) {
	ATH_MSG_WARNING (" Empty pointer in vector of VxTrackAtVertex of the VxVertexOnJetAxis which is being fitted. No fit will be done...");
	return;
      } 
	if (*tracksIter==trackToUpdate) {
	  found=true;
	  break;
	}
      
    }  
    
    if (!found) {
      ATH_MSG_WARNING ("Track was not found in the VxVertexOnJetAxis's list. No fit will be done...");
      return;
    }
    
    double trkWeight = trackToUpdate->weight();
    
    ATH_MSG_VERBOSE ("Weight is: " << trkWeight);
    
    const Trk::RecVertexPositions & old_vrt = candidateToUpdate->getRecVertexPositions();
    
    if (trackToUpdate->linState()==nullptr) {
      ATH_MSG_WARNING ("Linearized state not associated to track. Aborting fit... " << old_vrt);
      return;
    }
    
    RecVertexPositions fit_vrt;
    if (vertexToUpdate!=candidateToUpdate->getPrimaryVertex()) {//if not in primary vertex, then special update
      fit_vrt = positionUpdate(*candidateToUpdate,trackToUpdate->linState(),trkWeight, sign,
			       vertexToUpdate->getNumVertex(),false,doFastUpdate);
    } else {//if in primary vertex, use dist=0
      fit_vrt = positionUpdate(*candidateToUpdate,trackToUpdate->linState(),trkWeight,sign,
			       vertexToUpdate->getNumVertex(),true,doFastUpdate);
    }
    
    //forming a final candidate  
    candidateToUpdate->setRecVertexPositions(fit_vrt);

    trackToUpdate->setTrackQuality(FitQuality(fit_vrt.fitQuality().chiSquared()-old_vrt.fitQuality().chiSquared(),2));
    
    
  }//end of update method

  
  //actual method where the position update happens 
  Trk::RecVertexPositions 
    KalmanVertexOnJetAxisUpdator::positionUpdate(VxJetCandidate& candidateToUpdate,
                                                 const LinearizedTrack * trk, 
                                                 double trackWeight, int sign,
                                                 int numVertex, bool isPrimary,
                                                 bool doFastUpdate) const {

    //linearized track information

    const int numrow_toupdate = isPrimary ? 4 : numRow(numVertex);
//    std::cout << " num row to update " << numrow_toupdate << std::endl;
    
    const VertexPositions & linearizationPositions=candidateToUpdate.getLinearizationVertexPositions();
    
//    std::cout << " Linearization vertex positions: " << linearizationPositions << std::endl;

    const Amg::VectorX & initialjetdir=linearizationPositions.position();
    
    //calculate 3-dim position on jet axis from linearizationPositionVector and calculate Jacobian of transformation
    std::pair<Amg::Vector3D,Eigen::Matrix3Xd> PosOnJetAxisAndTransformMatrix = 
      createTransformJacobian(initialjetdir, numVertex, isPrimary,
                              /*truncate=*/ !doFastUpdate); // original fast updator didn't have feature
    
    //only position jacobian changed from A ->oldA
    const AmgMatrix(5,3)& oldA = trk->positionJacobian();

#ifdef Updator_DEBUG
    std::cout << "the old jacobian xyz d0z0phithetaqoverp vs xyz " << oldA << std::endl; 
#endif    

    //now create the new jacobian which you should use
    Eigen::Matrix<double,5,Eigen::Dynamic> A=oldA*PosOnJetAxisAndTransformMatrix.second;
// AISWHAT

#ifdef Updator_DEBUG
      std::cout << "the new jacobian " << A << std::endl;
#endif
    
    const AmgMatrix(5,3) & B = trk->momentumJacobian();
    const AmgVector(5)& trackParameters = trk->expectedParametersAtPCA();
    
    
    //   CLHEP::HepVector constantTerm = trk->constantTerm();
    //   the constant term has to be recalculated because of the use of the new parameters

#ifdef KalmanVertexUpdate_OLD
    const AmgVector(5) constantTerm=trk->constantTerm() + oldA*PosOnJetAxisAndTransformMatrix.first
      -A*initialjetdir;
#endif
#ifdef KalmanVertexUpdate_NEW
    const AmgVector(5) constantTerm=trk->constantTerm() + oldA*PosOnJetAxisAndTransformMatrix.first
      -A*initialjetdir.segment(0,numrow_toupdate+1);
#endif

    //vertex to be updated, needs to be copied
    RecVertexPositions myPosition=candidateToUpdate.getRecVertexPositions();

    if (doFastUpdate) {

      const AmgSymMatrix(5)& trackParametersCovariance  = trk->expectedCovarianceAtPCA();
   
      const Amg::VectorX & old_vrt_pos = myPosition.position(); // old jetvertex in covariance formalism
      const Amg::MatrixX & old_vrt_cov = myPosition.covariancePosition();

      //now create three additional variables for the momenta error
      // (which is nearly infinite... use a big number here... then pay attention to numerical instabilities...)
      AmgSymMatrix(3) old_vrt_cov_momentum; old_vrt_cov_momentum.setZero();
      old_vrt_cov_momentum(0,0) = m_initialMomentumError*m_initialMomentumError;
      old_vrt_cov_momentum(1,1) = m_initialMomentumError*m_initialMomentumError;
      old_vrt_cov_momentum(2,2) = m_initialMomentumError*m_initialMomentumError/10000.;

      //R_k_k-1=V_k+A C_k-1 A_T + B D_k-1 B_T
      AmgSymMatrix(5) old_residual_cov =
        trackParametersCovariance + A * old_vrt_cov * A.transpose() +
        B * old_vrt_cov_momentum * B.transpose();

      //the nice thing of the method is that the matrix to invert (old_residual) is just 5x5,
      //which is much better than n.track+5 x n.track+5 !!!
      if (old_residual_cov.determinant() == 0. ) {
        ATH_MSG_ERROR ("The old_residual matrix inversion failed");
        ATH_MSG_ERROR ("same vertex as before is returned");
        return Trk::RecVertexPositions(myPosition);
      }
      //      AmgSymMatrix(5) old_residual_cov_inv = old_residual_cov.inverse().eval();
      AmgSymMatrix(5) old_residual_cov_inv = old_residual_cov.inverse().eval();

      Eigen::Matrix<double,Eigen::Dynamic,5> Kk1 = old_vrt_cov*A.transpose()*old_residual_cov_inv;
      AmgVector(5) residual_vector=trackParameters-constantTerm-A*old_vrt_pos;

      //obtain new position
      Amg::VectorX new_vrt_pos=old_vrt_pos+Kk1*residual_vector;


      Amg::MatrixX new_vrt_cov=old_vrt_cov+Kk1*old_residual_cov*Kk1.transpose()-2.*Kk1*A*old_vrt_cov;


      double chi2 = myPosition.fitQuality().chiSquared() +
                    residual_vector.transpose()*old_residual_cov_inv*residual_vector;

      //NOT SO NICE: BUT: if there was already a track at this vertex,
      //then add 2, otherwise add 1 (additional parameter in the fit decreases +2 ndf)
      double ndf=myPosition.fitQuality().numberDoF()+sign*1.;
      if (isPrimary) {
        ndf+=sign*1.;
      } else {
        ndf+=sign*1.;
      }
      return Trk::RecVertexPositions(new_vrt_pos,new_vrt_cov,ndf,chi2);

    } 
    
      const AmgSymMatrix(5) & trackParametersWeight  = trk->expectedWeightAtPCA();
    
      if (trackParametersWeight.determinant()<=0)  
      {
        ATH_MSG_ERROR(" The determinant of the track covariance matrix is negative: " << trackParametersWeight.determinant());
      }
      

      //vertex to be updated, needs to be copied 
      myPosition=candidateToUpdate.getRecVertexPositions();
      if(myPosition.covariancePosition().determinant() ==0.0) {
        ATH_MSG_WARNING ("The vertex-positions covariance matrix is not invertible");
        ATH_MSG_WARNING ("The copy of initial vertex returned");
        return Trk::RecVertexPositions(myPosition);
      }
#ifdef KalmanVertexUpdate_OLD
      const Amg::VectorX & old_vrt_pos = myPosition.position();
      const Amg::MatrixX & old_vrt_weight = myPosition.covariancePosition().inverse();
#endif
#ifdef KalmanVertexUpdate_NEW
      const Amg::VectorX & old_full_vrt_pos = myPosition.position();
      Amg::VectorX old_vrt_pos = old_full_vrt_pos.segment(0,numrow_toupdate+1);

//      std::cout << " COV MATRIX " << myPosition.covariancePosition() << std::endl;

      const Amg::MatrixX & old_full_vrt_weight = myPosition.covariancePosition().inverse();

    
      if (trackParametersWeight.determinant()<=0)  
      {
        ATH_MSG_WARNING(" The determinant of the track covariance matrix is zero or negative: " << trackParametersWeight.determinant());
      }


      Amg::MatrixX         old_vrt_weight(numrow_toupdate+1,numrow_toupdate+1);
      old_vrt_weight = old_full_vrt_weight.block(0,0,numrow_toupdate+1,numrow_toupdate+1);


#endif

      //making the intermediate quantities:
      //W_k = (B^T*G*B)^(-1)  
      AmgSymMatrix(3) S = B.transpose()*(trackParametersWeight*B);



      if(S.determinant() ==0.0) {
        ATH_MSG_WARNING ("The S matrix is not invertible");
        ATH_MSG_WARNING ("A copy of initial vertex returned");
        return Trk::RecVertexPositions(myPosition);
      }
      S = S.inverse().eval();


      //G_b = G_k - G_k*B_k*W_k*B_k^(T)*G_k  

      AmgSymMatrix(5) gB = trackParametersWeight - trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose();
	//gB=gB/2.0+gB.transpose().eval()/2.0;



#ifdef Updator_DEBUG
      std::cout<<"Gain factor obtained: "<<trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose()<<std::endl;
      std::cout<<"Resulting Gain Matrix: "<<gB<<std::endl;
#endif
   
      //new vertex weight matrix, called "cov" for later inversion  

//      std::cout << " A " << A  << std::endl;

//      std::cout << " old_vrt_weight " << old_vrt_weight << std::endl;
//      std::cout << " add second piece " << trackWeight * sign * A.transpose() * gB * A << std::endl;

      //      Amg::MatrixX new_vrt_cov = old_vrt_weight + trackWeight * sign * gB.similarityT(A); GP bug of Eigen ???
      Amg::MatrixX new_vrt_cov = old_vrt_weight + trackWeight * sign * A.transpose() * ( gB * A );

      if (sign<0)
      {
        std::cout << " ATTENTION! Sign is " << sign << std::endl;
      }

      if (new_vrt_cov.determinant()<=0)
      {
        ATH_MSG_WARNING(std::scientific << "The new vtx weight  matrix determinant is negative: "<< new_vrt_cov.determinant());
      }
      
//      std::cout << " new_vrt_cov " << new_vrt_cov << std::endl;

//      std::cout << " NEW weight matrix before inversion " << new_vrt_cov << std::endl;

      // now invert back to covariance 
#ifdef KalmanVertexUpdate_OLD
      if (new_vrt_cov.determinant() == 0.0) {
        ATH_MSG_ERROR ("The reduced weight matrix is not invertible, returning copy of initial vertex.");
        Trk::RecVertexPositions r_vtx(myPosition);
        return r_vtx;
      }
      new_vrt_cov = new_vrt_cov.inverse().eval();

#endif
#ifdef KalmanVertexUpdate_NEW
      try 
      {
//4.Oct.2014 Temporarily remove smart inversion as it seems to cause negative errors from time to time
//	Symmetrize the matix before inversion, to give ride of the precision problem of Eigen.
//	wesong@cern.ch, 28/05/2016
	new_vrt_cov=(new_vrt_cov+new_vrt_cov.transpose().eval())/2.0;
	smartInvert(new_vrt_cov);
        if (new_vrt_cov.determinant() == 0.0) {
          ATH_MSG_ERROR ("The reduced weight matrix is not invertible, returning copy of initial vertex.");
          const Trk::RecVertexPositions& r_vtx(myPosition);
          return r_vtx;
        }
//	After symmetrizing the matirx by hand, as mentioned above, the smart inversion works agian.
        //new_vrt_cov = new_vrt_cov.inverse().eval();
      }
      catch (std::string a)
      {
        ATH_MSG_ERROR( a << " Previous vertex returned " );
        const Trk::RecVertexPositions& r_vtx(myPosition);
        return r_vtx;
      }
#endif
#ifdef Updator_DEBUG
      std::cout << " new vertex covariance " << new_vrt_cov << std::endl;

      if (new_vrt_cov.determinant()<=0)
      {
        ATH_MSG_WARNING("The new vtx cov. matrix determinant is negative: "<< new_vrt_cov.determinant());
      }
#endif

      //new vertex position
      Amg::VectorX new_vrt_position = 
          new_vrt_cov*(old_vrt_weight * old_vrt_pos + trackWeight * sign * (A.transpose() * (gB *(trackParameters - constantTerm)))); 
#ifdef Updator_DEBUG
      std::cout << " new position " << new_vrt_position << std::endl;
#endif

      //refitted track momentum  
      Amg::Vector3D newTrackMomentum = S*B.transpose()*trackParametersWeight*
          (trackParameters - constantTerm - A*new_vrt_position);
#ifdef Updator_DEBUG
      std::cout << " new momentum : " << newTrackMomentum << std::endl;
#endif

      //refitted track parameters
      AmgVector(5) refTrackParameters = constantTerm + A * new_vrt_position + B * newTrackMomentum;

      //parameters difference 
      AmgVector(5) paramDifference = trackParameters - refTrackParameters;
      Amg::VectorX posDifference = new_vrt_position - old_vrt_pos;
      //

      double chi2 = myPosition.fitQuality().chiSquared()+
        (paramDifference.transpose()*trackParametersWeight*paramDifference)(0,0)*trackWeight*sign+
        (posDifference.transpose()*old_vrt_weight*posDifference)(0,0); // matrices are 1x1 but make scalar through (0,0)

#ifdef Updator_DEBUG
      std::cout << " new chi2 : " << chi2 << std::endl;
#endif

      //NOT SO NICE: BUT: if there was already a track at this vertex,
      //then add 2, otherwise add 1 (additional parameter in the fit decreases +2 ndf)
      double ndf=myPosition.fitQuality().numberDoF()+sign*1.;
      if (isPrimary) {
        ndf+=sign*1.;
      } else {
        ndf+=sign*1.;
      }

      for (int i=0;i<new_vrt_cov.rows();i++)
      {
        bool negative(false);
        if (new_vrt_cov(i,i)<=0.) 
        {
          ATH_MSG_WARNING(" Diagonal element ("<<i<<","<<i<<") of covariance matrix after update negative: "<<new_vrt_cov(i,i) <<". Giving back previous vertex.");
          negative=true;
        }
        if (negative) {
          const Trk::RecVertexPositions& r_vtx(myPosition);
          return r_vtx;
        }
      }
      
    
#ifdef KalmanVertexUpdate_OLD
      Trk::RecVertexPositions r_vtx(new_vrt_position,new_vrt_cov,ndf, chi2);
#endif
#ifdef KalmanVertexUpdate_NEW
      Amg::VectorX new_full_vrt_pos(old_full_vrt_pos);
      new_full_vrt_pos.segment(0,new_vrt_position.rows()) = new_vrt_position;
      Amg::MatrixX new_full_vrt_cov(myPosition.covariancePosition());
      new_full_vrt_cov.block(0,0,new_vrt_cov.rows(),new_vrt_cov.cols()) = new_vrt_cov;

      Trk::RecVertexPositions r_vtx(new_full_vrt_pos,new_full_vrt_cov,ndf, chi2);
#endif
      return r_vtx; 
    
    //method which avoids inverting huge covariance matrix still needs to be implemented

  }//end of position update method
  
  void KalmanVertexOnJetAxisUpdator::updateChi2NdfInfo(VxTrackAtVertex* trackToUpdate,
						       const VxVertexOnJetAxis* vertexToUpdate,
						       VxJetCandidate* vertexCandidate) const {

    //check that all pointers are there...
    if (vertexCandidate==nullptr||trackToUpdate==nullptr) { 
      ATH_MSG_WARNING( " Empty pointers then calling fit method updateChi2NdfInfo. No update will be done..." );
      return;
    }

    //getting tracks at vertex and verify if your trackToUpdate is there
    bool found=false;
    const std::vector<Trk::VxTrackAtVertex*> & tracksAtVertex(vertexToUpdate->getTracksAtVertex()); 
    
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksBegin=tracksAtVertex.begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksEnd=tracksAtVertex.end();
    
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksIter=tracksBegin;tracksIter!=tracksEnd;++tracksIter) {
      if ((*tracksIter)==0) {
	ATH_MSG_WARNING( " Empty pointer in vector of VxTrackAtVertex of the VxVertexOnJetAxis whose chi2 is being updated. No update will be done..." );
	return;
      } 
	if (*tracksIter==trackToUpdate) {
	  found=true;
	  break;
	}
      
    }  
    
    if (!found) {
      ATH_MSG_WARNING( " Track was not found in the VxVertexOnJetAxis's list. No update will be done..." );
      return;
    }

    double trkWeight = trackToUpdate->weight();
    
    ATH_MSG_VERBOSE ("Weight is: " << trkWeight);
    
    Trk::RecVertexPositions vertexPositions = vertexCandidate->getRecVertexPositions();
    
    //    log << MSG::VERBOSE << "vertexPositions (update chi2): " << vertexPositions << endmsg;

    if (trackToUpdate->linState()==nullptr) {
      ATH_MSG_WARNING( "Linearized state not associated to track. Aborting chi2 update... " << vertexPositions );
      return;
    }

    double chi2(vertexPositions.fitQuality().chiSquared());

    ATH_MSG_VERBOSE ("old chi2: " << chi2);

    if (vertexToUpdate!=vertexCandidate->getPrimaryVertex()) {//if not in primary vertex, then special update
      chi2+=calculateTrackChi2(*vertexCandidate,trackToUpdate->linState(),trkWeight,
			       vertexToUpdate->getNumVertex(),false);
    } else {
      chi2+=calculateTrackChi2(*vertexCandidate,trackToUpdate->linState(),trkWeight,
			       vertexToUpdate->getNumVertex(),true);
    }
    
    vertexPositions.setFitQuality(FitQuality(chi2,vertexPositions.fitQuality().numberDoF()));
    
    ATH_MSG_VERBOSE (" new chi2: " << chi2);


    trackToUpdate->setTrackQuality(FitQuality(chi2-vertexPositions.fitQuality().chiSquared(),
					      trackToUpdate->trackQuality().numberDoF()));
    
    vertexCandidate->setRecVertexPositions(vertexPositions);
    
  }


  double KalmanVertexOnJetAxisUpdator::calculateTrackChi2(const VxJetCandidate& vertexCandidate,
							const LinearizedTrack * trk, 
							double trackWeight, 
							int numVertex,bool isPrimary) const {
    
    
    //linearized track information 
    
    const VertexPositions & linearizationPositions=vertexCandidate.getLinearizationVertexPositions();
    
    const Amg::VectorX & initialjetdir=linearizationPositions.position();

    
    //calculate 3-dim position on jet axis from linearizationPositionVector and calculate Jacobian of transformation
    std::pair<Amg::Vector3D,Eigen::Matrix3Xd> PosOnJetAxisAndTransformMatrix = 
      createTransformJacobian(initialjetdir, numVertex, isPrimary, true);

    //only position jacobian changed from A ->oldA
    const AmgMatrix(5,3)& oldA = trk->positionJacobian();

     //now create the new jacobian which you should use
    Eigen::Matrix<double,5,Eigen::Dynamic> A=oldA*PosOnJetAxisAndTransformMatrix.second;
   
    const AmgMatrix(5,3)& B = trk->momentumJacobian();
    const AmgVector(5) & trackParameters = trk->expectedParametersAtPCA();

    AmgVector(5) constantTerm=trk->constantTerm() + oldA*PosOnJetAxisAndTransformMatrix.first
      -A*initialjetdir;

    const AmgSymMatrix(5) & trackParametersWeight  = trk->expectedWeightAtPCA();
    
    const RecVertexPositions & myPosition=vertexCandidate.getRecVertexPositions();

    const Amg::VectorX & new_vrt_position = myPosition.position();

    AmgSymMatrix(3) S = B.transpose()*trackParametersWeight*B;
    if(S.determinant() ==0.0) {
      ATH_MSG_ERROR ("The matrix S is not invertible, return chi2 0");
      return -0.;
    } 
    S = S.inverse().eval();

    //refitted track momentum  
    Amg::Vector3D newTrackMomentum = S*B.transpose()*trackParametersWeight*(trackParameters - constantTerm - A*new_vrt_position);

    //refitted track parameters
    AmgVector(5) refTrackParameters = constantTerm + A * new_vrt_position + B * newTrackMomentum;

    //parameters difference 
    AmgVector(5) paramDifference = trackParameters - refTrackParameters;
   
    return (paramDifference.transpose()*trackParametersWeight*paramDifference)(0,0)*trackWeight;
   
  }


  void KalmanVertexOnJetAxisUpdator::updateVertexChi2(VxJetCandidate* vertexCandidate) const {

    //check that all pointers are there...
    if (vertexCandidate==nullptr) { 
      ATH_MSG_WARNING( " Empty pointers then calling chi2 update method updateVertexChi2. No update will be done..." );
      return;
    }

    const Trk::RecVertexPositions & OldPosition=vertexCandidate->getConstraintVertexPositions();

    const Amg::VectorX & old_vrt_position = OldPosition.position();

    const Amg::MatrixX & old_vrt_weight = OldPosition.covariancePosition().inverse();

    const RecVertexPositions & myPosition=vertexCandidate->getRecVertexPositions();

    const Amg::VectorX & new_vrt_position = myPosition.position();

    AmgVector(5) posDifference = (new_vrt_position - old_vrt_position).segment(0,5);

    double chi2=(posDifference.transpose()*old_vrt_weight.block<5,5>(0,0)*posDifference)(0,0);

#ifdef Updator_DEBUG
    std::cout << " vertex diff chi2 : " << chi2 << std::endl;
#endif

    Trk::RecVertexPositions vertexPositions = vertexCandidate->getRecVertexPositions();
    
    vertexPositions.setFitQuality(FitQuality(vertexPositions.fitQuality().chiSquared()+chi2,
					     vertexPositions.fitQuality().numberDoF()));

    vertexCandidate->setRecVertexPositions(vertexPositions);

  }


  std::pair<Amg::Vector3D,Eigen::Matrix3Xd> 
    KalmanVertexOnJetAxisUpdator::createTransformJacobian(const Amg::VectorX & initialjetdir,
							  int numVertex,
							  bool isPrimary,
                                                          bool truncateDimensions) const {

    //now modify the position jacobian in the way you need!
    const unsigned int numrow_toupdate=isPrimary ? 4: numRow(numVertex);
    const unsigned int matrixdim = truncateDimensions ?
                                   numrow_toupdate+1  :
                                   initialjetdir.rows();

    //store values of RecVertexOnJetAxis
    double xv=initialjetdir[Trk::jet_xv];
    double yv=initialjetdir[Trk::jet_yv];
    double zv=initialjetdir[Trk::jet_zv];
    double phi=initialjetdir[Trk::jet_phi];
    double theta=initialjetdir[Trk::jet_theta];
    double dist=0;
    if (!isPrimary) {
      dist=initialjetdir[numrow_toupdate];
    }
    
    ATH_MSG_VERBOSE ("actual initialjetdir values : xv " << xv << " yv " << yv << " zv " << zv << 
      " phi " << phi << " theta " << theta << " dist " << dist);
    ATH_MSG_VERBOSE ("numrow_toupdate " << numrow_toupdate << " when primary it's -5");
    
    //now create matrix transformation to go from vertex position jacobian to vertex in jet jacobian
    //(essentially Jacobian of spherical coordinate system transformation)
    Eigen::Matrix3Xd transform(3,matrixdim); transform.setZero();
    transform(0,Trk::jet_xv)=1;
    transform(1,Trk::jet_yv)=1;
    transform(2,Trk::jet_zv)=1;
    if (!isPrimary) {
      transform(0,Trk::jet_phi)   = -dist*sin(theta)*sin(phi);
      transform(0,Trk::jet_theta) =  dist*cos(theta)*cos(phi);
      transform(0,numrow_toupdate)=  sin(theta)*cos(phi);
      transform(1,Trk::jet_phi)   =  dist*sin(theta)*cos(phi);
      transform(1,Trk::jet_theta) =  dist*cos(theta)*sin(phi);
      transform(1,numrow_toupdate)=  sin(theta)*sin(phi);
      transform(2,Trk::jet_theta) = -dist*sin(theta);
      transform(2,numrow_toupdate)=  cos(theta);
    }
    
    ATH_MSG_DEBUG ("The transform matrix xyzphitheta is: " << transform);
    
    Amg::Vector3D posOnJetAxis;
    posOnJetAxis(0)=xv+dist*cos(phi)*sin(theta);
    posOnJetAxis(1)=yv+dist*sin(phi)*sin(theta);
    posOnJetAxis(2)=zv+dist*cos(theta);

    ATH_MSG_DEBUG("the lin position on jet axis x " << xv+dist*cos(phi)*sin(theta) << 
      " y " << yv+dist*sin(phi)*sin(theta) << " z " << zv+dist*cos(theta));

    return std::pair<Amg::Vector3D,Eigen::Matrix3Xd>(posOnJetAxis,transform);

  }

  void KalmanVertexOnJetAxisUpdator::smartInvert(Amg::MatrixX & new_vrt_weight) const
  {

    int numRows=new_vrt_weight.rows();
    if (numRows<=6) {
      if(new_vrt_weight.determinant() ==0)
        // if |A|==0; then A is not invertible;
      {
        throw std::string("The reduced weight matrix is not invertible. Previous vertex returned ");
      }
      new_vrt_weight=new_vrt_weight.inverse().eval();
      return;
    } 

    AmgSymMatrix(5) A = new_vrt_weight.block<5,5>(0,0);
    Eigen::Matrix<double,5,Eigen::Dynamic> B(5,numRows-5);//Eigen::Dynamic we are not sure about the number of Row minus 5, so a dynamic one is given here. 
    B.setZero();
    for (int i=0;i<5;++i)
    {
      for (int j=5;j<numRows;++j)
      {
        B(i,j-5)=new_vrt_weight(i,j);
      }
    }

    // CLHEP::HepSymMatrix D=new_vrt_weight.sub(6,numRows);
    Amg::MatrixX D = new_vrt_weight.block(5,5,numRows-5,numRows-5); // will be of dim (numrows-5)

    
    // Eigen::DiagonalMatrix<double,Eigen::Dynamic> DdiagINV(D.rows()); // was CLHEP::HepDiagMatrix
//    if (numRows>6 && D.isDiagonal(/*precision<scalar>=*/1e-7))

//GP: 4.10.2014 Just ignore that due to numerical precision the non diagonal elements can be ||>1e-6
//This is not necessay, as we will set them to be zero
/*    if (numRows>6 && fabs(D(0,1))>1e-6)
    {
      //const std::streamsize old = std::cout.precision();
      ATH_MSG_ERROR( std::setprecision(10) << " Will invert normally, because the non diagonal element is: " << D(0,1) );

      new_vrt_weight=new_vrt_weight.inverse().eval();
      return;
    }
*/

    // Eigen's diagonal matrices don't have the full MatrixBase members like determinant(),
    // similarity() etc needed later -> therefore do not use them and invert by hand.
    // The inverse of D is a diagonal matrix of the inverted diagonal elements -WL
    // Eigen::DiagonalMatrix<double,Eigen::Dynamic> DdiagINV(D); do something...
    for (unsigned int i=0; i<D.rows(); i++) {
      if ( D(i,i) == 0.) {
        throw std::string("Cannot invert diagonal matrix...");
        break;
      }
//Set the non-diagonal elements of D to be zero, which is as expected; the numerical proplem will change them into non-zero. wesong@cern.ch, 28/05/2016
	for(unsigned int j=0; j<i; j++){
	D(i,j)=0;
	D(j,i)=0;
	}
      D(i,i) = 1./D(i,i);
    }
//      D=D.inverse().eval();

    Amg::MatrixX E = A - B*(D*B.transpose());
    if (E.determinant() == 0.) {
      throw std::string("Cannot invert E matrix...");
    }
    E=E.inverse().eval();

    
    Amg::MatrixX finalWeight(numRows,numRows); 
    finalWeight.setZero();
    finalWeight.block<5,5>(0,0) = E;
    Eigen::Matrix<double,5,Eigen::Dynamic> F = -E*(B*D);
    finalWeight.block(0,5,5,D.rows()) = F;
    finalWeight.block(5,0,D.rows(),5) = F.transpose();
    finalWeight.block(5,5,D.rows(),D.rows()) = 
        D+(D*((B.transpose()*(E*B))*D.transpose()));


//    Amg::MatrixX normalInversion=new_vrt_weight.inverse().eval();
//    Amg::MatrixX smartInversion=finalWeight;
    
/*    bool mismatch(false);
    
    for (int i=0;i<numRows;i++)
    {
      for (int j=0;j<numRows;j++)
      {
        if (fabs(normalInversion(i,j)-smartInversion(i,j))/fabs(normalInversion(i,j))>1e-4) 
	{
	   mismatch=true;
	   std::cout<<"(i,j)="<<"("<<i<<","<<j<<")"<<std::endl;
	}
      }
    }
*/

//    new_vrt_weight = new_vrt_weight.inverse().eval();//finalWeight;//MODIFIED!!
    new_vrt_weight = finalWeight;//MODIFIED!!

    if (new_vrt_weight.determinant()<=0) ATH_MSG_DEBUG("smartInvert() new_vrt_weight FINAL det. is: " << new_vrt_weight.determinant());

  }

}//end of namespace definition
