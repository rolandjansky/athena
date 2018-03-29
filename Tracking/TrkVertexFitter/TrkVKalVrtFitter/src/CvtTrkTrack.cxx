/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Convert TrkTrack parameters to internal VKalVrt parameters
// and sets up common reference system for ALL tracks 
// even if in the beginning in was different
//------------------------------------------------------------------ 
// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
//-------------------------------------------------
// Other stuff
//----
#include  "TrkTrack/Track.h"

#include <iostream>

namespace Trk{

    extern vkalPropagator  myPropagator;

//--------------------------------------------------------------------
//  Extract TrkTracks
//  
//  Use perigee ONLY!!!  
// Then in normal conditions reference frame is always (0,0,0)
//


 StatusCode TrkVKalVrtFitter::CvtTrkTrack(const std::vector<const Trk::Track*>& InpTrk,
        long int& ntrk) {

    std::vector<const Track*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0;
    const  Perigee* mPer;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,BMAG_FIXED;
//
// ----- Set reference frame to (0.,0.,0.) == ATLAS frame
// ----- Magnetic field is taken in reference point
//
     m_refFrameX=m_refFrameY=m_refFrameZ=0.;
     m_fitField->setAtlasMagRefFrame( 0., 0., 0.);
//
//  Cycle to determine common reference point for the fit
//
    int counter =0;
    Amg::Vector3D perGlobalVrt,perGlobalPos;
    m_trkControl.clear();
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
       mPer = (*i_ntrk)->perigeeParameters(); if( mPer == 0 ){ continue; } 
       perGlobalPos =  mPer->position(); //Global position of perigee point
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
//       tmp_refFrameX += perGlobalPos.x()+VectPerig[0]*sin(VectPerig[2]);   // Reference point for given perigee
//       tmp_refFrameY += perGlobalPos.y()-VectPerig[0]*cos(VectPerig[2]);
//       tmp_refFrameZ += perGlobalPos.z()-VectPerig[1];
// std::cout<<" Global posVKAL="<<perGlobalPos.x()<<", "<<perGlobalPos.y()<<", "<<perGlobalPos.z()<<'\n';
       //VK//perGlobalVrt =  mPer->vertex();   //Global position of reference point for perigee
       perGlobalVrt =  mPer->associatedSurface().center();      //Global position of reference point
// std::cout<<" Global vrtVKAL="<<perGlobalVrt.x()<<", "<<perGlobalVrt.y()<<", "<<perGlobalVrt.z()<<'\n';
       //tmp_refFrameX += perGlobalVrt.x() ;   // Reference point for given perigee
       //tmp_refFrameY += perGlobalVrt.y() ;   // Normally gives (0,0,
       //tmp_refFrameZ += perGlobalVrt.z() ;
       tmp_refFrameX += perGlobalPos.x() ;	// Reference system calculation
       tmp_refFrameY += perGlobalPos.y() ;	// Use hit position itself to get more precise 
       tmp_refFrameZ += perGlobalPos.z() ;	// magnetic field
       TrkMatControl tmpMat;
       tmpMat.trkRefGlobPos=Amg::Vector3D(perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.rotateToField=false; if(m_useMagFieldRotation)tmpMat.rotateToField=true;
       tmpMat.trkRotation = Amg::RotationMatrix3D::Identity();
       tmpMat.extrapolationType=2;                   // Perigee point strategy
       tmpMat.TrkPnt=mPer;
       tmpMat.prtMass = 139.5702;
       if(counter<(int)m_MassInputParticles.size())tmpMat.prtMass = m_MassInputParticles[counter];
       tmpMat.TrkID=counter; m_trkControl.push_back(tmpMat);
       counter++;
    }
    if(counter == 0) return StatusCode::FAILURE;
    tmp_refFrameX /= counter;                          // Reference frame for the fit
    tmp_refFrameY /= counter;
    tmp_refFrameZ /= counter;
    m_refGVertex = Amg::Vector3D(tmp_refFrameX, tmp_refFrameY, tmp_refFrameZ);
// std::cout<<" Global   check="<<tmp_refFrameX <<", "<<tmp_refFrameY <<", "<<tmp_refFrameZ<<'\n';
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
       mPer = (*i_ntrk)->perigeeParameters(); if( mPer == 0 ){ continue; } 
       VectPerig = mPer->parameters(); 
       perGlobalPos =  mPer->position();    //Global position of perigee point
       //VK//perGlobalVrt =  mPer->vertex();      //Global position of reference point
       perGlobalVrt =  mPer->associatedSurface().center();      //Global position of reference point
       m_refFrameX=m_refFrameY=m_refFrameZ=0.; m_fitField->setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame
       m_fitField->getMagFld(  perGlobalPos.x() , perGlobalPos.y() , perGlobalPos.z() ,   // Magnetic field
                               fx, fy, BMAG_FIXED);                                     // at perigee point
       if(fabs(BMAG_FIXED) < 0.01) BMAG_FIXED=0.01;
 
       if( !convertAmg5SymMtx(mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
       VKalTransform( BMAG_FIXED, (double)VectPerig(0), (double)VectPerig(1),
              (double)VectPerig(2), (double)VectPerig(3), (double)VectPerig(4), CovVertTrk,
                     m_ich[ntrk],&m_apar[ntrk][0],&m_awgt[ntrk][0]);
//
// Check if propagation to common reference point is needed and make it
       m_refFrameX=perGlobalVrt.x();  // initial track reference position
       m_refFrameY=perGlobalVrt.y();
       m_refFrameZ=perGlobalVrt.z();
       m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);
       double dX=tmp_refFrameX-perGlobalVrt.x();
       double dY=tmp_refFrameY-perGlobalVrt.y();
       double dZ=tmp_refFrameZ-perGlobalVrt.z();
       if(fabs(dX)+fabs(dY)+fabs(dZ) != 0.) {
	  double pari[5],covi[15]; double vrtini[3]={0.,0.,0.}; double vrtend[3]={dX,dY,dZ};
	  for(int i=0; i<5; i++) pari[i]=m_apar[ntrk][i];
	  for(int i=0; i<15;i++) covi[i]=m_awgt[ntrk][i];
          long int Charge = (long int) mPer->charge();  
//VK 17.06/2008 Wrong!!! m_fitPropagator is defined only when InDet extrapolator is provided!!!
          //m_fitPropagator->Propagate(ntrk,Charge, pari, covi, vrtini, vrtend,&m_apar[ntrk][0],&m_awgt[ntrk][0]);
          myPropagator.Propagate(ntrk,Charge, pari, covi, vrtini, vrtend,&m_apar[ntrk][0],&m_awgt[ntrk][0]);
       }

//
       ntrk++; if(ntrk>=NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    m_refFrameX=tmp_refFrameX;
    m_refFrameY=tmp_refFrameY;
    m_refFrameZ=tmp_refFrameZ;
    m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);

    return StatusCode::SUCCESS;
  }



//  Create Trk::Track with perigee defined at m_refGVertex vertex
//
  Track* TrkVKalVrtFitter::CreateTrkTrack( const std::vector<double>& VKPerigee,
                                                const std::vector<double>& VKCov)
  {

    const Trk::Perigee*	perigee = CreatePerigee(0., 0., 0., VKPerigee, VKCov);
				      
    const Trk::FitQuality* fitQuality = new Trk::FitQuality(10.,1);
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces 
	= new DataVector<const Trk::TrackStateOnSurface>;
    const Trk::TrackStateOnSurface* trackSOS =
	    new Trk::TrackStateOnSurface(0, perigee, 0,  0);
	trackStateOnSurfaces->push_back(trackSOS);
	
//    Trk::Track::TrackAuthor author = Trk::Track::unknown;
    Trk::TrackInfo info;
    return new Trk::Track( info, trackStateOnSurfaces, fitQuality) ;
  }




// Function creates a Trk::Perigee on the heap
//  Don't forget to remove it after use
//  vX,vY,vZ are in LOCAL SYSTEM with respect to m_refGVertex
//----------------------------------------------------------------------------------------------
  Perigee * TrkVKalVrtFitter::CreatePerigee(double vX, double vY, double vZ,
     			  	            const std::vector<double>& VKPerigee,
                                            const std::vector<double>& VKCov)
  {
//
// ------  Magnetic field access
    double fx,fy,BMAG_CUR;
    m_fitField->getMagFld(vX,vY,vZ,fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  //safety

    double TrkP1, TrkP2, TrkP3, TrkP4, TrkP5;
    VKalToTrkTrack(BMAG_CUR, VKPerigee[2], VKPerigee[3],VKPerigee[4], TrkP3, TrkP4, TrkP5);
    TrkP1=-VKPerigee[0];   /*!!!! Change of sign !!!!*/
    TrkP2= VKPerigee[1];
    TrkP5=-TrkP5;                  /*!!!! Change of sign of charge!!!!*/

    AmgSymMatrix(5) *CovMtx =new AmgSymMatrix(5);
    double Deriv[5][5],CovMtxOld[5][5];
    int i,j,ik,jk;
    for(i=0;i<5;i++){ for(j=0;j<5;j++) {Deriv[i][j]=0.; CovMtxOld[i][j]=0.;}}
    Deriv[0][0]=-1.;
    Deriv[1][1]= 1.;
    Deriv[2][3]= 1.;
    Deriv[3][2]= 1.;
    Deriv[4][2]= (cos(VKPerigee[2])/(m_CNVMAG*BMAG_CUR)) * VKPerigee[4];
    Deriv[4][4]=-(sin(VKPerigee[2])/(m_CNVMAG*BMAG_CUR));

    CovMtxOld[0][0]                =VKCov[0];
    CovMtxOld[0][1]=CovMtxOld[1][0]=VKCov[1];
    CovMtxOld[1][1]                =VKCov[2];
    CovMtxOld[0][2]=CovMtxOld[2][0]=VKCov[3];
    CovMtxOld[1][2]=CovMtxOld[2][1]=VKCov[4];
    CovMtxOld[2][2]                =VKCov[5];
    CovMtxOld[0][3]=CovMtxOld[3][0]=VKCov[6];
    CovMtxOld[1][3]=CovMtxOld[3][1]=VKCov[7];
    CovMtxOld[2][3]=CovMtxOld[3][2]=VKCov[8];
    CovMtxOld[3][3]                =VKCov[9];
    CovMtxOld[0][4]=CovMtxOld[4][0]=VKCov[10];
    CovMtxOld[1][4]=CovMtxOld[4][1]=VKCov[11];
    CovMtxOld[2][4]=CovMtxOld[4][2]=VKCov[12];
    CovMtxOld[3][4]=CovMtxOld[4][3]=VKCov[13];
    CovMtxOld[4][4]                =VKCov[14];

    for(i=0;i<5;i++){
     for(j=i;j<5;j++){
       double tmp=0.;
       for(ik=4;ik>=0;ik--){
         if(Deriv[i][ik]==0.)continue;
         for(jk=4;jk>=0;jk--){
           if(Deriv[j][jk]==0.)continue;
           tmp += Deriv[i][ik]*CovMtxOld[ik][jk]*Deriv[j][jk];
       }}
       (*CovMtx)(i,j)=(*CovMtx)(j,i)=tmp;
    }}

    return  new Perigee( TrkP1,TrkP2,TrkP3,TrkP4,TrkP5, 
                                  PerigeeSurface(Amg::Vector3D(m_refFrameX+vX, m_refFrameY+vY, m_refFrameZ+vZ)),
                                  CovMtx  );
  }


       
} // end of namespace      
       
       

