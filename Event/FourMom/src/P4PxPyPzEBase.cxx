/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4PxPyPzEBase.h"
#include <boost/io/ios_state.hpp>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cstdlib>

P4PxPyPzEBase::~P4PxPyPzEBase()
{}

double P4PxPyPzEBase::m2() const
{
  const double px=this->px();
  const double py=this->py();
  const double pz=this->pz();
  const double e =this->e();
  
  const double m2 = e*e - (px*px + py*py + pz*pz);
  return m2;
}

double P4PxPyPzEBase::m() const
{
  const double m2 = this->m2();  

  return m2 < 0.0 ? -std::sqrt(-m2) : std::sqrt (m2);
}

double P4PxPyPzEBase::eta() const 
{ 
  const double e=this->e();
  
  const double px=this->px();
  const double py=this->py();
  const double pz=this->pz();
  // FIXME: should we use a more underflow-friendly formula:
  //  sqrt(a**2 + b**2) 
  //   => y.sqrt(1+(x/y)**2) where y=max(|a|,|b|) and x=min(|a|,|b|)
  const double p =std::sqrt (px*px + py*py + pz*pz);
  if (p==0.0) return 0.0;
  if (p==+pz) return +std::numeric_limits<double>::infinity();
  if (p==-pz) return -std::numeric_limits<double>::infinity();
  //PO flip if negative e
  return (e>0 ? 1. : -1.)* 0.5*log((p+pz)/(p-pz));
}

double P4PxPyPzEBase::phi() const 
{ 
  const double e=this->e();
  const double px= (e>0 ? 1. : -1.)*this->px();
  const double py= (e>0 ? 1. : -1.)*this->py();
  return px == 0.0 && py == 0.0 ? 0.0 : atan2(py,px);
}


double P4PxPyPzEBase::p2() const 
{ 
  const double px=this->px();
  const double py=this->py();
  const double pz=this->pz();

  return px*px+py*py+pz*pz;
}

double P4PxPyPzEBase::p() const 
{ 
  //PO return the correc 
  const double e=this->e();
  
  return  (e>0 ? 1. : -1.)*std::sqrt(this->p2());
}


double P4PxPyPzEBase::cosPhi() const
{
 return this->px()/this->pt();
}

double P4PxPyPzEBase::sinPhi() const
{
 return this->py()/this->pt();
}

double P4PxPyPzEBase::tanTh() const
{ 
 return this->pt()/this->pz();
}

double P4PxPyPzEBase::cotTh() const
{ 
 return this->pz()/this->pt();
}


double P4PxPyPzEBase::cosTh() const
{ 
  
  return this->pz()/this->p();
  
}


double P4PxPyPzEBase::sinTh() const
{ 
  
 return this->pt()/this->p();

}


double P4PxPyPzEBase::pt() const
{
  const double e=this->e();
  const double px=this->px();
  const double py=this->py();
  //PO flip sign if negative energy
  return  (e>0 ? 1. : -1.)*std::sqrt(px*px+py*py);

   }


double P4PxPyPzEBase::et() const
   {
     //to be improved
  return this->e()*this->sinTh();
   }

double P4PxPyPzEBase::iPt() const
  { return 1./this->pt();
   }


double P4PxPyPzEBase::rapidity() const
{
  const double e =this->e();
  const double pz=this->pz();
  if (e==0.0) return 0.0;
  if (e==+pz) return +std::numeric_limits<double>::infinity();
  if (e==-pz) return -std::numeric_limits<double>::infinity();
  //PO invariant under flipping of 4-mom with neg E
  return 0.5*std::log((e+pz)/(e-pz));
}

 CLHEP::HepLorentzVector P4PxPyPzEBase::hlv() const
{ return CLHEP::HepLorentzVector(this->px(),this->py(),this->pz(),this->e());
 
}

std::ostream& P4PxPyPzEBase::dump( std::ostream& out ) const
{

  boost::io::ios_all_saver ias(out);
  out << "[px,py,pz,e] ="
	     << std::right << std::scientific << std::setprecision(8) 
	     << std::setw(16) << this->px()
	     << std::setw(16) << this->py()
	     << std::setw(16) << this->pz()
	     << std::setw(16) << this->e();

  ias.restore();
  
  return out;

}

const I4MomentumError* P4PxPyPzEBase::errors() const
{
  return 0;
}


void P4PxPyPzEBase::set4Mom(const I4Momentum &  )
{ 
  std::cout << "FATAL ERROR dummy P4PxPyPzEBase::set4Mom called " << std::endl ;
  std::abort();
}

void P4PxPyPzEBase::set4Mom(const I4Momentum * const  )
{ 
  std::cout << "FATAL ERROR dummy P4PxPyPzEBase::set4Mom called " << std::endl ;
  std::abort();

}

void P4PxPyPzEBase::set4Mom(const CLHEP::HepLorentzVector & )
{
  std::cout << "FATAL ERROR dummy P4PxPyPzEBase::set4Mom called " << std::endl ;
  std::abort();
}

