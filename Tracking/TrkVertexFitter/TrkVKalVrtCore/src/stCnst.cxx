/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  for(int ic=0; ic<NCEntries; ic++){
    vk->ConstraintList[ic]->applyConstraint();
  }      
//
// Effect of symmetrization
//
    for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
       for(int ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
         vk->ConstraintList[ii]->h0t[ic].X *= 0.5 ;
         vk->ConstraintList[ii]->h0t[ic].Y *= 0.5 ;
         vk->ConstraintList[ii]->h0t[ic].Z *= 0.5 ;
         for(int it=0; it<NTRK; it++){
	    vk->ConstraintList[ii]->f0t.at(it)[ic].X *= 0.5 ;
	    vk->ConstraintList[ii]->f0t[it][ic].Y    *= 0.5 ;
	    vk->ConstraintList[ii]->f0t[it][ic].Z    *= 0.5 ;
         }
       }
    }
    }


 VKConstraintBase::VKConstraintBase(const int NC,int NTRK, VKContraintType t, VKVertex* vrt): NCDim(NC), NTrk(NTRK),
   aa(NC, 0.), f0t(NTRK, std::vector< Vect3DF >(NC, Vect3DF())), h0t(NC, Vect3DF()), m_originVertex(vrt), m_type(t)
  {
  }


  VKConstraintBase::~VKConstraintBase()= default;
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
    VKConstraintBase(1,NTRK, VKContraintType::Mass, vk),
    m_usedParticles(NTRK,0), m_targetMass(mass)
  {
    for(int i=0; i<NTrk; i++) m_usedParticles[i]=i;
  }
  VKMassConstraint::VKMassConstraint(int NTRK, double mass, const std::vector<int> &listTrk, VKVertex *vk) :
    VKConstraintBase(1,NTRK, VKContraintType::Mass, vk),
    m_usedParticles(0), m_targetMass(mass)
  {
    for(int i=0; i<(int)listTrk.size(); i++) m_usedParticles.push_back(listTrk[i]);
    m_originVertex = vk;
  }
  VKMassConstraint::~VKMassConstraint()= default;
  std::ostream &  operator << ( std::ostream& out, const VKMassConstraint & cnst  )
  {
        const VKVertex * vk = cnst.getOriginVertex();
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
        out<< (VKConstraintBase&)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   Angular constraints
//
  VKPhiConstraint::VKPhiConstraint(int NTRK, VKVertex *vk):
    VKConstraintBase(1,NTRK, VKContraintType::Phi, vk)
  {m_originVertex = vk;}
  VKPhiConstraint::~VKPhiConstraint()= default;
  std::ostream &  operator << ( std::ostream& out, const VKPhiConstraint & cnst  )
  {     const VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Phi constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        out<< (VKConstraintBase&)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              

  VKThetaConstraint::VKThetaConstraint(int NTRK, VKVertex *vk):
    VKConstraintBase(1,NTRK, VKContraintType::Theta, vk)
  {m_originVertex = vk;}
  VKThetaConstraint::~VKThetaConstraint()= default;
  std::ostream &  operator << ( std::ostream& out, const VKThetaConstraint & cnst  )
  {     const VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Theta constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        out<< (VKConstraintBase&)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   Pointing constraint
//
  VKPointConstraint::VKPointConstraint(int NTRK, double vrt[3], VKVertex *vk, bool onlyZ=false):
    VKConstraintBase(2,NTRK, VKContraintType::Point, vk)
  { m_originVertex = vk;m_targetVertex[0]=vrt[0]; m_targetVertex[1]=vrt[1]; m_targetVertex[2]=vrt[2];
    m_onlyZ=onlyZ;  // For Z only constraint
  }
  VKPointConstraint::~VKPointConstraint()= default;
  std::ostream &  operator << ( std::ostream& out, const VKPointConstraint & cnst  )
  {     const VKVertex * vk = cnst.getOriginVertex();
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
        out<< (VKConstraintBase&)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              
//
//                   Vertex in plane constraint
//
  VKPlaneConstraint::VKPlaneConstraint(int NTRK, double a, double b, double c, double d, VKVertex *vk):
    VKConstraintBase(1,NTRK, VKContraintType::Plane, vk)
  {m_A=a; m_B=b; m_C=c; m_D=d; }
  VKPlaneConstraint::~VKPlaneConstraint()= default;
  std::ostream &  operator << ( std::ostream& out, const VKPlaneConstraint & cnst  )
  {     const VKVertex * vk = cnst.getOriginVertex();
	//out.setf( std::ios::scientific); out.precision(7); out << std::endl;
	out.precision(7); out << std::defaultfloat;
        out << " Vertex in plane constraint  (total NTRK="<<vk->TrackList.size()<<")"<< std::endl;
        out << " Plane(A,B,C,D):"<<cnst.getA()<<", "<<cnst.getB()<<", "<<cnst.getC()<<", "<<cnst.getD()<<std::endl;
        out<< (VKConstraintBase&)cnst <<'\n';
	out.precision(6); //restore default
	return out;                                 
  }                                              

  VKConstraintBase* VKMassConstraint::clone() const{
     return new VKMassConstraint(*this);
  }
  VKConstraintBase* VKPhiConstraint::clone() const{
     return new VKPhiConstraint(*this);
  }
  VKConstraintBase* VKThetaConstraint::clone() const{
     return new VKThetaConstraint(*this);
  }
  VKConstraintBase* VKPointConstraint::clone() const{
     return new VKPointConstraint(*this);
  }
  VKConstraintBase* VKPlaneConstraint::clone() const{
     return new VKPlaneConstraint(*this);
  }

  void VKMassConstraint::applyConstraint(){  calcMassConstraint(this); }
  void VKPhiConstraint::applyConstraint(){  calcPhiConstraint(this); }
  void VKThetaConstraint::applyConstraint(){  calcThetaConstraint(this); }
  void VKPointConstraint::applyConstraint(){  calcPointConstraint(this); }
  void VKPlaneConstraint::applyConstraint(){  calcPlaneConstraint(this); }
} /* End of namespace */


