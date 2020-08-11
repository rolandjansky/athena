/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
//-------------------------------------------------
// Other stuff
#include "GaudiKernel/IChronoStatSvc.h"
//
#include<iostream>
#include<algorithm>


 namespace Trk {

 extern void cfpest( int ntrk, double *vrt, long int *Charge, double (*part)[5], double (*par0)[3]);
 extern void xyztrp( long int* Charge, double* vrt, double* Mom, double* CovVrtMom, double BMAG, double* Perig, double* CovPerig );

 extern int CFit(VKalVrtControl *FitCONTROL, int ifCovV0, int NTRK, 
	      long int *ich, double xyz0[3], double (*par0)[3],
	      double (*inp_Trk5)[5], double (*inp_CovTrk5)[15], 
	      double xyzfit[3], double (*parfs)[3], double ptot[4],
              double covf[21], double & chi2, double *chi2tr);


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
	double& Chi2,
        IVKalState& istate,
        bool ifCovV0 /*= false*/) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
//
//------  extract information about selected tracks
//

    int ntrk=0;
    StatusCode sc=CvtTrkTrack(InpTrk,ntrk,state);
    if(sc.isFailure())return StatusCode::FAILURE;

    int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, 
                            Chi2PerTrk, TrkAtVrt,Chi2, state, ifCovV0 ) ;
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
	double& Chi2,
        IVKalState& istate,
        bool ifCovV0 /*= false*/) const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);

//
//------  extract information about selected tracks
//
    int ntrk=0;
    std::vector<const TrackParameters*>   tmpInputC(0);
    StatusCode sc; sc.setChecked(); 
    double closestHitR=1.e6;   //VK needed for FirstMeasuredPointLimit if this hit itself is absent
    if(m_firstMeasuredPoint){               //First measured point strategy
       //------
       if(!InpTrkC.empty()){
          if( m_InDetExtrapolator == nullptr ){
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
              if(tmpInputC[tmpInputC.size()-1]==nullptr){  //Extrapolation failure 
              if(msgLvl(MSG::WARNING))msg()<< "InDetExtrapolator can't etrapolate xAOD::TrackParticle Perigee "<<
                                              "to FirstMeasuredPoint radius! Stop vertex fit!" << endmsg;
                for(unsigned int i=0; i<tmpInputC.size()-1; i++) delete tmpInputC[i]; 
                return StatusCode::FAILURE;
	      }
            }
          }
          sc=CvtTrackParameters(tmpInputC,ntrk,state);
          if(sc.isFailure()){
            for(unsigned int i=0; i<tmpInputC.size(); i++) delete tmpInputC[i]; 
            return StatusCode::FAILURE;
          }
       }
    }else{
       if(!InpTrkC.empty()) sc=CvtTrackParticle(InpTrkC,ntrk,state);
    }
    if(sc.isFailure())return StatusCode::FAILURE;
    if(!InpTrkN.empty()){sc=CvtNeutralParticle(InpTrkN,ntrk,state); if(sc.isFailure())return StatusCode::FAILURE;}
//--
    int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, ifCovV0 ) ;
//
//-- Check vertex position with respect to first measured hit and refit with plane constraint if needed
    state.m_planeCnstNDOF = 0;
    if(m_firstMeasuredPointLimit && !ierr){
       Amg::Vector3D  cnstRefPoint(0.,0.,0.);
       //----------- Use as reference either hit(state.m_globalFirstHit) or its radius(closestHitR) if hit is absent
       if(state.m_globalFirstHit)cnstRefPoint=state.m_globalFirstHit->position();
       else if(closestHitR < 1.e6){
	  Amg::Vector3D unitMom=Amg::Vector3D(Momentum.Vect().Unit().x(),Momentum.Vect().Unit().y(),Momentum.Vect().Unit().z());
	  if((Vertex+unitMom).perp() < Vertex.perp()) unitMom=-unitMom;
	  cnstRefPoint=Vertex+(closestHitR-Vertex.perp())*unitMom;
       }
       //------------
       if(Vertex.perp()>cnstRefPoint.perp() && cnstRefPoint.perp()>0.){
          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Vertex behind first measured point is detected. Constraint is applied!"<<endmsg;
          state.m_planeCnstNDOF = 1;   // Additional NDOF due to plane constraint
          double pp[3]={Momentum.Px()/Momentum.Rho(),Momentum.Py()/Momentum.Rho(),Momentum.Pz()/Momentum.Rho()};
          double D= pp[0]*(cnstRefPoint.x()-state.m_refFrameX)
                   +pp[1]*(cnstRefPoint.y()-state.m_refFrameY)
                   +pp[2]*(cnstRefPoint.z()-state.m_refFrameZ);
          state.m_vkalFitControl.setUsePlaneCnst( pp[0], pp[1], pp[2], D); 
	  std::vector<double> saveApproxV(3,0.); state.m_ApproximateVertex.swap(saveApproxV); 
          state.m_ApproximateVertex[0]=cnstRefPoint.x();
          state.m_ApproximateVertex[1]=cnstRefPoint.y();
          state.m_ApproximateVertex[2]=cnstRefPoint.z();
          ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2, state, ifCovV0 );
          state.m_vkalFitControl.setUsePlaneCnst(0.,0.,0.,0.);
          if (ierr)  {                                                                             // refit without plane cnst
             ierr = VKalVrtFit3(ntrk,Vertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2, state, ifCovV0); // if fit with it failed
             state.m_planeCnstNDOF = 0;
          }
	  state.m_ApproximateVertex.swap(saveApproxV); 
       }
    }
