/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        FastVertexFitter.cxx  -  Description
 ***************************************************************************/
#include "TrkVertexBilloirTools/FastVertexFitter.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "VxVertex/VxCandidate.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkLinks/LinkToXAODTrackParticle.h" 
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h" 
//xAOD includes 
#include "xAODTracking/Vertex.h" 
#include "xAODTracking/TrackParticle.h" 

/* These are some local helper classes only needed for convenience, therefor
within anonymous namespace. They do contain temporary calculations of matrices
and vectors resulting from the Billoir calculation (Note: no transformation
of the perigee parameters is done anymore). */
namespace
{
	struct BilloirTrack
	{
		BilloirTrack() : originalPerigee(nullptr),chi2{} { DtWD.setZero(); DtWDx.setZero(); xpVec.setZero();}
		virtual ~BilloirTrack() {}
		const Trk::TrackParameters * originalPerigee;
		AmgMatrix(3,3) DtWD;
		Amg::Vector3D DtWDx;
		Amg::Vector3D xpVec;
		double chi2;
	};

	struct BilloirVertex
	{
		BilloirVertex() : chi2{},ndf{} { DtWD_Sum.setZero(); DtWDx_Sum.setZero();}
		virtual ~BilloirVertex() {}
		AmgMatrix(3,3) DtWD_Sum;
		Amg::Vector3D DtWDx_Sum;
		double chi2;
		unsigned int ndf;
	};
}

namespace Trk
{
	StatusCode FastVertexFitter::initialize()
	{

		if ( m_extrapolator.retrieve().isFailure() )
		{
			msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
			return StatusCode::FAILURE;
		}
		else
		{
			msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
		}

		if ( m_linFactory.retrieve().isFailure() )
		{
			msg(MSG::FATAL) << "Failed to retrieve tool " << m_linFactory << endreq;
			return StatusCode::FAILURE;
		}
		else
		{
			msg(MSG::INFO) << "Retrieved tool " << m_linFactory << endreq;
		}

		//XAOD Converter   
 		  if ( m_xaodConverter.retrieve().isFailure() ) { 
 		    msg(MSG::FATAL) << "Failed to retrieve tool " << m_xaodConverter << endreq; 
 		    return StatusCode::FAILURE; 
 		  } else { 
 		    msg(MSG::INFO) << "Retrieved tool " << m_xaodConverter << endreq; 
 		  } 
 		  


		msg(MSG::INFO)  << "Initialize successful" << endreq;
		return StatusCode::SUCCESS;
	}

	StatusCode FastVertexFitter::finalize()
	{
		msg(MSG::INFO)  << "Finalize successful" << endreq;
		return StatusCode::SUCCESS;
	}

	FastVertexFitter::FastVertexFitter ( const std::string& t, const std::string& n, const IInterface*  p ) : AthAlgTool ( t,n,p ),
														  m_maxIterations ( 3 ),
														  m_maxDchi2PerNdf ( 0.000001 ),
														  m_extrapolator ( "Trk::Extrapolator" ),
														  m_linFactory ( "Trk::FullLinearizedTrackFactory" ),
														   m_xaodConverter("Trk::VxCandidateXAODVertex")
														  
	{
	  declareProperty ( "MaxIterations",        m_maxIterations );
	  declareProperty ( "MaxChi2PerNdf",        m_maxDchi2PerNdf );
	  declareProperty ( "Extrapolator",         m_extrapolator );
	  declareProperty("XAODConverter",m_xaodConverter); 
	  declareProperty ( "LinearizedTrackFactory", m_linFactory );
	  declareInterface<IVertexFitter> ( this );
	}

	FastVertexFitter::~FastVertexFitter() {}


