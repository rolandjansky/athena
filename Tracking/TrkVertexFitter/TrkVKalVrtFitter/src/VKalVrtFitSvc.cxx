/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
//-------------------------------------------------
// Other stuff
#include "GaudiKernel/IChronoStatSvc.h"
//
#include<iostream>


 namespace Trk {
 extern
  void cfpest( long int* ntrk, double* vrt, long int* Charge,double* part, double* par0);
 extern
  void xyztrp( long int* Charge, double* Vertex, double* Mom,
                double* CovVrtMom, double* Perig, double* CovPerig );
 extern
   int fiterm( long int ntrk, double* errmtx);

 extern
  void cfmasserr_( long int* NTRK,  long int* List, double* parfs, double* wm, double* Deriv,
		  double* dM, double* MassError);
 extern
   void getWeights( long int NTRK, double* Weights);

 extern vkalMagFld      myMagFld;
 extern vkalPropagator  myPropagator;

 extern
      int CFit(long int iflag, long int ifCovV0, long int NTRK, 
	       long int *ich, double *xyz0, double *par0,
	       double *inp_Trk5, double *inp_CovTrk5, 
	       double *xyzfit, double *parfs, double *ptot,
               double *covf, double *chi2, double *chi2tr);
 extern
   void vksetUsePlaneCnst(double , double , double , double ); 

//__________________________________________________________________________
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//                        Interface
//

StatusCode TrkVKalVrtFitter::VKalVrtFit(const std::vector<const Track*>& InpTrk,
        Amg::Vector3D& Vertex,
	TLorentzVector&   Momentum,
	long int& Charge,
	dvect& ErrorMatrix, 
	dvect& Chi2PerTrk, 
        std::vector< std::vector<double> >& TrkAtVrt,
	double& Chi2 ) 
{
//
//------  extract information about selected tracks
//
    if(!m_isFieldInitialized)setInitializedField();  //to allow callback for init
    Trk::myMagFld.setMagHandler(m_fitField);             // needed for reenterability
    if(m_PropagatorType <=1 ){                           // needed for reenterability
       Trk::myPropagator.setTypeProp(m_PropagatorType);  // needed for reenterability
    }else{                                               // needed for reenterability
       Trk::myPropagator.setPropagator(m_fitPropagator); // needed for reenterability
    }

    long int ntrk=0;
    StatusCode sc=CvtTrkTrack(InpTrk,ntrk);
    if(sc.isFailure())return StatusCode::FAILURE;

    long int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                                 Chi2PerTrk, TrkAtVrt,Chi2 ) ;
    if (ierr) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}



StatusCode TrkVKalVrtFitter::VKalVrtFit(const std::vector<const xAOD::TrackParticle*>   & InpTrkC,
                                        const std::vector<const xAOD::NeutralParticle*> & InpTrkN,
        Amg::Vector3D& Vertex,
	TLorentzVector&   Momentum,
	long int& Charge,
	dvect& ErrorMatrix, 
	dvect& Chi2PerTrk, 
        std::vector< std::vector<double> >& TrkAtVrt,
	double& Chi2 ) 
{
    if(!m_isFieldInitialized)setInitializedField();  //to allow callback for init
    Trk::myMagFld.setMagHandler(m_fitField);             // needed for reenterability
    if(m_PropagatorType <=1 ){                           // needed for reenterability
       Trk::myPropagator.setTypeProp(m_PropagatorType);  // needed for reenterability
    }else{                                               // needed for reenterability
       Trk::myPropagator.setPropagator(m_fitPropagator); // needed for reenterability
    }
//
//------  extract information about selected tracks
//
    long int ntrk=0;
    std::vector<const TrackParameters*>   tmpInputC(0);
    StatusCode sc; sc.setChecked(); 
    double closestHitR=1.e6;   //VK needed for FirstMeasuredPointLimit if this hit itself is absent
    if(m_firstMeasuredPoint){               //First measured point strategy
       //------
       if(InpTrkC.size()){
          if( m_InDetExtrapolator == 0 && m_PropagatorType != 3 ){
            if(msgLvl(MSG::WARNING))msg()<< "No InDet extrapolator given."<<
	                                 "Can't use FirstMeasuredPoint with xAOD::TrackParticle!!!" << endmsg;
            return StatusCode::FAILURE;        
          }
          std::vector<const xAOD::TrackParticle*>::const_iterator     i_ntrk;
          if(msgLvl(MSG::DEBUG))msg()<< "Start FirstMeasuredPoint handling"<<'\n';
          unsigned int indexFMP;
          for (i_ntrk = InpTrkC.begin(); i_ntrk < InpTrkC.end(); ++i_ntrk) {
	    if ((*i_ntrk)->indexOfParameterAtPosition(indexFMP, xAOD::FirstMeasurement)){
              if(msgLvl(MSG::DEBUG))msg()<< "FirstMeasuredPoint on track is discovered. Use it."<<'\n';
	      tmpInputC.push_back(new CurvilinearParameters((*i_ntrk)->curvilinearParameters(indexFMP)));
	    }else{
              if(msgLvl(MSG::DEBUG))msg()<< "FirstMeasuredPoint on track is absent."<<
	                                    "Try extrapolation from Perigee to FisrtMeasuredPoint radius"<<'\n';
              tmpInputC.push_back(m_fitPropagator->myxAODFstPntOnTrk((*i_ntrk))); 
              if( (*i_ntrk)->radiusOfFirstHit() < closestHitR ) closestHitR=(*i_ntrk)->radiusOfFirstHit();
              if(tmpInputC[tmpInputC.size()-1]==0){  //Extrapolation failure 
              if(msgLvl(MSG::WARNING))msg()<< "InDetExtrapolator can't etrapolate xAOD::TrackParticle Perigee "<<
                                              "to FirstMeasuredPoint radius! Stop vertex fit!" << endmsg;
                for(unsigned int i=0; i<tmpInputC.size()-1; i++) delete tmpInputC[i]; 
                return StatusCode::FAILURE;
	      }
            }
          }
          sc=CvtTrackParameters(tmpInputC,ntrk);
          if(sc.isFailure()){
            for(unsigned int i=0; i<tmpInputC.size(); i++) delete tmpInputC[i]; 
            return StatusCode::FAILURE;
          }
       }
    }else{
       if(InpTrkC.size()) sc=CvtTrackParticle(InpTrkC,ntrk);
    }
    if(sc.isFailure())return StatusCode::FAILURE;
    if(InpTrkN.size()){sc=CvtNeutralParticle(InpTrkN,ntrk); if(sc.isFailure())return StatusCode::FAILURE;}
//--
    long int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                                 Chi2PerTrk, TrkAtVrt,Chi2 ) ;
