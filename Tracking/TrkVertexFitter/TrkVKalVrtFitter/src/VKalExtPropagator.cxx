/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
// The VKalExtPropagator object is created if ATHENA propagator exists 
// and is supplied via jobOptions. A pointer to it is supplied to VKalVrtCore
// for every vertex fit via VKalVrtControlBase object.
// VKalVrtCore uses it to extrapolate tracks for a vertex fit.
// 
// If ATHENA propagator doesn't exist, the VKalVrtCore uses its internal 
// propagator without material. 
//
// myPropagator object exists always in VKalVrtCore and should be used
// by default for track propagation in VKalVrtFitter.
//------------------------------------------------------------------------- 



// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/StraightLineSurface.h"
//-------------------------------------------------
#include<iostream>


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//                  External propagator access for VKalVrt

namespace Trk {
 
  // Constructor
  VKalExtPropagator::VKalExtPropagator( TrkVKalVrtFitter* pnt)
  {
       m_extrapolator = nullptr;
       m_vkalFitSvc = pnt;
  }
  VKalExtPropagator::~VKalExtPropagator()= default;


  void VKalExtPropagator::setPropagator(const IExtrapolator*  Pnt)
  {
     m_extrapolator = Pnt;
  }

//Protection against exit outside ID volume
//
  double VKalExtPropagator::Protection(double *RefEnd,
                                       const IVKalState& istate) const
  {   
      const TrkVKalVrtFitter::State& state = static_cast<const TrkVKalVrtFitter::State&> (istate);

      double Xend=RefEnd[0] + state.m_refFrameX;
      double Yend=RefEnd[1] + state.m_refFrameY;
      double Zend=RefEnd[2] + state.m_refFrameZ;
      double Rlim=sqrt(Xend*Xend+Yend*Yend) / m_vkalFitSvc->m_IDsizeR;
      double Zlim=fabs(Zend)                / m_vkalFitSvc->m_IDsizeZ;
      double Scale = Rlim; if(Zlim>Rlim) Scale=Zlim;
//std::cout<<"relative TARG="<<RefEnd[0]<<","<<RefEnd[1]<<","<<RefEnd[2]
//<<" global ref.="<<m_vkalFitSvc->state.m_refFrameX<<","<<m_vkalFitSvc->state.m_refFrameY<<","<<m_vkalFitSvc->state.m_refFrameZ
//<<" Limits="<<m_vkalFitSvc->m_IDsizeR<<","<<m_vkalFitSvc->m_IDsizeZ<<" scale="<<Scale<<'\n';
      return Scale;
  }

