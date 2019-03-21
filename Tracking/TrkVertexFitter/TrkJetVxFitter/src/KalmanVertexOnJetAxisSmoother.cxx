/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkJetVxFitter/KalmanVertexOnJetAxisSmoother.h"
#include "VxJetVertex/RecVertexPositions.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/JetVtxParamDefs.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkJetVxFitter/KalmanVertexOnJetAxisUpdator.h"
#include <math.h>

//#define KalmanVertexOnJetAxisSmoother_DEBUG

namespace Trk
{  

  // namespace {
  //   int numRow(int numVertex) {
  //     return numVertex+5;
  //   }
  // }

  
 StatusCode KalmanVertexOnJetAxisSmoother::initialize()
 {
  AthAlgTool::initialize().ignore();

  //retrieving the udator itself 	 
  StatusCode sc = m_Updator.retrieve();
  if(sc.isFailure()) 	  { 	 
    msg(MSG::ERROR)<<" Unable to retrieve "<<m_Updator<<endreq; 	 
    return StatusCode::FAILURE; 	 
  }
  m_initial_cov_momentum.setZero();
  m_initial_cov_momentum(0,0) = m_initialMomentumError*m_initialMomentumError;
  m_initial_cov_momentum(1,1) = m_initialMomentumError*m_initialMomentumError;
  m_initial_cov_momentum(2,2) = m_initialMomentumError*m_initialMomentumError/10000.;

  return StatusCode::SUCCESS;
 }
 
 StatusCode KalmanVertexOnJetAxisSmoother::finalize()
 {
   msg(MSG::INFO)  << "Finalize successful" << endreq;
  return StatusCode::SUCCESS;
 }
 
 KalmanVertexOnJetAxisSmoother::KalmanVertexOnJetAxisSmoother(const std::string& t, const std::string& n, const IInterface*  p):
   AthAlgTool(t,n,p),
   m_Updator("Trk::KalmanVertexOnJetAxisUpdator"),
   m_maxWeight(0.001),
   m_initialMomentumError(1000.)
 { 
   declareProperty("Updator",m_Updator);
   declareProperty("MaximalWeight",m_maxWeight);
   declareProperty("initialMomentumError",m_initialMomentumError);
   declareInterface<KalmanVertexOnJetAxisSmoother>(this);
 }
 
 KalmanVertexOnJetAxisSmoother::~KalmanVertexOnJetAxisSmoother()
 {
  
 }


  void KalmanVertexOnJetAxisSmoother::update(VxVertexOnJetAxis* vertexToSmooth,
                                             const VxJetCandidate* candidateToUpdate,
					     bool updateTrack) const {
  
    const RecVertexPositions & myFinalPosition=candidateToUpdate->getRecVertexPositions();
    const VertexPositions & linearizationPositions=candidateToUpdate->getLinearizationVertexPositions();
    bool isPrimary = (vertexToSmooth==candidateToUpdate->getPrimaryVertex());
    update(vertexToSmooth,isPrimary,myFinalPosition,linearizationPositions,updateTrack,false);
    
  }

  void KalmanVertexOnJetAxisSmoother::fastUpdate(VxVertexOnJetAxis* vertexToSmooth,
                                                 const VxJetCandidate* candidateToUpdate,
                                                 bool updateTrack) const {
    const RecVertexPositions & myFinalPosition=candidateToUpdate->getRecVertexPositions();
    const VertexPositions & linearizationPositions=candidateToUpdate->getLinearizationVertexPositions();
    bool isPrimary = (vertexToSmooth==candidateToUpdate->getPrimaryVertex());
    update(vertexToSmooth,isPrimary,myFinalPosition,linearizationPositions,updateTrack,true);
  }

