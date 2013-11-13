///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4BASEPTETAPHIM_H
#define FOURMOM_P4BASEPTETAPHIM_H

// STL includes
#include <cmath>
#include <sstream>
#include <ostream>
#include <iomanip>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

/** 
   @class P4BasePtEtaPhiM

   P4BasePtEtaPhiM is a base class for classes with 4-momentum behavior, 
for which pt, eta, phi and M are natural parameters, which is typically 
the case for a ger object.
   Any class deriving from it should implement pt(), eta(), phi(), m().
    
   @author Olga Igonkina olya@cern.ch
 */


class P4BasePtEtaPhiM
{
 public:

  /** virtual destructor needed for inheritance */
  virtual ~P4BasePtEtaPhiM();  

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */

  double px() const;
  double py() const;
  double pz() const;
  virtual double m()   const = 0;
  double m2() const;
  double p()  const;
  double p2() const;
  virtual double eta() const = 0;
  virtual double phi() const = 0;
  double e() const;
  double et() const;   
  virtual double pt() const = 0;
  double iPt() const;
  double rapidity() const;
  double cosPhi() const;
  double sinPhi() const;
  double tanTh() const;
  double cosTh() const;
  double sinTh() const;
  double cotTh() const;
  CLHEP::HepLorentzVector hlv() const;

  /// Print @c I4Momentum content
  std::ostream& dump( std::ostream& out ) const;

  /** @}
   */

  // Not in @c I4Momentum interface
  //  double tanTh() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

};

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4BasePtEtaPhiM::~P4BasePtEtaPhiM()
{}

inline double P4BasePtEtaPhiM::px() const
{ 
  return this->pt() * std::cos(this->phi());
}

inline double P4BasePtEtaPhiM::py() const
{ 
  return this->pt() * std::sin(this->phi());
}

inline  double P4BasePtEtaPhiM::pz() const
{ 
  return this->pt()/this->tanTh();
}

inline double P4BasePtEtaPhiM::m2() const 
{ 
  const double mass = this->m();
  return mass*mass;
}

inline double P4BasePtEtaPhiM::p() const 
{ 
  const double thePt = this->pt();
  const double thePz = this->pz();

  //DR from Frank Paige
  // if negative pt point in the opposite direction
  // BUT eta and phi still the same !!!

  const double eSign = (thePt >= 0.) ? +1. : -1.;
  return eSign * std::sqrt( thePt*thePt + thePz*thePz );
}

inline double P4BasePtEtaPhiM::p2() const 
{ 
  // This method has been implemented so as to give the same as would be 
  // obtained from pow((this->p()),2) with this->p() implemented according to 
  // Frank Paige's algorithm above. 

  const double pt = this->pt();
  const double pz = this->pz();

  return pt*pt + pz*pz;
}

inline double P4BasePtEtaPhiM::e() const 
{
  const double theMass = this->m();
  const double thePt   = this->pt();
  const double thePz   = this->pz();

  //DR from Frank Paige
  // if negative pt point in the opposite direction
  // BUT eta and phi still the same !!!

  const double eSign = (thePt >= 0.) ? +1. : -1.;
  return eSign * std::sqrt( thePt*thePt + thePz*thePz +  theMass*theMass);
}

inline double P4BasePtEtaPhiM::et() const
{
  return this->e()*this->sinTh();
}

inline double P4BasePtEtaPhiM::iPt() const
{ 
  return 1./this->pt();
}

inline double P4BasePtEtaPhiM::rapidity() const
  {
    const double theE=this->e();
    const double thePz=this->pz();
    return 0.5*std::log((theE+thePz)/(theE-thePz));
   }

inline double P4BasePtEtaPhiM::cosPhi() const
{
 return std::cos(this->phi());
}

inline double P4BasePtEtaPhiM::sinPhi() const
{
 return std::sin(this->phi());
}



inline double P4BasePtEtaPhiM::cosTh() const
{ 
  return std::tanh(this->eta());
}

inline double P4BasePtEtaPhiM::sinTh() const
{ 
  // avoid numeric overflow if very large eta

  double aEta=std::abs(this->eta());
  if ( aEta>710) {
    aEta=710;
  }

  return 1./std::cosh(aEta);
}

inline double P4BasePtEtaPhiM::cotTh() const
{ 
  return std::sinh(this->eta());
}

inline double P4BasePtEtaPhiM::tanTh() const
{
 return 1./std::sinh(this->eta());
}

// less efficient
//inline double P4BasePtEtaPhiM::tanTh() const
//{
//  return std::tan( 2.* std::atan( std::exp( -(this->eta()) ) ) );
//}

inline CLHEP::HepLorentzVector P4BasePtEtaPhiM::hlv() const
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

inline std::ostream& P4BasePtEtaPhiM::dump( std::ostream& out ) const
{
  std::stringstream s;
  s << "[pt,eta,phi,m] ="
    << std::right << std::scientific << std::setprecision(8) 
    << std::setw(16) << this->pt()
    << std::setw(16) << this->eta()
    << std::setw(16) << this->phi()
    << std::setw(16) << this->m();

  out << s.str();
  return out;
}

#endif