//
//-- Check vertex position with respect to first measured hit and refit with plane constraint if needed
    m_planeCnstNDOF = 0;
    if(m_firstMeasuredPointLimit && !ierr){
       Amg::Vector3D  cnstRefPoint(0.,0.,0.);
       //----------- Use as reference either hit(m_globalFirstHit) or its radius(closestHitR) if hit is absent
       if(m_globalFirstHit)cnstRefPoint=m_globalFirstHit->position();
       else if(closestHitR < 1.e6){
	  Amg::Vector3D unitMom=Amg::Vector3D(Momentum.Vect().Unit().x(),Momentum.Vect().Unit().y(),Momentum.Vect().Unit().z());
	  if((Vertex+unitMom).perp() < Vertex.perp()) unitMom=-unitMom;
	  cnstRefPoint=Vertex+(closestHitR-Vertex.perp())*unitMom;
       }
       //------------
       if(Vertex.perp()>cnstRefPoint.perp() && cnstRefPoint.perp()>0.){
          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Vertex behind first measured point is detected. Constraint is applied!"<<endmsg;
          m_planeCnstNDOF = 1;   // Additional NDOF due to plane constraint
          double pp[3]={Momentum.Px()/Momentum.Rho(),Momentum.Py()/Momentum.Rho(),Momentum.Pz()/Momentum.Rho()};
          double D= pp[0]*(cnstRefPoint.x()-m_refFrameX)
                   +pp[1]*(cnstRefPoint.y()-m_refFrameY)
                   +pp[2]*(cnstRefPoint.z()-m_refFrameZ);
          vksetUsePlaneCnst( pp[0], pp[1], pp[2], D); 
	  std::vector<double> saveApproxV(3,0.); m_ApproximateVertex.swap(saveApproxV); 
          m_ApproximateVertex[0]=cnstRefPoint.x();
          m_ApproximateVertex[1]=cnstRefPoint.y();
          m_ApproximateVertex[2]=cnstRefPoint.z();
          ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                                 Chi2PerTrk, TrkAtVrt,Chi2 ) ;
          vksetUsePlaneCnst(0.,0.,0.,0.);
          if (ierr)  {
	     ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge,   // refit without plane cnst
	                  ErrorMatrix, Chi2PerTrk, TrkAtVrt,Chi2 ) ;        // if fit with it failed
             m_planeCnstNDOF = 0;
          }
	  m_ApproximateVertex.swap(saveApproxV); 
       }
    }
//--
    for(unsigned int i=0; i<tmpInputC.size(); i++) delete tmpInputC[i]; 
    if (ierr) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}



StatusCode TrkVKalVrtFitter::VKalVrtFit(const std::vector<const TrackParticleBase*>& InpTrk,
        Amg::Vector3D& Vertex,
	TLorentzVector&   Momentum,
	long int& Charge,
	dvect& ErrorMatrix, 
	dvect& Chi2PerTrk, 
        std::vector< std::vector<double> >& TrkAtVrt,
	double& Chi2 ) 
{
    if(!m_isFieldInitialized)setInitializedField();  //to allow callback for init
    Trk::myMagFld.setMagHandler(m_fitField);             // needed for reenterability
    if(m_PropagatorType <=1 ){                           // needed for reenterability
       Trk::myPropagator.setTypeProp(m_PropagatorType);  // needed for reenterability
    }else{                                               // needed for reenterability
       Trk::myPropagator.setPropagator(m_fitPropagator); // needed for reenterability
    }
//
//------  extract information about selected tracks
//
    long int ntrk=0;
    StatusCode sc;
    std::vector<const TrackParameters*> baseInpTrk;
    if(m_firstMeasuredPoint){               //First measured point strategy
       std::vector<const TrackParticleBase*>::const_iterator   i_ntrk;
       for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) baseInpTrk.push_back(GetFirstPoint(*i_ntrk));
       sc=CvtTrackParameters(baseInpTrk,ntrk);
       if(sc.isFailure()){ntrk=0; sc=CvtTrackParticle(InpTrk,ntrk);}
    }else{
       sc=CvtTrackParticle(InpTrk,ntrk);
    }
    if(sc.isFailure())return StatusCode::FAILURE;

    long int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                                 Chi2PerTrk, TrkAtVrt,Chi2 ) ;
    if (ierr) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}