//--
    for(unsigned int i=0; i<tmpInputC.size(); i++) delete tmpInputC[i]; 
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
	double& Chi2,
        IVKalState& istate,
        bool ifCovV0 /*= false*/)  const
{
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);

//
//------  extract information about selected tracks
//
    int ntrk=0;
    StatusCode sc; sc.setChecked();
    if(!InpTrkC.empty()){
      sc=CvtTrackParameters(InpTrkC,ntrk,state);
      if(sc.isFailure())return StatusCode::FAILURE;
    }
    if(!InpTrkN.empty()){
      sc=CvtNeutralParameters(InpTrkN,ntrk,state);
      if(sc.isFailure())return StatusCode::FAILURE;
    }
    
    if(state.m_ApproximateVertex.empty() && state.m_globalFirstHit){  //Initial guess if absent
	state.m_ApproximateVertex.reserve(3);
        state.m_ApproximateVertex.push_back(state.m_globalFirstHit->position().x());
        state.m_ApproximateVertex.push_back(state.m_globalFirstHit->position().y());
        state.m_ApproximateVertex.push_back(state.m_globalFirstHit->position().z());
    }
    int ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt,Chi2, state, ifCovV0 ) ;
    if (ierr) return StatusCode::FAILURE;
//
//-- Check vertex position with respect to first measured hit and refit with plane constraint if needed
    state.m_planeCnstNDOF = 0;
    if(state.m_globalFirstHit && m_firstMeasuredPointLimit && !ierr){
       if(Vertex.perp()>state.m_globalFirstHit->position().perp()){
          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Vertex behind first measured point is detected. Constraint is applied!"<<endmsg;
          state.m_planeCnstNDOF = 1;   // Additional NDOF due to plane constraint
          double pp[3]={Momentum.Px()/Momentum.Rho(),Momentum.Py()/Momentum.Rho(),Momentum.Pz()/Momentum.Rho()};
          double D= pp[0]*(state.m_globalFirstHit->position().x()-state.m_refFrameX)
                   +pp[1]*(state.m_globalFirstHit->position().y()-state.m_refFrameY)
                   +pp[2]*(state.m_globalFirstHit->position().z()-state.m_refFrameZ);
          state.m_vkalFitControl.setUsePlaneCnst( pp[0], pp[1], pp[2], D); 
	  std::vector<double> saveApproxV(3,0.); state.m_ApproximateVertex.swap(saveApproxV); 
          state.m_ApproximateVertex[0]=state.m_globalFirstHit->position().x();
          state.m_ApproximateVertex[1]=state.m_globalFirstHit->position().y();
          state.m_ApproximateVertex[2]=state.m_globalFirstHit->position().z();
          ierr = VKalVrtFit3( ntrk, Vertex, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt,Chi2, state, ifCovV0 ) ;
          state.m_vkalFitControl.setUsePlaneCnst(0.,0.,0.,0.);
          if (ierr)  {                                                                                   // refit without plane cnst
	     ierr = VKalVrtFit3(ntrk,Vertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2, state, ifCovV0 ) ;     // if fit with it failed
             state.m_planeCnstNDOF = 0;
          }
	  state.m_ApproximateVertex.swap(saveApproxV); 
       }
    }
    if (ierr) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}






