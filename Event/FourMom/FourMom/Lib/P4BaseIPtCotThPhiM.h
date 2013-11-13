///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4BASEIPTCOTTHPHIM_H
#define FOURMOM_P4BASEIPTCOTTHPHIM_H

// STL includes
#include <cmath>
#include <ostream>
#include <sstream>
#include <iomanip>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

/** 
   @class P4BaseIPtCotThPhiM

   P4BaseIPtCotThPhiM is a base class for classes with 4-momentum behavior,
for which 1/Pt, cottan(tehta), phi and M are natural parameters, which is 
typically the case of some (not all!) tracking classes. 
   Any class deriving from it should implement iPt(), cotTh(), phi(), m()
    
   @author David Rousseau rousseau@lal.in2p3.fr
   @author Sebastien Binet binet@cern.ch
 */

class P4BaseIPtCotThPhiM
{
 public:

  /** virtual destructor needed for inheritance */
  virtual ~P4BaseIPtCotThPhiM();

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
  double p() const;
  double p2() const;
  double eta() const;
  virtual double phi() const = 0;
  double e() const;
  double et() const;   
  double pt() const;
  virtual double iPt() const = 0;
  double rapidity() const;
  double cosPhi() const;
  double sinPhi() const;
  double tanTh()  const;
  double cosTh() const;
  double sinTh() const;
  virtual double cotTh() const = 0;
  CLHEP::HepLorentzVector hlv() const;

  /// Print @c I4Momentum content
  std::ostream& dump( std::ostream& out ) const;

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
};

inline P4BaseIPtCotThPhiM::~P4BaseIPtCotThPhiM()
{}

inline 
double 
P4BaseIPtCotThPhiM::px() const
{ 
  return std::cos(this->phi())/this->iPt();
}

inline
double
P4BaseIPtCotThPhiM::py() const
{ 
  return std::sin(this->phi())/this->iPt();
}

inline
double
P4BaseIPtCotThPhiM::pz() const
{ 
  return this->cotTh()/this->iPt();
}

inline
double
P4BaseIPtCotThPhiM::m2() const 
{ 
  const double mass = this->m();
  return mass*mass;
}

inline
double
P4BaseIPtCotThPhiM::p() const 
{ 
  const double theCotTh = this->cotTh();
  return std::sqrt( 1 + theCotTh*theCotTh ) / this->iPt() ;
}

inline
double
P4BaseIPtCotThPhiM::p2() const 
{ 
  const double cotTh = this->cotTh();
  const double iPt   = this->iPt();
  return ( 1 + cotTh*cotTh ) / (iPt*iPt) ;
}

inline
double
P4BaseIPtCotThPhiM::eta() const 
{
  const double theCotTh = this->cotTh();
  const double aux = std::sqrt(1+theCotTh*theCotTh);
  return -0.5 * std::log( (aux-theCotTh) / (aux+theCotTh) );
}

inline
double
P4BaseIPtCotThPhiM::rapidity() const
{
  const double theE=this->e();
  const double thePz=this->pz();
  return 0.5*std::log((theE+thePz)/(theE-thePz));
}


inline
double
P4BaseIPtCotThPhiM::e() const 
{ 
  const double theM = this->m();
  const double theP = this->p();
  if ( theM == 0. ) {
    return theP;
  } else {
    return std::sqrt( theP*theP + theM*theM ); 
  }
}

inline
double
P4BaseIPtCotThPhiM::et() const 
{ 
  const double theE     = this->e();
  const double theCotTh = this->cotTh();
  return theE / std::sqrt( 1 + theCotTh*theCotTh );
}

inline
double
P4BaseIPtCotThPhiM::pt() const
{
  return 1./this->iPt();
}

inline
double
P4BaseIPtCotThPhiM::tanTh() const 
{ 
  return 1./this->cotTh() ;
}

inline
double
P4BaseIPtCotThPhiM::cosTh() const 
{ 
  const double theCotTh  = this->cotTh();
  const double theCotTh2 = theCotTh*theCotTh;
  const double theCosTh  = std::sqrt(theCotTh2/(1.+theCotTh2));
  if ( theCotTh >= 0. ) {
    return  theCosTh;
  } else{
    return -theCosTh;
  }
}

inline
double
P4BaseIPtCotThPhiM::sinTh() const 
{ 
  const double theCotTh = this->cotTh();
  return 1. / std::sqrt( 1 + theCotTh*theCotTh );
}

inline
double
P4BaseIPtCotThPhiM::cosPhi() const
{
 return std::cos(this->phi());
}

inline
double
P4BaseIPtCotThPhiM::sinPhi() const
{
  return std::sin(this->phi());
}

inline
CLHEP::HepLorentzVector
P4BaseIPtCotThPhiM::hlv() const
{ 
  //minimize the number of calculation and dereference
  const double thePt    = this->pt();  
  const double theM     = this->m();  
  const double thePx 	= thePt*this->cosPhi();
  const double thePy 	= thePt*this->sinPhi();
  const double thePz    = thePt * this->cotTh();
  const double theE     = std::sqrt(   thePt * thePt 
				     + thePz * thePz 
				     + theM  *  theM );

  return CLHEP::HepLorentzVector( thePx, thePy, thePz, theE );
}

inline
std::ostream&
P4BaseIPtCotThPhiM::dump( std::ostream& out ) const
{
  std::stringstream s;
  s << "[ipt,cotTh,phi,m] ="
    << std::right << std::scientific << std::setprecision(8) 
    << std::setw(16) << this->iPt()
    << std::setw(16) << this->cotTh()
    << std::setw(16) << this->phi()
    << std::setw(16) << this->m();

  out << s.str();
  
  return out;
}

#endif // FOURMOM_P4BASEIPTCOTTHPHIM_H

