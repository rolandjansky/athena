/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include  "TrkSurfaces/StraightLineSurface.h"
//----
#include <iostream> 

namespace Trk {

 extern vkalPropagator  myPropagator;

//--------------------------------------------------------------------
//  Extract TrackParameters
//

 StatusCode TrkVKalVrtFitter::CvtTrackParameters(const std::vector<const TrackParameters*>& InpTrk,
        long int& ntrk) {

    //MsgStream log(msgSvc(), name());
    std::vector<const TrackParameters*>::const_iterator   i_pbase;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    Amg::Vector3D perGlobalPos,perGlobalVrt;
    const Trk::Perigee* mPer=0;
    const Trk::AtaStraightLine* Line=0; 
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,fz,BMAG_FIXED;
    double rxyMin=1000000.;

//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
     m_refFrameX=m_refFrameY=m_refFrameZ=0.;
     m_fitField->setAtlasMagRefFrame( 0., 0., 0.);

     if( m_InDetExtrapolator == 0 && m_PropagatorType != 3 ){
       //log << MSG::WARNING  << "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       if(msgLvl(MSG::WARNING))msg()<< "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       return StatusCode::FAILURE;        
     }
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     m_trkControl.clear(); m_trkControl.reserve(InpTrk.size());
     for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
       perGlobalPos =  (*i_pbase)->position();                  //Global position of hit
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;
       tmp_refFrameY += perGlobalPos.y() ;
       tmp_refFrameZ += perGlobalPos.z() ;
       TrkMatControl tmpMat;                                    // Here we create structure to control material effects
       tmpMat.trkRefGlobPos=Amg::Vector3D(perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.rotateToField=false; if(m_useMagFieldRotation)tmpMat.rotateToField=true;
       tmpMat.trkRotation = Amg::RotationMatrix3D::Identity();
       if(m_firstMeasuredPoint){ tmpMat.extrapolationType=0;}   //First measured point strategy
       else{                     tmpMat.extrapolationType=1;}   //Any measured point strategy
       tmpMat.TrkPnt=(*i_pbase);
       tmpMat.prtMass = 139.5702;
       if(counter<(int)m_MassInputParticles.size())tmpMat.prtMass = m_MassInputParticles[counter];
       tmpMat.TrkID=counter; m_trkControl.push_back(tmpMat);
       counter++;
//std::cout<<" Glob="<<tmpMat.trkRefGlobPos<<" Stra="<<tmpMat.extrapolationType<<'\n';
       if(perGlobalPos.perp()<rxyMin){rxyMin=perGlobalPos.perp(); m_globalFirstHit=(*i_pbase);}
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;                          // based on hits positions
    tmp_refFrameZ /= counter;
    m_refGVertex = Amg::Vector3D(tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);
//
    m_fitField->getMagFld( tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ, fx,fy,fz);  //Rotation parameters in case of rotation use   
    Amg::RotationMatrix3D magFldRot=getMagFldRotation(fx,fy,fz, tmp_refFrameX, tmp_refFrameY, 0.);
    StraightLineSurface lineTarget(new Amg::Transform3D( magFldRot.inverse() , Amg::Vector3D(tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ) ));
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//  TracksParameters are extrapolated to common point and converted to Perigee
//  This is needed for VKalVrtCore engine.
//
//
    counter=0;
    for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
       const TrackParameters* trkparO = (*i_pbase);
     if(!m_useMagFieldRotation){
       if( trkparO ){
         const Trk::TrackParameters* trkparN = m_fitPropagator->myExtrapWithMatUpdate( ntrk, trkparO, &m_refGVertex );
         if(trkparN == 0) return StatusCode::FAILURE;
         mPer = dynamic_cast<const Trk::Perigee*>(trkparN); 
         if( mPer == 0) {   delete trkparN;  return StatusCode::FAILURE; }
         VectPerig    =  mPer->parameters(); 
         perGlobalPos =  mPer->position();    //Global position of perigee point
         //perGlobalVrt =  mPer->vertex();      //Global position of reference point
         perGlobalVrt =  mPer->associatedSurface().center();      //Global position of reference point
         if( !convertAmg5SymMtx(mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
         delete trkparN;
      }
// std::cout<<" Tr="<<ntrk<<" GlobPosTrn="<<perGlobalPos.x()<<", "<<perGlobalPos.y()<<", "<<perGlobalPos.z()<<'\n';
// std::cout<<" Common Ref. point="<<perGlobalVrt.x()<<", "<<perGlobalVrt.y()<<", "<<perGlobalVrt.z()<<'\n';
       m_refFrameX=m_refFrameY=m_refFrameZ=0.; m_fitField->setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame for safety
       m_fitField->getMagFld(  perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(),  // Magnetic field
                               fx, fy, BMAG_FIXED);                                 // at perigee point
       if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;
     }else{                   // For rotation case only!!!
       if( trkparO ){
         const Trk::TrackParameters* trkparN = m_fitPropagator->myExtrapToLine((long int)counter, trkparO, &m_refGVertex, lineTarget);
         if(trkparN == 0) return StatusCode::FAILURE;
         Line = dynamic_cast<const Trk::AtaStraightLine*>(trkparN); 
         if( Line == 0) {   delete trkparN;  return StatusCode::FAILURE; }
         VectPerig    =  Line->parameters(); 
         perGlobalPos =  Line->position();    //Global position of perigee point
         Amg::Vector3D perMomentum(Line->momentum().x(),Line->momentum().y(),Line->momentum().z());
         Amg::Vector3D rotatedMomentum=magFldRot*perMomentum;
//         VectPerig[2] += atan2(rotatedMomentum.y(),rotatedMomentum.x());  //VK wrong 27.09.10
         VectPerig[2] = atan2(rotatedMomentum.y(),rotatedMomentum.x());
         VectPerig[3] = atan2(rotatedMomentum.perp(),rotatedMomentum.z());
         if( !convertAmg5SymMtx(Line->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
         delete trkparN;
         BMAG_FIXED=sqrt(fx*fx+fy*fy+fz*fz);
         m_fitRotatedField->setAtlasMag(BMAG_FIXED);  //set fixed ROTATED field in corresponding VKal oblect
       }else{  return StatusCode::FAILURE;   }
       m_trkControl[counter].trkRotation=magFldRot;
       m_trkControl[counter].trkRotationVertex=   Amg::Vector3D( m_refGVertex.x(), m_refGVertex.y(), m_refGVertex.z());
       m_trkControl[counter].trkSavedLocalVertex= Amg::Vector3D(0.,0.,0.);
     }
       counter++;
//std::cout<<"TESTVK="<<'\n'; std::cout.precision(16); for(int ik=0; ik<15; ik++)std::cout<<CovVertTrk[ik]<<'\n';
       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     m_ich[ntrk],&m_apar[ntrk][0],&m_awgt[ntrk][0]);
       if( trkparO==0 ) {                                              //neutral track
         m_ich[ntrk]=0; 
         if(m_apar[ntrk][4]<0){ m_apar[ntrk][4]  = -m_apar[ntrk][4];      // Charge=0 is always equal to Charge=+1
                                m_awgt[ntrk][10] = -m_awgt[ntrk][10];
                                m_awgt[ntrk][11] = -m_awgt[ntrk][11];
                                m_awgt[ntrk][12] = -m_awgt[ntrk][12];
                                m_awgt[ntrk][13] = -m_awgt[ntrk][13]; }
       }
       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    m_refFrameX=tmp_refFrameX;
    m_refFrameY=tmp_refFrameY;
    m_refFrameZ=tmp_refFrameZ;
    m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);

    return StatusCode::SUCCESS;
 }


 StatusCode TrkVKalVrtFitter::CvtNeutralParameters(const std::vector<const NeutralParameters*>& InpTrk,
        long int& ntrk) {

    //MsgStream log(msgSvc(), name());
    std::vector<const NeutralParameters*>::const_iterator   i_pbase;
    AmgVector(5) VectPerig;
    Amg::Vector3D perGlobalPos,perGlobalVrt;
    const NeutralPerigee* mPerN=0;
    double CovVertTrk[15];
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,fz,BMAG_FIXED;
    double rxyMin=1000000.;

//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
    m_refFrameX=m_refFrameY=m_refFrameZ=0.;
    m_fitField->setAtlasMagRefFrame( 0., 0., 0.);

    if( m_InDetExtrapolator == 0 && m_PropagatorType != 3 ){
       //log << MSG::WARNING  << "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       if(msgLvl(MSG::WARNING))msg()<< "No InDet extrapolator given. Can't use TrackParameters!!!" << endmsg;
       return StatusCode::FAILURE;        
    }
//
//  Cycle to determine common reference point for the fit
//
     int counter =0;
     m_trkControl.clear(); m_trkControl.reserve(InpTrk.size());
     for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
       perGlobalPos =  (*i_pbase)->position();                  //Global position of hit
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;
       tmp_refFrameY += perGlobalPos.y() ;
       tmp_refFrameZ += perGlobalPos.z() ;
       TrkMatControl tmpMat;                                    // Here we create structure to control material effects
       tmpMat.trkRefGlobPos=Amg::Vector3D(perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z()); // on track extrapolation
       tmpMat.rotateToField=false;
       tmpMat.trkRotation = Amg::RotationMatrix3D::Identity();
       tmpMat.extrapolationType=0;   //First measured point strategy
       tmpMat.TrkPnt=NULL;           //No reference point for neutral track for the moment  !!!
       tmpMat.prtMass = 139.5702;
       if(counter<(int)m_MassInputParticles.size())tmpMat.prtMass = m_MassInputParticles[counter];
       tmpMat.TrkID=counter; m_trkControl.push_back(tmpMat);
       counter++;
       if(perGlobalPos.perp()<rxyMin){rxyMin=perGlobalPos.perp(); m_globalFirstHit=NULL;}
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;                          // based on hits positions
    tmp_refFrameZ /= counter;
    m_refGVertex = Amg::Vector3D(tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);
//
    m_fitField->getMagFld( tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ, fx,fy,fz);  //Rotation parameters in case of rotation use   
    Amg::RotationMatrix3D magFldRot=getMagFldRotation(fx,fy,fz, tmp_refFrameX, tmp_refFrameY, 0.);
    StraightLineSurface lineTarget(new Amg::Transform3D( magFldRot.inverse() , Amg::Vector3D(tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ) ));
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//  TracksParameters are extrapolated to common point and converted to Perigee
//  This is needed for VKalVrtCore engine.
//
//
    counter=0;
    for (i_pbase = InpTrk.begin(); i_pbase != InpTrk.end(); ++i_pbase) {
      if(!m_useMagFieldRotation){
         const Trk::NeutralParameters* neuparO = (*i_pbase);
         if(neuparO == 0) return StatusCode::FAILURE;
         const Trk::NeutralParameters* neuparN = m_fitPropagator->myExtrapNeutral( neuparO, &m_refGVertex );
         mPerN = dynamic_cast<const Trk::NeutralPerigee*>(neuparN); 
         if( mPerN == 0) {   delete neuparN;  return StatusCode::FAILURE; }
         VectPerig    =  mPerN->parameters(); 
         perGlobalPos =  mPerN->position();    //Global position of perigee point
         //perGlobalVrt =  mPerN->vertex();      //Global position of reference point
         perGlobalVrt =  mPerN->associatedSurface().center();      //Global position of reference point
         if( !convertAmg5SymMtx(mPerN->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
         delete neuparN;
         m_refFrameX=m_refFrameY=m_refFrameZ=0.; m_fitField->setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame for safety
         m_fitField->getMagFld(  perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(),  // Magnetic field
                               fx, fy, BMAG_FIXED);                                 // at perigee point
         if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;
       }else{                   // For rotation case only!!!
         return StatusCode::FAILURE; 
       }
       counter++;

//std::cout<<" BaseEMtx="<<CovMtx.fast(1,1)<<", "<<CovMtx.fast(2,2)<<", "<<CovMtx.fast(3,3)<<", "
//                       <<CovMtx.fast(4,4)<<", "<<CovMtx.fast(5,5)<<'\n';
       VKalTransform( BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     m_ich[ntrk],&m_apar[ntrk][0],&m_awgt[ntrk][0]);
       m_ich[ntrk]=0; 
       if(m_apar[ntrk][4]<0){ m_apar[ntrk][4]  = -m_apar[ntrk][4];      // Charge=0 is always equal to Charge=+1
                              m_awgt[ntrk][10] = -m_awgt[ntrk][10];
                              m_awgt[ntrk][11] = -m_awgt[ntrk][11];
                              m_awgt[ntrk][12] = -m_awgt[ntrk][12];
                              m_awgt[ntrk][13] = -m_awgt[ntrk][13]; }
       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    m_refFrameX=tmp_refFrameX;
    m_refFrameY=tmp_refFrameY;
    m_refFrameZ=tmp_refFrameZ;
    m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);

    return StatusCode::SUCCESS;
  }

} // end of namespace bracket