//--------------------------------------------------------------------------------------------------
//  Main code
//
int TrkVKalVrtFitter::VKalVrtFit3( int ntrk,
        Amg::Vector3D& Vertex,
	TLorentzVector&   Momentum,
	long int& Charge,
	dvect& ErrorMatrix, 
	dvect& Chi2PerTrk, 
        std::vector< std::vector<double> >& TrkAtVrt,
	double& Chi2,
        State& state,
        bool ifCovV0) const
{
//
//------ Variables and arrays needed for fitting kernel
//
    int ierr,i;
    double xyz0[3],covf[21],chi2f=-10.;
    double ptot[4]={0.};
    double xyzfit[3]={0.};
//
//--- Set field value at (0.,0.,0.) - some safety 
//
    double Bx,By,Bz;
    state.m_fitField.getMagFld(-state.m_refFrameX,-state.m_refFrameY,-state.m_refFrameZ,Bx,By,Bz);
//std::cout.precision(8);std::cout<<" Exact mag="<<Bx<<", "<<By<<", "<<Bz<<" at 0,0,0"<<'\n';
//
//------  Fit option setting
//
    VKalVrtConfigureFitterCore(ntrk, state);
//
//------  Fit itself
//
    state.m_FitStatus=0;
    state.m_vkalFitControl.renewFullCovariance(nullptr);                                               //
    state.m_vkalFitControl.setVertexMass(-1.);
    state.m_vkalFitControl.setVrtMassError(-1.);
    if(state.m_ApproximateVertex.size()==3 && fabs(state.m_ApproximateVertex[2])<m_IDsizeZ &&
         sqrt(state.m_ApproximateVertex[0]*state.m_ApproximateVertex[0]+state.m_ApproximateVertex[1]*state.m_ApproximateVertex[1])<m_IDsizeR)
    {
       xyz0[0]=(double)state.m_ApproximateVertex[0] - state.m_refFrameX;
       xyz0[1]=(double)state.m_ApproximateVertex[1] - state.m_refFrameY;
       xyz0[2]=(double)state.m_ApproximateVertex[2] - state.m_refFrameZ;
    } else {
       xyz0[0]=xyz0[1]=xyz0[2]=0.;
    }
    double par0[NTrMaxVFit][3];   //used only for fit preparation
    Trk::cfpest( ntrk, xyz0, state.m_ich, state.m_apar, par0);

    Chi2PerTrk.resize (ntrk);
    ierr=Trk::CFit( &state.m_vkalFitControl, ifCovV0, ntrk, state.m_ich, xyz0, par0, state.m_apar, state.m_awgt,
                    xyzfit, state.m_parfs, ptot, covf, chi2f,
                    Chi2PerTrk.data());

    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "VKalVrt fit status="<<ierr<<" Chi2="<<chi2f<<endmsg;

    Chi2 = 100000000.;
    if(ierr){
      return ierr;
    }
//  
//  Postfit operation. Creation of array for different error calculations and full error matrix copy
//
    state.m_FitStatus=ntrk;
    if(ifCovV0 && state.m_vkalFitControl.getFullCovariance()){   //If full fit error matrix is returned by VKalVrtCORE 
       int SymCovMtxSize=(3*ntrk+3)*(3*ntrk+4)/2;
       state.m_ErrMtx.assign (state.m_vkalFitControl.getFullCovariance(),
                                state.m_vkalFitControl.getFullCovariance()+SymCovMtxSize);
       state.m_vkalFitControl.renewFullCovariance(nullptr);
       ErrorMatrix.clear(); ErrorMatrix.reserve(21); ErrorMatrix.assign(covf,covf+21);
    } else {
       ErrorMatrix.clear(); ErrorMatrix.reserve(6);  ErrorMatrix.assign(covf,covf+6);
    }
//---------------------------------------------------------------------------
    Momentum.SetPxPyPzE( ptot[0], ptot[1], ptot[2], ptot[3] );
    Chi2 = (double) chi2f;

    Vertex[0]= xyzfit[0] + state.m_refFrameX;
    Vertex[1]= xyzfit[1] + state.m_refFrameY;
    Vertex[2]= xyzfit[2] + state.m_refFrameZ;

    state.m_save_xyzfit[0]=xyzfit[0];    // saving of vertex position 
    state.m_save_xyzfit[1]=xyzfit[1];    // for full error matrix
    state.m_save_xyzfit[2]=xyzfit[2];
//
// ------  Magnetic field in fitted vertex
//
    double fx,fy,BMAG_CUR;
    state.m_fitField.getMagFld(xyzfit[0] ,xyzfit[1] ,xyzfit[2] ,fx,fy,BMAG_CUR); 
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  // Safety


    //double pmom[4]={0.};
    //for ( i=0; i<ntrk; i++){
    //  double Pt = m_CNVMAG*BMAG_CUR/fabs( state.m_parfs[i][2]);
    //  double Px = Pt*cos(state.m_parfs[i][1]);
    //  double Py = Pt*sin(state.m_parfs[i][1]);
    //  double Pz = Pt/tan(state.m_parfs[i][0]);
    //  double Ee = sqrt(Px*Px+Py*Py+Pz*Pz+state.m_vkalFitControl.vk_forcft.wm[i]*state.m_vkalFitControl.vk_forcft.wm[i]);
    //  pmom[0] += Px; pmom[1] += Py; pmom[2] += Pz; pmom[3] += Ee;
    //}
    //Momentum.SetPxPyPzE( pmom[0], pmom[1], pmom[2], pmom[3] );

    Charge=0; for(i=0; i<ntrk; i++){Charge+=state.m_ich[i];};
    Charge=-Charge; //VK 30.11.2009 Change sign acoording to ATLAS 

//  std::cout.precision(8);
//  std::cout<<" Pmom="<<pmom[0]<<", "<<pmom[1]<<", "<<pmom[2]<<", "<<pmom[3]<<'\n';
//  std::cout<<" Ptot="<<ptot[0]<<", "<<ptot[1]<<", "<<ptot[2]<<", "<<ptot[3]<<'\n';
//  std::cout<<" Vertex="<<Vertex.x()<<", "<<Vertex.y()<<", "<<Vertex.z()
//           <<" LocalMag="<<BMAG_CUR<<" Chi2="<<Chi2<<" Mass="<<Momentum.m()<<'\n';


    TrkAtVrt.clear(); TrkAtVrt.reserve(ntrk);
    for(i=0; i<ntrk; i++){
      std::vector<double> TrkPar(3);
      VKalToTrkTrack(BMAG_CUR,(double)state.m_parfs[i][0],(double)state.m_parfs[i][1],(double) state.m_parfs[i][2],
                      TrkPar[0],TrkPar[1],TrkPar[2]);
      TrkPar[2] = -TrkPar[2];        // Change of sign needed
      TrkAtVrt.push_back( TrkPar );
    }
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
				      dvect& CovPerigee,
                                      IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
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
    state.m_refFrameX=state.m_refFrameY=state.m_refFrameZ=0.; //VK Work in ATLAS ref frame ONLY!!!
    long int vkCharge=-Charge; //VK 30.11.2009 Change sign according to ATLAS
//
// ------  Magnetic field in vertex
//
    double fx,fy,BMAG_CUR;
    state.m_fitField.getMagFld(Vrt[0], Vrt[1], Vrt[2] ,fx,fy,BMAG_CUR); 
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  // Safety

    Trk::xyztrp( &vkCharge, Vrt, PMom, Cov0, BMAG_CUR, Per, CovPer );

    Perigee.clear();
    CovPerigee.clear();

    for(i=0; i<5;  i++) Perigee.push_back((double)Per[i]);
    for(i=0; i<15; i++) CovPerigee.push_back((double)CovPer[i]);

//std::cout<<"Perigee V0"<<Per[0]<<", "<<Per[1]<<", "<<Per[2]<<", "<<Per[3]<<", "<<Per[4]<<'\n';
//std::cout<<"CovPeri V0"<<CovPer[10]<<", "<<CovPer[11]<<", "<<CovPer[12]<<", "<<CovPer[13]<<", "<<CovPer[14]<<'\n';

    return StatusCode::SUCCESS;
  }


  void TrkVKalVrtFitter::VKalToTrkTrack( double curBMAG, double  vp1, double  vp2, double  vp3,
                                         double& tp1, double& tp2, double& tp3) const
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
  

