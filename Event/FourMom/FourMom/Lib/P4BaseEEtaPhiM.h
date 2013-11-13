///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4BASEEETAPHIM_H
#define FOURMOM_P4BASEEETAPHIM_H

// STL includes
#include <cmath>
#include <ostream>
#include <sstream>
#include <iomanip>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

/** 
   @class P4BaseEEtaPhiM

   P4BaseEEtaPhiM is a base class for classes with 4-momentum behavior, 
for which E, eta, phi and M are natural parameters, which is typically 
the case for a calorimeter object.
   Any class deriving from it should implement e(), eta(), phi(), m().
    
   @author David Rousseau rousseau@lal.in2p3.fr
 */


class P4BaseEEtaPhiM
{
 public:

  /** virtual destructor needed for inheritance */
  virtual ~P4BaseEEtaPhiM();  

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */

  double px() const;
  double py() const;
  double pz() const;
  virtual double m() const = 0;
  double m2() const;
  double rapidity() const;
  double p() const;
  double p2() const;
  virtual double eta() const = 0;
  virtual double phi() const = 0;
  virtual double e() const = 0;
  double et() const;   
  double pt() const;
  double iPt() const;
  double sinPhi() const;
  double cosPhi() const;
  double tanTh() const;
  double cosTh() const;
  double sinTh() const;
  double cotTh() const;
  CLHEP::HepLorentzVector hlv() const;

  /// Print @c I4Momentum content
  std::ostream& dump( std::ostream& out ) const;
  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
};

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4BaseEEtaPhiM::~P4BaseEEtaPhiM()
{}

inline double P4BaseEEtaPhiM::px() const
{ 
  return this->pt()*cos(this->phi());
}

inline double P4BaseEEtaPhiM::py() const
{ 
  return this->pt()*sin(this->phi());
}

inline double P4BaseEEtaPhiM::pz() const
{ 
  return this->p()*this->cosTh();
}

inline double P4BaseEEtaPhiM::m2() const
{ 
  const double mass = this->m();
  return mass*mass;
}

inline double P4BaseEEtaPhiM::p() const 
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
    double eSign = (theE >= 0.) ? +1. : -1.;
    return eSign*std::sqrt(theE*theE-theM*theM); 
  }

}

inline double P4BaseEEtaPhiM::p2() const
{ 
  /** This p2() implementaion is derived from the (somewhat unusual) Frank Paige implementation used to calculate p() above. What we do is look at what would happen if we were to square the answer returned by Frank's algorithm:
   *
   * (1) The "eSign" would square to +1 and disappear,
   * (2) The sqrt would disappear leaving theE*theE-theM*theM
   * (3) In the event that theM==0, this theE*theE would indeed
   *     still equal theE*theE-theM*theM, so we simply return this quantity.
   */
  const double mass = this->m();
  const double ene  = this->e();
  
  return ene*ene - mass*mass;
}

inline double P4BaseEEtaPhiM::et() const
{
  return this->e()*this->sinTh();
}

inline double P4BaseEEtaPhiM::rapidity() const
  {
    const double theE=this->e();
    const double thePz=this->pz();
    return 0.5*std::log((theE+thePz)/(theE-thePz));
   }

inline double P4BaseEEtaPhiM::pt() const
{
  return this->p()*this->sinTh();
}

inline double P4BaseEEtaPhiM::iPt() const
{ 
  return 1./this->pt();
}

inline double P4BaseEEtaPhiM::cosPhi() const
{
 return std::cos(this->phi());
}

inline double P4BaseEEtaPhiM::sinPhi() const
{ 
  return std::sin(this->phi());
}


inline double P4BaseEEtaPhiM::tanTh() const
{ 
  double theEta=this->eta();
  if ( std::abs(theEta)>710) {
    theEta=theEta>0 ? 710 : -710;
    return 1./std::sinh(theEta);
  }
  return 1./this->cotTh();
}
 
inline double P4BaseEEtaPhiM::cosTh() const
{ 
  return std::tanh(this->eta());
}

inline double P4BaseEEtaPhiM::sinTh() const
{ 
  // avoid numeric overflow if very large eta

  double aEta=std::abs(this->eta());
  if ( aEta>710) {
    aEta=710;
  }

  return 1./std::cosh(aEta);
}

inline double P4BaseEEtaPhiM::cotTh() const
{ 
  return std::sinh(this->eta());
}

inline CLHEP::HepLorentzVector P4BaseEEtaPhiM::hlv() const
{
  //minimize the number of calculation and dereference
  const double theE = this->e();
  const double theCosTh = this->cosTh();  
  
  // DR from Frank Paige
  // negative energy point in opposite direction
  // BUT Eta and Phi still the same
  //  double theP=theE;
  
  const double theP = this->p();
  
  const double theSinTh = std::sqrt(1.-theCosTh*theCosTh);
  const double thePt = theP*theSinTh;
  const double thePx = thePt*this->cosPhi();
  const double thePy = thePt*this->sinPhi();
  const double thePz = theP*theCosTh;  
  
  return CLHEP::HepLorentzVector(thePx,thePy,thePz,theE);
}

inline std::ostream& P4BaseEEtaPhiM::dump( std::ostream& out ) const
{
  std::stringstream s;
  s << "[e,eta,phi,m] ="
    << std::right << std::scientific << std::setprecision(8) 
    << std::setw(16) << this->e()
    << std::setw(16) << this->eta()
    << std::setw(16) << this->phi()
    << std::setw(16) << this->m();

  out << s.str();
  return out;

}


#endif // FOURMOM_P4BASEEETAPHIM_H

