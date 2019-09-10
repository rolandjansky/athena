/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
//-------------------------------------------------
#include<iostream>

//extern "C" {
//  void prcfit_(const long int& ntrk,const double& wm,const double& wmfit,
//               const double& BMAG,const double& vrt,const double& vrte,
//	       const long int& irob);           
//  void setmasscnst_(const long int& NCnstTrk,const long int& IndexTrk,const double& WMCNST);
//}
namespace Trk {
extern void prcfit(long int *,double *,double* ,
                    double* , double* , double*);           
extern void setmasscnst_(long int* , long int* , double* );
extern void vksetIterationNum(long int Iter);
extern void vksetIterationPrec(double Prec);
extern void vksetRobustness(long int Rob);
extern void vksetRobustScale(double Scale);
extern void vksetUseMassCnst();
extern void vksetUsePhiCnst();
extern void vksetUseThetaCnst();
extern void vksetUseAprioriVrt();
extern void vksetUsePointingCnst(int iType = 1 );
extern void vksetUsePassNear(int iType = 1 );
}

namespace Trk{
//
// Option setting for VKalVrt core
//
  void TrkVKalVrtFitter::VKalVrtSetOptions(long int ntrk)
  { 

    m_FitStatus = 0;     // Drop all previous fit results

    double MFit    = (double) (m_MassForConstraint);
    long int Rob  = (long int) m_Robustness;
    for(int i=0; i<6; i++){m_CovVrtCst[i] = 0.;}
    for(int i=0; i<3; i++){m_VrtCst[i]    = 0;}
//---
    int i,j;
    double SumMass = 0;
    for(i=0; i<ntrk; i++){
      if(i<(int)m_MassInputParticles.size()){
         m_wm[i]  = (double)(m_MassInputParticles[i]);
         SumMass +=   m_wm[i];      
      }
      else {m_wm[i]=(double)(139.5702);}
    }
    if ( MFit>0 && MFit < SumMass ) MFit=SumMass+1.e-4;
//---
    if(m_VertexForConstraint.size() >= 3){
      m_VrtCst[0]    =m_VertexForConstraint[0] - m_refFrameX;
      m_VrtCst[1]    =m_VertexForConstraint[1] - m_refFrameY;
      m_VrtCst[2]    =m_VertexForConstraint[2] - m_refFrameZ;
    }
//---
    if(m_CovVrtForConstraint.size() >= 6){
      for(i=0; i<6; i++){m_CovVrtCst[i] =(double)(m_CovVrtForConstraint[i]);}
    }
//---
    if(m_TrackCharge.size() > 0){
      for(i=0; i<(int)m_TrackCharge.size(); i++){ 
         if(m_TrackCharge[i] < 1   )    continue;
         if(m_TrackCharge[i] > ntrk)    continue;
	 j=m_TrackCharge[i]-1;
//   std::cout<<" Set charge "<<i<<", "<<m_apar[j][4]<<'\n';
	 m_ich[j]=0;
	 if(m_apar[j][4]<0){ m_apar[j][4] =-m_apar[j][4];      // Charge=0 is always equal to Charge=+1
	                     m_awgt[j][10]=-m_awgt[j][10];
	                     m_awgt[j][11]=-m_awgt[j][11];
	                     m_awgt[j][12]=-m_awgt[j][12];
	                     m_awgt[j][13]=-m_awgt[j][13]; }
      }
      m_TrackCharge.clear();
    }
//--- This call actually resets all constraints
    Trk::prcfit(&ntrk,m_wm,&MFit,&m_BMAG,m_VrtCst,m_CovVrtCst);
//---
//   Additional change of settings 
//---
    long int Index[NTrMaxVFit+1]={0};
    if(m_PartMassCnst.size() > 0) {
      for(int ic=0; ic<(int)m_PartMassCnst.size(); ic++){ 
        long int NTrk=m_PartMassCnstTrk[ic].size();
        for(int it=0; it<NTrk; it++) Index[it]=m_PartMassCnstTrk[ic][it];
        double CnstMass= (double) m_PartMassCnst[ic];
        Trk::setmasscnst_(&NTrk,Index,&CnstMass);
      }
    }
//---
    if(m_IterationNumber > 0) Trk::vksetIterationNum(int(m_IterationNumber));
//---
    if(m_IterationPrecision > 0.) Trk::vksetIterationPrec(double(m_IterationPrecision));
//---
    if(m_Robustness > 0) Trk::vksetRobustness( Rob );
//---
    Trk::vksetRobustScale( m_RobustScale );

  }

