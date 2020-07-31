/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        FullVertexFitter.cxx  -  Description
 ***************************************************************************/
#include "TrkVertexBilloirTools/FullVertexFitter.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkLinks/LinkToXAODTrackParticle.h" 
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "VxVertex/LinearizedTrack.h"
#include "VxVertex/VxTrackAtVertex.h"
#include <cmath>
//xAOD includes 
#include "xAODTracking/Vertex.h" 
#include "xAODTracking/TrackParticle.h" 

/* These are some local helper classes only needed for convenience, therefor
within anonymous namespace. They do contain temporary calculations of matrices
and vectors resulting from the Billoir calculation (Note: no transformation
of the perigee parameters is done anymore).*/
namespace
{
	struct BilloirTrack
	{
		BilloirTrack() : perigee ( nullptr ), originalPerigee( nullptr ), linTrack( nullptr ) {}
		virtual ~BilloirTrack() 
		{
		  // linTrack needs to be deleted
		  delete linTrack; linTrack=nullptr;
		}
                
                BilloirTrack(const BilloirTrack& arg) 
                {
                  linTrack = arg.linTrack->clone();
                  perigee = arg.perigee; // does not own it
                  originalPerigee = arg.originalPerigee; // does not own it
                  chi2    = arg.chi2   ;
                  Di_mat  = arg.Di_mat ;
                  Ei_mat  = arg.Ei_mat ;
                  Gi_mat  = arg.Gi_mat ;
                  Bi_mat  = arg.Bi_mat ;
                  Ci_inv  = arg.Ci_inv ;
                  Ui_vec  = arg.Ui_vec ;
                  BCi_mat = arg.BCi_mat;
                  Dper    = arg.Dper   ;
                }
		
		Trk::TrackParameters * perigee;
		const Trk::TrackParameters * originalPerigee;
		Trk::LinearizedTrack * linTrack;
		double chi2;
		AmgMatrix(5,3) Di_mat;
		AmgMatrix(5,3) Ei_mat;
		AmgMatrix(3,3) Gi_mat;
		AmgMatrix(3,3) Bi_mat;     // Bi = Di.T * Wi * Ei
		AmgMatrix(3,3) Ci_inv;     // Ci = (Ei.T * Wi * Ei)^-1
		Amg::Vector3D Ui_vec;     // Ui = Ei.T * Wi * dqi
		AmgMatrix(3,3) BCi_mat;       // BCi = Bi * Ci^-1
		AmgVector(5) Dper;
	};

	struct BilloirVertex
	{
                BilloirVertex()
                {
                  A_mat.setZero();
                  T_vec.setZero();
                  BCB_mat.setZero();
                  BCU_vec.setZero();
                };
		double chi2;
		unsigned int ndf;
		AmgMatrix(3,3) A_mat;              // T  = sum{Di.T * Wi * Di}
		Amg::Vector3D T_vec;              // A  = sum{Di.T * Wi * dqi}
		AmgMatrix(3,3) BCB_mat;       // BCB = sum{Bi * Ci^-1 * Bi.T}
		Amg::Vector3D BCU_vec;       // BCU = sum{Bi * Ci^-1 * Ui}
	};
}

namespace Trk
{
	StatusCode FullVertexFitter::initialize()
	{
		if ( m_extrapolator.retrieve().isFailure() )
		{
			msg(MSG::FATAL)<< "Failed to retrieve tool " << m_extrapolator << endmsg;
			return StatusCode::FAILURE;
		}
		
		
			msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
		

		if ( m_linFactory.retrieve().isFailure() )
		{
			msg(MSG::FATAL)<< "Failed to retrieve tool " << m_linFactory << endmsg;
			return StatusCode::FAILURE;
		}
		
		
			msg(MSG::INFO) << "Retrieved tool " << m_linFactory << endmsg;
		

		msg(MSG::INFO)  << "Initialize successful" << endmsg;
		return StatusCode::SUCCESS;
	}

	StatusCode FullVertexFitter::finalize()
	{
		msg(MSG::INFO)  << "Finalize successful" << endmsg;
		return StatusCode::SUCCESS;
	}

