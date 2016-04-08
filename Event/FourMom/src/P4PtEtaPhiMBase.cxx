/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4PtEtaPhiMBase.h"
#include <boost/io/ios_state.hpp>
#include <cmath>
#include <iomanip>
#include <cstdlib>

P4PtEtaPhiMBase::~P4PtEtaPhiMBase()
{}

double P4PtEtaPhiMBase::e() const 
{
  const double theMass = this->m();
  const double thePt   = this->pt();
  const double thePz   = this->pz();

  //DR from Frank Paige
  // if negative pt point in the opposite direction
  // BUT eta and phi still the same !!!

    double eSign = (thePt >= 0.) ? +1. : -1.;
    return eSign*std::sqrt(thePt*thePt + thePz*thePz +  theMass*theMass);
}

double P4PtEtaPhiMBase::p() const 
{ 
  const double thePt = this->pt();
  const double thePz = this->pz();

  //DR from Frank Paige
  // if negative pt point in the opposite direction
  // BUT eta and phi still the same !!!

  double eSign = (thePt >= 0.) ? +1. : -1.;
  return eSign*std::sqrt(thePt*thePt + thePz*thePz);
}

double P4PtEtaPhiMBase::p2() const 
{ 
  /* This method has been implemented so as to give the same as would be obtained from pow((this->p()),2) with this->p() implemented according to Frank Paige's algorithm above. */

  const double thePt = this->pt();
  const double thePz = this->pz();

  return thePt*thePt + thePz*thePz;
}

double P4PtEtaPhiMBase::m2() const
{ const double theM=this->m();
 return theM*theM;
}

double P4PtEtaPhiMBase::cosPhi() const
{
 return std::cos(this->phi());
}

double P4PtEtaPhiMBase::sinPhi() const
{ 
 return std::sin(this->phi());
}

double P4PtEtaPhiMBase::cotTh() const
{
 return std::sinh(this->eta());
}

double P4PtEtaPhiMBase::cosTh() const
{
 return std::tanh(this->eta());
}

double P4PtEtaPhiMBase::sinTh() const
{
  // avoid numeric overflow if very large eta

  double aEta=std::abs(this->eta());
  if ( aEta>710) {
    aEta=710;
  }

  return 1./std::cosh(aEta);
}

double P4PtEtaPhiMBase::tanTh() const
{
  // avoid numeric overflow if very large eta

  double theEta=this->eta();
  if ( std::abs(theEta)>710) {
    theEta=theEta>0 ? 710 : -710;
    return 1./std::sinh(theEta);
  }
 
  return 1./std::sinh(this->eta());
}


double P4PtEtaPhiMBase::et() const
   {
     return this->e()*this->sinTh();
   }
 
double P4PtEtaPhiMBase::iPt() const
  { return 1./this->pt();
   }

double P4PtEtaPhiMBase::rapidity() const
  {
    const double theE=this->e();
    const double thePz=this->pz();
    return 0.5*std::log((theE+thePz)/(theE-thePz));
   }

 double P4PtEtaPhiMBase::px() const
   { return this->pt()*this->cosPhi();
   }

 double P4PtEtaPhiMBase::py() const
   { return this->pt()*this->sinPhi();
   }

 double P4PtEtaPhiMBase::pz() const
{ return this->pt()*this->cotTh();   }

 CLHEP::HepLorentzVector P4PtEtaPhiMBase::hlv() const
{
  //minimize the number of calculation and dereference
  const double theM = this->m();
  // note that pt can have negative sign : then it points in opposite direction but eta and phi are still on the same side
  const double thePt = this->pt();  
  

  const double thePx = thePt*this->cosPhi();
  const double thePy = thePt*this->sinPhi();

  const double thePz = thePt*this->cotTh();
  
  const double theE=std::sqrt(thePt*thePt+thePz*thePz+theM*theM);
  

  return CLHEP::HepLorentzVector( thePx, thePy, thePz, theE );
}

std::ostream& P4PtEtaPhiMBase::dump( std::ostream& out ) const
{
  boost::io::ios_all_saver ias(out);
  
  out << "[pt,eta,phi,m] ="
	     << std::right << std::scientific << std::setprecision(8) 
	     << std::setw(16) << this->pt()
	     << std::setw(16) << this->eta()
	     << std::setw(16) << this->phi()
	     << std::setw(16) << this->m();
  ias.restore();
  
  return out;
  
}

const I4MomentumError* P4PtEtaPhiMBase::errors() const
{
  return 0;
}

void P4PtEtaPhiMBase::set4Mom(const I4Momentum &  )
{ 
  std::cout << "FATAL ERROR dummy P4PtEtaPhiMBase::set4Mom called " << std::endl ;
  std::abort();
}

void P4PtEtaPhiMBase::set4Mom(const I4Momentum * const  )
{ 
  std::cout << "FATAL ERROR dummy P4PtEtaPhiMBase::set4Mom called " << std::endl; ;
  std::abort();

}

void P4PtEtaPhiMBase::set4Mom(const CLHEP::HepLorentzVector & )
{
  std::cout << "FATAL ERROR dummy P4PtEtaPhiMBase::set4Mom called " << std::endl ;
  std::abort();
}