  bool VKalExtPropagator::checkTarget( double *RefEnd,
                                       const IVKalState& istate) const
  {
      //double targV[3]={ RefEnd[0], RefEnd[1], RefEnd[2]};
      return Protection(RefEnd, istate) <= 1.;
  }
/*----------------------------------------------------------------------------------*/
//  Addres of this fuction is supplied to TrkVKalVrtCore for ATLAS InDet extrapolation 
//  with material. Each time VKalVrt needs propagation - it calls it. 
//
//   In case of usage with first measured point
//    it always use this point as starting point, 
//     so ParOld,CovOld,RefStart are irrelevant
//
//  VKalVrtCore works in relative coordinates wrt (state.m_refFrameX,state.m_refFrameY,state.m_refFrameZ)
//  For ATLAS propagator the Core coordinates must be moved back to global ref.frame
/*------------------------------------------------------------------------------------*/
  void VKalExtPropagator::Propagate( long int trkID, long int Charge, 
                                     double *ParOld, double *CovOld, double *RefStart, 
                                     double *RefEnd, double *ParNew, double *CovNew,
                                     const IVKalState& istate) const
  {
      const TrkVKalVrtFitter::State& state = static_cast<const TrkVKalVrtFitter::State&> (istate);

      int trkID_loc=trkID; if(trkID_loc<0)trkID_loc=0;
//std::cout<<__func__<<" Ext.Propagator TrkID="<<trkID<<"to (local!!!)="<<RefEnd[0]<<", "<<RefEnd[1]<<", "<<RefEnd[2]<<'\n';
//-----------
      double vX=RefEnd[0]; double vY=RefEnd[1]; double vZ=RefEnd[2]; //relative coords
      // Propagation target in GLOBAL frame
      Amg::Vector3D endPointG( vX + state.m_refFrameX, vY + state.m_refFrameY, vZ + state.m_refFrameZ);
//
// ---- Make MeasuredPerigee from input. Mag.field at start point is used here
//
      std::vector<double> PerigeeIni( ParOld, ParOld+5 );
      std::vector<double> CovPerigeeIni( 15, 0. );
      if( CovOld != nullptr) {
//        for(int i=0; i<15;i++) CovPerigeeIni.push_back( CovOld[i] );
        std::copy(CovOld,CovOld+15,CovPerigeeIni.begin() );
      }else{
//        for(int i=0; i<15;i++) CovPerigeeIni.push_back(0.);
        CovPerigeeIni[0]=1.e6;CovPerigeeIni[2]=1.e6;CovPerigeeIni[5]=1.;CovPerigeeIni[9]=1.;CovPerigeeIni[14]=fabs(PerigeeIni[4]);
      }        
      //--- This creates Perigee in GLOBAL frame from input in realtive coordinates
      const Perigee* inpPer = 
          m_vkalFitSvc->CreatePerigee( RefStart[0], RefStart[1], RefStart[2], PerigeeIni, CovPerigeeIni, state);
      const TrackParameters * inpPar= (const TrackParameters*) inpPer;
//
// ----- Magnetic field is taken at target point (GLOBAL calculated from relative frame input)
//
      double fx,fy,BMAG_FIXED;
      state.m_fitField.getMagFld(vX,vY,vZ,fx,fy,BMAG_FIXED);
//
//-------------------- Extrapolation itself
//
      const Trk::TrackParameters* endPer = nullptr;
      if(trkID<0){
            endPer = myExtrapWithMatUpdate( trkID, inpPar, &endPointG, state);
      }else{
            endPer = myExtrapWithMatUpdate( trkID, inpPar, &endPointG, state);
      }
//-----------------------------------
      if( endPer == nullptr ) {   // No extrapolation done!!!
        ParNew[0]=0.; ParNew[1]=0.;ParNew[2]=0.;ParNew[3]=0.;ParNew[4]=0.;
        delete inpPer; return;
      }
      const Perigee*          mPer = dynamic_cast<const Perigee*>(endPer);
      const AtaStraightLine*  Line = dynamic_cast<const AtaStraightLine*>(endPer);
      AmgVector(5) VectPerig; VectPerig.setZero();
      const AmgSymMatrix(5) *CovMtx=nullptr;
      if( mPer ){
        VectPerig = mPer->parameters(); 
        CovMtx    = mPer->covariance();
      } 
      if( Line ){
        VectPerig = Line->parameters(); 
        CovMtx    = Line->covariance();
      }      
      if( (Line==nullptr && mPer==nullptr) || CovMtx==nullptr ){         
        ParNew[0]=0.; ParNew[1]=0.;ParNew[2]=0.;ParNew[3]=0.;ParNew[4]=0.;
        delete inpPer; return;
      }

      if((*CovMtx)(0,0)<=0. || (*CovMtx)(1,1)<=0.){                     //protection against bad error matrix
        ParNew[0]=0.; ParNew[1]=0.;ParNew[2]=0.;ParNew[3]=0.;ParNew[4]=0.;
        delete inpPer; delete endPer;
        return;
      }        
      double CovVertTrk[15];
      long int locCharge=Charge;
      CovVertTrk[ 0] =(*CovMtx)(0,0);
      CovVertTrk[ 1] =(*CovMtx)(1,0);
      CovVertTrk[ 2] =(*CovMtx)(1,1);
      CovVertTrk[ 3] =(*CovMtx)(2,0);
      CovVertTrk[ 4] =(*CovMtx)(2,1);
      CovVertTrk[ 5] =(*CovMtx)(2,2);
      CovVertTrk[ 6] =(*CovMtx)(3,0);
      CovVertTrk[ 7] =(*CovMtx)(3,1);
      CovVertTrk[ 8] =(*CovMtx)(3,2);
      CovVertTrk[ 9] =(*CovMtx)(3,3);
      CovVertTrk[10] =(*CovMtx)(4,0);
      CovVertTrk[11] =(*CovMtx)(4,1);
      CovVertTrk[12] =(*CovMtx)(4,2);
      CovVertTrk[13] =(*CovMtx)(4,3);
      CovVertTrk[14] =(*CovMtx)(4,4);
//std::cout<<" extrapPoint="<<endPer->position().x()<<", "<<endPer->position().y()<<", "<<endPer->position().y()<<'\n';
//std::cout<<" extrapCov="<<(*CovMtx)(0,0)<<", "<<(*CovMtx)(1,1)<<", "<<(*CovMtx)(2,2)<<
//                                          ", "<<(*CovMtx)(3,3)<<", "<<(*CovMtx)(4,4)<<'\n';          

      if(CovNew != nullptr) {
         m_vkalFitSvc->VKalTransform( BMAG_FIXED, VectPerig(0), VectPerig(1),
            VectPerig(2), VectPerig(3), VectPerig(4), CovVertTrk,
                      locCharge,  &ParNew[0] , &CovNew[0]);
      }else{
         double CovVertTrkTmp[15];
         m_vkalFitSvc->VKalTransform( BMAG_FIXED, VectPerig(0), VectPerig(1),
            VectPerig(2), VectPerig(3), VectPerig(4), CovVertTrk,
                      locCharge,  &ParNew[0] , CovVertTrkTmp);
      }
      delete inpPer; delete endPer;
  }						    


/*--------------------------------------------------------------------------------------*/
/* Logic of material subtruction or addition to track error matrix is implemented here. */
/* What to do depends on direction of extrapolation and relative position of initial,   */
/* final and track reference points                                                     */
/*											*/
/*    All points are in GLOBAL Atlas frame here!					*/
/*    											*/
/*   Real track (TrkID>=0) extrapolation always starts from INITIAL track object 	*/
/*   and NOT from parameters provided by VKalVrtCore. More CPU time but better precision*/
/*   Only combined (TrkID<0) tracks use VKalVrtCore parameters for start		*/
/*--------------------------------------------------------------------------------------*/
  const TrackParameters* VKalExtPropagator::myExtrapWithMatUpdate(long int TrkID, 
                                                                  const TrackParameters *inpPer,
                                                                  Amg::Vector3D * endPoint,
                                                                  const IVKalState& istate) const
  {   
      const TrkVKalVrtFitter::State& state = static_cast<const TrkVKalVrtFitter::State&> (istate);

      const Trk::TrackParameters* endPer=nullptr;
      const Trk::TrackParameters* tmpPer=nullptr;
      ParticleHypothesis prtType = pion;
//End surface
      PerigeeSurface surfEnd( *endPoint );
//Initial point (global frame)
      Amg::Vector3D iniPoint =  inpPer->position();
      //const MeasuredPerigee* mPer=dynamic_cast<const MeasuredPerigee*> (inpPer); /* This a reference system point */
      //if( mPer != 0) iniPoint = mPer->vertex();                                  /* and NOT point on track!!!*/
      Amg::Vector3D pmom=inpPer->momentum();
//Track reference point ( some point on track provided initially, global frame )
      Amg::Vector3D refPoint(0.,0.,0.);
      if(TrkID>=0)refPoint = state.m_trkControl.at(TrkID).trkRefGlobPos;
//
      Amg::Vector3D step  = (*endPoint) - iniPoint;
//
      int Strategy = 0; if(TrkID>=0) Strategy = state.m_trkControl[TrkID].extrapolationType;
//
// Extrapolation for new track - no material at all
//
      const TrackParameters *pntOnTrk=nullptr;
      if(TrkID<0){  
        prtType = undefined;
        if( pmom.dot(step) > 0.) {
          endPer = m_extrapolator->extrapolateDirectly( *inpPer, surfEnd, alongMomentum, true, pion);
        }else{
          endPer = m_extrapolator->extrapolateDirectly( *inpPer, surfEnd, oppositeMomentum, true, pion);
        }
        return endPer;
      }
        pntOnTrk=dynamic_cast<const TrackParameters*> (state.m_trkControl.at(TrkID).TrkPnt);
        if(pntOnTrk==nullptr)return endPer;
        //double inpMass=state.m_trkControl[TrkID].prtMass;
        //if(     inpMass >  0. && inpMass< 20.) {  prtType=electron; }  //VK  Disabled according to users request
        //else if(inpMass > 20. && inpMass<120.) {  prtType=muon; }      //    May be activated in future
        //else if(inpMass >120. && inpMass<200.) {  prtType=pion; }
        //else if(inpMass >200. && inpMass<700.) {  prtType=kaon; }
        //else if(inpMass >700. && inpMass<999.) {  prtType=proton; }
        //else {  prtType=undefined; }
        prtType=pion;                     // Pion hypothesis is always used for extrapolation
        iniPoint =  pntOnTrk->position(); // Redefinition of starting point for extrapolation
        step  = (*endPoint) - iniPoint;
      
// std::cout<<" TrkID="<<TrkID<<" Strat="<<Strategy<<" mom="<<pmom<<'\n';
// std::cout<<" Step ="<< step<<'\n';
// std::cout<<" iniPoint="<< iniPoint<<" endPoint="<<(*endPoint)<<" refPoint="<< refPoint<<'\n';
//
// Extrapolation for first measured point strategy. Start from it and always add material
//
      if( Strategy == 0) {  
        PropDirection dir=alongMomentum; if(pmom.dot(step)<0) dir=oppositeMomentum;
        endPer = m_extrapolator->extrapolate(*pntOnTrk, surfEnd, dir, true, prtType, addNoise);
        return endPer;
      }
// 
// Extrapolation for any measured point
//
      if( Strategy == 1) {  
         PropDirection dir=alongMomentum;  MaterialUpdateMode mmode=addNoise;
	 if(pmom.dot(step)<0){ dir=oppositeMomentum; mmode=removeNoise;}
         endPer = m_extrapolator->extrapolate( *pntOnTrk, surfEnd, dir, true, prtType, mmode);
         return endPer; 
      }
// 
// Extrapolation for perigee and B-hit
//
      if( Strategy == 2) {  
        double Border=25.; 
        bool dirPositive=true; if( pmom.dot(step) < 0.) dirPositive=false;
        if( (*endPoint).perp()>Border &&  iniPoint.perp()>Border){
	   if( dirPositive){
             endPer = m_extrapolator->extrapolate( *pntOnTrk, surfEnd, alongMomentum, true, prtType, addNoise);
           }else{
             endPer = m_extrapolator->extrapolate( *pntOnTrk, surfEnd, oppositeMomentum, true, prtType, removeNoise);
           }
           return endPer; 
        } 
        if( (*endPoint).perp()<Border &&  iniPoint.perp()<Border){
	   if( dirPositive){
             endPer = m_extrapolator->extrapolate( *pntOnTrk, surfEnd, alongMomentum, true, prtType, removeNoise);
           }else{
             endPer = m_extrapolator->extrapolate( *pntOnTrk, surfEnd, oppositeMomentum, true, prtType, addNoise);
           }
           return endPer; 
 	}
	Amg::Transform3D *trnsf = new Amg::Transform3D(); 
	(*trnsf).setIdentity();
	CylinderSurface surfBorder( trnsf, Border, 3000.);        
        if( iniPoint.perp()<Border){
          tmpPer = m_extrapolator->extrapolate( *pntOnTrk, surfBorder, alongMomentum, true, prtType, removeNoise);
	  if(tmpPer==nullptr)return nullptr;
          endPer = m_extrapolator->extrapolate( *tmpPer, surfEnd,    alongMomentum, true, prtType,    addNoise);
        }else{
          endPer = m_extrapolator->extrapolate( *pntOnTrk, surfEnd, oppositeMomentum , true, prtType, addNoise);
          return endPer; 
        }
        delete tmpPer;
        return endPer; 
      }
      return endPer; 
  }

/*--------------------------------------------------------------------------------------*/
/* Extrapolation to line                                                                */
/*											*/
/*    All points are in GLOBAL Atlas frame here!					*/
/*         DOESN't WORK FOR COMBINED TRACKS!						*/
/*--------------------------------------------------------------------------------------*/
  const TrackParameters* VKalExtPropagator::myExtrapToLine(long int TrkID, 
                                                           const TrackParameters *inpPer,
                                                           Amg::Vector3D * endPoint,
                                                           StraightLineSurface  &lineTarget,
                                                           const IVKalState& istate) const
  {   
      const TrkVKalVrtFitter::State& state = static_cast<const TrkVKalVrtFitter::State&> (istate);

      const Trk::TrackParameters* endPer=nullptr;
      ParticleHypothesis prtType = muon;
//Initial point
      Amg::Vector3D iniPoint =  inpPer->position();
      Amg::Vector3D step  = (*endPoint) - iniPoint;
//
      int Strategy = 0; if(TrkID>=0) Strategy = state.m_trkControl[TrkID].extrapolationType;
//
// Extrapolation for new track - no material at all
//
      const TrackParameters *pntOnTrk=nullptr;
      if(TrkID<0){  
        return endPer;
      }
        pntOnTrk=dynamic_cast<const TrackParameters*> (state.m_trkControl.at(TrkID).TrkPnt);
        if(!pntOnTrk) return endPer;
        //double inpMass=state.m_trkControl[TrkID].prtMass;
        //if(     inpMass >  0. && inpMass< 20.) {  prtType=electron; }  //VK  Disabled according to users request
        //else if(inpMass > 20. && inpMass<120.) {  prtType=muon; }      //    May be activated in future
        //else if(inpMass >120. && inpMass<200.) {  prtType=pion; }
        //else if(inpMass >200. && inpMass<700.) {  prtType=kaon; }
        //else if(inpMass >700. && inpMass<999.) {  prtType=proton; }
        //else {  prtType=undefined; }
        prtType=muon;                   // Muon hypothesis is always used for extrapolation
        iniPoint =  pntOnTrk->position();
        step  = (*endPoint) - iniPoint;
      
      Amg::Vector3D pmom=pntOnTrk->momentum();
//
// Extrapolation for first measured point strategy. Start from it and always add material
//
      if( Strategy == 0) {  
        PropDirection dir=alongMomentum; if(pmom.dot(step)<0) dir=oppositeMomentum;
        endPer = m_extrapolator->extrapolate(*pntOnTrk, lineTarget, dir, true, prtType, addNoise);
        if(!endPer)endPer = m_extrapolator->extrapolateDirectly(*pntOnTrk, lineTarget, dir, true, prtType);
        return endPer;
      }
// 
// Extrapolation for any measured point
//
      if( Strategy == 1 || Strategy == 2) {  
         PropDirection dir=alongMomentum;  MaterialUpdateMode mmode=addNoise;
	 if(pmom.dot(step)<0){ dir=oppositeMomentum; mmode=removeNoise;}
         endPer = m_extrapolator->extrapolate(*pntOnTrk, lineTarget, dir, true, prtType, mmode);
         return endPer; 
      }
      return endPer; 
  }

/*--------------------------------------------------------------------------------------*/
/* Simple extrapolation of neutral tracks                                               */
/*--------------------------------------------------------------------------------------*/
  const NeutralParameters* VKalExtPropagator::myExtrapNeutral( const NeutralParameters *inpPer,
                                                               Amg::Vector3D * endPoint) const
  {   
      const Trk::NeutralParameters* endPer=nullptr;
//End surface
      PerigeeSurface surfEnd( *endPoint );
      endPer = m_extrapolator->extrapolate( *inpPer, surfEnd, anyDirection, true);
      return endPer;
  }

/*--------------------------------------------------------------------------------------*/
/*   xAOD method to find FirstMeasuredPoint on TrackParticle.				*/
/*											*/
/*    All points are in GLOBAL Atlas frame here!					*/
/*    											*/
/*   xAOD::TrackParticle Perigee is extrapolated to cylinder with radius of first hit	*/
/*--------------------------------------------------------------------------------------*/
  const Perigee* VKalExtPropagator::myxAODFstPntOnTrk(const xAOD::TrackParticle* xprt) const
  {
    if(!xprt->isAvailable<float>("radiusOfFirstHit")) return nullptr;  // No radiusOfFirstHit on track

    const Trk::Perigee*  mPer = &(xprt->perigeeParameters()); 
    Amg::Transform3D *trnsf = new Amg::Transform3D(); 
    (*trnsf).setIdentity();
    CylinderSurface surfacePntOnTrk( trnsf, xprt->radiusOfFirstHit(), 20000.);        
    ParticleHypothesis prtType = pion;
    
    const TrackParameters *hitOnTrk = 
          m_extrapolator->extrapolate( *mPer, surfacePntOnTrk, alongMomentum, true, prtType, removeNoise);
//std::cout<<" Radius="<<xprt->radiusOfFirstHit()<<" extrap="<<hitOnTrk<<'\n';
    if(hitOnTrk==nullptr)hitOnTrk=m_extrapolator->extrapolateDirectly( *mPer, surfacePntOnTrk, alongMomentum, true, prtType);
    if(hitOnTrk==nullptr)return nullptr;

    //convert result to Perigee 
    PerigeeSurface surfacePerigee( hitOnTrk->position() );
    const TrackParameters *hitOnTrkPerig = m_extrapolator->extrapolate( *hitOnTrk, surfacePerigee);
    delete hitOnTrk;  // Delete temporary results
    if(hitOnTrkPerig==nullptr)return nullptr;
//std::cout<<" perig="<<(*hitOnTrkPerig)<<'\n';
    return dynamic_cast<const Perigee* > (hitOnTrkPerig);
  }

//--------------------------------------------------------------------------
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//  Setting interface to setup VKalVrt InDet extrapolator
  void TrkVKalVrtFitter::setAthenaPropagator(const Trk::IExtrapolator*  Pnt)
  {
     // Save external propagator in VKalExtPropagator object and send it to TrkVKalVrtCore
//
     if(m_fitPropagator != nullptr) delete m_fitPropagator;
     m_fitPropagator = new VKalExtPropagator( this );
     m_fitPropagator->setPropagator(Pnt);
     m_InDetExtrapolator = Pnt;  // Pointer to InDet extrapolator
  }

}