StatusCode TrkVKalVrtFitter::VKalVrtFit(const std::vector<const TrackParameters*>    & InpTrkC,
                                        const std::vector<const NeutralParameters*>  & InpTrkN,
        Amg::Vector3D& Vertex,
	TLorentzVector&   Momentum,
	long int& Charge,
	dvect& ErrorMatrix, 
	dvect& Chi2PerTrk, 
        std::vector< std::vector<double> >& TrkAtVrt,
	double& Chi2 ) 
{
    if(!m_isFieldInitialized)setInitializedField();  //to allow callback for init
    Trk::myMagFld.setMagHandler(m_fitField);             // needed for reenterability
    if(m_PropagatorType <=1 ){                           // needed for reenterability
       Trk::myPropagator.setTypeProp(m_PropagatorType);  // needed for reenterability
    }else{                                               // needed for reenterability
       Trk::myPropagator.setPropagator(m_fitPropagator); // needed for reenterability
    }
//
//------  extract information about selected tracks
//
    long int ntrk=0;
    StatusCode sc; sc.setChecked();
    if(InpTrkC.size()>0){
      sc=CvtTrackParameters(InpTrkC,ntrk);
      if(sc.isFailure())return StatusCode::FAILURE;
    }
    if(InpTrkN.size()>0){
      sc=CvtNeutralParameters(InpTrkN,ntrk);
      if(sc.isFailure())return StatusCode::FAILURE;
    }
    
    if(m_ApproximateVertex.size()==0 && m_globalFirstHit){  //Initial guess if absent
	m_ApproximateVertex.reserve(3);
        m_ApproximateVertex.push_back(m_globalFirstHit->position().x());
        m_ApproximateVertex.push_back(m_globalFirstHit->position().y());
        m_ApproximateVertex.push_back(m_globalFirstHit->position().z());
    }
    long int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                                 Chi2PerTrk, TrkAtVrt,Chi2 ) ;
    if (ierr) return StatusCode::FAILURE;
//
//-- Check vertex position with respect to first measured hit and refit with plane constraint if needed
    m_planeCnstNDOF = 0;
    if(m_globalFirstHit && m_firstMeasuredPointLimit && !ierr){
       if(Vertex.perp()>m_globalFirstHit->position().perp()){
          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Vertex behind first measured point is detected. Constraint is applied!"<<endmsg;
          m_planeCnstNDOF = 1;   // Additional NDOF due to plane constraint
          double pp[3]={Momentum.Px()/Momentum.Rho(),Momentum.Py()/Momentum.Rho(),Momentum.Pz()/Momentum.Rho()};
          double D= pp[0]*(m_globalFirstHit->position().x()-m_refFrameX)
                   +pp[1]*(m_globalFirstHit->position().y()-m_refFrameY)
                   +pp[2]*(m_globalFirstHit->position().z()-m_refFrameZ);
          vksetUsePlaneCnst( pp[0], pp[1], pp[2], D); 
	  std::vector<double> saveApproxV(3,0.); m_ApproximateVertex.swap(saveApproxV); 
          m_ApproximateVertex[0]=m_globalFirstHit->position().x();
          m_ApproximateVertex[1]=m_globalFirstHit->position().y();
          m_ApproximateVertex[2]=m_globalFirstHit->position().z();
          ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                                 Chi2PerTrk, TrkAtVrt,Chi2 ) ;
          vksetUsePlaneCnst(0.,0.,0.,0.);
          if (ierr)  {
	     ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge,   // refit without plane cnst
	                  ErrorMatrix, Chi2PerTrk, TrkAtVrt,Chi2 ) ;        // if fit with it failed
             m_planeCnstNDOF = 0;
          }
	  m_ApproximateVertex.swap(saveApproxV); 
       }
    }
    if (ierr) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}






