/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigVKalFitter/TrigVKalFitter.h"
//-------------------------------------------------
#include "GaudiKernel/MsgStream.h"
#include<iostream>


namespace Trk {
   extern void prcfit(long int *,double *,double* ,
                      double* , double* , double* );           
   extern void setmasscnst_(long int* , long int* , double* );
   extern void vksetIterationNum(long int Iter);
   extern void vksetIterationPrec(double Prec);
   extern void vksetRobustness(long int Rob);
}

//
// Option setting for VKalVrt core
//
void TrigVKalFitter::VKalVrtSetOptions(long int ntrk)
{ 

   m_FitStatus = 0;     // Drop all previous fit results

   double MFit    = (double) (m_MassForConstraint);
   long int Rob  = (long int) m_Robustness;
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
      for(i=0; i<3; i++){m_VrtCst[i]    =(double)(m_VertexForConstraint[i]);}
   }else{
      for(i=0; i<3; i++){m_VrtCst[i]    = 0;}
   }
   //---
   if(m_CovVrtForConstraint.size() >= 6){
      for(i=0; i<6; i++){m_CovVrtCst[i] =(double)(m_CovVrtForConstraint[i]);}
   }else{
      for(i=0; i<6; i++){m_CovVrtCst[i] = 0.;}
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
   //---
   Trk::prcfit(&ntrk,m_wm,&MFit,&m_BMAG,m_VrtCst,m_CovVrtCst);
   //---
   //   Additional change of settings 
   //---
   long int Index[NTRMAXTRIG];
   if(m_PartMassCnst.size() > 0) {
      for(int ic=0; ic<(int)m_PartMassCnst.size(); ic++){ 
         long int NTrk=m_PartMassCnstTrk[ic].size();
         for(int it=0; it<NTrk; it++) Index[it]=m_PartMassCnstTrk[ic][it];
         double CnstMass= (double) m_PartMassCnst[ic];
         Trk::setmasscnst_(&NTrk,Index,&CnstMass);
      }
   }
   //---
   if(m_IterationNumber > 0) {
      Trk::vksetIterationNum(int(m_IterationNumber));
   }
   //---
   if(m_IterationPrecision > 0.) {
      Trk::vksetIterationPrec(double(m_IterationPrecision));
   }
   //---
   if(m_Robustness > 0) Trk::vksetRobustness( Rob );

}



void TrigVKalFitter::setApproximateVertex(double X,double Y,double Z)
{  m_ApproximateVertex.clear();
   m_ApproximateVertex.push_back(X);
   m_ApproximateVertex.push_back(Y);
   m_ApproximateVertex.push_back(Z);
}

void TrigVKalFitter::setRobustness(int IROB)
{ m_Robustness = IROB;}

void TrigVKalFitter::setCnstType(int TYPE)
{ m_iflag = TYPE;
   if(m_iflag<0)m_iflag=0;
   if(m_iflag>8)m_iflag=0;
}

void TrigVKalFitter::setMassForConstraint(double MASS)
{ m_MassForConstraint = MASS;}

void TrigVKalFitter::setMassForConstraint(double MASS, std::vector<int> TrkIndex)
{  
   m_PartMassCnst.push_back(MASS);
   m_PartMassCnstTrk.push_back(TrkIndex);
}

void TrigVKalFitter::setMomCovCalc(int TYPE)
{ m_ifcovv0 = abs(TYPE);}

void TrigVKalFitter::setIterations(int Num, double Prec)
{
   m_IterationNumber    = Num;
   m_IterationPrecision = Prec;
}

void TrigVKalFitter::setVertexForConstraint(double X,double Y,double Z)
{
   m_VertexForConstraint.clear();
   m_VertexForConstraint.push_back(X);
   m_VertexForConstraint.push_back(Y);
   m_VertexForConstraint.push_back(Z);
}

void TrigVKalFitter::setCovVrtForConstraint(double XX,double XY,double YY,
                                            double XZ,double YZ,double ZZ)
{
   m_CovVrtForConstraint.clear();
   m_CovVrtForConstraint.push_back(XX);			  
   m_CovVrtForConstraint.push_back(XY);			  
   m_CovVrtForConstraint.push_back(YY);			  
   m_CovVrtForConstraint.push_back(XZ);			  
   m_CovVrtForConstraint.push_back(YZ);			  
   m_CovVrtForConstraint.push_back(ZZ);
}			  

void TrigVKalFitter::setMassInputParticles( std::vector<double>& mass)
{
   m_MassInputParticles.clear();
   for(int i=0; i<(int)mass.size(); i++) m_MassInputParticles.push_back(fabs(mass[i]));
}

void TrigVKalFitter::setZeroCharge(int Track)
{
   m_TrackCharge.push_back(Track);
}


void TrigVKalFitter::setDefault()
{
   //   std::cout<<" In"<<'\n';
   setApproximateVertex(0.,0.,0.);
   setRobustness(0);
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
}