	FullVertexFitter::FullVertexFitter ( const std::string& t, const std::string& n, const IInterface*  p ) : base_class ( t,n,p ),
														  m_maxIterations ( 5 ),
														  m_maxDchi2PerNdf ( 0.000001 ),
														  m_extrapolator ( "Trk::Extrapolator" )

	{
		declareProperty ( "MaxIterations",        m_maxIterations );
		declareProperty ( "MaxChi2PerNdf",        m_maxDchi2PerNdf );
		declareProperty ( "Extrapolator",         m_extrapolator );
		declareProperty ( "LinearizedTrackFactory", m_linFactory );
		declareInterface<IVertexFitter> ( this );
	}

	FullVertexFitter::~FullVertexFitter() = default;

	/** Interface for ParametersBase with starting point */
	xAOD::Vertex * FullVertexFitter::fit ( const std::vector<const Trk::TrackParameters*> & originalPerigees,
	                                       const Amg::Vector3D& firstStartingPoint ) const
	{
                xAOD::Vertex constraint;
                constraint.makePrivateStore();
                constraint.setPosition( firstStartingPoint );
                constraint.setCovariancePosition( AmgSymMatrix(3)(3,3) );
                constraint.setFitQuality( 0.,0.);
		return fit ( originalPerigees, constraint );
	}