  void TrkVKalVrtFitter::initCnstList()
  {
//--- Mass constraint is restored here
    if( m_MassForConstraint>0. || m_PartMassCnst.size()>0 ) vksetUseMassCnst();
    if( m_useAprioriVertex ) vksetUseAprioriVrt();
    if( m_useThetaCnst )     vksetUseThetaCnst();
    if( m_usePhiCnst   )     vksetUsePhiCnst();
    if( m_usePointingCnst )  vksetUsePointingCnst(1);
    if( m_useZPointingCnst)  vksetUsePointingCnst(2);
    if( m_usePassNear)       vksetUsePassNear(1);
    if( m_usePassWithTrkErr) vksetUsePassNear(2);
  }


  void TrkVKalVrtFitter::setApproximateVertex(double X,double Y,double Z)
  {  m_ApproximateVertex.clear(); m_ApproximateVertex.reserve(3);
     m_ApproximateVertex.push_back(X);
     m_ApproximateVertex.push_back(Y);
     m_ApproximateVertex.push_back(Z);
  }
  
  void TrkVKalVrtFitter::setRobustness(int IROB)
  { if(IROB>0)msg(MSG::DEBUG)<< "Robustness is changed at execution stage "<<m_Robustness<<"=>"<<IROB<< endmsg;
    m_Robustness = IROB;
    if(m_Robustness<0)m_Robustness=0;
    if(m_Robustness>7)m_Robustness=0;
  }

  void TrkVKalVrtFitter::setRobustScale(double Scale)
  { if(Scale!=m_RobustScale)msg(MSG::DEBUG)<< "Robust Scale is changed at execution stage "<<m_RobustScale<<"=>"<<Scale<< endmsg;
    m_RobustScale = Scale;
    if(m_RobustScale<0.01) m_RobustScale=1.;
    if(m_RobustScale>100.) m_RobustScale=1.;
  }
 
  void TrkVKalVrtFitter::setCnstType(int TYPE)
  { if(TYPE>0)msg(MSG::DEBUG)<< "ConstraintType is changed at execution stage "<<m_iflag<<"=>"<<TYPE<< endmsg;
    m_iflag = TYPE;
    if(m_iflag<0)m_iflag=0;
    if(m_iflag>14)m_iflag=0;
    if( m_iflag == 2)  m_usePointingCnst   = true;    
    if( m_iflag == 3)  m_useZPointingCnst  = true;    
    if( m_iflag == 4)  m_usePointingCnst   = true;    
    if( m_iflag == 5)  m_useZPointingCnst  = true;    
    if( m_iflag == 6)  m_useAprioriVertex  = true;    
    if( m_iflag == 7)  m_usePassWithTrkErr = true;    
    if( m_iflag == 8)  m_usePassWithTrkErr = true;    
    if( m_iflag == 9)  m_usePassNear       = true;    
    if( m_iflag == 10) m_usePassNear       = true;    
    if( m_iflag == 11) m_usePhiCnst = true;    
    if( m_iflag == 12) { m_usePhiCnst = true; m_useThetaCnst = true;}
    if( m_iflag == 13) { m_usePhiCnst = true; m_usePassNear  = true;}
    if( m_iflag == 14) { m_usePhiCnst = true; m_useThetaCnst = true; m_usePassNear  = true;}
  }

  void TrkVKalVrtFitter::setMassForConstraint(double MASS)
  { m_MassForConstraint = MASS;}

  void TrkVKalVrtFitter::setMassForConstraint(double MASS, std::vector<int> TrkIndex)
  { m_PartMassCnst.push_back(MASS);
    m_PartMassCnstTrk.push_back(TrkIndex);
  }

  void TrkVKalVrtFitter::setMomCovCalc(int TYPE)
  { m_ifcovv0 = abs(TYPE);}

  void TrkVKalVrtFitter::setCascadeCnstPrec(double Prec)
  { if(Prec!=m_cascadeCnstPrecision) msg(MSG::DEBUG)<< "Cascade precision is changed at execution stage "<<m_cascadeCnstPrecision<<"=>"<<Prec<< endmsg;
    if(Prec<1.e-7)Prec=1.e-7;
    if(Prec>1.   )Prec=1.;
    m_cascadeCnstPrecision=Prec; 
  }

