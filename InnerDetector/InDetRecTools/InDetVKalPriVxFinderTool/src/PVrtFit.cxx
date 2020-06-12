/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalPriVxFinderTool/InDetVKalPriVxFinderTool.h"
//-------------------------------------------------
// Other stuff
#include "TMath.h"
#include <cmath>
//

 
//
//--------------------------------------------------------
//   Routine for global secondary vertex fitting
//
namespace InDet {

void
InDetVKalPriVxFinderTool::CleanTrkSet(
  Cache& cache,
  std::vector<const Trk::TrackParticleBase*>& ListParticles,
  std::vector<const Trk::Track*>& ListTracks,
  Amg::Vector3D& FitVertex,
  std::vector<double>& Chi2PerTrk,
  std::vector<const Trk::TrackParticleBase*>& badPart,
  std::vector<const Trk::Track*>& badTrk) const
{
  int Selector = 2; // Initial choice
  int NTracksVrt = 0;
  badPart.clear();
  badTrk.clear();
  const bool particlesExist{ !ListParticles.empty() };
  const bool tracksExist{ !ListTracks.empty() };
  if ((not particlesExist) and (not tracksExist))
    return;
  if (particlesExist and (not tracksExist)) {
    Selector = 1;
    NTracksVrt = ListParticles.size();
  }
  if ((not particlesExist) and tracksExist) {
    Selector = 2;
    NTracksVrt = ListTracks.size();
  }
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "CleanTrkSet() called. Total tracks= " << NTracksVrt
                    << endmsg;
  if (NTracksVrt < 10)
    return; // nothing to do
  //
  //
  // Cleaning
  //
  int it, jt;
  std::vector<int> setOfBadTrk;
  std::vector<const Trk::TrackParticleBase*> ListP(2);
  std::vector<const Trk::Track*> ListT(2);
  std::vector<double> trkChi2, ErrorMatrix;
  std::vector<std::vector<double>> tmpAtV;
  Amg::Vector3D tmpFV;
  double tmpChi2;
  std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
  for (it = 0; it < NTracksVrt - 1; it++) {
    if (Chi2PerTrk[it] < 2.)
      continue;
    for (jt = it + 1; jt < NTracksVrt; jt++) {
      if (Chi2PerTrk[jt] < 2.)
        continue;
      if (Chi2PerTrk[it] + Chi2PerTrk[jt] < 10.)
        continue; // too close pair
      if (Selector == 1) {
        ListP[0] = ListParticles[it];
        ListP[1] = ListParticles[jt];
        cache.m_sc = m_fitSvc->VKalVrtFit(ListP,
                                    tmpFV,
                                    cache.m_Momentum,
                                    cache.m_Charge,
                                    ErrorMatrix,
                                    trkChi2,
                                    tmpAtV,
                                    tmpChi2,
                                    *state);
      }
      if (Selector == 2) {
        ListT[0] = ListTracks[it];
        ListT[1] = ListTracks[jt];
        cache.m_sc = m_fitSvc->VKalVrtFit(ListT,
                                    tmpFV,
                                    cache.m_Momentum,
                                    cache.m_Charge,
                                    ErrorMatrix,
                                    trkChi2,
                                    tmpAtV,
                                    tmpChi2,
                                    *state);
      }
      if (tmpChi2 < 1.0) { // Good pair away from primary vertex found
        double direction = (tmpFV.x() - FitVertex.x()) * cache.m_Momentum.Px() +
                           +(tmpFV.y() - FitVertex.y()) * cache.m_Momentum.Py() +
                           +(tmpFV.z() - FitVertex.z()) * cache.m_Momentum.Pz();
        double det =
          ErrorMatrix[0] * ErrorMatrix[2] - ErrorMatrix[1] * ErrorMatrix[1];
        double wgt11 = ErrorMatrix[2] / det;
        double wgt22 = ErrorMatrix[0] / det;
        double wgt12 = -ErrorMatrix[1] / det;
        double distR = sqrt(
          (tmpFV.x() - FitVertex.x()) * (tmpFV.x() - FitVertex.x()) * wgt11 +
          (tmpFV.y() - FitVertex.y()) * (tmpFV.y() - FitVertex.y()) * wgt22 +
          2. * (tmpFV.x() - FitVertex.x()) * (tmpFV.y() - FitVertex.y()) *
            wgt12);
        double distZ = fabs(tmpFV.z() - FitVertex.z()) / sqrt(ErrorMatrix[5]);
        if (distR < 6.0)
          continue;
        if (direction < 0)
          continue;
        if (distZ < 4.0)
          continue;

        setOfBadTrk.push_back(it);
        setOfBadTrk.push_back(jt);
      }
    }
  }
  m_fitSvc->setVertexForConstraint(
    cache.m_BeamCnst[0], cache.m_BeamCnst[1], FitVertex.z(), *state);
  m_fitSvc->setCovVrtForConstraint(cache.m_BeamCnstWid[0] * cache.m_BeamCnstWid[0],
                                   0.,
                                   cache.m_BeamCnstWid[1] * cache.m_BeamCnstWid[1],
                                   0.,
                                   0.,
                                   56. * 56.,
                                   *state);
  m_fitSvc->setCnstType(6, *state);
  ListP.resize(1);
  ListT.resize(1);
  for (it = 0; it < NTracksVrt; it++) {
    if (Chi2PerTrk[it] < 4.)
      continue;
    if (Selector == 1) {
      ListP[0] = ListParticles[it];
      cache.m_sc = m_fitSvc->VKalVrtFit(ListP,
                                  tmpFV,
                                  cache.m_Momentum,
                                  cache.m_Charge,
                                  ErrorMatrix,
                                  trkChi2,
                                  tmpAtV,
                                  tmpChi2,
                                  *state);
    }
    if (Selector == 2) {
      ListT[0] = ListTracks[it];
      cache.m_sc = m_fitSvc->VKalVrtFit(ListT,
                                  tmpFV,
                                  cache.m_Momentum,
                                  cache.m_Charge,
                                  ErrorMatrix,
                                  trkChi2,
                                  tmpAtV,
                                  tmpChi2,
                                  *state);
    }
    if (tmpChi2 < 0.5)
      setOfBadTrk.push_back(it);
  }
  //
  // Bad track removal
  //
  ListP.clear();
  ListT.clear();
  badPart.clear();
  badTrk.clear();
  if (setOfBadTrk.empty())
    return; // nothing found
  //
  for (it = 0; it < NTracksVrt; it++) {
    std::vector<int>::iterator found =
      find(setOfBadTrk.begin(), setOfBadTrk.end(), it);
    if (found != setOfBadTrk.end()) {
      if (Selector == 1)
        badPart.push_back(ListParticles[it]);
      if (Selector == 2)
        badTrk.push_back(ListTracks[it]);
    } else {
      if (Selector == 1)
        ListP.push_back(ListParticles[it]);
      if (Selector == 2)
        ListT.push_back(ListTracks[it]);
    }
  }
  //
  if (Selector == 1)
    if (ListP.size() < badPart.size())
      return; // protection
  if (Selector == 2)
    if (ListT.size() < badTrk.size())
      return; // protection
  //
  if (Selector == 1) {
    ListParticles.resize(ListP.size());
    copy(ListP.begin(), ListP.end(), ListParticles.begin());
    std::vector<const Trk::TrackParticleBase*>::iterator last =
      unique(badPart.begin(), badPart.end());
    badPart.erase(last, badPart.end());
  }
  if (Selector == 2) {
    ListTracks.resize(ListT.size());
    copy(ListT.begin(), ListT.end(), ListTracks.begin());
    std::vector<const Trk::Track*>::iterator last =
      unique(badTrk.begin(), badTrk.end());
    badTrk.erase(last, badTrk.end());
  }
  return;
  }

