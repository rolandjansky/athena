/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//   TEMPORARY until track EDM is finalised!!!!
//  
//  Convert TrackParameters and NeutralParameters to internal VKalVrt parameters
// and sets up common reference system for ALL tracks 
// even if in the beginning in was different
//------------------------------------------------------------------ 
// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
//-------------------------------------------------
// Other stuff
//----
#include  "TrkParameters/TrackParameters.h"
#include <iostream> 

namespace Trk {

//--------------------------------------------------------------------
//  Extract TrackParameters
//

 StatusCode
 TrkVKalVrtFitter::CvtTrackParameters(const std::vector<const TrackParameters*>& InpTrk,
                                      int& ntrk,
                                      State& state) const
 {

    //MsgStream log(msgSvc(), name());
    std::vector<const TrackParameters*>::const_iterator   i_pbase;
    AmgVector(5) VectPerig; VectPerig.setZero();
    Amg::Vector3D perGlobalPos,perGlobalVrt;
    const Trk::Perigee* mPer=nullptr;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,fz,BMAG_FIXED;
    double rxyMin=1000000.;

//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
     state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.;
     state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);

     if( m_InDetExtrapolator == nullptr ){
       //log << MSG::WARNING  << "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       if(msgLvl(MSG::WARNING))msg()<< "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       return StatusCode::FAILURE;        
     }
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     state.m_trkControl.clear(); state.m_trkControl.reserve(InpTrk.size());
     for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
       perGlobalPos =  (*i_pbase)->position();                  //Global position of hit
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;
       tmp_refFrameY += perGlobalPos.y() ;
       tmp_refFrameZ += perGlobalPos.z() ;
       TrkMatControl tmpMat;                                    // Here we create structure to control material effects
       tmpMat.trkRefGlobPos=Amg::Vector3D(perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       if(m_firstMeasuredPoint){ tmpMat.extrapolationType=0;}   //First measured point strategy
       else{                     tmpMat.extrapolationType=1;}   //Any measured point strategy
       tmpMat.TrkPnt=(*i_pbase);
       tmpMat.prtMass = 139.5702;
       if(counter<(int)state.m_MassInputParticles.size())tmpMat.prtMass = state.m_MassInputParticles[counter];
       tmpMat.TrkID=counter; state.m_trkControl.push_back(tmpMat);
       counter++;
//std::cout<<" Glob="<<tmpMat.trkRefGlobPos<<" Stra="<<tmpMat.extrapolationType<<'\n';
       if(perGlobalPos.perp()<rxyMin){rxyMin=perGlobalPos.perp(); state.m_globalFirstHit=(*i_pbase);}
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;                          // based on hits positions
    tmp_refFrameZ /= counter;
    Amg::Vector3D refGVertex (tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);
//
    state.m_fitField.getMagFld( tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ, fx,fy,fz);  //Rotation parameters in case of rotation use   
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//  TracksParameters are extrapolated to common point and converted to Perigee
//  This is needed for VKalVrtCore engine.
//
//
    for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
       long int TrkID=ntrk;
       const TrackParameters* trkparO = (*i_pbase);
       if( trkparO ){
         const Trk::TrackParameters* trkparN = m_fitPropagator->myExtrapWithMatUpdate( TrkID, trkparO, &refGVertex, state );
         if(trkparN == nullptr) return StatusCode::FAILURE;
         mPer = dynamic_cast<const Trk::Perigee*>(trkparN); 
         if( mPer == nullptr) {   delete trkparN;  return StatusCode::FAILURE; }
         VectPerig    =  mPer->parameters(); 
         perGlobalPos =  mPer->position();    //Global position of perigee point
         //perGlobalVrt =  mPer->vertex();      //Global position of reference point
         perGlobalVrt =  mPer->associatedSurface().center();      //Global position of reference point
         if( !convertAmg5SymMtx(mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
         delete trkparN;
      }
// std::cout<<" Tr="<<TrkID<<" GlobPosTrn="<<perGlobalPos.x()<<", "<<perGlobalPos.y()<<", "<<perGlobalPos.z()<<'\n';
// std::cout<<" Common Ref. point="<<perGlobalVrt.x()<<", "<<perGlobalVrt.y()<<", "<<perGlobalVrt.z()<<'\n';
       state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.; state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame for safety
       state.m_fitField.getMagFld(  perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(),  // Magnetic field
                               fx, fy, BMAG_FIXED);                                 // at perigee point
       if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;
//std::cout<<"TESTVK="<<'\n'; std::cout.precision(16); for(int ik=0; ik<15; ik++)std::cout<<CovVertTrk[ik]<<'\n';
       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     state.m_ich[ntrk],&state.m_apar[ntrk][0],&state.m_awgt[ntrk][0]);
       if( trkparO==nullptr ) {                                              //neutral track
         state.m_ich[ntrk]=0; 
         if(state.m_apar[ntrk][4]<0){ state.m_apar[ntrk][4]  = -state.m_apar[ntrk][4];      // Charge=0 is always equal to Charge=+1
                                state.m_awgt[ntrk][10] = -state.m_awgt[ntrk][10];
                                state.m_awgt[ntrk][11] = -state.m_awgt[ntrk][11];
                                state.m_awgt[ntrk][12] = -state.m_awgt[ntrk][12];
                                state.m_awgt[ntrk][13] = -state.m_awgt[ntrk][13]; }
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


 StatusCode
 TrkVKalVrtFitter::CvtNeutralParameters(const std::vector<const NeutralParameters*>& InpTrk,
                                        int& ntrk,
                                        State& state) const
 {

    //MsgStream log(msgSvc(), name());
    std::vector<const NeutralParameters*>::const_iterator   i_pbase;
    AmgVector(5) VectPerig;
    Amg::Vector3D perGlobalPos,perGlobalVrt;
    const NeutralPerigee* mPerN=nullptr;
    double CovVertTrk[15];
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,fz,BMAG_FIXED;
    double rxyMin=1000000.;

//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
    state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.;
    state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);

    if( m_InDetExtrapolator == nullptr ){
       if(msgLvl(MSG::WARNING))msg()<< "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       return StatusCode::FAILURE;        
    }
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     state.m_trkControl.clear(); state.m_trkControl.reserve(InpTrk.size());
     for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
       perGlobalPos =  (*i_pbase)->position();                  //Global position of hit
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;
       tmp_refFrameY += perGlobalPos.y() ;
       tmp_refFrameZ += perGlobalPos.z() ;
       TrkMatControl tmpMat;                                    // Here we create structure to control material effects
       tmpMat.trkRefGlobPos=Amg::Vector3D(perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z()); // on track extrapolation
       tmpMat.extrapolationType=0;   //First measured point strategy
       tmpMat.TrkPnt=nullptr;           //No reference point for neutral track for the moment  !!!
       tmpMat.prtMass = 139.5702;
       if(counter<(int)state.m_MassInputParticles.size())tmpMat.prtMass = state.m_MassInputParticles[counter];
       tmpMat.TrkID=counter; state.m_trkControl.push_back(tmpMat);
       counter++;
       if(perGlobalPos.perp()<rxyMin){rxyMin=perGlobalPos.perp(); state.m_globalFirstHit=nullptr;}
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;                          // based on hits positions
    tmp_refFrameZ /= counter;
    Amg::Vector3D refGVertex (tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);
//
    state.m_fitField.getMagFld( tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ, fx,fy,fz);  //Rotation parameters in case of rotation use   
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//  TracksParameters are extrapolated to common point and converted to Perigee
//  This is needed for VKalVrtCore engine.
//
//
    for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
         const Trk::NeutralParameters* neuparO = (*i_pbase);
         if(neuparO == nullptr) return StatusCode::FAILURE;
         const Trk::NeutralParameters* neuparN = m_fitPropagator->myExtrapNeutral( neuparO, &refGVertex );
         mPerN = dynamic_cast<const Trk::NeutralPerigee*>(neuparN); 
         if( mPerN == nullptr) {   delete neuparN;  return StatusCode::FAILURE; }
         VectPerig    =  mPerN->parameters(); 
         perGlobalPos =  mPerN->position();    //Global position of perigee point
         //perGlobalVrt =  mPerN->vertex();      //Global position of reference point
         perGlobalVrt =  mPerN->associatedSurface().center();      //Global position of reference point
         if( !convertAmg5SymMtx(mPerN->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
         delete neuparN;
         state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.; state.m_fitField.setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame for safety
         state.m_fitField.getMagFld(  perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(),  // Magnetic field
                               fx, fy, BMAG_FIXED);                                 // at perigee point
         if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;

//std::cout<<" BaseEMtx="<<CovMtx.fast(1,1)<<", "<<CovMtx.fast(2,2)<<", "<<CovMtx.fast(3,3)<<", "
//                       <<CovMtx.fast(4,4)<<", "<<CovMtx.fast(5,5)<<'\n';
       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     state.m_ich[ntrk],&state.m_apar[ntrk][0],&state.m_awgt[ntrk][0]);
       state.m_ich[ntrk]=0; 
       if(state.m_apar[ntrk][4]<0){ state.m_apar[ntrk][4]  = -state.m_apar[ntrk][4];      // Charge=0 is always equal to Charge=+1
                              state.m_awgt[ntrk][10] = -state.m_awgt[ntrk][10];
                              state.m_awgt[ntrk][11] = -state.m_awgt[ntrk][11];
                              state.m_awgt[ntrk][12] = -state.m_awgt[ntrk][12];
                              state.m_awgt[ntrk][13] = -state.m_awgt[ntrk][13]; }
       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    state.m_refFrameX=tmp_refFrameX;
    state.m_refFrameY=tmp_refFrameY;
    state.m_refFrameZ=tmp_refFrameZ;
    state.m_fitField.setAtlasMagRefFrame( state.m_refFrameX, state.m_refFrameY, state.m_refFrameZ);

    return StatusCode::SUCCESS;
  }

} // end of namespace bracket
