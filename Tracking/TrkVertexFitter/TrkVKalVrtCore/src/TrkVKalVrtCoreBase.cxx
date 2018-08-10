/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include "TrkVKalVrtCore/Derivt.h"

namespace Trk {

  VKalVrtControlBase::VKalVrtControlBase(const baseMagFld* baseFld,   const addrMagHandler addrFld, 
                                     const basePropagator* baseP, const addrPropagator addrP): 
       m_objMagFld(baseFld),
       m_funcMagFld(addrFld),
       m_objProp(baseP), 
       m_funcProp(addrP){}

  VKalVrtControlBase::~VKalVrtControlBase(){}

  VKalVrtControlBase::VKalVrtControlBase(const VKalVrtControlBase & src ): 
       m_objMagFld(src.m_objMagFld),
       m_funcMagFld(src.m_funcMagFld),
       m_objProp(src.m_objProp), 
       m_funcProp(src.m_funcProp){}

  VKalVrtControl::VKalVrtControl(const VKalVrtControlBase & base): VKalVrtControlBase(base) { 
    m_fullCovariance=0;
    m_vrtMassTot=-1.;
    m_vrtMassError=-1.;
  }
  VKalVrtControl::VKalVrtControl(const VKalVrtControl & src) : VKalVrtControlBase(src) { 
    m_fullCovariance=0; 
    m_forcft=src.m_forcft;
    m_vrtMassTot=src.m_vrtMassTot;
    m_vrtMassError=src.m_vrtMassError;
  }
  VKalVrtControl::~VKalVrtControl()  { if(m_fullCovariance) delete[] m_fullCovariance; }


  VKTrack::VKTrack(long int iniId, double Perigee[], double Covariance[], VKVertex * vk, double m):
  Chi2(0), m_mass(m)
  {
     Id = iniId; Charge=1; if(Perigee[4]<0)Charge=-1;
     for(int i=0; i<3;  i++) {fitP[i]=0; cnstP[i]=0.; iniP[i]=0.;}
     for(int i=0; i<5;  i++) {Perig[i]=refPerig[i]=Perigee[i]; rmnd[i]=0;}
     for(int i=0; i<15; i++) {refCovar[i]=Covariance[i];}
     m_originVertex = vk;
  }
  VKTrack::~VKTrack(){}

  void VKTrack::setCurrent(double Perigee[], double Weight[])
  {  for(int i=0; i<5;  i++) Perig[i]=Perigee[i];
     for(int i=0; i<15; i++) WgtM[i]=WgtM_save[i]=Weight[i];  }
  void VKTrack::setReference(double Perigee[], double Covariance[])
  {  for(int i=0; i<5;  i++) refPerig[i]=Perigee[i];
     for(int i=0; i<15; i++) refCovar[i]=Covariance[i]; }
  void VKTrack::restoreCurrentWgt()
  { for(int i=0; i<15; i++) WgtM[i]=WgtM_save[i];}

//  VKTrack::VKTrack(const VKTrack & src )                 // copy operator
//  {  Charge  =src.Charge;
//     m_mass    =src.m_mass;
//     for(int i=0; i<5;  i++) {Perig[i]=src.Perig[i];refPerig[i]=src.refPerig[i];}
//     for(int i=0; i<15; i++) {refCovar[i]=src.refCovar[i];WgtM[i]=src.WgtM[i];}
//     for(int i=0; i<3;  i++) {fitP[i]=src.fitP[i];}
//  }  

  std::ostream &  operator << ( std::ostream& out, const VKTrack& track  )
  {
	out.setf( std::ios::scientific); out.precision(3); out << std::endl;
        out << " Track par:      Iteration   <->   Ref"       << std::endl;
	out << " * a_0   : "<< track.a0()   <<"   "<< track.r_a0()   << std::endl;
	out << " * z_0   : "<< track.z()    <<"   "<< track.r_z()    << std::endl;
	out << " * theta : "<< track.theta()<<"   "<< track.r_theta()<< std::endl;
	out << " * phi   : "<< track.phi()  <<"   "<< track.r_phi()  << std::endl;
	out << " * q/R   : "<< track.invR() <<"   "<< track.r_invR() << std::endl;
	out << " * Charge: "<< track.Charge                          << std::endl;
	out << "-> with ref ErrorMatrix: "        << std::endl;out.precision(5);
	out << track.refCovar[0] <<std::endl;
	out << track.refCovar[1] <<", "<<track.refCovar[2] <<std::endl;
	out << track.refCovar[3] <<", "<<track.refCovar[4] <<", "<<track.refCovar[5] <<std::endl;
	out << track.refCovar[6] <<", "<<track.refCovar[7] <<", "<<track.refCovar[8] <<", "
	    << track.refCovar[9]<<std::endl;
	out << track.refCovar[10]<<", "<<track.refCovar[11]<<", "<<track.refCovar[12]<<", "
	    << track.refCovar[13]<<", "<<track.refCovar[14]<<std::endl;
	out << "-> and iteration WeightMatrix: "        << std::endl;
	out << track.WgtM[0] <<std::endl;
	out << track.WgtM[1] <<", "<<track.WgtM[2] <<std::endl;
	out << track.WgtM[3] <<", "<<track.WgtM[4] <<", "<<track.WgtM[5] <<std::endl;
	out << track.WgtM[6] <<", "<<track.WgtM[7] <<", "<<track.WgtM[8] <<", "
	    << track.WgtM[9]<<std::endl;
	out << track.WgtM[10]<<", "<<track.WgtM[11]<<", "<<track.WgtM[12]<<", "
	    << track.WgtM[13]<<", "<<track.WgtM[14]<<std::endl;
	return out;                                 
  }                                              