  void TrkVKalVrtFitter::setIterations(int Num, double Prec)
  { if(Num!=m_IterationNumber) msg(MSG::DEBUG)<< "Iteration limit is changed at execution stage "<<m_IterationNumber<<"=>"<<Num<< endmsg;
     if(Prec!=m_IterationPrecision) msg(MSG::DEBUG)<< "Iteration precision is changed at execution stage "<<m_IterationPrecision<<"=>"<<Prec<< endmsg;
    m_IterationNumber    = Num;
    m_IterationPrecision = Prec;
  }

  void TrkVKalVrtFitter::setVertexForConstraint(const xAOD::Vertex & Vrt)
  {  m_VertexForConstraint.clear();
     m_VertexForConstraint.push_back(Vrt.position().x());
     m_VertexForConstraint.push_back(Vrt.position().y());
     m_VertexForConstraint.push_back(Vrt.position().z());

     m_CovVrtForConstraint.clear();
     m_CovVrtForConstraint.push_back(Vrt.covariancePosition()(Trk::x,Trk::x));			  
     m_CovVrtForConstraint.push_back(Vrt.covariancePosition()(Trk::x,Trk::y));			  
     m_CovVrtForConstraint.push_back(Vrt.covariancePosition()(Trk::y,Trk::y));			  
     m_CovVrtForConstraint.push_back(Vrt.covariancePosition()(Trk::x,Trk::z));			  
     m_CovVrtForConstraint.push_back(Vrt.covariancePosition()(Trk::y,Trk::z));			  
     m_CovVrtForConstraint.push_back(Vrt.covariancePosition()(Trk::z,Trk::z));			  
  }

  void TrkVKalVrtFitter::setVertexForConstraint(double X,double Y,double Z)
  {  m_VertexForConstraint.clear();
     m_VertexForConstraint.push_back(X);
     m_VertexForConstraint.push_back(Y);
     m_VertexForConstraint.push_back(Z);
  }

  void TrkVKalVrtFitter::setCovVrtForConstraint(double XX,double XY,double YY,
                                             double XZ,double YZ,double ZZ)
  {  m_CovVrtForConstraint.clear();
     m_CovVrtForConstraint.push_back(XX);			  
     m_CovVrtForConstraint.push_back(XY);			  
     m_CovVrtForConstraint.push_back(YY);			  
     m_CovVrtForConstraint.push_back(XZ);			  
     m_CovVrtForConstraint.push_back(YZ);			  
     m_CovVrtForConstraint.push_back(ZZ);
  }			  

  void TrkVKalVrtFitter::setMassInputParticles( const std::vector<double>& mass)
  { m_MassInputParticles.clear();
    for(int i=0; i<(int)mass.size(); i++) m_MassInputParticles.push_back(fabs(mass[i]));
  }

  void TrkVKalVrtFitter::setZeroCharge(int Track)
  { m_TrackCharge.push_back(Track);
  }


  void TrkVKalVrtFitter::setDefault()
  {
//   std::cout<<" In"<<'\n';
   setApproximateVertex(0.,0.,0.);
   setRobustness(0);
   setCascadeCnstPrec(1.e-4);
   setMassForConstraint(0.);
   setVertexForConstraint(0.,0.,0.);
   setCovVrtForConstraint(1.e6,0.,1.e6,0.,0.,1.e6);
   m_MassInputParticles.clear();
   setCnstType(0);
   setMomCovCalc(0);
   m_TrackCharge.clear();
   m_PartMassCnst.clear();
   m_PartMassCnstTrk.clear();
   m_IterationNumber = 0;
   m_IterationPrecision = 0.;
   m_useAprioriVertex = false;
   m_useThetaCnst = false;
   m_usePhiCnst = false;
   m_usePointingCnst = false;
   m_useZPointingCnst = false;
   m_usePassNear = false;
   m_usePassWithTrkErr = false;
  }

  int  TrkVKalVrtFitter::getCnstDOF(){
     if(m_iflag==1)return   1;
     if(m_iflag==2)return   2;
     if(m_iflag==3)return   1;
     if(m_iflag==4)return   3;
     if(m_iflag==5)return   2;
     if(m_iflag==6)return   3;
     if(m_iflag==7 || m_iflag==9 ) return   2;
     if(m_iflag==8 || m_iflag==10) return   3;
     if(m_iflag==11)return  1;
     if(m_iflag==12)return  2;
     if(m_iflag==13)return  3;
     if(m_iflag==14)return  4;
     return 0;
  }
} //end of namespace
