/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4IPtCotThPhiMBase.h"
#include <boost/io/ios_state.hpp>
#include <cmath>
#include <iomanip>

P4IPtCotThPhiMBase::~P4IPtCotThPhiMBase()
{}

double P4IPtCotThPhiMBase::pt() const
   {
     return 1./this->iPt();
   }


double P4IPtCotThPhiMBase::p() const 
{ 
  const double theCotTh=this->cotTh();
  return std::sqrt(1+theCotTh*theCotTh)/this->iPt() ;
}

double P4IPtCotThPhiMBase::p2() const 
{ 
  const double theCotTh=this->cotTh();
  const double theI_Pt=this->iPt();
  return (1+theCotTh*theCotTh)/(theI_Pt*theI_Pt) ;
}

double P4IPtCotThPhiMBase::m2() const {
  const double theM = this->m();
  return theM*theM;
}

double P4IPtCotThPhiMBase::tanTh() const 
{ 
  return 1./this->cotTh() ;
}

double P4IPtCotThPhiMBase::sinTh() const 
{ 
  const double theCotTh=this->cotTh();
  return 1./std::sqrt(1+theCotTh*theCotTh) ;
}

double P4IPtCotThPhiMBase::cosTh() const 
{ 
  const double theCotTh=this->cotTh();
  const double theCotTh2=theCotTh*theCotTh;
  const double theCosTh=std::sqrt(theCotTh2/(1.+theCotTh2));
  if (theCotTh>=0) {
    return theCosTh;
  }
  else{
    return -theCosTh;
  }  
}


double P4IPtCotThPhiMBase::e() const 
{ 
  const double theM=this->m();
  const double theP=this->p();
  if (theM==0.) return theP ;
        else return std::sqrt(theP*theP+theM*theM); 
}

double P4IPtCotThPhiMBase::et() const 
{ 
  const double theCotTh=this->cotTh();
  return this->e()/std::sqrt(1+theCotTh*theCotTh);
}


double P4IPtCotThPhiMBase::eta() const 
{
  const double theCotTh=this->cotTh();
  const double aux=std::sqrt(1+theCotTh*theCotTh);
  return -0.5*log((aux-theCotTh)/(aux+theCotTh));
 }


double P4IPtCotThPhiMBase::rapidity() const
  {
    const double theE=this->e();
    const double thePz=this->pz();
    return 0.5*std::log((theE+thePz)/(theE-thePz));
   }

 double P4IPtCotThPhiMBase::px() const
   { return this->cosPhi()/this->iPt();
   }

 double P4IPtCotThPhiMBase::py() const
   { return this->sinPhi()/this->iPt();
   }

 double P4IPtCotThPhiMBase::pz() const
   { return this->cotTh()/this->iPt();
   }
 
double P4IPtCotThPhiMBase::cosPhi() const
{
 return std::cos(this->phi());
}

double P4IPtCotThPhiMBase::sinPhi() const
{
 return std::sin(this->phi());
}

 CLHEP::HepLorentzVector P4IPtCotThPhiMBase::hlv() const
{ 
  //minimize the number of calculation and dereference
  const double thePt=this->pt();  
  const double theM=this->m();  
  const double thePx=thePt*this->cosPhi();
  const double thePy=thePt*this->sinPhi();
  const double thePz=thePt*this->cotTh();
  const double theE=std::sqrt(thePt*thePt+thePz*thePz+theM*theM);

  return CLHEP::HepLorentzVector(thePx,thePy,thePz,theE);
}

std::ostream& P4IPtCotThPhiMBase::dump( std::ostream& out ) const
{

  boost::io::ios_all_saver ias(out);

   out << "[ipt,cotTh,phi,m] ="
	     << std::right << std::scientific << std::setprecision(8) 
	     << std::setw(16) << this->iPt()
	     << std::setw(16) << this->cotTh()
	     << std::setw(16) << this->phi()
	     << std::setw(16) << this->m();

  ias.restore();
  
  return out;
}

const I4MomentumError* P4IPtCotThPhiMBase::errors() const
{
  return 0;
}


void P4IPtCotThPhiMBase::set4Mom(const I4Momentum &  )
{ 
  std::cout << "FATAL ERROR dummy P4IPtCotThPhiMBase::set4Mom called " << std::endl ;
  std::abort();
}

void P4IPtCotThPhiMBase::set4Mom(const I4Momentum * const  )
{ 
  std::cout << "FATAL ERROR dummy P4IPtCotThPhiMBase::set4Mom called " << std::endl ;
  std::abort();

}

void P4IPtCotThPhiMBase::set4Mom(const CLHEP::HepLorentzVector & )
{
  std::cout << "FATAL ERROR dummy P4IPtCotThPhiMBase::set4Mom called " << std::endl ;
  std::abort();
}
