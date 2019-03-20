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
       vk_objMagFld(baseFld),
       vk_funcMagFld(addrFld),
       vk_objProp(baseP), 
       vk_funcProp(addrP){}

  VKalVrtControlBase::~VKalVrtControlBase(){}

  VKalVrtControlBase::VKalVrtControlBase(const VKalVrtControlBase & src ): 
       vk_objMagFld(src.vk_objMagFld),
       vk_funcMagFld(src.vk_funcMagFld),
       vk_objProp(src.vk_objProp), 
       vk_funcProp(src.vk_funcProp){}

  VKalVrtControl::VKalVrtControl(const VKalVrtControlBase & base): VKalVrtControlBase(base) { 
    m_fullCovariance=nullptr;
    m_vrtMassTot=-1.;
    m_vrtMassError=-1.;
    m_cascadeEvent=nullptr;
  }
  VKalVrtControl::VKalVrtControl(const VKalVrtControl & src) : VKalVrtControlBase(src) { 
    m_fullCovariance=nullptr; 
    vk_forcft=src.vk_forcft;
    m_vrtMassTot=src.m_vrtMassTot;
    m_vrtMassError=src.m_vrtMassError;
    m_cascadeEvent=src.m_cascadeEvent;
  }
  VKalVrtControl::~VKalVrtControl()  { if(m_fullCovariance) delete[] m_fullCovariance;}


  VKTrack::VKTrack(long int iniId, double Perigee[], double Covariance[], VKVertex * vk, double m):
  Chi2(0), m_mass(m)
  {
     Id = iniId; Charge=1; if(Perigee[4]<0)Charge=-1;
     for(int i=0; i<3;  i++) {fitP[i]=0; cnstP[i]=0.; iniP[i]=0.;}
     for(int i=0; i<5;  i++) {Perig[i]=refPerig[i]=Perigee[i]; rmnd[i]=0;}
     for(int i=0; i<15; i++) {refCovar[i]=Covariance[i];}
     m_originVertex = vk;
  }


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
	//out.setf( std::ios::defaultfloat ); out.precision(5); out << std::endl;
	out.precision(5); out << std::defaultfloat;
        out << " Track par:      Iteration   <->   Ref"       << std::endl;
	out << " * a_0   : "<< track.a0()   <<"   "<< track.r_a0()   << std::endl;
	out << " * z_0   : "<< track.z()    <<"   "<< track.r_z()    << std::endl;
	out << " * theta : "<< track.theta()<<"   "<< track.r_theta()<< std::endl;
	out << " * phi   : "<< track.phi()  <<"   "<< track.r_phi()  << std::endl;
	out << " * q/R   : "<< track.invR() <<"   "<< track.r_invR() << std::endl;
	out << " * Charge: "<< track.Charge <<" Mass: "<<track.m_mass<< std::endl;
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
  {    vk_fitterControl = std::unique_ptr<VKalVrtControl>(new VKalVrtControl(FitControl));  }

  VKVertex::VKVertex(): 
     useApriorVertex(0), passNearVertex(false), passWithTrkCov(false), 
     TrackList(0), tmpArr(0), ConstraintList(0),nextCascadeVrt(0),includedVrt(0)
  { 
     for(int i=0; i<3; i++) apriorV[i]=refV[i]=iniV[i]=fitV[i]=cnstV[i]=fitMom[i]=0.;
     for(int i=0; i<6; i++) apriorVWGT[i]=0.;
     for(int i=0; i<21; i++) fitCovXYZMom[i]=savedVrtMomCov[i]=0.;
     fitCovXYZMom[0]=savedVrtMomCov[0]=100.; fitCovXYZMom[2] =savedVrtMomCov[2] =100.; fitCovXYZMom[5] =savedVrtMomCov[5] =100.;
     fitCovXYZMom[9]=savedVrtMomCov[9]=100.; fitCovXYZMom[14]=savedVrtMomCov[14]=100.; fitCovXYZMom[20]=savedVrtMomCov[20]=100.;
     Chi2=0.;
     truncatedStep=false;
     existFullCov=0;
     std::fill_n(ader,(3*vkalNTrkM+3)*(3*vkalNTrkM+3),0);
  }

  VKVertex::~VKVertex()
  {  
       for( int i=0; i<(int)TrackList.size(); i++) delete TrackList[i];
       for( int i=0; i<(int)tmpArr.size(); i++) delete tmpArr[i];
       for( int i=0; i<(int)ConstraintList.size(); i++) delete ConstraintList[i];
       for( int i=0; i<(int)includedVrt.size(); i++) includedVrt[i]=0;  // these vertice are not owned, then must not be deleted.
  }
  void VKVertex::setRefV(double v[3]){ refV[0]=v[0]; refV[1]=v[1]; refV[2]=v[2];}
  void VKVertex::setRefIterV(double v[]){ refIterV[0]=v[0]; refIterV[1]=v[1]; refIterV[2]=v[2];}
  void VKVertex::setIniV(double v[3]){ iniV[0]=v[0]; iniV[1]=v[1]; iniV[2]=v[2];}
  void VKVertex::setFitV(double v[3]){ fitV[0]=v[0]; fitV[1]=v[1]; fitV[2]=v[2];}
  void VKVertex::setCnstV(double v[3]){ cnstV[0]=v[0]; cnstV[1]=v[1]; cnstV[2]=v[2];}


  VKVertex::VKVertex(const VKVertex & src):        //copy constructor
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
    truncatedStep = src.truncatedStep;
    existFullCov = src.existFullCov;
    std::copy(src.ader,src.ader+(3*vkalNTrkM+3)*(3*vkalNTrkM+3),ader);
    //----- Creation of track copies
    TrackList.clear();
    for( int i=0; i<(int)src.TrackList.size(); i++) TrackList.push_back( new VKTrack(*(src.TrackList[i])) );
    vk_fitterControl = std::unique_ptr<VKalVrtControl>(new VKalVrtControl(*src.vk_fitterControl));
   }

  VKVertex& VKVertex::operator= (const VKVertex & src)        //Assignment operator
  {
    if (this!=&src){
      vk_fitterControl.reset();
      vk_fitterControl=std::unique_ptr<VKalVrtControl>(new VKalVrtControl(*(src.vk_fitterControl)));
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
      truncatedStep = src.truncatedStep;
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


  void VKalVrtControl::setIterationNum(int Iter)
  {
    if (Iter<3)   Iter=3;
    if (Iter>100) Iter=100;
    vk_forcft.IterationNumber    = Iter;
  }

  void VKalVrtControl::setIterationPrec(double Prec)
  {
    if (Prec<1.e-5)   Prec=1.e-5;
    if (Prec>1.e-1)   Prec=1.e-1;
    vk_forcft.IterationPrecision = Prec;
  }

  void VKalVrtControl::setRobustScale(double Scale)
  {
    if (Scale<0.01)   Scale=0.01;
    if (Scale>100.)   Scale=100.;
    vk_forcft.RobustScale = Scale;
  }

  void VKalVrtControl::setRobustness(int Rob)
  {
    if (Rob<0)   Rob=0;
    if (Rob>7)   Rob=7;
    vk_forcft.irob = Rob;
  }
  void VKalVrtControl::setMassCnstData(int Ntrk, double Mass){     // Define global mass constraint. It must be first
    double sumM(0.);
    for(int it=0; it<Ntrk; it++) sumM +=   vk_forcft.wm[it];                        //sum of particle masses
    if(sumM<Mass) {
      vk_forcft.wmfit[0]=Mass;
      for(int it=0; it<Ntrk; it++) vk_forcft.indtrkmc[0][it]=1;                     //Set participating particles
      vk_forcft.nmcnst=1;
    }
    vk_forcft.useMassCnst = 1;
  }
  void VKalVrtControl::setMassCnstData(int Ntrk, std::vector<int> & Index, double Mass){
    double sumM(0.);
    for(int it=0; it<Ntrk; it++) sumM +=   vk_forcft.wm[Index[it]];                 //sum of particle masses
    if(sumM<Mass) {
      vk_forcft.wmfit[0]=Mass;
      for(int it=0; it<Ntrk; it++) vk_forcft.indtrkmc[vk_forcft.nmcnst][Index[it]]=1;  //Set participating particles
      vk_forcft.nmcnst++;
    }
    vk_forcft.useMassCnst = 1;
  }

  void VKalVrtControl::setUsePhiCnst()   { vk_forcft.usePhiCnst = 1;}
  void VKalVrtControl::setUsePlaneCnst(double a, double b, double c, double d)   { 
    if(a+b+c+d == 0.){  vk_forcft.usePlaneCnst = 0;
    }else{              vk_forcft.usePlaneCnst = 1; }
    vk_forcft.Ap = a; vk_forcft.Bp = b; vk_forcft.Cp = c; vk_forcft.Dp = d;
  }
  void VKalVrtControl::setUseThetaCnst() { vk_forcft.useThetaCnst = 1;}
  void VKalVrtControl::setUseAprioriVrt(){ vk_forcft.useAprioriVrt = 1;}
  void VKalVrtControl::setUsePointingCnst(int iType = 1 ) { vk_forcft.usePointingCnst = iType<2 ? 1 : 2 ;}
  void VKalVrtControl::setUsePassNear(int iType = 1 ) { vk_forcft.usePassNear = iType<2 ? 1 : 2 ;}
  CascadeEvent * VKalVrtControl::getCascadeEvent() const { return m_cascadeEvent;}
  void VKalVrtControl::renewCascadeEvent(CascadeEvent * newevt) { 
     if(m_cascadeEvent)delete m_cascadeEvent;
     m_cascadeEvent=newevt;
  }
  double * VKalVrtControl::getFullCovariance() const { return m_fullCovariance; }
  void VKalVrtControl::renewFullCovariance(double * newarray) { 
     if(m_fullCovariance) delete[] m_fullCovariance;
     m_fullCovariance=newarray;
  }

} /* End of namespace */