  VKVertex::VKVertex(const VKalVrtControl & FitControl): VKVertex() 
  {    m_fitterControl = std::unique_ptr<VKalVrtControl>(new VKalVrtControl(FitControl));  }

  VKVertex::VKVertex()
  { 
     useApriorVertex=0;  //no apriori vertex position used
     passNearVertex=false;   //no "pass near" constraint used
     passWithTrkCov=false;   //no trk covariance for this constraint
     for(int i=0; i<3; i++) apriorV[i]=refV[i]=iniV[i]=fitV[i]=cnstV[i]=0.;
     for(int i=0; i<6; i++) apriorVWGT[i]=0.;
     nextCascadeVrt = 0;
     for(int i=0; i<3;  i++) fitMom[i]=0.;
     for(int i=0; i<21; i++) fitCovXYZMom[i]=savedVrtMomCov[i]=0.;
     fitCovXYZMom[0]=savedVrtMomCov[0]=100.; fitCovXYZMom[2] =savedVrtMomCov[2] =100.; fitCovXYZMom[5] =savedVrtMomCov[5] =100.;
     fitCovXYZMom[9]=savedVrtMomCov[9]=100.; fitCovXYZMom[14]=savedVrtMomCov[14]=100.; fitCovXYZMom[20]=savedVrtMomCov[20]=100.;
     Chi2=0.;
     existFullCov=0;
     std::fill(ader,ader+(3*vkalNTrkM+3)*(3*vkalNTrkM+3),0);
  }

  VKVertex::~VKVertex()
  {  
       for( int i=0; i<(int)TrackList.size(); i++) delete TrackList[i];
       for( int i=0; i<(int)tmpArr.size(); i++) delete tmpArr[i];
       for( int i=0; i<(int)ConstraintList.size(); i++) delete ConstraintList[i];
       for( int i=0; i<(int)includedVrt.size(); i++) includedVrt[i]=0;  // these vertice are not owned, then must not be deleted.
  }
  void VKVertex::setRefV(double v[]){ refV[0]=v[0]; refV[1]=v[1]; refV[2]=v[2];}
  void VKVertex::setRefIterV(double v[]){ refIterV[0]=v[0]; refIterV[1]=v[1]; refIterV[2]=v[2];}
  void VKVertex::setIniV(double v[]){ iniV[0]=v[0]; iniV[1]=v[1]; iniV[2]=v[2];}
  void VKVertex::setFitV(double v[]){ fitV[0]=v[0]; fitV[1]=v[1]; fitV[2]=v[2];}
  void VKVertex::setCnstV(double v[]){ cnstV[0]=v[0]; cnstV[1]=v[1]; cnstV[2]=v[2];}


  VKVertex::VKVertex(const VKVertex & src):        //copy constructor
  m_fitterControl(new VKalVrtControl(*src.m_fitterControl)),
  Chi2(src.Chi2),                         // vertex Chi2
  useApriorVertex(src.useApriorVertex),   //for a priory vertex position knowledge usage
  passNearVertex(src.passNearVertex),     // needed for "passing near vertex" constraint
  passWithTrkCov(src.passWithTrkCov),     //  Vertex, CovVertex, Charge and derivatives 
  FVC(src.FVC)
  {
    for( int i=0; i<6; i++) {
      fitVcov[i]    =src.fitVcov[i];  // range[0:5]
      apriorVWGT[i] =src.apriorVWGT[i];
      if(i>=3) continue;
      fitV[i]    =src.fitV[i];   // range[0:2]
      iniV[i]    =src.iniV[i];
      cnstV[i]   =src.cnstV[i];
      refIterV[i]=src.refIterV[i];
      refV[i]    =src.refV[i];
      apriorV[i] =src.apriorV[i];
      fitMom[i]  = src.fitMom[i];
    }
    for( int i=0; i<21; i++){
       savedVrtMomCov[i]=src.savedVrtMomCov[i];
       fitCovXYZMom[i]=src.fitCovXYZMom[i];
    }
    nextCascadeVrt = 0;
    existFullCov = src.existFullCov;
    std::copy(src.ader,src.ader+(3*vkalNTrkM+3)*(3*vkalNTrkM+3),ader);
    //----- Creation of track copies
    for( int i=0; i<(int)src.TrackList.size(); i++) TrackList.push_back( new VKTrack(*(src.TrackList[i])) );
  }

