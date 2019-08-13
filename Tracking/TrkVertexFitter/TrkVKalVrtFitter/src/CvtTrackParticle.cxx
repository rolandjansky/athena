/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//  Convert TrackParticle parameters to internal VKalVrt parameters
// and sets up common reference system for ALL tracks 
// even if in the beginning in was different
//------------------------------------------------------------------ 
// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
//-------------------------------------------------
// Other stuff
//----
#include  "TrkParameters/TrackParameters.h"
#include  "TrkNeutralParameters/NeutralParameters.h"
//----
#include <iostream> 

namespace Trk {

 extern const vkalPropagator  myPropagator;

//--------------------------------------------------------------------
//  Extract xAOD::TrackParticles
//

 StatusCode
 TrkVKalVrtFitter::CvtTrackParticle(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                    int& ntrk,
                                    State& state)
 {

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee*        mPer=0;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,BMAG_FIXED;
//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
     state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.;
     state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     Amg::Vector3D perGlobalPos;
     state.m_trkControl.clear(); state.m_trkControl.reserve(InpTrk.size());
     for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//-- (Measured)Perigee in xAOD::TrackParticle
       mPer = &(*i_ntrk)->perigeeParameters(); 
       if( mPer==0 ) continue; // No perigee!!!
       perGlobalPos =  mPer->position();    //Global position of perigee point
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;	// Reference system calculation
       tmp_refFrameY += perGlobalPos.y() ;	// Use hit position itself to get more precise 
       tmp_refFrameZ += perGlobalPos.z() ;	// magnetic field
       TrkMatControl tmpMat;
       tmpMat.trkRefGlobPos=Amg::Vector3D( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.extrapolationType=2;                   // Perigee point strategy
       tmpMat.TrkPnt=mPer; 
       tmpMat.prtMass = 139.5702;
       if(counter<(int)state.m_MassInputParticles.size())tmpMat.prtMass = state.m_MassInputParticles[counter];
       tmpMat.TrkID=counter; state.m_trkControl.push_back(tmpMat);
       counter++;
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;
    tmp_refFrameZ /= counter;
    Amg::Vector3D refGVertex (tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);

    PerigeeSurface surfGRefPoint( refGVertex );       // Reference perigee surface for current fit
//
//std::cout.setf( std::ios::scientific); std::cout.precision(9);
//std::cout<<" VK ref.frame="<<tmp_refFrameX<<", "<<tmp_refFrameY<<", "<<tmp_refFrameZ<<'\n';
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//

    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- (Measured)Perigee in TrackParticle
//
       mPer = &(*i_ntrk)->perigeeParameters(); 
       if( mPer==0 ) continue; // No perigee!!!
       perGlobalPos =  mPer->position();    //Global position of perigee point
       if( !convertAmg5SymMtx(mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!;
       state.m_fitField.getMagFld( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(),           // Magnetic field
                                                                  fx, fy, BMAG_FIXED);      // at the track perigee point
       if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;
//
//--- Move ref. frame to the track common point refGVertex
//    Small beamline inclination doesn't change track covariance matrix 
       AmgSymMatrix(5) * tmpCov = new AmgSymMatrix(5)(*(mPer->covariance()));
       const Perigee * tmpPer=surfGRefPoint.createTrackParameters(mPer->position(),mPer->momentum(),mPer->charge(),tmpCov);
       VectPerig    =  tmpPer->parameters(); 
//std::cout.precision(12);
//std::cout<<"beamtilt="<<(*i_ntrk)->beamlineTiltX()<<", "<<(*i_ntrk)->beamlineTiltY()<<'\n';
//std::cout<<" Def per==>"<<(*mPer)<<'\n';
//std::cout<<" Def srf==>"<<mPer->associatedSurface()<<'\n';
//std::cout<<" Def rot==>"<<mPer->associatedSurface().transform().rotation()<<'\n';
//std::cout<<" Def trn==>"<<mPer->associatedSurface().transform().translation()<<'\n';
//std::cout<<" New per==>"<<(*tmpPer)<<'\n';
//std::cout<<" New per==>"<<tmpPer->momentum()<<'\n';
//std::cout<<" New srf==>"<<tmpPer->associatedSurface()<<'\n';
//std::cout<<" New rot==>"<<tmpPer->associatedSurface().transform().rotation()<<'\n';
//std::cout<<" New trn==>"<<tmpPer->associatedSurface().transform().translation()<<'\n';
//
//--- Transform to internal parametrisation
       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     state.m_ich[ntrk],&state.m_apar[ntrk][0],&state.m_awgt[ntrk][0]);
       delete tmpPer; //tmpCov matrix is deleted here!!!
//
       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    state.m_refFrameX=tmp_refFrameX;
    state.m_refFrameY=tmp_refFrameY;
    state.m_refFrameZ=tmp_refFrameZ;
    state.m_fitField.setAtlasMagRefFrame( state.m_refFrameX, state.m_refFrameY, state.m_refFrameZ);

    return StatusCode::SUCCESS;
  }

//--------------------------------------------------------------------
//  Extract xAOD::NeutralParticles
//

 StatusCode
 TrkVKalVrtFitter::CvtNeutralParticle(const std::vector<const xAOD::NeutralParticle*>& InpTrk,
                                      int& ntrk,
                                      State& state)
 {
    std::vector<const xAOD::NeutralParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const  NeutralPerigee*        mPer=0;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,BMAG_FIXED;
//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
     state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.;
     state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     Amg::Vector3D perGlobalPos;
     state.m_trkControl.clear(); state.m_trkControl.reserve(InpTrk.size());
     for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//-- (Measured)Perigee in xAOD::NeutralParticle
       mPer = &(*i_ntrk)->perigeeParameters(); 
       if( mPer==0 ) continue; // No perigee!!!
       perGlobalPos =  mPer->position();    //Global position of perigee point
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;	// Reference system calculation
       tmp_refFrameY += perGlobalPos.y() ;	// Use hit position itself to get more precise 
       tmp_refFrameZ += perGlobalPos.z() ;	// magnetic field
       TrkMatControl tmpMat;
       tmpMat.trkRefGlobPos=Amg::Vector3D( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.extrapolationType=2;                   // Perigee point strategy
       tmpMat.TrkPnt=NULL;           //No reference point for neutral particle for the moment
       tmpMat.prtMass = 139.5702;
       if(counter<(int)state.m_MassInputParticles.size())tmpMat.prtMass = state.m_MassInputParticles[counter];
       tmpMat.TrkID=counter; state.m_trkControl.push_back(tmpMat);
       counter++;
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;
    tmp_refFrameZ /= counter;
    Amg::Vector3D refGVertex (tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);
    PerigeeSurface surfGRefPoint( refGVertex );       // Reference perigee surface for current fit
//
//std::cout.setf( std::ios::scientific); std::cout.precision(5);
//std::cout<<" VK ref.frame="<<tmp_refFrameX<<", "<<tmp_refFrameY<<", "<<tmp_refFrameZ<<'\n';
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//

    state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.;        //set ATLAS frame
    state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);  //set ATLAS frame
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- (Measured)Perigee in TrackParticle
//
       mPer = &(*i_ntrk)->perigeeParameters(); 
       if( mPer==0 ) continue; // No perigee!!!
       perGlobalPos =  mPer->position();    //Global position of perigee point
       if( !convertAmg5SymMtx(mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!;
       state.m_fitField.getMagFld( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(),         // Magnetic field
                                                                  fx, fy, BMAG_FIXED);      // at track perigee point
       if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;

//
//--- Move ref. frame to the track common point refGVertex
//    Small beamline inclination doesn't change track covariance matrix 
//
       AmgSymMatrix(5) * tmpCov = new AmgSymMatrix(5)(*(mPer->covariance()));
       const Perigee * tmpPer=surfGRefPoint.createTrackParameters(mPer->position(),mPer->momentum(),mPer->charge(),tmpCov);
       VectPerig    =  tmpPer->parameters(); 
       //--- Transform to internal parametrisation
       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     state.m_ich[ntrk],&state.m_apar[ntrk][0],&state.m_awgt[ntrk][0]);
       state.m_ich[ntrk]=0;
       if(state.m_apar[ntrk][4]<0){ state.m_apar[ntrk][4]  = -state.m_apar[ntrk][4];      // Charge=0 is always equal to Charge=+1
                              state.m_awgt[ntrk][10] = -state.m_awgt[ntrk][10];
                              state.m_awgt[ntrk][11] = -state.m_awgt[ntrk][11];
                              state.m_awgt[ntrk][12] = -state.m_awgt[ntrk][12];
                              state.m_awgt[ntrk][13] = -state.m_awgt[ntrk][13]; }
       delete tmpPer; //tmpCov matrix is deleted here!!!
//

       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    state.m_refFrameX=tmp_refFrameX;
    state.m_refFrameY=tmp_refFrameY;
    state.m_refFrameZ=tmp_refFrameZ;
    state.m_fitField.setAtlasMagRefFrame( state.m_refFrameX, state.m_refFrameY, state.m_refFrameZ);

    return StatusCode::SUCCESS;
  }




//--------------------------------------------------------------------
//  Extract Trk::TrackParticlesBase
//

 StatusCode
 TrkVKalVrtFitter::CvtTrackParticle(const std::vector<const TrackParticleBase*>& InpTrk,
                                    int& ntrk,
                                    State& state)
 {

    std::vector<const TrackParticleBase*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee*        mPer=0;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,BMAG_FIXED;
//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
     state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.;
     state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     Amg::Vector3D perGlobalPos;
     state.m_trkControl.clear(); state.m_trkControl.reserve(InpTrk.size());
     for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//-- (Measured)Perigee in TrackParticle
       mPer = GetPerigee(*i_ntrk); 
       if( mPer==0 ) continue; // No perigee!!!
       perGlobalPos =  mPer->position();    //Global position of perigee point
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;	// Reference system calculation
       tmp_refFrameY += perGlobalPos.y() ;	// Use hit position itself to get more precise 
       tmp_refFrameZ += perGlobalPos.z() ;	// magnetic field
       TrkMatControl tmpMat;
       tmpMat.trkRefGlobPos=Amg::Vector3D( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.extrapolationType=2;                   // Perigee point strategy
       tmpMat.TrkPnt=mPer;
       tmpMat.prtMass = 139.5702;
       if(counter<(int)state.m_MassInputParticles.size())tmpMat.prtMass = state.m_MassInputParticles[counter];
       tmpMat.TrkID=counter; state.m_trkControl.push_back(tmpMat);
       counter++;
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;
    tmp_refFrameZ /= counter;
//
//std::cout.setf( std::ios::scientific); std::cout.precision(5);
//std::cout<<" VK ref.frame="<<tmp_refFrameX<<", "<<tmp_refFrameY<<", "<<tmp_refFrameZ<<'\n';
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//

    Amg::Vector3D perGlobalVrt;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
       long int TrkID=ntrk;
//
//-- (Measured)Perigee in TrackParticle
//
       mPer = GetPerigee(*i_ntrk);
       if( mPer==0 ) continue; // No perigee!!!
       VectPerig    =  mPer->parameters(); 
       perGlobalVrt =  mPer->associatedSurface().center();      //Global position of reference point
       perGlobalPos =  mPer->position();    //Global position of perigee point
       if( !convertAmg5SymMtx(mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!;
       state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.; state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame
       state.m_fitField.getMagFld( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(), // Magnetic field
                              fx, fy, BMAG_FIXED);                                // at track perigee point
       if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;


       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     state.m_ich[ntrk],&state.m_apar[ntrk][0],&state.m_awgt[ntrk][0]);
//std::cout<< "New="<<state.m_awgt[ntrk][0]<<", "<<state.m_awgt[ntrk][1]<<", "<<state.m_awgt[ntrk][2]<<", "<<state.m_awgt[ntrk][3]<<'\n';
//std::cout<< "New="<<state.m_awgt[ntrk][11]<<", "<<state.m_awgt[ntrk][12]<<", "<<state.m_awgt[ntrk][13]<<", "<<state.m_awgt[ntrk][14]<<'\n';
//std::cout<<"-------------------------------------"<<'\n';
//
//
// Check if propagation to common reference point is needed and make it
       state.m_refFrameX=perGlobalVrt.x();  // initial track reference position
       state.m_refFrameY=perGlobalVrt.y();
       state.m_refFrameZ=perGlobalVrt.z();
       state.m_fitField.setAtlasMagRefFrame( state.m_refFrameX, state.m_refFrameY, state.m_refFrameZ);
       double dX=tmp_refFrameX-perGlobalVrt.x();   // Track shift for VKalVrtCore
       double dY=tmp_refFrameY-perGlobalVrt.y();
       double dZ=tmp_refFrameZ-perGlobalVrt.z();
       if(fabs(dX)+fabs(dY)+fabs(dZ) != 0.) {
	  double pari[5],covi[15]; double vrtini[3]={0.,0.,0.}; double vrtend[3]={dX,dY,dZ};
	  for(int i=0; i<5; i++) pari[i]=state.m_apar[ntrk][i];
	  for(int i=0; i<15;i++) covi[i]=state.m_awgt[ntrk][i];
          myPropagator.Propagate( TrkID, state.m_ich[ntrk], pari, covi, vrtini, vrtend,&state.m_apar[ntrk][0],&state.m_awgt[ntrk][0],&state.m_vkalFitControl);
       }

       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    state.m_refFrameX=tmp_refFrameX;
    state.m_refFrameY=tmp_refFrameY;
    state.m_refFrameZ=tmp_refFrameZ;
    state.m_fitField.setAtlasMagRefFrame( state.m_refFrameX, state.m_refFrameY, state.m_refFrameZ);

    return StatusCode::SUCCESS;
  }



//------------------------------------------------------------------------------------------------------
//-- (Measured)Perigee in Trk::TrackParticleBase. Trk::Track has ->perigeeParameters() member function.
//

  const Perigee* TrkVKalVrtFitter::GetPerigee( const TrackParticleBase* i_ntrk) 
  {
       const Perigee* mPer = 0;
       if(   i_ntrk->definingParameters().associatedSurface().type() == Surface::Perigee
          && i_ntrk->definingParameters().covariance() != 0 ) 
           mPer = dynamic_cast<const Perigee*> (&(i_ntrk->definingParameters()));
       return mPer ;
  }
  const TrackParameters* TrkVKalVrtFitter::GetFirstPoint(const Trk::TrackParticleBase* i_ntrk)
  {
      std::vector<const TrackParameters*>::const_iterator vpb = i_ntrk->trackParameters().begin();        
      std::vector<const TrackParameters*>::const_iterator vpe = i_ntrk->trackParameters().end();  
      const TrackParameters* res=0; 
      double minDst=1000000.; 
      for(;vpb != vpe; ++vpb) {
        if((*vpb)->position().perp()<25.)continue;
        if((*vpb)->position().perp()<minDst){ minDst=(*vpb)->position().perp(); res= (*vpb); }
      }
      if(!res) res=&(i_ntrk->definingParameters());  //if nothing is found - then use track defining parameters
                                                     // Perigee presumably
      return res;      
  }


//----------------------------------------------------------------------------------------------------------

  const Perigee* TrkVKalVrtFitter::GetPerigee( const TrackParameters* i_ntrk) 
  {
       const Perigee* mPer = 0;
       if(i_ntrk->associatedSurface().type()==Surface::Perigee && i_ntrk->covariance()!= 0 ) 
            mPer = dynamic_cast<const Perigee*> (i_ntrk);
       return mPer;
  }


} // end of namespace bracket indexOfParameterAtPosition