//--------------------------------------------------------------------------------------------------
//  Main code
//
long int TrkVKalVrtFitter::VKalVrtFit3( long int ntrk,
        Amg::Vector3D& Vertex,
	TLorentzVector&   Momentum,
	long int& Charge,
	dvect& ErrorMatrix, 
	dvect& Chi2PerTrk, 
        std::vector< std::vector<double> >& TrkAtVrt,
	double& Chi2 ) {
//
//-----  Timing
//
     if(m_timingProfile)m_timingProfile->chronoStart("Trk_VKalVrtFitter");
//
//------ Variables and arrays needed for fitting kernel
//
    long int ierr;
    int i;
    double xyz0[3],xyzfit[3],ptot[4],covf[21],chi2f=-10.;
    ptot[0]=ptot[1]=ptot[2]=ptot[3]=0.;
    xyzfit[0]=xyzfit[1]=xyzfit[2]=0.;
//
//--- Set field value at (0.,0.,0.) - some safety 
//
    double Bx,By,Bz;
    m_fitField->getMagFld(-m_refFrameX,-m_refFrameY,-m_refFrameZ,Bx,By,Bz);
    m_fitField->setAtlasMag(Bz);
//std::cout.precision(8);std::cout<<" Exact mag="<<Bx<<", "<<By<<", "<<Bz<<" at 0,0,0"<<'\n';
//
//------  Fit option setting
//
    VKalVrtSetOptions( ntrk );
    initCnstList();
    if(m_useMagFieldRotation) {                         // Set rotated magnetic field provider for VKalVrtCore
       Trk::myMagFld.setMagHandler(m_fitRotatedField);  // Rotated mag.field value is calculated during extrapolation
    }                                                   // This happens already in cfpest
//
//------  Fit itself
//
    m_FitStatus=0;
    if(m_ErrMtx)delete[] m_ErrMtx;              //delete previous array is exist
    m_ErrMtx=0;
    if(m_ApproximateVertex.size()==3 && fabs(m_ApproximateVertex[2])<m_IDsizeZ &&
         sqrt(m_ApproximateVertex[0]*m_ApproximateVertex[0]+m_ApproximateVertex[1]*m_ApproximateVertex[1])<m_IDsizeR)
    {
       xyz0[0]=(double)m_ApproximateVertex[0] - m_refFrameX;
       xyz0[1]=(double)m_ApproximateVertex[1] - m_refFrameY;
       xyz0[2]=(double)m_ApproximateVertex[2] - m_refFrameZ;
       if(m_useMagFieldRotation) {                         // Rotate initial guess
         Amg::Vector3D tmpv=m_trkControl[0].trkRotation*Amg::Vector3D(xyz0[0],xyz0[1],xyz0[2]);
         if(tmpv.mag()<20000.){ xyz0[0]=tmpv.x(); xyz0[1]=tmpv.y(); xyz0[2]=tmpv.z();}
         else{ xyz0[0]=0.; xyz0[1]=0.; xyz0[2]=0.; }                       //To avoid crazy initial guesses
       }
    } else {
       xyz0[0]=xyz0[1]=xyz0[2]=0.;
    }
    Trk::cfpest( &ntrk, xyz0, m_ich, &m_apar[0][0], &m_par0[0][0]);

    ierr=Trk::CFit( m_iflag, m_ifcovv0, ntrk,
                    m_ich, xyz0, &m_par0[0][0], &m_apar[0][0], &m_awgt[0][0],
                    xyzfit, &m_parfs[0][0], ptot, covf, &chi2f, m_chi2tr); 

    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "VKalVrt fit status="<<ierr<<" Chi2="<<chi2f<<endmsg;

    Chi2 = 100000000.;
    if(ierr){
      if(m_timingProfile)m_timingProfile->chronoStop("Trk_VKalVrtFitter");
      return ierr;
    }
//  
//  Postfit operation. Creation of array for different error calculations and full error matrix copy
//
    m_FitStatus=ntrk;
    if(m_ifcovv0){      //If exists - get full fit error matrix from CORE to keep it in FITTER for safety 
       m_ErrMtx = new double[ (3*ntrk+3)*(3*ntrk+4)/2  ];      //create new array for errors
       int IERR = Trk::fiterm(ntrk,m_ErrMtx);                //Real error matrix after fit
       if(IERR)  {delete[] m_ErrMtx; m_ErrMtx=0;}
       ErrorMatrix.clear(); ErrorMatrix.reserve(21); ErrorMatrix.assign(covf,covf+21);
    } else {
       ErrorMatrix.clear(); ErrorMatrix.reserve(6);  ErrorMatrix.assign(covf,covf+6);
    }
//--------------------------------------------------------------------------- 
// Rotate back to ATLAS frame if rotation was used in the fit
//
    if(m_useMagFieldRotation){
      double vnew[3],pnew[3],cnew[21];
      rotateBack(xyzfit, ptot, covf, vnew, pnew, cnew);
      for(int ik=0; ik<3;  ik++) { xyzfit[ik]=vnew[ik]; ptot[ik]=pnew[ik];}
      for(int ik=0; ik<21; ik++) { covf[ik]=cnew[ik];}
    }
//---------------------------------------------------------------------------
    Chi2 = (double) chi2f;

    Vertex[0]= xyzfit[0] + m_refFrameX;
    Vertex[1]= xyzfit[1] + m_refFrameY;
    Vertex[2]= xyzfit[2] + m_refFrameZ;

    m_save_xyzfit[0]=xyzfit[0];    // saving of vertex position 
    m_save_xyzfit[1]=xyzfit[1];    // for full error matrix
    m_save_xyzfit[2]=xyzfit[2];
//
// ------  Magnetic field in fitted vertex
//
    double fx,fy,fz,BMAG_CUR;
    m_fitField->getMagFld(xyzfit[0] ,xyzfit[1] ,xyzfit[2] ,fx,fy,fz); 
    BMAG_CUR=fz;
    if(m_useMagFieldRotation) BMAG_CUR=sqrt(fx*fx+fy*fy+fz*fz);
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  // Safety


    double Px,Py,Pz,Ee,Pt; double pmom[4]; pmom[0]=pmom[1]=pmom[2]=pmom[3]=0;
    for ( i=0; i<ntrk; i++){
      Pt = m_CNVMAG*BMAG_CUR/fabs( m_parfs[i][2]);
      Px = Pt*cos(m_parfs[i][1]);
      Py = Pt*sin(m_parfs[i][1]);
      Pz = Pt/tan(m_parfs[i][0]);
      if(m_useMagFieldRotation){
        Amg::Vector3D po=rotateBack(Px,Py,Pz); Px=po.x(); Py=po.y(); Pz=po.z();
        double npt=sqrt(Px*Px+Py*Py); if(m_parfs[i][2]<0)npt=-npt;
	double npp=sqrt(Px*Px+Py*Py+Pz*Pz);
	m_parfs[i][0]= acos( Pz/npp);
	m_parfs[i][1]=atan2( Py, Px);
	m_parfs[i][2]=m_CNVMAG*BMAG_CUR/npt;
      }
      Ee = sqrt(Px*Px+Py*Py+Pz*Pz+m_wm[i]*m_wm[i]);
      pmom[0] += Px; pmom[1] += Py; pmom[2] += Pz; pmom[3] += Ee;
    }

    Charge=0; for(i=0; i<ntrk; i++){Charge+=m_ich[i];};
    Charge=-Charge; //VK 30.11.2009 Change sign acoording to ATLAS 

    Momentum.SetPx( pmom[0] );
    Momentum.SetPy( pmom[1] );
    Momentum.SetPz( pmom[2] );
    Momentum.SetE(  pmom[3] );
//  std::cout.precision(8);
//  std::cout<<" M1="<<m_wm[0]<<", "<<m_wm[1]<<" Cnst="<<m_iflag<<" Prec="<<m_IterationPrecision<<'\n';
//  std::cout<<" Pmom="<<pmom[0]<<", "<<pmom[1]<<", "<<pmom[2]<<", "<<pmom[3]<<'\n';
//  std::cout<<" Ptot="<<ptot[0]<<", "<<ptot[1]<<", "<<ptot[2]<<", "<<ptot[3]<<'\n';
//  std::cout<<" Vertex="<<Vertex.x()<<", "<<Vertex.y()<<", "<<Vertex.z()
//           <<" LocalMag="<<BMAG_CUR<<" Chi2="<<Chi2<<" Mass="<<Momentum.m()<<'\n';


    Chi2PerTrk.clear(); Chi2PerTrk.reserve(ntrk);
    for(i=0; i<ntrk; i++){Chi2PerTrk.push_back( (double) m_chi2tr[i]); }

    TrkAtVrt.clear(); TrkAtVrt.reserve(ntrk);
    for(i=0; i<ntrk; i++){
      std::vector<double> TrkPar(3);
      VKalToTrkTrack(BMAG_CUR,(double)m_parfs[i][0],(double)m_parfs[i][1],(double) m_parfs[i][2],
                      TrkPar[0],TrkPar[1],TrkPar[2]);
      TrkPar[2] = -TrkPar[2];        // Change of sign needed
      TrkAtVrt.push_back( TrkPar );
    }
    if(m_timingProfile)m_timingProfile->chronoStop("Trk_VKalVrtFitter");
    return 0;
  }