	/** Interface for Track with starting point */
	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::Track*> & vectorTrk,
	                                      const Vertex& firstStartingPoint )
	{
		return fit ( vectorTrk, RecVertex ( firstStartingPoint ) );
	}

	/** Interface for Track with vertex constraint */
	/** the position of the constraint is ALWAYS the starting point */
	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::Track*>& vectorTrk,
	                                      const RecVertex& firstStartingPoint )
	{
		// push_back measured perigees of track into vector<const Trk::ParametersBase*>
		std::vector<const Trk::TrackParameters*> measuredPerigees;
		for ( std::vector<const Trk::Track*>::const_iterator trItr = vectorTrk.begin();
		        trItr != vectorTrk.end() ; ++trItr )
		{
			measuredPerigees.push_back ( ( *trItr )->perigeeParameters() );
		}

		Trk::VxCandidate* fittedVxCandidate = fit ( measuredPerigees, firstStartingPoint );

		if ( fittedVxCandidate == 0 ) return fittedVxCandidate;
		// assign the used tracks to the VxCandidate
		for ( unsigned int k = 0 ; k < vectorTrk.size() ; ++k )
		{
			LinkToTrack* link = new LinkToTrack;
			link->setElement ( vectorTrk[k] );
			// vxtrackatvertex takes ownership!
			( * ( fittedVxCandidate->vxTrackAtVertex() ) ) [k]->setOrigTrack ( link );
		}
		return fittedVxCandidate;
	}

	/** Interface for TrackParticleBase with starting point */
	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
	                                      const Vertex& firstStartingPoint )
	{
		return fit ( vectorTrk, RecVertex ( firstStartingPoint ) );
	}

	/** Interface for TrackParticleBase with vertex constraint */
	/** the position of the constraint is ALWAYS the starting point */
	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
	                                      const RecVertex& firstStartingPoint )
	{
		// push_back measured perigees of track into vector<const Trk::ParametersBase*>
		std::vector<const Trk::TrackParameters*> measuredPerigees;
		for ( std::vector<const Trk::TrackParticleBase*>::const_iterator trItr = vectorTrk.begin();
		        trItr != vectorTrk.end() ; ++trItr )
		{
			measuredPerigees.push_back ( & ( ( *trItr )->definingParameters() ) );
		}

		Trk::VxCandidate* fittedVxCandidate = fit ( measuredPerigees, firstStartingPoint );

		if ( fittedVxCandidate == 0 ) return fittedVxCandidate;
		// assign the used tracks to the VxCandidate
		for ( unsigned int k = 0 ; k < vectorTrk.size() ; ++k )
		{
			LinkToTrackParticleBase* link = new LinkToTrackParticleBase;
			link->setElement ( vectorTrk[k] );
			// vxtrackatvertex takes ownership!
			( * ( fittedVxCandidate->vxTrackAtVertex() ) ) [k]->setOrigTrack ( link );
		}
		return fittedVxCandidate;
	}

	/** Interface for MeasuredPerigee with starting point */
	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::TrackParameters*> & originalPerigees,
	                                      const Vertex& firstStartingPoint )
	{
		return fit ( originalPerigees, RecVertex ( firstStartingPoint ) );
	}

	/** Interface for MeasuredPerigee with vertex constraint */
	/** the position of the constraint is ALWAYS the starting point */
	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::TrackParameters*> & originalPerigees,
	                                      const RecVertex& firstStartingPoint )
	{
		if ( originalPerigees.empty() )
		{
			m_error = NOTRKS;
			ATH_MSG_VERBOSE("No tracks to fit in this event.");
			return 0;
		}

		/* Initialisation of variables */
		double chi2 = 2000000000000.;
		unsigned int nRP = originalPerigees.size();  // Number of tracks to fit
		int ndf = nRP * ( 5-3 ) - 3;                 // Number of degrees of freedom
		if ( ndf == -1 ) ndf = 1;

		/* Determine if we are doing a constraint fit.*/
		bool constraint = false;
		if ( firstStartingPoint.covariancePosition().trace() != 0. )
		{
			constraint = true;
			ndf += 3;
			ATH_MSG_DEBUG("Fitting with constraint.");
			ATH_MSG_VERBOSE(firstStartingPoint.covariancePosition().inverse().eval());
		}

		double chi2New=0.;double chi2Old=0.;

		Trk::Vertex linPoint ( firstStartingPoint ); // linearization point for track parameters (updated for every iteration)

		RecVertex fittedVertex;
		std::vector<VxTrackAtVertex*> tracksAtVertex;
		std::vector<BilloirTrack> billoirTracks;

		/* Iterate fits until the fit criteria are met, or the number of max
		iterations is reached. */
		for ( unsigned int niter=0; niter < m_maxIterations; ++niter )
		{
// 			msg(MSG::VERBOSE) << "Start of iteration " << niter << ", starting point ("
// 			<< linPoint.position() [0] << ", " << linPoint.position() [1] << ", " << linPoint.position() [2]
// 			<< ") and " << originalPerigees.size() << " tracks." << endreq;

			billoirTracks.clear();
			chi2Old = chi2New;
			chi2New = 0.;

			AmgMatrix(2,3) D;

			/* Linearize the track parameters wrt. starting point of the fit */
			Amg::Vector3D globalPosition ( linPoint.position() [0], linPoint.position() [1], linPoint.position() [2] );
			Trk::PerigeeSurface perigeeSurface ( globalPosition );

			BilloirVertex billoirVertex;
//                         unsigned int count(0);
			for ( std::vector<const Trk::TrackParameters*>::const_iterator iter = originalPerigees.begin() ; iter != originalPerigees.end() ; ++iter )
			{
				LinearizedTrack* linTrack = m_linFactory->linearizedTrack ( *iter, linPoint );
				if ( linTrack==0 )
				{
					ATH_MSG_DEBUG("Could not linearize track! Skipping this track!");
				}
				else
				{
					// local position
					Amg::Vector3D locXpVec = linTrack->expectedPositionAtPCA();
					locXpVec[0] = locXpVec[0] - linPoint.position()[0];
					locXpVec[1] = locXpVec[1] - linPoint.position()[1];
					locXpVec[2] = locXpVec[2] - linPoint.position()[2];
					
// 					msg(MSG::VERBOSE) << "Track: " << count << endreq;
//                                      count++;
// 					const Trk::MeasuredPerigee* tmpPerigee = dynamic_cast<const Trk::MeasuredPerigee*>(*iter);
					//AmgVector(5) expParameters = linTrack->expectedParametersAtPCA();
					
// 					msg(MSG::VERBOSE) << "locXp: " << locXpVec[0] << "\t" << locXpVec[1] << "\t" << locXpVec[2] << endreq;
					// first get the cov 2x2 sub matrix and then invert (don't get the 2x2 sub matrix of the 5x5 already inverted cov matrix)
					AmgMatrix(2,2) billoirCovMat = linTrack->expectedCovarianceAtPCA().block<2,2>(0,0);
// 					msg(MSG::VERBOSE) << "CovMatrix: " << billoirCovMat[0][0] << "\t" << billoirCovMat[0][1] << endreq;
// 					msg(MSG::VERBOSE) << "           " << billoirCovMat[1][0] << "\t" << billoirCovMat[1][1] << endreq;
					AmgMatrix(2,2) billoirWeightMat = billoirCovMat.inverse().eval();
// 					msg(MSG::VERBOSE) << "WeightMatrix: " << billoirWeightMat[0][0] << "\t" << billoirWeightMat[0][1] << endreq;
// 					msg(MSG::VERBOSE) << "              " << billoirWeightMat[1][0] << "\t" << billoirWeightMat[1][1] << endreq;
						// D matrix for d0 and z0
						D = linTrack->positionJacobian().block<2,3>(0,0);
// 						msg(MSG::VERBOSE) << "DMatrix:      " << D[0][0] << "\t" << D[0][1] << endreq;
// 						msg(MSG::VERBOSE) << "              " << D[1][0] << "\t" << D[1][1] << endreq;

						// Calculate DtWD and DtWD*x and sum them
						BilloirTrack locBilloirTrack;
						locBilloirTrack.xpVec = locXpVec;
						locBilloirTrack.DtWD = (D.transpose())*billoirWeightMat*D; 
						locBilloirTrack.chi2 = -1.0; 
						billoirVertex.DtWD_Sum += locBilloirTrack.DtWD;
						locBilloirTrack.DtWDx = ((D.transpose())*billoirWeightMat*D)*locXpVec;
						billoirVertex.DtWDx_Sum += locBilloirTrack.DtWDx;
						locBilloirTrack.originalPerigee = *iter;
						billoirTracks.push_back ( locBilloirTrack );
				
				}
			        delete linTrack; linTrack=0;
			}
			if ( billoirTracks.size() ==0 )
			{
				ATH_MSG_DEBUG("No linearized tracks left after linearization! Should not happen!");
				return 0;
			}
			if ( constraint )
			{
				// add V_del += wgtconst * (linPoint.position() - Vconst) and V_wgt +=wgtconst
				Amg::Vector3D constraintPosInBilloirFrame;
				constraintPosInBilloirFrame.setZero();
				// this will be 0 for first iteration but != 0 from second on
				constraintPosInBilloirFrame[0] = firstStartingPoint.position() [0]-linPoint.position() [0];
				constraintPosInBilloirFrame[1] = firstStartingPoint.position() [1]-linPoint.position() [1];
				constraintPosInBilloirFrame[2] = firstStartingPoint.position() [2]-linPoint.position() [2];
				billoirVertex.DtWDx_Sum += firstStartingPoint.covariancePosition().inverse().eval() *constraintPosInBilloirFrame;
				billoirVertex.DtWD_Sum  += firstStartingPoint.covariancePosition().inverse().eval();
			}

			AmgMatrix(3,3) cov_delta_V_mat = billoirVertex.DtWD_Sum.inverse( ) ;

			Amg::Vector3D delta_V = cov_delta_V_mat * billoirVertex.DtWDx_Sum;

			std::vector<BilloirTrack>::iterator BTIter;
			for ( BTIter = billoirTracks.begin(); BTIter != billoirTracks.end() ; ++BTIter )
			{
				// calculate chi2PerTrack = (xpVec - vtxPosInBilloirFrame).T * DtWD(track) * (xpVec - vtxPosInBilloirFrame); )
				Amg::Vector3D diff;
				diff.setZero();
				diff[0] = ( *BTIter ).xpVec[0] - delta_V[0];
				diff[1] = ( *BTIter ).xpVec[1] - delta_V[1];
				diff[2] = ( *BTIter ).xpVec[2] - delta_V[2];
				( *BTIter ).chi2 = diff.dot(( *BTIter ).DtWD* diff );
				if ( ( *BTIter ).chi2 < 0 )
				{
					std::cout << "VxFastFit::calculate: error in chi2_per_track\n";
					m_error = NEGTRCHI2;
					return 0;
				}
				chi2New += ( *BTIter ).chi2;
			}

			if ( constraint )
			{
				Amg::Vector3D deltaTrk;
				deltaTrk.setZero();
// last term will also be 0 again but only in the first iteration
//                  = calc. vtx in billoir frame - (    constraint pos. in billoir frame    )
				deltaTrk[0] = delta_V[0]                 - ( firstStartingPoint.position() [0] - linPoint.position() [0] );
				deltaTrk[1] = delta_V[1]                 - ( firstStartingPoint.position() [1] - linPoint.position() [1] );
				deltaTrk[2] = delta_V[2]                 - ( firstStartingPoint.position() [2] - linPoint.position() [2] );
				double chi2FromConstraint ( ( deltaTrk.transpose() * firstStartingPoint.covariancePosition().inverse().eval() * deltaTrk ) [0] );
				chi2New  += chi2FromConstraint;
			}

			/* assign new linearization point (= new vertex position in global frame) */
			Amg::Vector3D tmpPos ( linPoint.position() );
			tmpPos[0] += delta_V[0];   tmpPos[1] += delta_V[1];   tmpPos[2] += delta_V[2];
			linPoint = Trk::Vertex ( tmpPos );

//			msg(MSG::VERBOSE) << "Vertex of Iteration " << niter << " with chi2: " << chi2New << "\t old chi2: " << chi2 << endreq;
//			msg(MSG::VERBOSE) << "deltaV: ("    << delta_V[0] << ", " << delta_V[1] << ", " << delta_V[2] << ")" << endreq;
//			msg(MSG::VERBOSE) << linPoint << endreq;

			if ( chi2New < chi2 )
			{
				/* Store the vertex */
				chi2 = chi2New;
//				const AmgMatrix(3,3) * newCovarianceMatrix = &cov_delta_V_mat ;
//				const AmgMatrix(3,3) newErrorMatrix = newCovarianceMatrix->inverse().eval();
//				fittedVertex = RecVertex ( linPoint.position(), newErrorMatrix, ndf, chi2 );
                                // The cov_delta_V_mat does not need to be inverted.  -katy 2/3/16
				fittedVertex = RecVertex ( linPoint.position(), cov_delta_V_mat, ndf, chi2 );

				/* new go through vector and delete entries */
				for ( std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = tracksAtVertex.begin();
				        itr != tracksAtVertex.end(); ++itr )
				{
					delete ( *itr );
				}

				tracksAtVertex.clear();

				Amg::Vector3D pointToExtrapolateTo ( linPoint.position() [0], linPoint.position() [1], linPoint.position() [2] );
				Trk::PerigeeSurface perigeeSurface ( pointToExtrapolateTo );
				for ( BTIter = billoirTracks.begin(); BTIter != billoirTracks.end() ; ++BTIter )
				{
					// you need to extrapolate the original perigee ((*BTIter).originalPerigee) really to the
					// found vertex. The first propagation above is only to the starting point. But here we
					// want to store it wrt. to the last fitted vertex
					Trk::TrackParameters * extrapolatedPerigee = const_cast<Trk::TrackParameters*> ( m_extrapolator->extrapolate ( * ( *BTIter ).originalPerigee, perigeeSurface ) );
					if ( extrapolatedPerigee==0 )
					{
						extrapolatedPerigee = ( ( *BTIter ).originalPerigee )->clone();
						ATH_MSG_DEBUG("Could not extrapolate these track parameters to final vertex position! Storing original position as final one ...");
					}

					Trk::VxTrackAtVertex* tmpVxTrkAtVtx = new Trk::VxTrackAtVertex ( ( *BTIter ).chi2, extrapolatedPerigee, ( *BTIter ).originalPerigee ) ;
					tracksAtVertex.push_back ( tmpVxTrkAtVtx );
				}
			}

			if ( fabs ( chi2Old-chi2New ) < m_maxDchi2PerNdf * ndf )
			{
				break;
			}
		}
		Trk::VxCandidate * fittedVxCandidate = new Trk::VxCandidate ( fittedVertex, tracksAtVertex );
		ATH_MSG_VERBOSE("Final Vertex Fitted: " << fittedVxCandidate->recVertex());
		return fittedVxCandidate;
	}

	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::TrackParameters*>& perigeeList )
	{
		Vertex tmpVtx;
		return fit ( perigeeList, tmpVtx );
	}

	VxCandidate * FastVertexFitter::fit ( const std::vector<const Trk::Track*>& vectorTrk )
	{
		Vertex tmpVtx;
		return fit ( vectorTrk, tmpVtx );
	}

  //xAOD interfaced methods. Required to un-block the current situation  
 		// with the xAOD tracking design. 
 		 xAOD::Vertex * FastVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,const Vertex& startingPoint) 
 		 {                        
 		  return fit(vectorTrk, RecVertex(startingPoint));                                
 		 }//end of the xAOD starting point fit method 
 		 
 		     
 		 xAOD::Vertex * FastVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const RecVertex& constraint) 
 		 { 
 		   if(vectorTrk.size() == 0) 
 		   { 
 		    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endreq; 
 		    return 0; 
 		   } 
 		    
 		//making a list of perigee out of the vector of tracks   
 		   std::vector<const Trk::TrackParameters*> measuredPerigees; 
 		    
 		   for(std::vector<const xAOD::TrackParticle*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i) 
 		   { 
 		    const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters()); 
 		   
 		    if(tmpMeasPer!=0) measuredPerigees.push_back(tmpMeasPer); 
 		    else  msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq; 
 		   } 
 		    
 		    
 		   Trk::VxCandidate* fittedVxCandidate = fit( measuredPerigees, constraint ); 
 		 
 		//assigning the input tracks to the fitted vertex    
 		   if(fittedVxCandidate !=0) 
 		   { 
 		    if(fittedVxCandidate->vxTrackAtVertex() != 0) 
 		    { 
 		     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0) 
 		     { 
 		      for(unsigned int i = 0; i <vectorTrk.size(); ++i) 
 		      { 
 		 
 		       LinkToXAODTrackParticle * linkTT = new LinkToXAODTrackParticle; 
 		       linkTT->setElement(vectorTrk[i]); 
 		 
 		// vxtrackatvertex takes ownership! 
 		       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT); 
 		      }//end of loop for setting orig tracks in. 
 		     }//end of protection against unsuccessfull updates (no tracks were added) 
 		    }//end of vector of tracks check 
 		   }//end of pointer check 
		   
 		   xAOD::Vertex * return_vertex(0);    
 		   if(fittedVxCandidate) m_xaodConverter->createXAODVertex(*fittedVxCandidate,return_vertex); 
 		   return return_vertex; 
 		 }//end of the xAOD constrained fit method 
  
 		 


}

