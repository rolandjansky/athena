/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Convert TrackParticle parameters to internal VKalVrt parameters
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
#include  "TrkNeutralParameters/NeutralParameters.h"
//----
#include <iostream> 

namespace Trk {

    extern vkalPropagator  myPropagator;

//--------------------------------------------------------------------
//  Extract xAOD::TrackParticles
//

 StatusCode TrkVKalVrtFitter::CvtTrackParticle(const std::vector<const xAOD::TrackParticle*>& InpTrk,
         long int& ntrk) {

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee*        m_mPer=0;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,m_BMAG_FIXED;
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
     m_trkControl.clear(); m_trkControl.reserve(InpTrk.size());
     for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//-- (Measured)Perigee in xAOD::TrackParticle
       m_mPer = &(*i_ntrk)->perigeeParameters(); 
       if( m_mPer==0 ) continue; // No perigee!!!
       perGlobalVrt =  m_mPer->associatedSurface().center();      //Global position of reference point
       perGlobalPos =  m_mPer->position();    //Global position of perigee point
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       tmp_refFrameX += perGlobalPos.x() ;	// Reference system calculation
       tmp_refFrameY += perGlobalPos.y() ;	// Use hit position itself to get more precise 
       tmp_refFrameZ += perGlobalPos.z() ;	// magnetic field
       TrkMatControl tmpMat;
       tmpMat.trkRefGlobPos=Amg::Vector3D( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.trkRotation = Amg::RotationMatrix3D::Identity();
       tmpMat.rotateToField=false; if(m_useMagFieldRotation)tmpMat.rotateToField=true;
       tmpMat.extrapolationType=2;                   // Perigee point strategy
       tmpMat.TrkPnt=m_mPer; 
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
//
//std::cout.setf( std::ios::scientific); std::cout.precision(5);
//std::cout<<" VK ref.frame="<<tmp_refFrameX<<", "<<tmp_refFrameY<<", "<<tmp_refFrameZ<<'\n';
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//

    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- (Measured)Perigee in TrackParticle
//
       m_mPer = &(*i_ntrk)->perigeeParameters(); 
       if( m_mPer==0 ) continue; // No perigee!!!
       VectPerig    =  m_mPer->parameters(); 
       perGlobalVrt =  m_mPer->associatedSurface().center();      //Global position of reference point
       perGlobalPos =  m_mPer->position();    //Global position of perigee point
       if( !convertAmg5SymMtx(m_mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!;
       m_refFrameX=m_refFrameY=m_refFrameZ=0.; m_fitField->setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame
       m_fitField->getMagFld( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(), // Magnetic field
                                                                  fx, fy, m_BMAG_FIXED);                                // at track perigee point
       if(fabs(m_BMAG_FIXED) < 0.01) m_BMAG_FIXED=0.01;


       VKalTransform( m_BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     m_ich[ntrk],&m_apar[ntrk][0],&m_awgt[ntrk][0]);
//
// Check if propagation to common reference point is needed and make it
       m_refFrameX=perGlobalVrt.x();  // initial track reference position
       m_refFrameY=perGlobalVrt.y();
       m_refFrameZ=perGlobalVrt.z();
       m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);
       double dX=tmp_refFrameX-perGlobalVrt.x();   // Track shift for VKalVrtCore
       double dY=tmp_refFrameY-perGlobalVrt.y();
       double dZ=tmp_refFrameZ-perGlobalVrt.z();
       if(fabs(dX)+fabs(dY)+fabs(dZ) != 0.) {
	  double pari[5],covi[15]; double vrtini[3]={0.,0.,0.}; double vrtend[3]={dX,dY,dZ};
	  for(int i=0; i<5; i++) pari[i]=m_apar[ntrk][i];
	  for(int i=0; i<15;i++) covi[i]=m_awgt[ntrk][i];
          long int Charge = m_ich[ntrk];  
          myPropagator.Propagate(ntrk, Charge, pari, covi, vrtini, vrtend,&m_apar[ntrk][0],&m_awgt[ntrk][0]);
       }

       ntrk++; if(ntrk>=m_NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    m_refFrameX=tmp_refFrameX;
    m_refFrameY=tmp_refFrameY;
    m_refFrameZ=tmp_refFrameZ;
    m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);

    return StatusCode::SUCCESS;
  }



//--------------------------------------------------------------------
//  Extract TrackParticles
//

 StatusCode TrkVKalVrtFitter::CvtTrackParticle(const std::vector<const TrackParticleBase*>& InpTrk,
         long int& ntrk) {

    std::vector<const TrackParticleBase*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee*        m_mPer=0;
    const Trk::NeutralPerigee* m_mPerN=0;
    double CovVertTrk[15]; std::fill(CovVertTrk,CovVertTrk+15,0.);
    double tmp_refFrameX=0, tmp_refFrameY=0, tmp_refFrameZ=0;
    double fx,fy,m_BMAG_FIXED;
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
     m_trkControl.clear(); m_trkControl.reserve(InpTrk.size());
     for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//-- (Measured)Perigee in TrackParticle
       m_mPer = GetPerigee(*i_ntrk); m_mPerN=0;  //if( !m_mPer ) m_mPerN = GetNeutralPerigee(*i_ntrk); //VK No NEUTRAL TrackParticles for now!!!
       if( m_mPer==0 && m_mPerN==0) continue; // No perigee!!!
       if( m_mPer ){
          //perGlobalVrt =  m_mPer->vertex();      //Global position of reference point
          perGlobalVrt =  m_mPer->associatedSurface().center();      //Global position of reference point
          perGlobalPos =  m_mPer->position();    //Global position of perigee point
       }else{ 
          //perGlobalVrt =  m_mPerN->vertex();      //Global position of reference point
          perGlobalVrt =  m_mPerN->associatedSurface().center();      //Global position of reference point
          perGlobalPos =  m_mPerN->position();    //Global position of perigee point
       }
       if(fabs(perGlobalPos.z())   > m_IDsizeZ)return StatusCode::FAILURE;   // Crazy user protection
       if(     perGlobalPos.perp() > m_IDsizeR)return StatusCode::FAILURE;
       //tmp_refFrameX += perGlobalVrt.x() ;		// Reference system calculation
       //tmp_refFrameY += perGlobalVrt.y() ;		// Normally gives (0,0,)
       //tmp_refFrameZ += perGlobalVrt.z() ;
       tmp_refFrameX += perGlobalPos.x() ;	// Reference system calculation
       tmp_refFrameY += perGlobalPos.y() ;	// Use hit position itself to get more precise 
       tmp_refFrameZ += perGlobalPos.z() ;	// magnetic field
       TrkMatControl tmpMat;
       tmpMat.trkRefGlobPos=Amg::Vector3D( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z());
       tmpMat.trkRotation = Amg::RotationMatrix3D::Identity();
       tmpMat.rotateToField=false; if(m_useMagFieldRotation)tmpMat.rotateToField=true;
       tmpMat.extrapolationType=2;                   // Perigee point strategy
       if(m_mPer){ tmpMat.TrkPnt=m_mPer; }else{ tmpMat.TrkPnt=NULL;}
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
//
//std::cout.setf( std::ios::scientific); std::cout.precision(5);
//std::cout<<" VK ref.frame="<<tmp_refFrameX<<", "<<tmp_refFrameY<<", "<<tmp_refFrameZ<<'\n';
//
//  Common reference frame is ready. Start extraction of parameters for fit.
//

    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- (Measured)Perigee in TrackParticle
//
       m_mPer = GetPerigee(*i_ntrk); m_mPerN=0; //if( !m_mPer ) m_mPerN = GetNeutralPerigee(*i_ntrk); //VK No NEUTRAL TrackParticles for now!!!
       if( m_mPer==0 && m_mPerN==0) continue; // No perigee!!!
       if( m_mPer ){
          VectPerig    =  m_mPer->parameters(); 
          //perGlobalVrt =  m_mPer->vertex();      //Global position of reference point
          perGlobalVrt =  m_mPer->associatedSurface().center();      //Global position of reference point
          perGlobalPos =  m_mPer->position();    //Global position of perigee point
          if( !convertAmg5SymMtx(m_mPer->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!;
       }else{ 
          VectPerig    =  m_mPerN->parameters(); 
          //perGlobalVrt =  m_mPerN->vertex();      //Global position of reference point
          perGlobalVrt =  m_mPerN->associatedSurface().center();      //Global position of reference point
          perGlobalPos =  m_mPerN->position();    //Global position of perigee point
          if( !convertAmg5SymMtx(m_mPerN->covariance(), CovVertTrk) ) return StatusCode::FAILURE; //VK no good covariance matrix!
       }
       m_refFrameX=m_refFrameY=m_refFrameZ=0.; m_fitField->setAtlasMagRefFrame( 0., 0., 0.);  //restore ATLAS frame
       m_fitField->getMagFld( perGlobalPos.x(), perGlobalPos.y(), perGlobalPos.z(), // Magnetic field
                              fx, fy, m_BMAG_FIXED);                                // at track perigee point
       if(fabs(m_BMAG_FIXED) < 0.01) m_BMAG_FIXED=0.01;


       VKalTransform( m_BMAG_FIXED, (double)VectPerig[0], (double)VectPerig[1],
              (double)VectPerig[2], (double)VectPerig[3], (double)VectPerig[4], CovVertTrk,
                     m_ich[ntrk],&m_apar[ntrk][0],&m_awgt[ntrk][0]);
       if( m_mPerN ) {                                              //neutral track
         m_ich[ntrk]=0; 
         if(m_apar[ntrk][4]<0){ m_apar[ntrk][4]  = -m_apar[ntrk][4];      // Charge=0 is always equal to Charge=+1
                                m_awgt[ntrk][10] = -m_awgt[ntrk][10];
                                m_awgt[ntrk][11] = -m_awgt[ntrk][11];
                                m_awgt[ntrk][12] = -m_awgt[ntrk][12];
                                m_awgt[ntrk][13] = -m_awgt[ntrk][13]; }
       }
//std::cout<< "New="<<m_awgt[ntrk][0]<<", "<<m_awgt[ntrk][1]<<", "<<m_awgt[ntrk][2]<<", "<<m_awgt[ntrk][3]<<'\n';
//std::cout<< "New="<<m_awgt[ntrk][11]<<", "<<m_awgt[ntrk][12]<<", "<<m_awgt[ntrk][13]<<", "<<m_awgt[ntrk][14]<<'\n';
//std::cout<<"-------------------------------------"<<'\n';
//
//
// Check if propagation to common reference point is needed and make it
       m_refFrameX=perGlobalVrt.x();  // initial track reference position
       m_refFrameY=perGlobalVrt.y();
       m_refFrameZ=perGlobalVrt.z();
       m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);
       double dX=tmp_refFrameX-perGlobalVrt.x();   // Track shift for VKalVrtCore
       double dY=tmp_refFrameY-perGlobalVrt.y();
       double dZ=tmp_refFrameZ-perGlobalVrt.z();
       if(fabs(dX)+fabs(dY)+fabs(dZ) != 0.) {
	  double pari[5],covi[15]; double vrtini[3]={0.,0.,0.}; double vrtend[3]={dX,dY,dZ};
	  for(int i=0; i<5; i++) pari[i]=m_apar[ntrk][i];
	  for(int i=0; i<15;i++) covi[i]=m_awgt[ntrk][i];
          long int Charge = m_ich[ntrk];  
//VK 17.06/2008 Wrong!!! m_fitPropagator is defined only when InDet extrapolator is provided!!!
          //m_fitPropagator->Propagate(ntrk,Charge, pari, covi, vrtini, vrtend,&m_apar[ntrk][0],&m_awgt[ntrk][0]);
          myPropagator.Propagate(ntrk, Charge, pari, covi, vrtini, vrtend,&m_apar[ntrk][0],&m_awgt[ntrk][0]);
       }

       ntrk++; if(ntrk>=m_NTrMaxVFit) return StatusCode::FAILURE;
    }
//-------------- Finally setting new reference frame common for ALL tracks
    m_refFrameX=tmp_refFrameX;
    m_refFrameY=tmp_refFrameY;
    m_refFrameZ=tmp_refFrameZ;
    m_fitField->setAtlasMagRefFrame( m_refFrameX, m_refFrameY, m_refFrameZ);

    return StatusCode::SUCCESS;
  }



//------------------------------------------------------------------------------------------------------
//-- (Measured)Perigee in Trk::TrackParticleBase. Trk::Track has ->perigeeParameters() member function.
//

  const Perigee* TrkVKalVrtFitter::GetPerigee( const TrackParticleBase* i_ntrk) 
  {
       const Perigee* m_mPer = 0;
       if(   i_ntrk->definingParameters().associatedSurface().type() == Surface::Perigee
          && i_ntrk->definingParameters().covariance() != 0 ) 
           m_mPer = dynamic_cast<const Perigee*> (&(i_ntrk->definingParameters()));
       return m_mPer ;
  }

  const Perigee* TrkVKalVrtFitter::GetPerigee( const TrackParameters* i_ntrk) 
  {
       const Perigee* m_mPer = 0;
       if(i_ntrk->associatedSurface().type()==Surface::Perigee && i_ntrk->covariance()!= 0 ) 
            m_mPer = dynamic_cast<const Perigee*> (i_ntrk);
       return m_mPer;
  }

// Universal approach, apparently not needed anymore...
/*  const Perigee* TrkVKalVrtFitter::GetPerigee( const Track* i_ntrk) {
   const Perigee* m_mPer=0;
     const DataVector<const TrackParameters>* AllTrkPar = i_ntrk->trackParameters(); 
     DataVector<const TrackParameters>::const_iterator i_apar=AllTrkPar->begin();
     while( i_apar != AllTrkPar->end() ){
       if((*i_apar)->associatedSurface().type()==Surface::Perigee && (*i_apar)->covariance()!= 0 ){m_mPer=(*i_apar); break;}
       i_apar++;
     }
     return m_mPer;}
*/

//------------------------------------------------------------------------------------------------
//-- (Measured)NeutralPerigee in Trk::TrackParticleBase.   Trk::Track has ->perigeeParameters() member function 
//--  Others not defined yet.
//

/*VK Neutral TrackParticleBase doesn't exist for the moment!!!! 
  const NeutralPerigee* TrkVKalVrtFitter::GetNeutralPerigee( const TrackParticleBase* i_ntrk) 
  {
       const NeutralPerigee* m_mPer = 0;
       if(   i_ntrk->definingParameters().associatedSurface().type() == Surface::Perigee
          && i_ntrk->definingParameters().covariance() != 0 ) 
           m_mPer= dynamic_cast<const NeutralPerigee*> (&(i_ntrk->definingParameters()));
       return m_mPer ;
  }*/

  const NeutralPerigee* TrkVKalVrtFitter::GetNeutralPerigee( const NeutralParameters* i_ntrk) 
  {
       const NeutralPerigee* m_mPer = 0;
       if(i_ntrk->associatedSurface().type()==Surface::Perigee && i_ntrk->covariance()!= 0 ) m_mPer=dynamic_cast<const NeutralPerigee*>(i_ntrk);
       return m_mPer;
  }

//----------------------------------------------------------------------------------------------------------
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


} // end of namespace bracket indexOfParameterAtPosition
