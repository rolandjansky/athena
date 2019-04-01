/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//    Management of constraints 
//
//----------------------------------------------------
#include "TrkVKalVrtCore/ForCFT.h"
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <iostream>

namespace Trk {


extern void  calcMassConstraint( VKMassConstraint * );
extern void  calcPhiConstraint( VKPhiConstraint * );
extern void  calcThetaConstraint( VKThetaConstraint * );
extern void  calcPointConstraint( VKPointConstraint * );
extern void  calcPlaneConstraint( VKPlaneConstraint * );


void applyConstraints(VKVertex * vk)
{
  int NCEntries=vk->ConstraintList.size();
  int NTRK=vk->TrackList.size();
  VKMassConstraint  * mass_cnst;
  VKPhiConstraint   * p_cnst;
  VKThetaConstraint * t_cnst;
  VKPointConstraint * pnt_cnst;
  VKPlaneConstraint * pln_cnst;
  for(int ic=0; ic<NCEntries; ic++){
    mass_cnst = dynamic_cast<VKMassConstraint *>(vk->ConstraintList[ic]);
    if(mass_cnst){  calcMassConstraint( mass_cnst ); 
                 //std::cout<<(*mass_cnst)<<'\n';
    }
    p_cnst = dynamic_cast<VKPhiConstraint *>(vk->ConstraintList[ic]);
    if(p_cnst){  calcPhiConstraint( p_cnst ); 
                 //std::cout<<(*p_cnst)<<'\n';
    }
    t_cnst = dynamic_cast<VKThetaConstraint *>(vk->ConstraintList[ic]);
    if(t_cnst){  calcThetaConstraint( t_cnst ); 
                 //std::cout<<(*t_cnst)<<'\n';
    }
    pln_cnst = dynamic_cast<VKPlaneConstraint *>(vk->ConstraintList[ic]);
    if(pln_cnst){  calcPlaneConstraint( pln_cnst ); 
                 //std::cout<<(*pln_cnst)<<'\n';
    }
    pnt_cnst = dynamic_cast<VKPointConstraint *>(vk->ConstraintList[ic]);
    if(pnt_cnst){  calcPointConstraint( pnt_cnst ); 
                 //std::cout<<(*pnt_cnst)<<'\n';
/*//-------------------------------------
       int ici=1;  double aa0=vk->ConstraintList[ic]->aa[ici];
       double der1,der2,der3;
       for(int it=0; it<NTRK; it++){
          vk->TrackList[it]->cnstP[0] += 0.0001; calcPointConstraint( pnt_cnst ); 
          vk->TrackList[it]->cnstP[0] -= 0.0001;
          der1=(vk->ConstraintList[ic]->aa[ici]-aa0)/0.0001;
          vk->TrackList[it]->cnstP[1] += 0.0001; calcPointConstraint( pnt_cnst ); 
          vk->TrackList[it]->cnstP[1] -= 0.0001;
          der2=(vk->ConstraintList[ic]->aa[ici]-aa0)/0.0001;
          vk->TrackList[it]->cnstP[2] *= 1.001;  calcPointConstraint( pnt_cnst ); 
          vk->TrackList[it]->cnstP[2] /= 1.001;
          der3=(vk->ConstraintList[ic]->aa[ici]-aa0)/(vk->TrackList[it]->cnstP[2]*0.001);
          std::cout<<"Numerical deriv it="<<it<<"  "<<der1<<", "<<der2<<", "<<der3<<'\n';
       }
       vk->cnstV[0] += 0.001; calcPointConstraint( pnt_cnst ); 
       vk->cnstV[0] -= 0.001; der1=(vk->ConstraintList[ic]->aa[ici]-aa0)/0.001;
       vk->cnstV[1] += 0.001; calcPointConstraint( pnt_cnst ); 
       vk->cnstV[1] -= 0.001; der2=(vk->ConstraintList[ic]->aa[ici]-aa0)/0.001;
       vk->cnstV[2] += 0.001; calcPointConstraint( pnt_cnst ); 
       vk->cnstV[2] -= 0.001; der3=(vk->ConstraintList[ic]->aa[ici]-aa0)/0.001;
       std::cout<<"Numerical deriv vrt="<<der1<<", "<<der2<<", "<<der3<<'\n';
*///------------------------------------------
     }
  }      
//
// Effect of symmetrization
//
    for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
       for(int ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
         vk->ConstraintList[ii]->h0t[ic].X /= 2. ;
         vk->ConstraintList[ii]->h0t[ic].Y /= 2. ;
         vk->ConstraintList[ii]->h0t[ic].Z /= 2. ;
         for(int it=0; it<NTRK; it++){
	    vk->ConstraintList[ii]->f0t.at(it)[ic].X /= 2. ;
	    vk->ConstraintList[ii]->f0t[it][ic].Y    /= 2. ;
	    vk->ConstraintList[ii]->f0t[it][ic].Z    /= 2. ;
         }
       }
    }
    return;
}




  VKConstraintBase::VKConstraintBase(const int NC,int NTRK): NCDim(NC), NTrk(NTRK), aa(NC), f0t(NTRK), h0t(NC)
  {    
    std::vector< Vect3DF > tmp(NC);
    for(int i=0; i<NCDim; i++){
        aa[i]=0.; 
	h0t[i].X=0; h0t[i].Y=0; h0t[i].Z=0;
	tmp[i].X=0; tmp[i].Y=0; tmp[i].Z=0;
    }
    for(int i=0; i<NTrk; i++) f0t[i]=tmp;
  }
  VKConstraintBase::~VKConstraintBase(){}
  std::ostream &  operator << ( std::ostream& out, const VKConstraintBase & cnst  )
  {
        int NTRK=cnst.f0t.size();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Base constraint derivatives for NTRK="<<NTRK<<" CNST dim="<<cnst.NCDim<<std::endl;
        out << " Momentum derivatives "<< std::endl;
        for(int ic=0; ic<cnst.NCDim; ic++){
          out << "   d(...)/dTheta  d(...)/dPhi   d(...)/dInvR   NC="<<ic<<std::endl;
          for(int i=0; i<NTRK; i++){
	    out <<cnst.f0t[i][ic].X<<", "<<cnst.f0t[i][ic].Y<<", "<<cnst.f0t[i][ic].Z<<std::endl;
	  }
          out << "   d(...)/dXv  d(...)/dYy   d(...)/Zv"<< std::endl;
	  out <<cnst.h0t[ic].X<<", "<<cnst.h0t[ic].Y<<", "<<cnst.h0t[ic].Z<<std::endl;
          out << " aa="<<cnst.aa[ic]<<std::endl;
        }
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   MASS constraint
//
  VKMassConstraint::VKMassConstraint(int NTRK, double mass, VKVertex *vk) :
    VKConstraintBase(1,NTRK),
    m_usedParticles(NTRK,0), m_targetMass(mass)
  {
    for(int i=0; i<NTrk; i++) m_usedParticles[i]=i;
    m_originVertex = vk;
  }
  VKMassConstraint::VKMassConstraint(int NTRK, double mass, const std::vector<int> &listTrk, VKVertex *vk) :
    VKConstraintBase(1,NTRK),
    m_usedParticles(0), m_targetMass(mass)
  {
    for(int i=0; i<(int)listTrk.size(); i++) m_usedParticles.push_back(listTrk[i]);
    m_originVertex = vk;
  }
  VKMassConstraint::~VKMassConstraint(){}
  std::ostream &  operator << ( std::ostream& out, const VKMassConstraint & cnst  )
  {
        VKVertex * vk = cnst.getOriginVertex();
        int NP=cnst.m_usedParticles.size();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Mass constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
	out << " * target mass: "<< cnst.getTargetMass()                  << std::endl;
	out << " * particle indexes: ";
        for(int i=0; i<NP; i++){out <<cnst.m_usedParticles[i]<<", ";}
	out << std::endl;
	out << " * particle masses: ";
        for(int i=0; i<NP; i++){out<<vk->TrackList[cnst.m_usedParticles[i]]->getMass()<<", ";}
	out << std::endl;
        out<< (VKConstraintBase)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   Angular constraints
//
  VKPhiConstraint::VKPhiConstraint(int NTRK, VKVertex *vk):
    VKConstraintBase(1,NTRK)
  {m_originVertex = vk;}
  VKPhiConstraint::~VKPhiConstraint(){}
  std::ostream &  operator << ( std::ostream& out, const VKPhiConstraint & cnst  )
  {     VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Phi constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        out<< (VKConstraintBase)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              

  VKThetaConstraint::VKThetaConstraint(int NTRK, VKVertex *vk):
    VKConstraintBase(1,NTRK)
  {m_originVertex = vk;}
  VKThetaConstraint::~VKThetaConstraint(){}
  std::ostream &  operator << ( std::ostream& out, const VKThetaConstraint & cnst  )
  {     VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Theta constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        out<< (VKConstraintBase)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   Pointing constraint
//
  VKPointConstraint::VKPointConstraint(int NTRK, double vrt[3], VKVertex *vk, bool onlyZ=false):
    VKConstraintBase(2,NTRK)
  { m_originVertex = vk;m_targetVertex[0]=vrt[0]; m_targetVertex[1]=vrt[1]; m_targetVertex[2]=vrt[2];
    m_onlyZ=onlyZ;  // For Z only constraint
  }
  VKPointConstraint::~VKPointConstraint(){}
  std::ostream &  operator << ( std::ostream& out, const VKPointConstraint & cnst  )
  {     VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        if(!cnst.onlyZ()){
          out << " Point constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        }else{
          out << " Z point constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        }
        out << " target vertex="<<cnst.getTargetVertex()[0]<<", "
                                <<cnst.getTargetVertex()[1]<<", "
                                <<cnst.getTargetVertex()[2]<<std::endl;
        out<< (VKConstraintBase)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   Vertex in plane constraint
//
  VKPlaneConstraint::VKPlaneConstraint(int NTRK, double a, double b, double c, double d, VKVertex *vk):
    VKConstraintBase(1,NTRK)
  {m_A=a; m_B=b; m_C=c; m_D=d; m_originVertex = vk;}
  VKPlaneConstraint::~VKPlaneConstraint(){}
  std::ostream &  operator << ( std::ostream& out, const VKPlaneConstraint & cnst  )
  {     VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Vertex in plane constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        out << " Plane(A,B,C,D):"<<cnst.getA()<<", "<<cnst.getB()<<", "<<cnst.getC()<<", "<<cnst.getD()<<std::endl;
        out<< (VKConstraintBase)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              

} /* End of namespace */