	/** Interface for ParametersBase with vertex constraint */
	/** the position of the constraint is ALWAYS the starting point */
	xAOD::Vertex * FullVertexFitter::fit ( const std::vector<const Trk::TrackParameters*> & originalPerigees,
	                                       const xAOD::Vertex& firstStartingPoint ) const
	{
		if ( originalPerigees.empty() )
		{
			ATH_MSG_VERBOSE("No tracks to fit in this event.");
			return nullptr;
		}

		/* Initialisation of variables */
		double chi2 = 2000000000000.;
		unsigned int nRP = originalPerigees.size();  // Number of tracks to fit
		int ndf = nRP * ( 5-3 ) - 3;            // Number of degrees of freedom
		if ( ndf < 0 ) {ndf = 1;}

		/* Determine if we are doing a constraint fit.*/
		bool constraint = false;
		if ( firstStartingPoint.covariancePosition().trace() != 0. )
		{
			constraint = true;
			ndf += 3;
			ATH_MSG_DEBUG("Fitting with constraint.");
			ATH_MSG_VERBOSE(firstStartingPoint.covariancePosition().inverse());
		}

		double chi2New=0.;

		Amg::Vector3D linPoint ( firstStartingPoint.position() ); // linearization point for track parameters (updated for every iteration)

		Amg::Vector3D p0;

		std::vector<Amg::Vector3D> mom_at_Origin;

		xAOD::Vertex * fittedVertex = new xAOD::Vertex;
                fittedVertex->makePrivateStore(); // xAOD::VertexContainer will take ownership of AuxStore when ActualVertex is added to it

		std::vector<VxTrackAtVertex> tracksAtVertex;
		std::vector<BilloirTrack> billoirTracks;

		/* Iterate fits until the fit criteria are met, or the number of max
		iterations is reached. */
		for ( unsigned int niter=0; niter < m_maxIterations; ++niter )
		{
			ATH_MSG_VERBOSE("Start of iteration " << niter << ", starting point ("
			<< linPoint [0] << ", " << linPoint [1] << ", " << linPoint [2]
			<< ") and " << originalPerigees.size() << " tracks.");

			billoirTracks.clear();
			chi2New = 0.;

			AmgMatrix(2,3) D;
			BilloirVertex billoirVertex;

			/* Linearize the track parameters wrt. starting point of the fit */
			Amg::Vector3D globalPosition = linPoint;
			Trk::PerigeeSurface perigeeSurface ( globalPosition );

			/* Extrapolate the perigees to the startpoint of the fit */
			unsigned int count(0);
			for ( std::vector<const Trk::TrackParameters*>::const_iterator iter = originalPerigees.begin(); iter != originalPerigees.end() ; ++iter )
			{

				if ( niter == 0 )
				{
					// need to cast to access parameters() (this is to guarantee that the code knows if it is neutrals or charged parameters)
					const Trk::TrackParameters* chargedParameters ( nullptr );
					chargedParameters = dynamic_cast<const Trk::TrackParameters*> ( *iter );
					if ( chargedParameters==nullptr )
					{
						ATH_MSG_ERROR("Track parameters are not charged tracks ... full fit aborted (this will be handled correctly soon)");
						return nullptr;
					}
					
					
						p0.x() =  chargedParameters->parameters() [Trk::phi];
						p0.y() = chargedParameters->parameters() [Trk::theta];
						p0.z() = chargedParameters->parameters() [Trk::qOverP] ;
						mom_at_Origin.push_back ( p0 );
					
				}

				// FOR NOW THE ONLY WAY TO CHECK IF EXTRAPOLATION WORKED IS TO CHECK THE RETURN POINTER
				// if it is ZERO then take the original perigee
				// there is a tiny tiny chance that the extrapolation fails because of a non converging runge kutta procedure
				// in all other cases it did not extrapolate because the reference surface of the original perigee
				// is already given to the extrapolation point (or very close nearby)

				LinearizedTrack* linTrack = m_linFactory->linearizedTrack ( *iter, linPoint );
				if ( linTrack==nullptr )
				{
					ATH_MSG_DEBUG("Could not linearize track! Skipping this track!");
				}
				else
				{
					BilloirTrack locBilloirTrack;
					
					locBilloirTrack.originalPerigee = *iter;
                                        locBilloirTrack.linTrack = linTrack;
					double d0 = linTrack->expectedParametersAtPCA()[Trk::d0];
					double z0 = linTrack->expectedParametersAtPCA()[Trk::z0];
					double phi = linTrack->expectedParametersAtPCA()[Trk::phi];
					double theta = linTrack->expectedParametersAtPCA()[Trk::theta];
					double qOverP = linTrack->expectedParametersAtPCA()[Trk::qOverP];

					// calculate f(V_0,p_0)  f_d0 = f_z0 = 0
					double f_phi = mom_at_Origin[count] ( 0 );
					double f_theta = mom_at_Origin[count] ( 1 );
					double f_qOverP = mom_at_Origin[count] ( 2 );

					//calculate Dper[i]
					locBilloirTrack.Dper[0] = d0;
					locBilloirTrack.Dper[1] = z0;
					locBilloirTrack.Dper[2] = phi - f_phi;
					locBilloirTrack.Dper[3] = theta - f_theta;
					locBilloirTrack.Dper[4] = qOverP -f_qOverP;

					// the D matrix
					AmgMatrix(5,3) D_mat;
					D_mat.setZero();
					D_mat = linTrack->positionJacobian();

					// calculate the E matrix
					AmgMatrix(5,3) E_mat;
					E_mat.setZero();
					E_mat = linTrack->momentumJacobian();

					// and cache martix operations related to these matrices
					AmgMatrix(3,5) Dt_W_mat;
					Dt_W_mat.setZero();
					AmgMatrix(3,5) Et_W_mat;
					Et_W_mat.setZero();
					Dt_W_mat = D_mat.transpose() * ( linTrack->expectedWeightAtPCA() );
					Et_W_mat = E_mat.transpose() * ( linTrack->expectedWeightAtPCA() );

					// these are the Billoir computed results
					locBilloirTrack.Di_mat = D_mat;
					locBilloirTrack.Ei_mat = E_mat;
					locBilloirTrack.Gi_mat = Et_W_mat*E_mat;
					locBilloirTrack.Bi_mat = Dt_W_mat * E_mat; // Di.T * Wi * Ei
					locBilloirTrack.Ui_vec = Et_W_mat * locBilloirTrack.Dper; // Ei.T * Wi * dqi
					locBilloirTrack.Ci_inv = Et_W_mat * E_mat ; // (Ei.T * Wi * Ei)^-1
					// we need the inverse matrix here
					locBilloirTrack.Ci_inv.inverse().eval();
						// sum up over all tracks
						billoirVertex.T_vec       += Dt_W_mat * locBilloirTrack.Dper; // sum{Di.T * Wi * dqi}
						billoirVertex.A_mat = billoirVertex.A_mat + Dt_W_mat * D_mat ; // sum{Di.T * Wi * Di}

						// remember those results for all tracks
						locBilloirTrack.BCi_mat = locBilloirTrack.Bi_mat * locBilloirTrack.Ci_inv; // BCi = Bi * Ci^-1

						// and some summed results
						billoirVertex.BCU_vec += locBilloirTrack.BCi_mat * locBilloirTrack.Ui_vec; // sum{Bi * Ci^-1 * Ui}
						billoirVertex.BCB_mat =  billoirVertex.BCB_mat + locBilloirTrack.BCi_mat * locBilloirTrack.Bi_mat.transpose() ;// sum{Bi * Ci^-1 * Bi.T}
						billoirTracks.push_back ( locBilloirTrack );
                			        count++;

				}
			}

			// calculate delta (billoirFrameOrigin-position), might be changed by the beam-const
			Amg::Vector3D    V_del     = billoirVertex.T_vec - billoirVertex.BCU_vec;      // V_del = T-sum{Bi*Ci^-1*Ui}
			AmgMatrix(3,3) V_wgt_mat = billoirVertex.A_mat - billoirVertex.BCB_mat;      // V_wgt = A-sum{Bi*Ci^-1*Bi.T}

			if ( constraint )
			{
				// add V_del += wgtconst * (billoirFrameOrigin - Vconst) and V_wgt +=wgtconst
				Amg::Vector3D constraintPosInBilloirFrame;
				constraintPosInBilloirFrame.setZero();
				// this will be 0 for first iteration but != 0 from second on
				constraintPosInBilloirFrame[0] = firstStartingPoint.position() [0]-linPoint [0];
				constraintPosInBilloirFrame[1] = firstStartingPoint.position() [1]-linPoint [1];
				constraintPosInBilloirFrame[2] = firstStartingPoint.position() [2]-linPoint [2];

				V_del     += firstStartingPoint.covariancePosition().inverse() *constraintPosInBilloirFrame;
				V_wgt_mat += firstStartingPoint.covariancePosition().inverse();
			}
			// cov(delta_V) = V_wgt^-1
			AmgMatrix(3,3) cov_delta_V_mat = V_wgt_mat.inverse().eval() ;

			// delta_V = cov_(delta_V) * V_del;
			Amg::Vector3D delta_V = cov_delta_V_mat * V_del;
			/* Momentum Calculation follows here. */
			/* ---------------------------------------------------------------------------------------- */
			std::vector<AmgMatrix(5,5)> cov_delta_P_mat ( nRP );
			std::vector<double> chi2PerTrack;
			chi2PerTrack.clear();

			// ===> loop over tracks
			std::vector<BilloirTrack>::iterator BTIter;
			int iRP =0;
			for ( BTIter = billoirTracks.begin(); BTIter != billoirTracks.end() ; ++BTIter )
			{
				// delta_P calculation
				BilloirTrack locP ( ( *BTIter ) );
				Amg::Vector3D delta_P = ( locP.Ci_inv ) * ( ( locP.Ui_vec )- ( locP.Bi_mat.transpose() ) *delta_V );
				mom_at_Origin[iRP] ( 0 ) += delta_P[0];
				if ( fabs ( mom_at_Origin[iRP] ( 0 ) ) > 10*M_PI )
				{ // protect while loop
					if (msgLvl(MSG::WARNING))
                                        {
                                          msg(MSG::WARNING) << " Track direction angles have numerical problems, stop perigee parameter update." << endmsg;
					  msg(MSG::WARNING) << " Phi value: "<<mom_at_Origin[iRP] ( 0 ) <<endmsg;
                                        }
					return nullptr;
				}
				mom_at_Origin[iRP] ( 1 ) += delta_P[1];
				if ( fabs ( mom_at_Origin[iRP] ( 0 ) ) > 5*M_PI )
				{ // protect while loop
                                        if (msgLvl(MSG::WARNING))
                                        {
                                          msg(MSG::WARNING) << " Track direction angles have numerical problems, stop perigee parameter update." << endmsg;
					  msg(MSG::WARNING) << " Theta value: "<<mom_at_Origin[iRP] ( 1 ) <<endmsg;
                                        }
                                        return nullptr;
				}
				mom_at_Origin[iRP] ( 2 ) += delta_P[2];
				// 	mom_at_Origin[iRP](0) -= M_PI > M_PI ? M_PI : 0;
				// 	mom_at_Origin[iRP](0) += M_PI < -M_PI ? M_PI : 0;
				//correct phi, theta coordinates
				while ( fabs ( mom_at_Origin[iRP] ( 0 ) ) > M_PI ) mom_at_Origin[iRP] ( 0 ) += ( mom_at_Origin[iRP] ( 0 ) > 0 ) ? -2*M_PI : 2*M_PI;
				while ( mom_at_Origin[iRP] ( 1 ) > 2*M_PI ) mom_at_Origin[iRP] ( 1 ) -= 2*M_PI;
				while ( mom_at_Origin[iRP] ( 1 ) < -M_PI ) mom_at_Origin[iRP] ( 1 ) += M_PI;
				if ( mom_at_Origin[iRP] ( 1 ) > M_PI )
				{
					mom_at_Origin[iRP] ( 1 ) = 2*M_PI - mom_at_Origin[iRP] ( 1 );
                                        // cppcheck-suppress identicalInnerCondition; false positive
					if ( mom_at_Origin[iRP] ( 0 ) >= 0 )   mom_at_Origin[iRP] ( 0 )  += ( mom_at_Origin[iRP] ( 0 ) >0 ) ? -M_PI : M_PI;
				}
				if ( mom_at_Origin[iRP] ( 1 ) < 0.0 )
				{
					mom_at_Origin[iRP] ( 1 )  = - mom_at_Origin[iRP] ( 1 );
                                        // cppcheck-suppress identicalInnerCondition; false positive
					if ( mom_at_Origin[iRP] ( 0 ) >= 0 )  mom_at_Origin[iRP] ( 0 ) += ( mom_at_Origin[iRP] ( 0 ) >0 ) ? -M_PI : M_PI;
				}

				//calculate 5x5 cov_delta_P matrix
				// d(d0,z0,phi,theta,qOverP)/d(x,y,z,phi,theta,qOverP)-transformation matrix
				AmgMatrix(5,6) trans_mat;
				trans_mat.setZero();
				trans_mat ( 1,1 ) = locP.Di_mat ( 1,1 ); trans_mat ( 1,2 ) = locP.Di_mat ( 1,2 );
				trans_mat ( 2,1 ) = locP.Di_mat ( 2,1 ); trans_mat ( 2,2 ) = locP.Di_mat ( 2,2 ); trans_mat ( 2,3 ) = 1.;
				trans_mat ( 3,4 ) = 1.; trans_mat ( 4,5 ) = 1.; trans_mat ( 5,6 ) = 1.;

				//some intermediate calculations to get 5x5 matrix
				//cov(V,V)
				AmgMatrix(3,3) V_V_mat; V_V_mat.setZero();  V_V_mat = cov_delta_V_mat ;
				//          std::cout<<"V_V_mat = "<<V_V_mat<<std::endl;
				//cov(V,P)
				AmgMatrix(3,3) V_P_mat; V_P_mat.setZero(); V_P_mat =  -cov_delta_V_mat*locP.Gi_mat*locP.Ci_inv ;
				//          std::cout<<"V_P_mat = "<<V_P_mat<<std::endl;
				//cov(P,P)
				AmgMatrix(3,3) P_P_mat; P_P_mat.setZero(); P_P_mat =  locP.Ci_inv + locP.BCi_mat.transpose() *cov_delta_V_mat*locP.BCi_mat ;
				//          std::cout<<"P_P_mat = "<<P_P_mat<<std::endl;

				AmgMatrix(6,6) cov_mat;
				cov_mat.setZero();
				cov_mat.block<3,3>(0,3) = V_P_mat;
				cov_mat.block<3,3>(3,0) = V_P_mat.transpose() ;
				cov_mat.block<3,3>(0,0) = V_V_mat ;
				cov_mat.block<3,3>(3,3) = P_P_mat ;

				// cov_delta_P calculation
				//          std::cout<<"trans_mat = "<<trans_mat<<std::endl;
				cov_delta_P_mat[iRP] = trans_mat*cov_mat*trans_mat.transpose() ;
				//          std::cout<<"cov_delta_P_mat[iRP] = "<<cov_delta_P_mat[iRP]<<std::endl;

				++iRP;

				/* Calculate chi2 per track. */
				( *BTIter ).chi2= ( ( ( *BTIter ).Dper- ( *BTIter ).Di_mat*delta_V - ( *BTIter ).Ei_mat*delta_P ).transpose() * ( *BTIter ).linTrack->expectedWeightAtPCA() * ( ( *BTIter ).Dper - ( *BTIter ).Di_mat*delta_V - ( *BTIter ).Ei_mat*delta_P ) ) [0];

				if ( ( *BTIter ).chi2 < 0 )
				{
					ATH_MSG_WARNING( "FullVertexFitter::calculate: error in chi2_per_track" );
					return nullptr;
				}
				chi2New += ( *BTIter ).chi2;
			}

			if ( constraint )
			{
				Amg::Vector3D deltaTrk;
				deltaTrk.setZero();
				// last term will also be 0 again but only in the first iteration
				//                  = calc. vtx in billoir frame - (    constraint pos. in billoir frame    )
				deltaTrk[0] = delta_V[0] - ( firstStartingPoint.position() [0] - linPoint [0] );
				deltaTrk[1] = delta_V[1] - ( firstStartingPoint.position() [1] - linPoint [1] );
				deltaTrk[2] = delta_V[2] - ( firstStartingPoint.position() [2] - linPoint [2] );
				chi2New  += ( deltaTrk.transpose() * firstStartingPoint.covariancePosition().inverse() * deltaTrk ) [0];
			}

			/* assign new linearization point (= new vertex position in global frame) */
			Amg::Vector3D tmpPos ( linPoint );
			tmpPos[0] += delta_V[0];   tmpPos[1] += delta_V[1];   tmpPos[2] += delta_V[2];
			linPoint = tmpPos;

			if ( chi2New < chi2 )
			{
				/* Store the vertex */
				chi2 = chi2New;
				//const AmgMatrix(3,3) * newCovarianceMatrix =  &cov_delta_V_mat ;
				//const AmgMatrix(3,3) newErrorMatrix = newCovarianceMatrix->inverse().eval();
				//fittedVertex = RecVertex ( linPoint.position(), newErrorMatrix, ndf, chi2 );

                                //the cov_delta_V_mat is already the inverted form.  -katy 2/2/16
				fittedVertex->setPosition( linPoint );
                                fittedVertex->setCovariancePosition( cov_delta_V_mat );
                                fittedVertex->setFitQuality( chi2, ndf );

				// new go through vector and delete entries
				/* // TODO: not needed anymore, tracksAtVertex doesn't store pointers - just the objects themselves <David Shope> (EDM Migration) 03/21/16
				for ( std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = tracksAtVertex.begin();
				        itr != tracksAtVertex.end(); ++itr )
				{
					delete ( *itr );
				}
				*/

				tracksAtVertex.clear();
				/* Store the tracks at vertex */
				Amg::Vector3D Vertex(linPoint[0], linPoint[1], linPoint[2]);
				const Trk::PerigeeSurface Surface ( Vertex );
				TrackParameters * refittedPerigee ( nullptr );
				unsigned int iter= 0;
				std::vector<BilloirTrack>::iterator BTIter;
				for ( BTIter = billoirTracks.begin(); BTIter != billoirTracks.end() ; ++BTIter )
				{
					const AmgMatrix(5,5) * newTrackCovarianceMatrix =  &cov_delta_P_mat[iter] ;
					//Covariance matrix does not need to be inverted:
					//					AmgMatrix(5,5)  newTrackErrorMatrix = (AmgMatrix(5,5)) newTrackCovarianceMatrix->inverse().eval();
					AmgMatrix(5,5)  newTrackErrorMatrix = (AmgMatrix(5,5)) newTrackCovarianceMatrix->eval();
					refittedPerigee = new Trk::Perigee ( 0.,0.,mom_at_Origin[iter][0],mom_at_Origin[iter][1],mom_at_Origin[iter][2], Surface, &newTrackErrorMatrix );
					Trk::VxTrackAtVertex* tmpVxTrkAtVtx = new Trk::VxTrackAtVertex ( ( *BTIter ).chi2, refittedPerigee, ( *BTIter ).originalPerigee );
					tracksAtVertex.push_back ( *tmpVxTrkAtVtx );
					// TODO: here is where the vxTracksAtVertex pointers are deleted
					delete tmpVxTrkAtVtx; // TODO: is this ok?
					iter ++;
				}
			}
		} // end of iteration
		fittedVertex->vxTrackAtVertex() = tracksAtVertex;
		//ATH_MSG_VERBOSE("Final Vertex Fitted: " << fittedVxCandidate->recVertex()); // TODO: can no longer print vertex after converting to xAOD
		return fittedVertex;
	}