/* Returns a complete error matrix after fit 
  useMom=0 def (V,Perigee1,Perigee2....PerigeeNTrk) 
  useMom=1     (V,PxPyPz1,PxPyPz2....PxPyPzNTrk) 
*/

 
  StatusCode
  TrkVKalVrtFitter::VKalGetFullCov( long int NTrk, dvect& CovVrtTrk,
                                    const IVKalState& istate,
                                    bool useMom) const
  {
    assert(dynamic_cast<const State*> (&istate)!=nullptr);
    const State& state = static_cast<const State&> (istate);
    if(!state.m_FitStatus)       return StatusCode::FAILURE;
    if(NTrk<1)             return StatusCode::FAILURE;
    if(NTrk>NTrMaxVFit)    return StatusCode::FAILURE;
    if(state.m_ErrMtx.empty())    return StatusCode::FAILURE; //Now error matrix is taken from CORE in VKalVrtFit3.
//
// ------  Magnetic field access
//
    double fx,fy,BMAG_CUR;
    state.m_fitField.getMagFld(state.m_save_xyzfit[0],state.m_save_xyzfit[1],state.m_save_xyzfit[2],fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.01) BMAG_CUR=0.01;  // Safety
//
// ------ Base code
//
    int i,j,ik,jk,ip,iTrk;
    int DIM=3*NTrk+3;       //Current size of full covariance matrix
//-----------  Change to heap
//    double CovMtxOld[3*m_NTrMaxVFit+3][3*m_NTrMaxVFit+3];
//    double Deriv    [3*m_NTrMaxVFit+3][3*m_NTrMaxVFit+3];
    std::vector<std::vector<double> > Deriv (DIM);
    for (std::vector<double>& v : Deriv) v.resize (DIM);
    std::vector<double> CovMtxOld(DIM*DIM);
//-----------  Change to heap  VK 06.03.2008 temporary make a datamember 
//    int NDim=3*m_NTrMaxVFit+3;
//    double ErrMtx[ (3*m_NTrMaxVFit+3)*(3*m_NTrMaxVFit+4)/2 ];
//    double * ErrMtx = new double[ NDim*(NDim+1)/2 ];


//    long int vkNTrk = NTrk;
//    int IERR = Trk::fiterm(vkNTrk,state.m_ErrMtx.data()); //Real error matrix after fit
//    if(IERR)  {
//      for(i=0; i<DIM; i++) delete[]Deriv[i];     delete[]Deriv;
//      delete[]CovMtxOld;
//      return StatusCode::FAILURE;
//    }
//std::cout<<" ErrF1="<<state.m_ErrMtx[0] <<", "<<state.m_ErrMtx[2] <<", "<<state.m_ErrMtx[5] <<", "<<
//                      state.m_ErrMtx[9] <<", "<<state.m_ErrMtx[14]<<", "<<state.m_ErrMtx[20]<<", "<<
//                      state.m_ErrMtx[27]<<", "<<state.m_ErrMtx[35]<<", "<<state.m_ErrMtx[44]<<'\n';
 
    CovVrtTrk.resize(DIM*(DIM+1)/2);

    ip=0;
    for( i=0; i<DIM;i++) {
      for( j=0; j<=i; j++) {
         CovMtxOld[i*DIM+j]=CovMtxOld[j*DIM+i]=state.m_ErrMtx[ip++];
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
      Theta=state.m_parfs[iTrk][0];
      Phi  =state.m_parfs[iTrk][1];
      invR =state.m_parfs[iTrk][2];
       /*-----------*/
       /* dNew/dOld */
      iSt = 3 + iTrk*3;        
      if( !useMom ){
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

    return StatusCode::SUCCESS;

  }





  StatusCode TrkVKalVrtFitter::VKalGetMassError( double& dM, double& MassError,
                                                 const IVKalState& istate) const
  {    
    assert(dynamic_cast<const State*> (&istate)!=nullptr);
    const State& state = static_cast<const State&> (istate);
    if(!state.m_FitStatus) return StatusCode::FAILURE;
    dM        = state.m_vkalFitControl.getVertexMass();
    MassError = state.m_vkalFitControl.getVrtMassError();
    return StatusCode::SUCCESS;
  }
  

  StatusCode  TrkVKalVrtFitter::VKalGetTrkWeights(dvect& trkWeights,
                                                  const IVKalState& istate) const
  {
    assert(dynamic_cast<const State*> (&istate)!=nullptr);
    const State& state = static_cast<const State&> (istate);
    if(!state.m_FitStatus) return StatusCode::FAILURE;  // no fit made
    trkWeights.clear();

    int NTRK=state.m_FitStatus;

    for (int i=0; i<NTRK; i++) trkWeights.push_back(state.m_vkalFitControl.vk_forcft.robres[i]);

    return StatusCode::SUCCESS;
  }
  

  int  TrkVKalVrtFitter::VKalGetNDOF(const State& state) const
  {    
    if(!state.m_FitStatus) return 0;
    int NDOF=2*state.m_FitStatus-3;
    if(state.m_usePointingCnst)         { NDOF+=2; }
    else if(state.m_useZPointingCnst)   { NDOF+=1; }
    if( state.m_usePassNear || state.m_usePassWithTrkErr ) { NDOF+= 2; } 

    if( state.m_massForConstraint>0. )  { NDOF+=1; }
    if( !state.m_partMassCnst.empty() ) { NDOF+= state.m_partMassCnst.size(); }
    if( state.m_useAprioriVertex )      { NDOF+= 3; }    
    if( state.m_usePhiCnst )            { NDOF+=1; }  
    if( state.m_useThetaCnst )          { NDOF+=1; }  
    return NDOF;
  }
  
}