  VKVertex& VKVertex::operator= (const VKVertex & src)        //Assignment operator
  {
    if (this!=&src){
      m_fitterControl.reset();
      m_fitterControl=std::unique_ptr<VKalVrtControl>(new VKalVrtControl(*(src.m_fitterControl)));
      Chi2=src.Chi2;                         // vertex Chi2
      useApriorVertex=src.useApriorVertex;    //for a priory vertex position knowledge usage
      passNearVertex=src.passNearVertex;      // needed for "passing near vertex" constraint
      passWithTrkCov=src.passWithTrkCov;      //  Vertex, CovVertex, Charge and derivatives 
      FVC=src.FVC;
      for( int i=0; i<6; i++) {
        fitVcov[i]    =src.fitVcov[i];  // range[0:5]
        apriorVWGT[i] =src.apriorVWGT[i];
        if(i>=3) continue;
        fitV[i]    =src.fitV[i];   // range[0:2]
        iniV[i]    =src.iniV[i];
        cnstV[i]   =src.cnstV[i];
        refIterV[i]=src.refIterV[i];
        refV[i]    =src.refV[i];
        apriorV[i] =src.apriorV[i];
        fitMom[i]  = src.fitMom[i];
      }
      for( int i=0; i<21; i++){
        savedVrtMomCov[i]=src.savedVrtMomCov[i];
        fitCovXYZMom[i]=src.fitCovXYZMom[i];
      }
      nextCascadeVrt = 0;
      existFullCov = src.existFullCov;
      std::copy(src.ader,src.ader+(3*vkalNTrkM+3)*(3*vkalNTrkM+3),ader);
    //----- Creation of track copies
      TrackList.clear();
      tmpArr.clear();
      ConstraintList.clear();
      for( int i=0; i<(int)src.TrackList.size(); i++) TrackList.push_back( new VKTrack(*(src.TrackList[i])) );
    //for( int i=0; i<(int)src.ConstraintList.size(); i++) ConstraintList.push_back( new (*(src.TrackList[i])) );
    }
    return *this;
  }

  TWRK::TWRK(){}
  TWRK::~TWRK(){}

  void VKalVrtControl::setIterationNum(int Iter)
  {
    if (Iter<3)   Iter=3;
    if (Iter>100) Iter=100;
    m_forcft.IterationNumber    = Iter;
  }

  void VKalVrtControl::setIterationPrec(double Prec)
  {
    if (Prec<1.e-5)   Prec=1.e-5;
    if (Prec>1.e-1)   Prec=1.e-1;
    m_forcft.IterationPrecision = Prec;
  }

  void VKalVrtControl::setRobustScale(double Scale)
  {
    if (Scale<0.01)   Scale=0.01;
    if (Scale>100.)   Scale=100.;
    m_forcft.RobustScale = Scale;
  }

  void VKalVrtControl::setRobustness(int Rob)
  {
    if (Rob<0)   Rob=0;
    if (Rob>7)   Rob=7;
    m_forcft.irob = Rob;
  }
  void VKalVrtControl::setMassCnstData(int Ntrk, double Mass){     // Define global mass constraint. It must be first
    double sumM(0.);
    for(int it=0; it<Ntrk; it++) sumM +=   m_forcft.wm[it];                        //sum of particle masses
    if(sumM<Mass) {
      m_forcft.wmfit[0]=Mass;
      for(int it=0; it<Ntrk; it++) m_forcft.indtrkmc[0][it]=1;                     //Set participating particles
      m_forcft.nmcnst=1;
    }
    m_forcft.useMassCnst = 1;
  }
  void VKalVrtControl::setMassCnstData(int Ntrk, std::vector<int> Index, double Mass){
    double sumM(0.);
    for(int it=0; it<Ntrk; it++) sumM +=   m_forcft.wm[Index[it]];                 //sum of particle masses
    if(sumM<Mass) {
      m_forcft.wmfit[0]=Mass;
      for(int it=0; it<Ntrk; it++) m_forcft.indtrkmc[m_forcft.nmcnst][Index[it]]=1;  //Set participating particles
      m_forcft.nmcnst++;
    }
    m_forcft.useMassCnst = 1;
  }

  void VKalVrtControl::setUsePhiCnst()   { m_forcft.usePhiCnst = 1;}
  void VKalVrtControl::setUsePlaneCnst(double a, double b, double c, double d)   { 
    if(a+b+c+d == 0.){  m_forcft.usePlaneCnst = 0;
    }else{              m_forcft.usePlaneCnst = 1; }
    m_forcft.Ap = a; m_forcft.Bp = b; m_forcft.Cp = c; m_forcft.Dp = d;
  }
  void VKalVrtControl::setUseThetaCnst() { m_forcft.useThetaCnst = 1;}
  void VKalVrtControl::setUseAprioriVrt(){ m_forcft.useAprioriVrt = 1;}
  void VKalVrtControl::setUsePointingCnst(int iType = 1 ) { m_forcft.usePointingCnst = iType<2 ? 1 : 2 ;}
  void VKalVrtControl::setUsePassNear(int iType = 1 ) { m_forcft.usePassNear = iType<2 ? 1 : 2 ;}

} /* End of namespace */