 void KalmanVertexOnJetAxisSmoother::update(VxVertexOnJetAxis* vertexToSmooth,bool isPrimary,
					    const RecVertexPositions & myFinalPosition,
					    const VertexPositions & linearizationPositions,
					    bool updateTrack, bool doFastUpdate) const {
 
 //  void KalmanVertexOnJetAxisSmoother::update(VxVertexOnJetAxis* vertexToSmooth,const VxJetCandidate* candidateToUpdate) const
   //  {

    if (vertexToSmooth==0) {
      msg(MSG::WARNING) << " Empty pointers then calling fit method update. No fit will be done..." << endreq;
      return;
    }

    ATH_MSG_DEBUG("It's smoothing "<<(isPrimary? "with":"without")<<" primary vertex ");
    ATH_MSG_DEBUG("It's " << (updateTrack?"with":"without") << " updating the track.");
    ATH_MSG_DEBUG("It's the "<<(doFastUpdate?"fast":"normal (weight-formalism)")<<" smoother");

    const std::vector<VxTrackAtVertex*> & allTracksToSmooth(vertexToSmooth->getTracksAtVertex());

    //care first about the transformation matrix (which is the same for all tracks...)
    
    
    if (allTracksToSmooth.size()==0) {
      ATH_MSG_DEBUG(" Nothing to smooth ");
      if (!isPrimary)
	ATH_MSG_WARNING ("Nothing to smooth and it's not a primary vertex: BUG... ");
      return;
    }

    const Amg::VectorX & initialjetdir=linearizationPositions.position();

    int numVertex=vertexToSmooth->getNumVertex();

    
    //calculate 3-dim position on jet axis from linearizationPositionVector and calculate Jacobian of transformation
    std::pair<Amg::Vector3D,Eigen::Matrix3Xd> PosOnJetAxisAndTransformMatrix = 
      m_Updator->createTransformJacobian(initialjetdir, numVertex, isPrimary, /*truncate=*/false);

    #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
    std::cout << " smoother: " << PosOnJetAxisAndTransformMatrix.first << " and transform matrix: " << PosOnJetAxisAndTransformMatrix.second << 
      std::endl;
    #endif


    //prepare a RecVertexPositions to be updated with removal 
    //of all tracks of the present vertex...
    //THIS TIME copy the RecVertexPositions to a new object... :-)
    RecVertexPositions myPositionWithRemovedTracks=myFinalPosition; // for fast updator (cov formalism)
    RecVertexPositions myPositionInWeightFormalism=myFinalPosition; // for full updator (weight formalism)
    const Amg::VectorX & final_vrt_pos=myFinalPosition.position();
    const Amg::MatrixX & final_vrt_covariance = myFinalPosition.covariancePosition();

    Amg::MatrixX         final_vrt_weightmatrix;
    if (!doFastUpdate) {
      // Hack the JetFitter EDM... avoid to invert big cov matrix at each iteration
      // by storing position --> weight*position
      // and        covariance -> weightmatrix
      if (final_vrt_covariance.determinant() == 0.0) {
        ATH_MSG_WARNING("Jet vertex positions matrix not invertible right at start of smoother!" <<
                      " -> stop smoothing.");
        return;
      }
      final_vrt_weightmatrix = final_vrt_covariance.inverse();
      myPositionInWeightFormalism.setPosition(final_vrt_weightmatrix*
                                              myPositionInWeightFormalism.position());
      myPositionInWeightFormalism.setCovariancePosition(final_vrt_weightmatrix);
    }
    //from here onwards (until finishing adding tracks) it is weight*position TO MAKE THINGS FASTER 

    double track_compatibility_chi2(0.);
    double track_ndf(0.);

    //nice! that was easy...
    //now iterate over all tracks in order to calculated the updated TrackParameters + the residual chi2...
    const std::vector<VxTrackAtVertex*>::const_iterator TracksBegin=allTracksToSmooth.begin();
    const std::vector<VxTrackAtVertex*>::const_iterator TracksEnd=allTracksToSmooth.end();

    for (std::vector<VxTrackAtVertex*>::const_iterator TracksIter=TracksBegin;TracksIter!=TracksEnd;++TracksIter) {
      
      //get linearized track
      const LinearizedTrack * trk=(*TracksIter)->linState();
      
      if (trk==0) {
        ATH_MSG_WARNING (" Empty pointers then calling smoothing method update. No smoothing will be performed...");
        return;
      }
      
      double trackWeight = (*TracksIter)->weight();

      //only position jacobian changed from A ->oldA
      const AmgMatrix(5,3)& oldA = trk->positionJacobian();
      
      #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
      std::cout << "the old jacobian xyz d0z0phithetaqoverp vs xyz " << oldA << std::endl; 
      #endif
      
      //now create the new jacobian which you should use
      Eigen::Matrix<double,5,Eigen::Dynamic> A = oldA*PosOnJetAxisAndTransformMatrix.second;
    
      #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
      std::cout << "the new jacobian " << A << std::endl;
      #endif
      
      const AmgMatrix(5,3) & B = trk->momentumJacobian();
      const AmgVector(5)& trackParameters = trk->expectedParametersAtPCA();

      #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
      std::cout << "expected perigee at pca " << trackParameters << std::endl;
      std::cout << "expected position at pca " << trk->expectedPositionAtPCA() << std::endl;
      std::cout << " constant term " << trk->constantTerm() << 
	" old A " << oldA << " * PosOnJetAxis " << PosOnJetAxisAndTransformMatrix.first << 
	" A " << A << " * initialjetdir " << initialjetdir << std::endl;
      #endif

      AmgVector(5) constantTerm=trk->constantTerm() + oldA*PosOnJetAxisAndTransformMatrix.first
	-A*initialjetdir;
      const AmgSymMatrix(5)& trackParametersWeight  = trk->expectedWeightAtPCA();
      AmgSymMatrix(3) S = B.transpose()*trackParametersWeight*B;
      if(S.determinant()==0.0) {
        ATH_MSG_WARNING ("The S matrix inversion failed during smoothing iteration cycle");
        continue;
      }
      S=S.inverse().eval();
      
      if (doFastUpdate) {
        const Amg::VectorX old_vrt_pos   = myPositionWithRemovedTracks.position();
        const Amg::MatrixX & old_vrt_cov = myPositionWithRemovedTracks.covariancePosition();

        AmgSymMatrix(5) old_residual_cov = -trk->expectedCovarianceAtPCA() + 
            A*old_vrt_cov*A.transpose() + B*m_initial_cov_momentum*B.transpose();
        if (old_residual_cov.determinant() == 0.0 ) {
          ATH_MSG_ERROR ("The old_residual matrix can not be inverted during smoothing interation cycle");
          continue;
        }
        AmgSymMatrix(5) old_residual_cov_inv = old_residual_cov.inverse().eval();
        Eigen::Matrix<double,Eigen::Dynamic,5> Kk1=old_vrt_cov*A.transpose()*old_residual_cov_inv;
        AmgVector(5) residual_vector=trackParameters-constantTerm-A*old_vrt_pos;
        Amg::VectorX new_vrt_pos = old_vrt_pos+Kk1*residual_vector;
        Amg::MatrixX new_vrt_cov = old_vrt_cov+Kk1*old_residual_cov*Kk1.transpose()
                                   - 2.*Kk1*A*old_vrt_cov;
        myPositionWithRemovedTracks = RecVertexPositions(new_vrt_pos,new_vrt_cov);

      } else {

        //REMEMBER: there's also the RecVertexPositions w/ removed tracks: **myPositionWithRemovedTracks**

        #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
	std::cout << " Old weight Matrix is " << myPositionInWeightFormalism.covariancePosition() << std::endl;
        #endif

        // the full smoother writes weight matrices into RecVertexPositions
        const Amg::MatrixX & old_vrt_weight = myPositionInWeightFormalism.covariancePosition();
        const Amg::VectorX old_vrt_weight_times_vrt_pos = myPositionInWeightFormalism.position();
      
        //G_b = G_k - G_k*B_k*W_k*B_k^(T)*G_k  
        AmgSymMatrix(5) gB = trackParametersWeight - trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose();
        //(S.similarity(B)).similarity(trackParametersWeight);
      
        #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
	std::cout<<"Gain factor obtained: "<<trackParametersWeight*(B*(S*B.transpose()))*trackParametersWeight.transpose()<<std::endl;
	std::cout<<"Resulting Gain Matrix: "<<gB<<std::endl;
        #endif
      
        //new vertex weight matrix  
        Amg::MatrixX new_vrt_weight = old_vrt_weight - trackWeight * A.transpose()*gB*A;
      
        #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
	std::cout<<"New vertex weight obtained: "<<new_vrt_weight<<std::endl;
        #endif
      
        //new vertex position
        Amg::VectorX new_weight_matrix_times_vrt_position =old_vrt_weight_times_vrt_pos - trackWeight * A.transpose() * gB *(trackParameters - constantTerm);
        //  new_vrt_cov*(old_vrt_weight * old_vrt_pos + trackWeight * sign * A.T() * gB *(trackParameters - constantTerm) ); 
        // avoid to invert the weight matrix to obtain the COVARIANCE MATRIX!!!
        //this line COPIES things two times, it would be nice to have a operator=(pos,errm) method in RecVertexPositions...    
        myPositionInWeightFormalism = 
          RecVertexPositions(new_weight_matrix_times_vrt_position,new_vrt_weight);
      } 

      //refitted track momentum  
      Amg::Vector3D newTrackMomentum = S*B.transpose()*trackParametersWeight*(trackParameters - constantTerm - A*final_vrt_pos);
      //refitted track parameters
      AmgVector(5) refTrackParameters = constantTerm + A * final_vrt_pos + B * newTrackMomentum;

      if (updateTrack) {

	const AmgSymMatrix(3) final_vrt_covariance_xyz = 
            (PosOnJetAxisAndTransformMatrix.second)*final_vrt_covariance*(PosOnJetAxisAndTransformMatrix.second).transpose();
	
	AmgVector(5) refTrackParametersToStore(refTrackParameters);


	//bring parameter phi again in the limits, if 
	//phi is slightly outside (as can be expected 
	//due to refitting)
	//thanks to Wolfgang L. / Sebastian F. for the piece of code with fmod!
	//	bool isSth=false;
	if (refTrackParametersToStore[Trk::phi0] > M_PI)  {
	  //	  isSth=true;
	  ATH_MSG_DEBUG ("-U- phi = " << refTrackParametersToStore[Trk::phi0]);
	  refTrackParametersToStore[Trk::phi0] = fmod(refTrackParametersToStore[Trk::phi0]+M_PI,2*M_PI)-M_PI;
	  ATH_MSG_DEBUG (" out of range, now corrected to " << refTrackParametersToStore[Trk::phi0]);
	} else if(refTrackParametersToStore[Trk::phi0]<-M_PI) {
	  ATH_MSG_DEBUG ("-U- phi = " << refTrackParametersToStore[Trk::phi0]);
	  refTrackParametersToStore[Trk::phi0] = fmod(refTrackParametersToStore[Trk::phi0]-M_PI,2*M_PI)+M_PI;
	  ATH_MSG_DEBUG (" out of range, now corrected to " << refTrackParametersToStore[Trk::phi0]);
	}

	if (refTrackParametersToStore[Trk::theta] > M_PI)
	{
	  ATH_MSG_DEBUG (" Theta out of range: correcting theta: " << refTrackParametersToStore[Trk::theta]);
	  refTrackParametersToStore[Trk::theta]=fmod(refTrackParametersToStore[Trk::theta]+M_PI,2*M_PI)-M_PI;
	  ATH_MSG_DEBUG (" to: " << refTrackParametersToStore[Trk::theta]);
	}
	else if (refTrackParametersToStore[Trk::theta]<0) {
	  ATH_MSG_DEBUG (" Theta out of range: correcting theta: " << refTrackParametersToStore[Trk::theta]);
	  refTrackParametersToStore[Trk::theta]=fmod(refTrackParametersToStore[Trk::theta]-M_PI,2*M_PI)+M_PI;
	  ATH_MSG_DEBUG (" to: " << refTrackParametersToStore[Trk::theta]);
	}	  

	if (refTrackParametersToStore[Trk::theta] < 0) {
	  refTrackParametersToStore[Trk::theta] =
	    - refTrackParametersToStore[Trk::theta];
	  refTrackParametersToStore[Trk::phi0] +=
	    (refTrackParametersToStore[Trk::phi0] > 0 ? -M_PI : M_PI);
	}
	else if (refTrackParametersToStore[Trk::theta] > M_PI) {//this is in principle not needed anymore
	  refTrackParametersToStore[Trk::theta] =
	    2*M_PI - refTrackParametersToStore[Trk::theta];
	  refTrackParametersToStore[Trk::phi0] +=
	    (refTrackParametersToStore[Trk::phi0] > 0 ? -M_PI : M_PI);
	}

	//making a new track covariance matrix
	AmgMatrix(Eigen::Dynamic,3) partialCovariance     = A.transpose() * trackParametersWeight * B *S;

	AmgMatrix(Eigen::Dynamic,3) posMomentumCovariance =  -final_vrt_covariance * partialCovariance;

	AmgMatrix(Eigen::Dynamic,3) posMomentumCovariance_xyz = PosOnJetAxisAndTransformMatrix.second * posMomentumCovariance;
	
	//new momentum covariance 
	AmgSymMatrix(3) momentumCovariance = doFastUpdate ?
            S + (partialCovariance.transpose()*final_vrt_covariance*partialCovariance) :
            S + (posMomentumCovariance.transpose()*final_vrt_weightmatrix*posMomentumCovariance);
	
	//full (x,y,z,phi, theta, q/p) covariance matrix
	AmgSymMatrix(6) fullTrkCov; fullTrkCov.setZero();

        #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
	std::cout << " posMomentumCovariance " << posMomentumCovariance << 
	  " final_vrt_covariance " << final_vrt_covariance << " momentumCovariance " << 
	  momentumCovariance << std::endl;
        #endif

	fullTrkCov.block<3,3>(0,3) = posMomentumCovariance_xyz.transpose();//GP: to be checked: .T() is here or down?
	fullTrkCov.block<3,3>(3,0) = posMomentumCovariance_xyz;
	fullTrkCov.block<3,3>(0,0) = final_vrt_covariance_xyz; 
	fullTrkCov.block<3,3>(3,3) = momentumCovariance; 
	//debug hacking   
	AmgMatrix(5,6) trJac; trJac.setZero();
	trJac(4,5) = 1.;
	trJac(3,4) = 1.; 
	trJac(2,3) = 1.;
	
	//first row
	trJac(0,0) = - sin(refTrackParametersToStore[Trk::phi0]);
	trJac(0,1) =   cos(refTrackParametersToStore[Trk::phi0]);
	
	//second row  
	trJac(1,0) = -cos(refTrackParametersToStore[Trk::phi0])/tan(refTrackParametersToStore[Trk::theta]); 
	trJac(1,1) = -sin(refTrackParametersToStore[Trk::phi0])/tan(refTrackParametersToStore[Trk::theta]);
	trJac(1,2) = 1.;
	
	AmgSymMatrix(5)* perFullTrkCov = new AmgSymMatrix(5)(trJac*fullTrkCov*trJac.transpose());
        Trk::PerigeeSurface pSurf(trk->linearizationPoint());
	Trk::TrackParameters* refittedPerigee = new Perigee(refTrackParametersToStore[0], 
							    refTrackParametersToStore[1], 
							    refTrackParametersToStore[2], 
							    refTrackParametersToStore[3], 
							    refTrackParametersToStore[4],
							    pSurf,
							    perFullTrkCov);

	//set the refitted track parameters
	(*TracksIter)->setPerigeeAtVertex(refittedPerigee);
      }
      
      
      //parameters difference 
      AmgVector(5) paramDifference = trackParameters - refTrackParameters;
      
      track_compatibility_chi2 += 
        (paramDifference.transpose()*trackParametersWeight*paramDifference)(0,0)*trackWeight;
      track_ndf+=2.;
      ATH_MSG_DEBUG ("new chi2 with compatibility of the tracks to the vertices " << 
                     track_compatibility_chi2 << " new ndf: " << track_ndf);    
    }//end iteration of tracks at vertex on jet axis
    
    if (!isPrimary) {
      track_ndf-=1.;//consider the vertex
    }
    if (!doFastUpdate) { // strange, the original fastSmoother didn't have such calculation
      //in the end calculate the real position... :-)
      const Amg::VectorX & vrt_removed_tracks_weight_times_vrt_pos =
         myPositionInWeightFormalism.position();
      const Amg::MatrixX & vrt_removed_tracks_weight =
         myPositionInWeightFormalism.covariancePosition();
      //perform a FAST inversion of the weight matrix - done here for the first time
      Amg::MatrixX vrt_removed_tracks_covariance = vrt_removed_tracks_weight;

      //4.October 2014 remove smartInversion for now (problems with numerical accuracy)
//      vrt_removed_tracks_covariance=vrt_removed_tracks_weight.inverse().eval();
	vrt_removed_tracks_covariance=(vrt_removed_tracks_covariance.transpose().eval()+vrt_removed_tracks_covariance)/2.0;
// After symmetrizing the matrix before inversion, the smart inversion works again, and move back. wesong@cern.ch 28/05/2016
      try 
      {
        m_Updator->smartInvert(vrt_removed_tracks_covariance);
      }
      catch (std::string a)
      {
        msg(MSG::ERROR)  << a << " Doing inversion the normal way " << endreq;
        vrt_removed_tracks_covariance=vrt_removed_tracks_weight.inverse().eval();
      }

      const Amg::VectorX vrt_removed_tracks_pos=vrt_removed_tracks_covariance*vrt_removed_tracks_weight_times_vrt_pos;
    
      #ifdef KalmanVertexOnJetAxisSmoother_DEBUG
      std::cout << " final_vrt_pos " << final_vrt_pos << " vrt_removed_tracks_pos " << vrt_removed_tracks_pos << std::endl;
      std::cout << " weight removed tracks " << vrt_removed_tracks_weight << std::endl;
      #endif

      track_compatibility_chi2 += 
          ((final_vrt_pos-vrt_removed_tracks_pos).transpose()*vrt_removed_tracks_weight*(final_vrt_pos-vrt_removed_tracks_pos))(0,0);


      ATH_MSG_DEBUG (" new chi2 with the vertex residual" << track_compatibility_chi2 <<
                     " new ndf: " << track_ndf);
    }
    vertexToSmooth->setFitQuality(FitQuality(track_compatibility_chi2,track_ndf));
#ifdef KalmanVertexOnJetAxisSmoother_DEBUG
    std::cout << " vertexToSmooth pointer is " << vertexToSmooth << std::endl;
#endif
  
 }
  
}//end of namespace definitions 
  
