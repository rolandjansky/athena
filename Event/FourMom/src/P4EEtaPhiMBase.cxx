/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4EEtaPhiMBase.h"
#include <boost/io/ios_state.hpp>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <limits>

P4EEtaPhiMBase::~P4EEtaPhiMBase()
{}

double P4EEtaPhiMBase::m2() const {
  const double theM = this->m();
  return theM*theM;
}



double P4EEtaPhiMBase::p() const 
{ 
  const double theM=this->m();
  const double theE=this->e();
  //  if (theM==0.) return theE ;
  //    else return sqrt(theE*theE-theM*theM); 
  //DR from Frank Paige
  // if negative energy point in the opposite direction
  // BUT eta and phi still the same !!!
  if (theM==0.) {
    return theE;
  } else {
    double eSign = (theE >= 0) ? +1 : -1;
    return eSign*std::sqrt(theE*theE-theM*theM); 
  }

}

double P4EEtaPhiMBase::p2() const 
{ 
  /** This p2() implementaion is derived from the (somewhat unusual) Frank Paige implementation used to calculate p() above. What we do is look at what would happen if we were to square the answer returned by Frank's algorithm:
   *
   * (1) The "eSign" would square to +1 and disappear,
   * (2) The sqrt would disappear leaving theE*theE-theM*theM
   * (3) In the event that theM==0, this theE*theE would indeed
   *     still equal theE*theE-theM*theM, so we simply return this quantity.
   */
  const double theM=this->m();
  const double theE=this->e();
  
  return theE*theE-theM*theM;
}

double P4EEtaPhiMBase::cosPhi() const
{ 
 return std::cos(this->phi());
}

double P4EEtaPhiMBase::sinPhi() const
{ 
 return std::sin(this->phi());
}

double P4EEtaPhiMBase::tanTh() const
{ 
  // avoid numeric overflow if very large eta

  double theEta=this->eta();
  if ( std::abs(theEta)>710) {
    theEta=theEta>0 ? 710 : -710;
    return 1./std::sinh(theEta);
  }

 return 1./this->cotTh();
}

double P4EEtaPhiMBase::cotTh() const
{
 return std::sinh(this->eta());
}

double P4EEtaPhiMBase::cosTh() const
{
 return std::tanh(this->eta());
}

double P4EEtaPhiMBase::sinTh() const
{ 
  // avoid numeric overflow if very large eta

  double aEta=std::abs(this->eta());
  if ( aEta>710) {
    aEta=710;
  }

  return 1./std::cosh(aEta);
  
}


double P4EEtaPhiMBase::pt() const
   {
     return this->p()*this->sinTh();
   }

double P4EEtaPhiMBase::et() const
   {
     return this->e()*this->sinTh();
   }
 
double P4EEtaPhiMBase::iPt() const
  { return 1./this->pt();
   }

double P4EEtaPhiMBase::rapidity() const
  {
    const double theE=this->e();
    const double thePz=this->pz();
    return 0.5*std::log((theE+thePz)/(theE-thePz));
   }

double P4EEtaPhiMBase::px() const
   { return this->pt()*this->cosPhi();
   }

double P4EEtaPhiMBase::py() const
   { return this->pt()*this->sinPhi();
   }

double P4EEtaPhiMBase::pz() const
   { return this->p()*this->cosTh();
   }

CLHEP::HepLorentzVector P4EEtaPhiMBase::hlv() const
{
  //minimize the number of calculation and dereference
  const double theCosTh=this->cosTh();  

  // DR from Frank Paige
  // negative energy point in opposite direction
  // BUT Eta and Phi still the same
  //  double theP=theE;

  const double theP=this->p();

  const double theSinTh=std::sqrt(1.-theCosTh*theCosTh);
  const double thePt=theP*theSinTh;
  const double thePx=thePt*this->cosPhi();
  const double thePy=thePt*this->sinPhi();
  const double thePz=theP*theCosTh;  


 return CLHEP::HepLorentzVector(thePx,thePy,thePz,this->e());
 
}



std::ostream& P4EEtaPhiMBase::dump( std::ostream& out ) const
{

  boost::io::ios_all_saver ias(out);
  out << "[e,eta,phi,m] ="
	     << std::right << std::scientific << std::setprecision(8) 
	     << std::setw(16) << this->e()
	     << std::setw(16) << this->eta()
	     << std::setw(16) << this->phi()
	     << std::setw(16) << this->m();

  ias.restore();
  
  return out;
}

const I4MomentumError* P4EEtaPhiMBase::errors() const
{
  return 0;
}


void P4EEtaPhiMBase::set4Mom(const I4Momentum &  )
{ 
  std::cout << "FATAL ERROR dummy P4EEtaPhiMBase::set4Mom called " << std::endl; ;
  std::abort();
}

void P4EEtaPhiMBase::set4Mom(const I4Momentum * const  )
{ 
  std::cout << "FATAL ERROR dummy P4EEtaPhiMBase::set4Mom called " << std::endl ;
  std::abort();

}

void P4EEtaPhiMBase::set4Mom(const CLHEP::HepLorentzVector & )
{
  std::cout << "FATAL ERROR dummy P4EEtaPhiMBase::set4Mom called " << std::endl ;
  std::abort();
}