  double
  InDetVKalPriVxFinderTool::FitCommonVrt(
    Cache& cache,
    std::vector<const Trk::TrackParticleBase*>& ListParticles,
    std::vector<const Trk::Track*>& ListTracks,
    double ZEstimation,
    Amg::Vector3D& FitVertex,
    std::vector<double>& ErrorMatrix,
    std::vector<double>& TrkWeights) const
  {
      int Selector=2;   // Initial choice
      int NTracksVrt = 0;
      if( ListParticles.size() == 0 && ListTracks.size() == 0 ){return -10000.; }
      if( ListParticles.size() != 0 && ListTracks.size() == 0 ){ Selector =1; NTracksVrt=ListParticles.size(); }
      if( ListParticles.size() == 0 && ListTracks.size() != 0 ){ Selector =2; NTracksVrt=ListTracks.size();}
      
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "FitCommonVrt() called. Total tracks= " << NTracksVrt<< endmsg;
      if(Selector==1 && msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Trk::TrackParticleBases are used for common Vrt!!!"<<endmsg;
      if(Selector==2 && msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Trk::Tracks are used for common Vrt!!!"<<endmsg;
//preparation
      std::vector<double> Chi2PerTrk;
      std::vector< std::vector<double> > TrkAtVrt; 
      double  Chi2 = 0.,PtF=0.,PtS=0.; int Outlier=1, OutlierNext=1, i=0;
//
// Start of fit
//
      std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
      m_fitSvc->setApproximateVertex(cache.m_BeamCnst[0],cache.m_BeamCnst[1],ZEstimation, *state);  /* Use as starting point */
      if(m_BeamConstraint) {
         m_fitSvc->setVertexForConstraint(cache.m_BeamCnst[0],cache.m_BeamCnst[1],ZEstimation,*state);
         m_fitSvc->setCovVrtForConstraint(cache.m_BeamCnstWid[0]*cache.m_BeamCnstWid[0], 0., cache.m_BeamCnstWid[1]*cache.m_BeamCnstWid[1], 0., 0., 56.*56., *state);
	 m_fitSvc->setCnstType(6, *state);
      }
      //m_fitSvc->setRobustness(m_TypeRobust);
      m_fitSvc->setRobustness(3, *state);
      m_fitSvc->setRobustScale(m_RobustScale, *state);
//
//fit itself
//
      int IterationLimit=NTracksVrt-1; if(IterationLimit<1)IterationLimit=1;
      for (i=0; i < IterationLimit; i++) {
         if(Selector==1){ cache.m_sc=m_fitSvc->VKalVrtFit(ListParticles,FitVertex, cache.m_Momentum,cache.m_Charge,
                                         ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,
                                         *state);
             NTracksVrt=ListParticles.size();
	 }	
         if(Selector==2){ cache.m_sc=m_fitSvc->VKalVrtFit(ListTracks,FitVertex, cache.m_Momentum,cache.m_Charge,
                                         ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,
                                         *state);
             NTracksVrt=ListTracks.size();
         }
	 if(NTracksVrt <= 2 )                  break;                     // Only <2 tracks left
         if(cache.m_sc.isFailure() ||  Chi2 > 1000000. ) { return -10000.;}     // No fit
         Outlier = FindMax( Chi2PerTrk ); OutlierNext=FindMaxSecond( Chi2PerTrk );
	 PtF=fabs(sin(TrkAtVrt[Outlier][0])/TrkAtVrt[Outlier][2]); PtS=fabs(sin(TrkAtVrt[OutlierNext][0])/TrkAtVrt[OutlierNext][2]);
         TrkWeights.clear();
         cache.m_sc=m_fitSvc->VKalGetTrkWeights(TrkWeights, *state);  if(cache.m_sc.isFailure() ) return -10000.;  // problem
         double dof=0.; for(int itk=0; itk<NTracksVrt; itk++)dof += TrkWeights[itk]; 
	 long int nDoF=(long int)(5.*dof-3.*NTracksVrt-3.); if(m_BeamConstraint)nDoF += 2; if(nDoF<1)nDoF=1;
         float vrtProb=TMath::Prob(Chi2,nDoF);
if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Out1="<< Chi2PerTrk[Outlier]<<", Wei="<<TrkWeights[Outlier]<<" Z="<<FitVertex.z()<<" prob="<<vrtProb<<" iter="<<i<<endmsg;
 	 if( Chi2PerTrk[FindMax(Chi2PerTrk)] < m_SecTrkChi2Cut ){
            //if(NTracksVrt<3) break;
	    break;
         }
//Track removal(outlier)
         if( (Chi2PerTrk[OutlierNext]/Chi2PerTrk[Outlier])>0.9 && PtF>PtS ) Outlier=OutlierNext;
         if(Selector==1)RemoveEntryInList(ListParticles, Outlier);
         if(Selector==2)RemoveEntryInList(ListTracks,    Outlier);
         RemoveEntryInList(Chi2PerTrk, Outlier);
//to speed up procedure -> remove second outlier       
         Outlier = FindMax( Chi2PerTrk );
         if( Chi2PerTrk[Outlier] > 20.){
            if(Selector==1 && ListParticles.size()>20)RemoveEntryInList(ListParticles,Outlier);
            if(Selector==2 && ListTracks.size()   >20)RemoveEntryInList(ListTracks,Outlier);
            RemoveEntryInList(Chi2PerTrk, Outlier);
            Outlier = FindMax( Chi2PerTrk );
         }
//
// Switch to default functional
         if(Chi2PerTrk[Outlier]<20.) m_fitSvc->setRobustness(m_TypeRobust, *state);
//
// Prepare next iteration
         m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z(), *state); /*Use as starting point*/
         if(m_BeamConstraint) {
           m_fitSvc->setVertexForConstraint(cache.m_BeamCnst[0],cache.m_BeamCnst[1],FitVertex.z(), *state);
           m_fitSvc->setCovVrtForConstraint(cache.m_BeamCnstWid[0]*cache.m_BeamCnstWid[0],0.,cache.m_BeamCnstWid[1]*cache.m_BeamCnstWid[1],0.,0., 56.*56., *state);
         }
      }
//----------------------------------------------------------------------------
//  Cleaning + refit with full error matrix
//

      state = m_fitSvc->makeState();
      if(m_BeamConstraint) {
         m_fitSvc->setVertexForConstraint(cache.m_BeamCnst[0],cache.m_BeamCnst[1],FitVertex.z(), *state);
         m_fitSvc->setCovVrtForConstraint(cache.m_BeamCnstWid[0]*cache.m_BeamCnstWid[0], 0., cache.m_BeamCnstWid[1]*cache.m_BeamCnstWid[1], 0., 0., 56.*56., *state);
	 m_fitSvc->setCnstType(6, *state);
      }
      m_fitSvc->setRobustness(m_TypeRobust, *state);
      m_fitSvc->setRobustScale(m_RobustScale, *state);
      if(Selector==1)cache.m_sc=m_fitSvc->VKalVrtFit(ListParticles,FitVertex,cache.m_Momentum,cache.m_Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,*state,true);
      if(Selector==2)cache.m_sc=m_fitSvc->VKalVrtFit(ListTracks,   FitVertex,cache.m_Momentum,cache.m_Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,*state,true);
      if(cache.m_sc.isFailure()) return -10000.;     // Problem
//
//
      Outlier = FindMax( Chi2PerTrk );
      if(Selector==1)NTracksVrt=ListParticles.size();
      if(Selector==2)NTracksVrt=ListTracks.size();
      if(NTracksVrt<1)return -10000.; //Protection
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Primary Vertex fit converged N,Chi2,ChiT="<< NTracksVrt<<", "
          <<Chi2<<", "<<Chi2PerTrk[Outlier]<<" Z="<<FitVertex.z()<<endmsg;
//--
      if( Chi2PerTrk[Outlier] > m_SecTrkChi2Cut  && NTracksVrt<=2)  
// VK 6.02.2008 Wrong!!!   Chi2/NTracksVrt > 4.                   )
//          TMath::Prob( Chi2,2*NTracksVrt-3)<1.e-3 )
      {   return -10000.;  }  
      TrkWeights.clear();
      cache.m_sc=m_fitSvc->VKalGetTrkWeights(TrkWeights, *state);
      if(cache.m_sc.isFailure()) return -10000.;     // Problem
//
//--   Create array on HEAP to store fitted track parameters. Should be removed in  SaveResults
//
      double** pntTracks=getWorkArr2(NTracksVrt,3);
      for( i=0; i<NTracksVrt; i++){ 
                 pntTracks[i][0]=TrkAtVrt[i][0];
                 pntTracks[i][1]=TrkAtVrt[i][1];
                 pntTracks[i][2]=TrkAtVrt[i][2];
      }
//
      cache.m_savedTrkFittedPerigees.push_back(pntTracks);
//
      std::vector <double> CovFull;
      StatusCode sc = m_fitSvc->VKalGetFullCov( (long int) NTracksVrt, CovFull,
                                                *state);
      int covarExist=0; if( sc.isSuccess() ) covarExist=1;
      AmgSymMatrix(5) One; One.setIdentity(); One(1,1)=0.; One(2,2)=0.; 
      std::vector< AmgSymMatrix(5) > tmpTrkCov;
      for( i=0; i<NTracksVrt; i++){ 
        if(covarExist) {  tmpTrkCov.push_back( FillCovMatrix( i,CovFull ) );}
        else{             tmpTrkCov.push_back( One ); }
      }
      cache.m_fittedTrkCov.push_back(tmpTrkCov);
//
      return Chi2;
   }

   /* Basic primary Z finder */

#define SigmaZBeam 156.
#define NMesh      140

   double
   InDetVKalPriVxFinderTool::FindZPosTrk(
     Cache& cache,
     std::vector<const Trk::Track*>& ListTracks,
     double& ControlVariable) const
   {
     std::vector<double> PtTrk, PxTrk, PyTrk;
     std::vector<double> ZTrk, PhiTrk;
     SetTrkParamVectors(cache,ListTracks, ZTrk, PtTrk, PxTrk, PyTrk, PhiTrk);
     return FindZPos(ZTrk, PtTrk, PxTrk, PyTrk, PhiTrk, ControlVariable);
  }

  double
  InDetVKalPriVxFinderTool::FindZPosTrk(
    Cache& cache,
    std::vector<const Trk::TrackParticleBase*>& ListTracks,
    double& ControlVariable) const
  {
    std::vector<double> PtTrk,PxTrk,PyTrk;
    std::vector<double> ZTrk,PhiTrk;
    SetTrkParamVectors(cache,ListTracks,ZTrk,PtTrk,PxTrk,PyTrk,PhiTrk);
    return FindZPos( ZTrk, PtTrk, PxTrk, PyTrk, PhiTrk, ControlVariable);
  }




 
//  Unified program for Z position estimation
//
  double
  InDetVKalPriVxFinderTool::FindZPos(std::vector<double>& ZTrk,
                                     std::vector<double>& PtTrk,
                                     std::vector<double>& PxTrk,
                                     std::vector<double>& PyTrk,
                                     std::vector<double>& PhiTrk,
                                     double& ControlVariable) const
  {
    double Step, LowLim, HighLim, Angle;
    int NTracks =ZTrk.size();
    int i,j;
   
    double Pt, PtSum=0., ZSum=0.;
    double ControlVariableCur, ZEstimation;
    double PtAng1[4],PtAng2[4];
    int Counter, CounterOld, Found;

    ControlVariable =-100000;
    ZEstimation     =-100000.;
    CounterOld      =0; 

    Step = 2.*SigmaZBeam/NMesh;

    for ( i=-NMesh; i< NMesh-1; i++){
        LowLim=i*Step; HighLim=LowLim+2.*Step;
        PtSum=0.;
        ZSum=0.;
	double PxSum=0., PySum=0.;
	Counter=0;
        for( j=0; j<4; j++) { PtAng1[j]=0; PtAng2[j]=0;}
        for( j=0; j<NTracks; j++){
	   if(ZTrk[j]<LowLim)continue;
	   if(ZTrk[j]>HighLim)continue;
	   ZSum  += ZTrk[j];
	   Pt= PtTrk[j] < 100000 ? PtTrk[j] : 100000.;
           PtSum += Pt;
	   PxSum += PxTrk[j]; PySum += PyTrk[j];
	   Angle=2.*PhiTrk[j]/M_PI;
	   if(Angle < 0) Angle += 4.;
 	   PtAng1[(int)Angle]                                        += Pt;
           PtAng2[(int)(Angle+0.5 <  4. ?  Angle+0.5  : Angle-3.5)]  += Pt;
	   Counter += 1;
	}	  
        if(Counter<1) continue;
	double Mass = PtSum*PtSum - PxSum*PxSum -PySum*PySum; if(Mass<=0.)Mass=0.; Mass=sqrt(Mass);
	double PtMiss = sqrt(PxSum*PxSum + PySum*PySum);
	double AngPtMax =0, AngPtMin =100000000000.;
        for ( j=0; j<4; j++) {AngPtMax = PtAng1[j]>AngPtMax ? PtAng1[j] : AngPtMax;}
        for ( j=0; j<4; j++) {if(PtAng1[j]>0) {AngPtMin = PtAng1[j]<AngPtMin ? PtAng1[j] : AngPtMin;}}
        for ( j=0; j<4; j++) {AngPtMax = PtAng2[j]>AngPtMax ? PtAng2[j] : AngPtMax;}
        for ( j=0; j<4; j++) {if(PtAng2[j]>0) {AngPtMin = PtAng2[j]<AngPtMin ? PtAng2[j] : AngPtMin;}}

        double tmpDiff=AngPtMax-AngPtMin; if( tmpDiff<0. || Counter<8) tmpDiff=0.;
        ControlVariableCur = tmpDiff+PtSum+Mass+PtMiss;
        ControlVariableCur = ControlVariableCur/Counter/1000. + Counter/30.;  //Mass and Pt are in MeV now!!!
        Found=0;
	if( Counter<=5 && Counter>CounterOld ) Found = 1;
	if( Counter> 5 && ControlVariableCur > ControlVariable) Found = 1;
        if(Found){
	   ControlVariable = ControlVariableCur;
	   ZEstimation   = ZSum/Counter;
	   CounterOld = Counter;
	}
    }
    
    return ZEstimation;
    
  }

  void
  InDetVKalPriVxFinderTool::SetTrkParamVectors(
    Cache& cache,
    std::vector<const Trk::Track*>& ListTracks,
    std::vector<double>& ZTrk,
    std::vector<double>& PtTrk,
    std::vector<double>& PxTrk,
    std::vector<double>& PyTrk,
    std::vector<double>& PhiTrk) const

  {    std::vector<const Trk::Track*>::const_iterator i_ntrk;
       AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
       const Trk::Perigee* mPer=nullptr;
       std::vector<double> Impact,ImpError;
//
       ZTrk.clear();PtTrk.clear();PxTrk.clear();PyTrk.clear(); PhiTrk.clear();
       for (i_ntrk = ListTracks.begin(); i_ntrk < ListTracks.end(); ++i_ntrk) {
//
//- Search of Perigee in TrackParameters
          mPer=GetPerigee( (*i_ntrk) ) ;
          if( mPer == nullptr ){continue;} 
          VectPerig = mPer->parameters(); // perigee
	  double InverseP = std::abs(VectPerig[4]);
	  PtTrk.push_back(std::sin(VectPerig[3])/InverseP);
	  PxTrk.push_back(std::sin(VectPerig[3])*std::cos(VectPerig[2])/InverseP);
	  PyTrk.push_back(std::sin(VectPerig[3])*std::sin(VectPerig[2])/InverseP);
	  PhiTrk.push_back(GetLimitAngle(VectPerig[2]));
	  if( cache.m_BeamCnst[0] == 0. && cache.m_BeamCnst[1] == 0.) {
	    ZTrk.push_back(VectPerig[1]);
          } else {
	    m_fitSvc->VKalGetImpact((*i_ntrk),cache.m_BeamCnst,1,Impact,ImpError);
	    ZTrk.push_back(Impact[1]);
	  }
       }
  }

  void
  InDetVKalPriVxFinderTool::SetTrkParamVectors(
    Cache& cache,
    std::vector<const Trk::TrackParticleBase*>& ListTracks,
    std::vector<double>& ZTrk,
    std::vector<double>& PtTrk,
    std::vector<double>& PxTrk,
    std::vector<double>& PyTrk,
    std::vector<double>& PhiTrk) const

  {    std::vector<const Trk::TrackParticleBase*>::const_iterator i_ntrk;
       AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
       const Trk::Perigee* mPer=nullptr;
       std::vector<double> Impact,ImpError;
//
       ZTrk.clear();PtTrk.clear();PxTrk.clear();PyTrk.clear(); PhiTrk.clear(); 
       for (i_ntrk = ListTracks.begin(); i_ntrk < ListTracks.end(); ++i_ntrk) {
//
//- Search of Perigee in TrackParameters
          mPer=GetPerigee( (*i_ntrk) ) ;
          if( mPer == nullptr ){continue;} 
          VectPerig = mPer->parameters(); // Measured perigee
	  double InverseP = std::abs(VectPerig[4]);
	  PtTrk.push_back(std::sin(VectPerig[3])/InverseP);
	  PxTrk.push_back(std::sin(VectPerig[3])*cos(VectPerig[2])/InverseP);
	  PyTrk.push_back(std::sin(VectPerig[3])*sin(VectPerig[2])/InverseP);
	  PhiTrk.push_back(GetLimitAngle(VectPerig[2]));
	  if( cache.m_BeamCnst[0] == 0. && cache.m_BeamCnst[1] == 0.) {
	    ZTrk.push_back(VectPerig[1]);
          } else {
            Amg::Vector3D refVrt(cache.m_BeamCnst.x(),cache.m_BeamCnst.y(),0.);             // Z==0 is needed for initial Z finder
	    m_fitSvc->VKalGetImpact((*i_ntrk),refVrt,1,Impact,ImpError); // 
	    ZTrk.push_back(Impact[1]);
	  }
       }
  }


}