	xAOD::Vertex * FullVertexFitter::fit ( const std::vector<const Trk::TrackParameters*>& perigeeList ) const
	{
                Amg::Vector3D tmpVtx(0.,0.,0.);
                return fit ( perigeeList, tmpVtx );
	}


 		//xAOD interfaced methods. Required to un-block the current situation  
 		// with the xAOD tracking design. 
 		 xAOD::Vertex * FullVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,const Amg::Vector3D& startingPoint)  const
 		 {
                  xAOD::Vertex constraint;
                  constraint.makePrivateStore();
                  constraint.setPosition( startingPoint );
                  constraint.setCovariancePosition( AmgSymMatrix(3)(3,3) );
                  constraint.setFitQuality( 0.,0.);
 		  return fit(vectorTrk, constraint);                                
 		 }//end of the xAOD starting point fit method 
 		 
 		     
 		 xAOD::Vertex * FullVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const xAOD::Vertex& constraint) const
 		 { 
 		   if(vectorTrk.empty()) 
 		   { 
 		    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endmsg; 
 		    return nullptr; 
 		   } 
 		    
 		   //making a list of perigee out of the vector of tracks   
 		   std::vector<const Trk::TrackParameters*> measuredPerigees; 
 		    
 		   for(std::vector<const xAOD::TrackParticle*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i) 
 		   { 
 		    const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters()); 
 		   
 		    if(tmpMeasPer!=nullptr) measuredPerigees.push_back(tmpMeasPer); 
 		    else  msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endmsg; //TODO: Failed to implicit cast the perigee parameters to track parameters?
 		   } 
 		    
 		    
 		   xAOD::Vertex* fittedVertex = fit( measuredPerigees, constraint ); 
 		 
 		   //assigning the input tracks to the fitted vertex through VxTrackAtVertices
 		   if(fittedVertex !=nullptr) 
 		   { 
 		    if( fittedVertex->vxTrackAtVertexAvailable() ) // TODO: I don't think vxTrackAtVertexAvailable() does the same thing as a null pointer check! 
 		    { 
 		     if(!fittedVertex->vxTrackAtVertex().empty()) 
 		     { 
 		      for(unsigned int i = 0; i <vectorTrk.size(); ++i) 
 		      { 
 		 
 		       LinkToXAODTrackParticle * linkTT = new LinkToXAODTrackParticle; 
 		       linkTT->setElement(vectorTrk[i]); 
 		 
 		       // vxtrackatvertex takes ownership! 
 		       ( fittedVertex->vxTrackAtVertex() )[i].setOrigTrack(linkTT); 
 		      }//end of loop for setting orig tracks in. 
 		     }//end of protection against unsuccessfull updates (no tracks were added) 
 		    }//end of vector of tracks check 
 		   }//end of pointer check

                   //now set links to xAOD::TrackParticles directly in the xAOD::Vertex
                   unsigned int VTAVsize = fittedVertex->vxTrackAtVertex().size();
                   for (unsigned int i = 0 ; i < VTAVsize ; ++i)
                   {
                     Trk::VxTrackAtVertex* VTAV = &( fittedVertex->vxTrackAtVertex().at(i) );
                     //TODO: Will this pointer really hold 0 if no VxTrackAtVertex is found?
                     if (not VTAV){
                       ATH_MSG_WARNING (" Trying to set link to xAOD::TrackParticle. The VxTrackAtVertex is not found");
                       continue;
                     }

                     Trk::ITrackLink* trklink = VTAV->trackOrParticleLink();

                     // See if the trklink is to an xAOD::TrackParticle
                     Trk::LinkToXAODTrackParticle* linkToXAODTP = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trklink);
                     if (linkToXAODTP)
                     {

                       //Now set the new link to the xAOD vertex
                       fittedVertex->addTrackAtVertex(*linkToXAODTP, VTAV->weight());

                     } else {
                       ATH_MSG_WARNING ("Skipping track. Trying to set link to something else than xAOD::TrackParticle. Neutrals not supported.");
                     }
                   } //end of loop

 		   return fittedVertex;

 		 }//end of the xAOD constrained fit method 



}