//  Converts Vertex, Mom, CovVrtMom in GLOBAL SYSTEM into perigee
//
//

  StatusCode TrkVKalVrtFitter::VKalVrtCvtTool(const Amg::Vector3D& Vertex,
	                              const TLorentzVector& Momentum,
	                              const dvect& CovVrtMom,
				      const long int& Charge,
				      dvect& Perigee,
				      dvect& CovPerigee)
  {
    int i,j,ij;				      
    double Vrt[3],PMom[4],Cov0[21],Per[5],CovPer[15];

    for(i=0; i<3;  i++) Vrt[i]=Vertex[i];
    for(i=0; i<3;  i++) PMom[i]=Momentum[i];
    for(ij=i=0; i<6; i++){
      for(j=0; j<=i; j++){
        Cov0[ij]=CovVrtMom[ij];
        ij++;
      }
    }
    m_refFrameX=m_refFrameY=m_refFrameZ=0.; //VK Work in ATLAS ref frame ONLY!!!
    long int vkCharge=-Charge; //VK 30.11.2009 Change sign according to ATLAS

    Trk::myMagFld.setMagHandler(m_fitField);             // needed for reenterability
    if(m_PropagatorType <=1 ){                           // needed for reenterability
       Trk::myPropagator.setTypeProp(m_PropagatorType);  // needed for reenterability
    }else{                                               // needed for reenterability
       Trk::myPropagator.setPropagator(m_fitPropagator); // needed for reenterability
    }

    Trk::xyztrp( &vkCharge, Vrt, PMom, Cov0, Per, CovPer );

    Perigee.clear();
    CovPerigee.clear();

    for(i=0; i<5;  i++) Perigee.push_back((double)Per[i]);
    for(i=0; i<15; i++) CovPerigee.push_back((double)CovPer[i]);

//std::cout<<"Perigee V0"<<Per[0]<<", "<<Per[1]<<", "<<Per[2]<<", "<<Per[3]<<", "<<Per[4]<<'\n';
//std::cout<<"CovPeri V0"<<CovPer[10]<<", "<<CovPer[11]<<", "<<CovPer[12]<<", "<<CovPer[13]<<", "<<CovPer[14]<<'\n';

    return StatusCode::SUCCESS;
  }


  void TrkVKalVrtFitter::VKalToTrkTrack( double curBMAG, double  vp1, double  vp2, double  vp3,
                                         double& tp1, double& tp2, double& tp3)
//tp - ATLAS parameters, vp - VKalVrt parameters//
  {   tp1= vp2;   //phi angle
      tp2= vp1;   //theta angle
      tp3= vp3 * sin( vp1 ) /(m_CNVMAG*curBMAG);
           // -pi < phi < pi  range
      while ( tp1 > 3.1415926535897) tp1 -= 2.*3.1415926535897;    
      while ( tp1 <-3.1415926535897) tp1 += 2.*3.1415926535897;    
           // 0 < Theta < pi   range
      while ( tp2 > 3.1415926535897) tp2 -= 2.*3.1415926535897;    
      while ( tp2 <-3.1415926535897) tp2 += 2.*3.1415926535897;    
      if    ( tp2 < 0.) { 
        tp2 = fabs(tp2); tp1 += 3.1415926535897; 
        while ( tp1 > 3.1415926535897) tp1 -= 2.*3.1415926535897;    
      } 

  } 
  

