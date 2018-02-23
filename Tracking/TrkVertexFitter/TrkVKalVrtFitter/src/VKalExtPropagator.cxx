/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
    extern vkalPropagator  myPropagator;
  }

  namespace Trk{

  // Constructor
  VKalExtPropagator::VKalExtPropagator( TrkVKalVrtFitter* pnt)
  {
       m_extrapolator = 0;
       m_vkalFitSvc = pnt;
  }
  VKalExtPropagator::~VKalExtPropagator(){}


  void VKalExtPropagator::setPropagator(const IExtrapolator*  Pnt)
  {
     m_extrapolator = Pnt;
     Trk::myPropagator.setPropagator(this);
  }

//Protection against exit outside ID volume
//
  double VKalExtPropagator::Protection(double *RefEnd) const
  {   
      double Xend=RefEnd[0] + m_vkalFitSvc->m_refFrameX;
      double Yend=RefEnd[1] + m_vkalFitSvc->m_refFrameY;
      double Zend=RefEnd[2] + m_vkalFitSvc->m_refFrameZ;
      double Rlim=sqrt(Xend*Xend+Yend*Yend) / m_vkalFitSvc->m_IDsizeR;
      double Zlim=fabs(Zend)                / m_vkalFitSvc->m_IDsizeZ;
      double Scale = Rlim; if(Zlim>Rlim) Scale=Zlim;
      return Scale;
  }

  bool VKalExtPropagator::checkTarget( double *RefEnd) const
  {
      double vX=RefEnd[0]; double vY=RefEnd[1]; double vZ=RefEnd[2];
      if( m_vkalFitSvc->m_trkControl[0].rotateToField) {
        double fx,fy,fz, oldField, newField;
        Amg::Vector3D step(RefEnd[0]-m_vkalFitSvc->m_trkControl[0].trkSavedLocalVertex.x(),
                           RefEnd[1]-m_vkalFitSvc->m_trkControl[0].trkSavedLocalVertex.y(),
                           RefEnd[2]-m_vkalFitSvc->m_trkControl[0].trkSavedLocalVertex.z());
        Amg::Vector3D globalRefEnd = (m_vkalFitSvc->m_trkControl[0].trkRotation.inverse())*step 
                                    + m_vkalFitSvc->m_trkControl[0].trkRotationVertex;
//
        vX=m_vkalFitSvc->m_trkControl[0].trkRotationVertex.x()-m_vkalFitSvc->m_refFrameX;
        vY=m_vkalFitSvc->m_trkControl[0].trkRotationVertex.y()-m_vkalFitSvc->m_refFrameY;
        vZ=m_vkalFitSvc->m_trkControl[0].trkRotationVertex.z()-m_vkalFitSvc->m_refFrameZ;
        m_vkalFitSvc->m_fitField->getMagFld(vX,vY,vZ,fx,fy,fz);
        oldField=sqrt(fx*fx+fy*fy+fz*fz); if(oldField<0.2) oldField=0.2;
//
        vX=globalRefEnd.x()-m_vkalFitSvc->m_refFrameX;
        vY=globalRefEnd.y()-m_vkalFitSvc->m_refFrameY;
        vZ=globalRefEnd.z()-m_vkalFitSvc->m_refFrameZ;
        m_vkalFitSvc->m_fitField->getMagFld(vX,vY,vZ,fx,fy,fz);
        newField=sqrt(fx*fx+fy*fy+fz*fz); if(newField<0.2) newField=0.2;
//
        if( fabs(newField-oldField)/oldField > 1.0 ) return false;
      }

      double targV[3]={ vX, vY, vZ};
      if( Protection(targV) >1. ) return false;
      return true;
  }
/*----------------------------------------------------------------------------------*/
//  Addres of this fuction is supplied to TrkVKalVrtCore for ATLAS InDet extrapolation 
//  with material. Each time VKalVrt needs propagation - it calls it. 
//
//   In case of usage with first measured point
//    it always use this point as starting point, 
//     so ParOld,CovOld,RefStart are irrelevant
//
//    When ROTATION is used in the fit   RefEnd is also in ROTATED frame,
//      and must be rotated back
/*------------------------------------------------------------------------------------*/
  void VKalExtPropagator::Propagate( long int trkID, long int Charge, 
                                     double *ParOld, double *CovOld, double *RefStart, 
                                     double *RefEnd, double *ParNew, double *CovNew) const
  {
      int trkID_loc=trkID; if(trkID_loc<0)trkID_loc=0;
//std::cout<<" Ext.Propagator TrkID="<<trkID<<"to (local!!!)="<<RefEnd[0]<<", "<<RefEnd[1]<<", "<<RefEnd[2]<<'\n';
//End point. In case of rotation extrapolation target is given in old rotated system
//           so it's rotated back using saved on given track rotation parameters 
//-----------
      double vX=RefEnd[0]; double vY=RefEnd[1]; double vZ=RefEnd[2];
      if(  m_vkalFitSvc->m_trkControl.at(trkID_loc).rotateToField &&
          !m_vkalFitSvc->m_trkControl[trkID_loc].trkRotation.isIdentity()) {

        Amg::Vector3D step(RefEnd[0]-m_vkalFitSvc->m_trkControl[trkID_loc].trkSavedLocalVertex.x(),
                           RefEnd[1]-m_vkalFitSvc->m_trkControl[trkID_loc].trkSavedLocalVertex.y(),
                           RefEnd[2]-m_vkalFitSvc->m_trkControl[trkID_loc].trkSavedLocalVertex.z());
        Amg::Vector3D globalRefEnd = (m_vkalFitSvc->m_trkControl[trkID_loc].trkRotation.inverse())*step 
                                    + m_vkalFitSvc->m_trkControl[trkID_loc].trkRotationVertex;
        vX=globalRefEnd.x()-m_vkalFitSvc->m_refFrameX;
        vY=globalRefEnd.y()-m_vkalFitSvc->m_refFrameY;
        vZ=globalRefEnd.z()-m_vkalFitSvc->m_refFrameZ;
      }
      Amg::Vector3D endPoint( vX + m_vkalFitSvc->m_refFrameX, vY + m_vkalFitSvc->m_refFrameY, vZ + m_vkalFitSvc->m_refFrameZ);
//
// ---- Make MeasuredPerigee from input. Mag.field at start point is used here
//
      std::vector<double> PerigeeIni( ParOld, ParOld+5 );
      std::vector<double> CovPerigeeIni( 15, 0. );
      if( CovOld != 0) {
//        for(int i=0; i<15;i++) CovPerigeeIni.push_back( CovOld[i] );
        std::copy(CovOld,CovOld+15,CovPerigeeIni.begin() );
      }else{
//        for(int i=0; i<15;i++) CovPerigeeIni.push_back(0.);
        CovPerigeeIni[0]=1.e6;CovPerigeeIni[2]=1.e6;CovPerigeeIni[5]=1.;CovPerigeeIni[9]=1.;CovPerigeeIni[14]=fabs(PerigeeIni[4]);
      }        
      const Perigee* inpPer = 
          m_vkalFitSvc->CreatePerigee( RefStart[0], RefStart[1], RefStart[2], PerigeeIni, CovPerigeeIni);
      const TrackParameters * inpPar= (const TrackParameters*) inpPer;
//
// ----- Magnetic field is taken at target point
//
      double fx,fy,fz,BMAG_FIXED;
      m_vkalFitSvc->m_fitField->getMagFld(vX,vY,vZ,fx,fy,fz);
      BMAG_FIXED=fz;  // standard when rotation to field direction is absent 
      if(m_vkalFitSvc->m_trkControl[trkID_loc].rotateToField) {
         BMAG_FIXED=sqrt(fx*fx+fy*fy+fz*fz);
         m_vkalFitSvc->m_fitRotatedField->setAtlasMag(BMAG_FIXED);  //set fixed ROTATED field in corresponding VKal oblect
      }
//
// ----- Prepare line target. In case of rotation - save new rotation parameters to track object
      Amg::RotationMatrix3D magFldRot; magFldRot.setIdentity();
      if(trkID>=0 && m_vkalFitSvc->m_trkControl[trkID].rotateToField){
          magFldRot=m_vkalFitSvc->getMagFldRotation(fx,fy,fz,m_vkalFitSvc->m_refFrameX,m_vkalFitSvc->m_refFrameY,0.);
          m_vkalFitSvc->m_trkControl[trkID].trkRotation=magFldRot;
          m_vkalFitSvc->m_trkControl[trkID].trkRotationVertex   = Amg::Vector3D(endPoint.x(),endPoint.y(),endPoint.z());
          m_vkalFitSvc->m_trkControl[trkID].trkSavedLocalVertex = Amg::Vector3D(RefEnd[0],RefEnd[1],RefEnd[2]);
      }
//
//-------------------- Extrapolation itself
//
      const Trk::TrackParameters* endPer = 0;
      if(trkID<0){
            endPer = myExtrapWithMatUpdate( trkID, inpPar, &endPoint);
      }else{
        if(m_vkalFitSvc->m_trkControl[trkID].rotateToField) {
            Amg::Vector3D lineCenter( vX + m_vkalFitSvc->m_refFrameX, vY + m_vkalFitSvc->m_refFrameY, vZ + m_vkalFitSvc->m_refFrameZ);
            StraightLineSurface lineTarget(new Amg::Transform3D( magFldRot.inverse() , lineCenter));
//=Check StraightLineSurface
//          GlobalPosition testpos(lineCenter.x()-100.*fx,lineCenter.y()-100.*fy,lineCenter.z()-100.*fz);
//          std::cout<<" checkLineSurface="<<*(lineTarget.globalToLocal(testpos))<<" BMAG="<<BMAG_FIXED<<'\n';
            endPer = myExtrapToLine(trkID,  inpPar, &endPoint, lineTarget);
        }else{	   
            endPer = myExtrapWithMatUpdate( trkID, inpPar, &endPoint);
        }
      }
//-----------------------------------
      if( endPer == 0 ) {   // No extrapolation done!!!
        ParNew[0]=0.; ParNew[1]=0.;ParNew[2]=0.;ParNew[3]=0.;ParNew[4]=0.;
        delete inpPer; return;
      }
      const Perigee*          mPer = dynamic_cast<const Perigee*>(endPer);
      const AtaStraightLine*  Line = dynamic_cast<const AtaStraightLine*>(endPer);
      AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
      const AmgSymMatrix(5) *CovMtx=0;
      if( mPer ){
        VectPerig = mPer->parameters(); 
        CovMtx    = mPer->covariance();
      } 
      if( Line ){
        VectPerig = Line->parameters(); 
        CovMtx    = Line->covariance();
      }      
      if( (Line==0 && mPer==0) || CovMtx==0 ){         
        ParNew[0]=0.; ParNew[1]=0.;ParNew[2]=0.;ParNew[3]=0.;ParNew[4]=0.;
        delete inpPer; return;
      }
      if(m_vkalFitSvc->m_trkControl[trkID_loc].rotateToField) {
         Amg::Vector3D rotatedMomentum(0.,0.,0.);
         if(mPer)rotatedMomentum=magFldRot*Amg::Vector3D(mPer->momentum().x(),mPer->momentum().y(),mPer->momentum().z());
         if(Line)rotatedMomentum=magFldRot*Amg::Vector3D(Line->momentum().x(),Line->momentum().y(),Line->momentum().z());
//         VectPerig[2] += atan2(rotatedMomentum.y(),rotatedMomentum.x()); //VK wrong 27.09.10
         VectPerig[2] = atan2(rotatedMomentum.y(),rotatedMomentum.x());
         VectPerig[3] = atan2(rotatedMomentum.perp(),rotatedMomentum.z());
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

      if(CovNew != 0) {
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
                                                                  Amg::Vector3D * endPoint) const
  {   
      const Trk::TrackParameters* endPer=0;
      const Trk::TrackParameters* tmpPer=0;
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
      if(TrkID>=0)refPoint = m_vkalFitSvc->m_trkControl.at(TrkID).trkRefGlobPos;
//
      Amg::Vector3D step  = (*endPoint) - iniPoint;
//
      int Strategy = 0; if(TrkID>=0) Strategy = m_vkalFitSvc->m_trkControl[TrkID].extrapolationType;
//
// Extrapolation for new track - no material at all
//
      const TrackParameters *pntOnTrk=0;
      if(TrkID<0){  
        prtType = undefined;
        if( pmom.dot(step) > 0.) {
          endPer = m_extrapolator->extrapolateDirectly( *inpPer, surfEnd, alongMomentum, true, pion);
        }else{
          endPer = m_extrapolator->extrapolateDirectly( *inpPer, surfEnd, oppositeMomentum, true, pion);
        }
        return endPer;
      }else{
        pntOnTrk=dynamic_cast<const TrackParameters*> (m_vkalFitSvc->m_trkControl.at(TrkID).TrkPnt);
        if(pntOnTrk==0)return endPer;
        //double inpMass=m_vkalFitSvc->m_trkControl[TrkID].prtMass;
        //if(     inpMass >  0. && inpMass< 20.) {  prtType=electron; }  //VK  Disabled according to users request
        //else if(inpMass > 20. && inpMass<120.) {  prtType=muon; }      //    May be activated in future
        //else if(inpMass >120. && inpMass<200.) {  prtType=pion; }
        //else if(inpMass >200. && inpMass<700.) {  prtType=kaon; }
        //else if(inpMass >700. && inpMass<999.) {  prtType=proton; }
        //else {  prtType=undefined; }
        prtType=pion;                     // Pion hypothesis is always used for extrapolation
        iniPoint =  pntOnTrk->position(); // Redefinition of starting point for extrapolation
        step  = (*endPoint) - iniPoint;
      }
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
	  if(tmpPer==0)return 0;
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
/* Extrapolation to line to allow rotation of coordinate system in nonuniform mag.field */
/*											*/
/*    All points are in GLOBAL Atlas frame here!					*/
/*         DOESN't WORK FOR COMBINED TRACKS!						*/
/*--------------------------------------------------------------------------------------*/
  const TrackParameters* VKalExtPropagator::myExtrapToLine(long int TrkID, 
                                                           const TrackParameters *inpPer,
                                                           Amg::Vector3D * endPoint,
                                                           StraightLineSurface  &lineTarget) const
  {   
      const Trk::TrackParameters* endPer=0;
      ParticleHypothesis prtType = muon;
//Initial point
      Amg::Vector3D iniPoint =  inpPer->position();
      Amg::Vector3D step  = (*endPoint) - iniPoint;
//
      int Strategy = 0; if(TrkID>=0) Strategy = m_vkalFitSvc->m_trkControl[TrkID].extrapolationType;
//
// Extrapolation for new track - no material at all
//
      const TrackParameters *pntOnTrk=0;
      if(TrkID<0){  
        return endPer;
      }else{
        pntOnTrk=dynamic_cast<const TrackParameters*> (m_vkalFitSvc->m_trkControl.at(TrkID).TrkPnt);
        if(!pntOnTrk) return endPer;
        //double inpMass=m_vkalFitSvc->m_trkControl[TrkID].prtMass;
        //if(     inpMass >  0. && inpMass< 20.) {  prtType=electron; }  //VK  Disabled according to users request
        //else if(inpMass > 20. && inpMass<120.) {  prtType=muon; }      //    May be activated in future
        //else if(inpMass >120. && inpMass<200.) {  prtType=pion; }
        //else if(inpMass >200. && inpMass<700.) {  prtType=kaon; }
        //else if(inpMass >700. && inpMass<999.) {  prtType=proton; }
        //else {  prtType=undefined; }
        prtType=muon;                   // Muon hypothesis is always used for extrapolation
        iniPoint =  pntOnTrk->position();
        step  = (*endPoint) - iniPoint;
      }
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
      const Trk::NeutralParameters* endPer=0;
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
    if(!xprt->isAvailable<float>("radiusOfFirstHit")) return 0;  // No radiusOfFirstHit on track

    const Trk::Perigee*  mPer = &(xprt->perigeeParameters()); 
    Amg::Transform3D *trnsf = new Amg::Transform3D(); 
    (*trnsf).setIdentity();
    CylinderSurface surfacePntOnTrk( trnsf, xprt->radiusOfFirstHit(), 20000.);        
    ParticleHypothesis prtType = pion;
    
    const TrackParameters *hitOnTrk = 
          m_extrapolator->extrapolate( *mPer, surfacePntOnTrk, alongMomentum, true, prtType, removeNoise);
//std::cout<<" Radius="<<xprt->radiusOfFirstHit()<<" extrap="<<hitOnTrk<<'\n';
    if(hitOnTrk==0)hitOnTrk=m_extrapolator->extrapolateDirectly( *mPer, surfacePntOnTrk, alongMomentum, true, prtType);
    if(hitOnTrk==0)return 0;

    //convert result to Perigee 
    PerigeeSurface surfacePerigee( hitOnTrk->position() );
    const TrackParameters *hitOnTrkPerig = m_extrapolator->extrapolate( *hitOnTrk, surfacePerigee);
    delete hitOnTrk;  // Delete temporary results
    if(hitOnTrkPerig==0)return 0;
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
     if(m_fitPropagator != 0) delete m_fitPropagator;
     m_fitPropagator = new VKalExtPropagator( this );
     m_fitPropagator->setPropagator(Pnt); 
     m_PropagatorType = 3;       // Set up external propagator
     m_InDetExtrapolator = Pnt;  // Pointer to InDet extrapolator
  }

}