/*--------------------------------------------------------*/
/* Returns (6x6) covariance matrix for (Vx,Vy,Vz,Px,Py,Pz)*/
/*   for track iTrk out of complete set of NTrk tracks    */
/*   Should be called only after successful fit           */
 
  StatusCode TrkVKalVrtFitter::VKalGetTrkCov(long int iTrk, long int NTrk, dvect& CovVrtTrk)
  {
    if(iTrk<1)       return StatusCode::FAILURE;
    if(iTrk>NTrk)    return StatusCode::FAILURE;
    if(!m_FitStatus) return StatusCode::FAILURE;
    if(!m_ErrMtx)    return StatusCode::FAILURE; //Now error matrix is taken from CORE in VKalVrtFit3.
//
// ------  Magnetic field access
//
    double fx,fy,BMAG_CUR;
    m_fitField->getMagFld(m_save_xyzfit[0],m_save_xyzfit[1],m_save_xyzfit[2],fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  // Safety
//
// ------- Base code
//
    int i,j,ik,jk;
//---- Change to heap    VK 06.03.2008 temporary make a datamember 
//    double ErrMtx[ (3*m_NTrMaxVFit+3)*(3*m_NTrMaxVFit+4)/2 ];
//    double * ErrMtx = new double[ (3*m_NTrMaxVFit+3)*(3*m_NTrMaxVFit+4)/2  ];
    double CovMtxOld[6][6];
    double CovMtx   [6][6];

//    long int vkNTrk = NTrk;
//    int IERR = Trk::fiterm(vkNTrk,m_ErrMtx); //Real error matrix after fit
//    if(IERR)       return StatusCode::FAILURE;

    CovVrtTrk.clear();

    CovMtxOld[0][0] =                   m_ErrMtx[0];
    CovMtxOld[1][0] = CovMtxOld[0][1] = m_ErrMtx[1];
    CovMtxOld[1][1] =                   m_ErrMtx[2];
    CovMtxOld[2][0] = CovMtxOld[0][2] = m_ErrMtx[3];
    CovMtxOld[2][1] = CovMtxOld[1][2] = m_ErrMtx[4];
    CovMtxOld[2][2] =                   m_ErrMtx[5];

    int pnt = (iTrk*3+1)*iTrk*3/2;
    CovMtxOld[3][0] = CovMtxOld[0][3] = m_ErrMtx[pnt];
    CovMtxOld[3][1] = CovMtxOld[1][3] = m_ErrMtx[pnt+1];
    CovMtxOld[3][2] = CovMtxOld[2][3] = m_ErrMtx[pnt+2];
    CovMtxOld[3][3] =                   m_ErrMtx[pnt+iTrk*3];
    pnt = (iTrk*3+1+1)*(iTrk*3+1)/2;
    CovMtxOld[4][0] = CovMtxOld[0][4] = m_ErrMtx[pnt];
    CovMtxOld[4][1] = CovMtxOld[1][4] = m_ErrMtx[pnt+1];
    CovMtxOld[4][2] = CovMtxOld[2][4] = m_ErrMtx[pnt+2];
    CovMtxOld[4][3] = CovMtxOld[3][4] = m_ErrMtx[pnt+iTrk*3];
    CovMtxOld[4][4] =                   m_ErrMtx[pnt+iTrk*3+1];
    pnt = (iTrk*3+2+1)*(iTrk*3+2)/2;
    CovMtxOld[5][0] = CovMtxOld[0][5] = m_ErrMtx[pnt];
    CovMtxOld[5][1] = CovMtxOld[1][5] = m_ErrMtx[pnt+1];
    CovMtxOld[5][2] = CovMtxOld[2][5] = m_ErrMtx[pnt+2];
    CovMtxOld[5][3] = CovMtxOld[3][5] = m_ErrMtx[pnt+iTrk*3];
    CovMtxOld[5][4] = CovMtxOld[4][5] = m_ErrMtx[pnt+iTrk*3+1];
    CovMtxOld[5][5] =                   m_ErrMtx[pnt+iTrk*3+2];

//std::cout<<" Err1="<<m_ErrMtx[0] <<", "<<m_ErrMtx[2] <<", "<<m_ErrMtx[5] <<", "<<
//                     m_ErrMtx[9] <<", "<<m_ErrMtx[14]<<", "<<m_ErrMtx[20]<<", "<<
//                     m_ErrMtx[27]<<", "<<m_ErrMtx[35]<<", "<<m_ErrMtx[44]<<'\n';
//    delete [] ErrMtx;
   
    double Theta=m_parfs[iTrk-1][0];
    double Phi  =m_parfs[iTrk-1][1];
    double invR =m_parfs[iTrk-1][2];
    double pt=(m_CNVMAG*BMAG_CUR)/fabs(invR);
    double px=pt*cos(Phi);
    double py=pt*sin(Phi);
    double pz=pt/tan(Theta);
    
    double Deriv[6][6];
    for(i=0;i<6;i++){ for(j=0;j<6;j++) {Deriv[i][j]=0.;}}
    Deriv[0][0]= 1.;
    Deriv[1][1]= 1.;
    Deriv[2][2]= 1.;

    Deriv[3][3]=   0;                           //dPx/dTheta
    Deriv[3][4]= -py;                           //dPx/dPhi
    Deriv[3][5]= -px/invR;                      //dPx/dinvR

    Deriv[4][3]=   0;                           //dPy/dTheta
    Deriv[4][4]=  px;                           //dPy/dPhi
    Deriv[4][5]= -py/invR;                      //dPy/dinvR

    Deriv[5][3]= -pt/sin(Theta)/sin(Theta);     //dPz/dTheta
    Deriv[5][4]=   0;                           //dPz/dPhi
    Deriv[5][5]= -pz/invR;                      //dPz/dinvR
//----------
    for(i=0;i<6;i++){
     for(j=i;j<6;j++){
       CovMtx[i][j]=0.;
       for(ik=0;ik<6;ik++){
         if(Deriv[i][ik]==0.)continue;
         for(jk=5;jk>=0;jk--){
           if(Deriv[j][jk]==0.)continue;
           CovMtx[i][j] += Deriv[i][ik]*CovMtxOld[ik][jk]*Deriv[j][jk];
       }}
    }}
    CovVrtTrk.resize(21);
    CovVrtTrk[0] =CovMtx[0]  [0];
    CovVrtTrk[1] =CovMtx[0]  [1];
    CovVrtTrk[2] =CovMtx[1]  [1];
    CovVrtTrk[3] =CovMtx[0]  [2];
    CovVrtTrk[4] =CovMtx[1]  [2];
    CovVrtTrk[5] =CovMtx[2]  [2];
    CovVrtTrk[6] =CovMtx[0]  [3];
    CovVrtTrk[7] =CovMtx[1]  [3];
    CovVrtTrk[8] =CovMtx[2]  [3];
    CovVrtTrk[9] =CovMtx[3]  [3];
    CovVrtTrk[10]=CovMtx[0]  [4];
    CovVrtTrk[11]=CovMtx[1]  [4];
    CovVrtTrk[12]=CovMtx[2]  [4];
    CovVrtTrk[13]=CovMtx[3]  [4];
    CovVrtTrk[14]=CovMtx[4]  [4];
    CovVrtTrk[15]=CovMtx[0]  [5];
    CovVrtTrk[16]=CovMtx[1]  [5];
    CovVrtTrk[17]=CovMtx[2]  [5];
    CovVrtTrk[18]=CovMtx[3]  [5];
    CovVrtTrk[19]=CovMtx[4]  [5];
    CovVrtTrk[20]=CovMtx[5]  [5];

//std::cout<<" Err2="<<CovVrtTrk[0] <<", "<<CovVrtTrk[2] <<", "<<CovVrtTrk[5] <<", "<<
//                     CovVrtTrk[9] <<", "<<CovVrtTrk[14]<<", "<<CovVrtTrk[20]<<'\n';

    return StatusCode::SUCCESS;

  }


/* Returns a complete error matrix after fit 
  useMom=0 def (V,Perigee1,Perigee2....PerigeeNTrk) 
  useMom=1     (V,PxPyPz1,PxPyPz2....PxPyPzNTrk) 
*/

 
  StatusCode TrkVKalVrtFitter::VKalGetFullCov( long int NTrk, dvect& CovVrtTrk, int useMom)
  {
    if(!m_FitStatus)       return StatusCode::FAILURE;
    if(NTrk<1)             return StatusCode::FAILURE;
    if(NTrk>NTrMaxVFit)    return StatusCode::FAILURE;
    if(!m_ErrMtx)    return StatusCode::FAILURE; //Now error matrix is taken from CORE in VKalVrtFit3.
//
// ------  Magnetic field access
//
    double fx,fy,BMAG_CUR;
    m_fitField->getMagFld(m_save_xyzfit[0],m_save_xyzfit[1],m_save_xyzfit[2],fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  // Safety
//
// ------ Base code
//
    int i,j,ik,jk,ip,iTrk;
    int DIM=3*NTrk+3;       //Current size of full covariance matrix
//-----------  Change to heap
//    double CovMtxOld[3*m_NTrMaxVFit+3][3*m_NTrMaxVFit+3];
//    double Deriv    [3*m_NTrMaxVFit+3][3*m_NTrMaxVFit+3];
    double **Deriv     = new double*[DIM]; for(i=0; i<DIM; i++) Deriv[i]     = new double[DIM];
    double *CovMtxOld  = new double[DIM*DIM];
//-----------  Change to heap  VK 06.03.2008 temporary make a datamember 
//    int NDim=3*m_NTrMaxVFit+3;
//    double ErrMtx[ (3*m_NTrMaxVFit+3)*(3*m_NTrMaxVFit+4)/2 ];
//    double * ErrMtx = new double[ NDim*(NDim+1)/2 ];


//    long int vkNTrk = NTrk;
//    int IERR = Trk::fiterm(vkNTrk,m_ErrMtx); //Real error matrix after fit
//    if(IERR)  {
//      for(i=0; i<DIM; i++) delete[]Deriv[i];     delete[]Deriv;
//      delete[]CovMtxOld;
//      return StatusCode::FAILURE;
//    }
//std::cout<<" ErrF1="<<m_ErrMtx[0] <<", "<<m_ErrMtx[2] <<", "<<m_ErrMtx[5] <<", "<<
//                      m_ErrMtx[9] <<", "<<m_ErrMtx[14]<<", "<<m_ErrMtx[20]<<", "<<
//                      m_ErrMtx[27]<<", "<<m_ErrMtx[35]<<", "<<m_ErrMtx[44]<<'\n';
 
    CovVrtTrk.resize(DIM*(DIM+1)/2);

    //m_timingProfile->chronoStart("Trk_CovMatrix");

    ip=0;
    for( i=0; i<DIM;i++) {
      for( j=0; j<=i; j++) {
         CovMtxOld[i*DIM+j]=CovMtxOld[j*DIM+i]=m_ErrMtx[ip++];
      }
    }

    //delete [] ErrMtx;

    for(i=0;i<DIM;i++){ for(j=0;j<DIM;j++) {Deriv[i][j]=0.;}}
    Deriv[0][0]= 1.;
    Deriv[1][1]= 1.;
    Deriv[2][2]= 1.;

    int iSt=0;
    double Theta,invR,Phi;
    for( iTrk=0; iTrk<NTrk; iTrk++){ 
      Theta=m_parfs[iTrk][0];
      Phi  =m_parfs[iTrk][1];
      invR =m_parfs[iTrk][2];
       /*-----------*/
       /* dNew/dOld */
      iSt = 3 + iTrk*3;        
      if( useMom == 0 ){
        Deriv[iSt  ][iSt+1] =   1;                                             //    Phi <-> Theta
        Deriv[iSt+1][iSt  ] =   1;                                             //    Phi <-> Theta
        Deriv[iSt+2][iSt  ] = -(cos(Theta)/(m_CNVMAG*BMAG_CUR)) * invR ;     //    d1/p  / dTheta
        Deriv[iSt+2][iSt+2] = -(sin(Theta)/(m_CNVMAG*BMAG_CUR))  ;           //    d1/p  / d1/R
      }else{
        double pt=(m_CNVMAG*BMAG_CUR)/fabs(invR);
        double px=pt*cos(Phi);
        double py=pt*sin(Phi);
        double pz=pt/tan(Theta);
        Deriv[iSt  ][iSt  ]=   0;                           //dPx/dTheta
        Deriv[iSt  ][iSt+1]= -py;                           //dPx/dPhi
        Deriv[iSt  ][iSt+2]= -px/invR;                      //dPx/dinvR

        Deriv[iSt+1][iSt  ]=   0;                           //dPy/dTheta
        Deriv[iSt+1][iSt+1]=  px;                           //dPy/dPhi
        Deriv[iSt+1][iSt+2]= -py/invR;                      //dPy/dinvR

        Deriv[iSt+2][iSt  ]= -pt/sin(Theta)/sin(Theta);     //dPz/dTheta
        Deriv[iSt+2][iSt+1]=   0;                           //dPz/dPhi
        Deriv[iSt+2][iSt+2]= -pz/invR;                      //dPz/dinvR
      }
    }
//----------  Only upper half if filled and saved
    int ipnt=0;
    double tmp, tmpTmp; 
    for(i=0;i<DIM;i++){
     for(j=0;j<=i;j++){
       tmp=0.; 
       for(ik=0;ik<DIM;ik++){
          if(Deriv[i][ik] == 0.) continue;
          tmpTmp=0;
          for(jk=DIM-1;jk>=0;jk--){
             if(Deriv[j][jk] == 0.) continue;
             tmpTmp += CovMtxOld[ik*DIM+jk]*Deriv[j][jk];
          }
          tmp += Deriv[i][ik]*tmpTmp;
       }
       CovVrtTrk[ipnt++]=tmp;
    }}
//------------------------------------------------------------------------------------
//std::cout<<" ErrF2="<<CovVrtTrk[0] <<", "<<CovVrtTrk[2] <<", "<<CovVrtTrk[5] <<", "<<
//                      CovVrtTrk[9] <<", "<<CovVrtTrk[14]<<", "<<CovVrtTrk[20]<<", "<<
//                      CovVrtTrk[27]<<", "<<CovVrtTrk[35]<<", "<<CovVrtTrk[44]<<'\n';

    for(i=0; i<DIM; i++) delete[]Deriv[i];
    delete[]Deriv;
    delete[]CovMtxOld;
    //m_timingProfile->chronoStop("Trk_CovMatrix");
    return StatusCode::SUCCESS;

  }





  StatusCode TrkVKalVrtFitter::VKalGetMassError( std::vector<int> ListOfTracks , double& dM, double& MassError)
  {    
    if(!m_FitStatus) return StatusCode::FAILURE;
    if((int) ListOfTracks.size() != m_FitStatus) return StatusCode::FAILURE;

    double Deriv[ 3*NTrMaxVFit+3 ] = {0.};
    long int Tist[NTrMaxVFit+1] = {0};
 
    for (int i=0; i<(int)ListOfTracks.size();i++){
       Tist[i]=0;
       if(ListOfTracks[i] != 0 ) Tist[i]=1;
    }

    long int NTRK=ListOfTracks.size();
        
    Trk::cfmasserr_(&NTRK, Tist, &m_parfs[0][0], m_wm, Deriv, &dM, &MassError);

    return StatusCode::SUCCESS;
  }
  

  StatusCode  TrkVKalVrtFitter::VKalGetTrkWeights(dvect& trkWeights)
  {    
    if(!m_FitStatus) return StatusCode::FAILURE;  // no fit made
    trkWeights.clear();

    double *tmp = new double[m_FitStatus];
    long int NTRK=m_FitStatus;

    Trk::getWeights( NTRK, tmp);

    for (int i=0; i<NTRK; i++) trkWeights.push_back(tmp[i]);
    delete[] tmp;
    return StatusCode::SUCCESS;
  }
  

  int  TrkVKalVrtFitter::VKalGetNDOF()
  {    
    if(!m_FitStatus) return 0;
    int NDOF=2*m_FitStatus-3;
    if(m_usePointingCnst)         { NDOF+=2; }
    else if(m_useZPointingCnst)   { NDOF+=1; }
    if( m_usePassNear || m_usePassWithTrkErr ) { NDOF+= 2; } 

    if( m_MassForConstraint>0. )  { NDOF+=1; }
    if( m_PartMassCnst.size()>0 ) { NDOF+= m_PartMassCnst.size(); }
    if( m_useAprioriVertex )      { NDOF+= 3; }    
    if( m_usePhiCnst )            { NDOF+=1; }  
    if( m_useThetaCnst )          { NDOF+=1; }  
    return NDOF;
  }
  
}
