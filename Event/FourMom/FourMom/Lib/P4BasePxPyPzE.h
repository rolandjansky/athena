///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4BasePxPyPzE.h 
// Header file for class P4::Base::PxPyPzE
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef FOURMOM_P4BASEPXPYPZE_H 
#define FOURMOM_P4BASEPXPYPZE_H 

// STL includes
#include <cmath>
#include <limits>
#include <sstream>
#include <ostream>
#include <iomanip>

// HepMC / CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Forward declaration

class P4BasePxPyPzE
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~P4BasePxPyPzE(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */

  virtual double px() const = 0;
  virtual double py() const = 0;
  virtual double pz() const = 0;
  double m()  const;
  double m2() const;
  double p()  const;
  double p2() const;
  double eta() const ;
  double rapidity() const ;
  double phi() const ;
  virtual double e() const = 0;
  double et()  const;
  double pt()  const;
  double iPt() const;
  double sinPhi() const;
  double cosPhi() const;
  double tanTh() const;
  double cotTh() const;
  double cosTh() const;
  double sinTh() const;
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

inline P4BasePxPyPzE::~P4BasePxPyPzE()
{}

inline double P4BasePxPyPzE::m() const
{
  const double m2 = this->m2();
  return m2 < 0. ? -std::sqrt(-m2) : std::sqrt(m2);
}

inline double P4BasePxPyPzE::m2() const
{
  const double px = this->px();
  const double py = this->py();
  const double pz = this->pz();
  const double e  = this->e();
  
  const double m2 = e*e - ( px*px + py*py + pz*pz );
  return m2;
}

inline double P4BasePxPyPzE::p() const 
{ 
  return std::sqrt( this->p2() );
}

inline double P4BasePxPyPzE::p2() const 
{ 
  const double px = this->px();
  const double py = this->py();
  const double pz = this->pz();

  return px*px + py*py + pz*pz;
}

inline double P4BasePxPyPzE::eta() const 
{ 
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
  return 0.5*log((p+pz)/(p-pz));
}

inline double P4BasePxPyPzE::cosPhi() const
{
  if ( this->px() == 0 && this->py() ==0 )
    return 0;
  return this->px()/this->pt();
}

inline double P4BasePxPyPzE::sinPhi() const
{
  if ( this->px() == 0 && this->py() ==0 )
    return 0;
 return this->py()/this->pt();
}

inline double P4BasePxPyPzE::phi() const 
{ 
  const double px=this->px();
  const double py=this->py();
  // return px == 0.0 && py == 0.0 ? 0.0 : std::atan2(py,px);
  // AFAIK, atan2 is doing exactly this
  return std::atan2(py,px);
}


inline double P4BasePxPyPzE::et() const
{
  if ( this->e() == 0 )
    return 0;
  return this->e()*this->sinTh();
}

inline double P4BasePxPyPzE::pt() const
{
  const double px=this->px();
  const double py=this->py();
  
  return std::sqrt(px*px + py*py);
}

inline double P4BasePxPyPzE::iPt() const
{ 
  const double pt = this->pt();
  if ( pt == 0 )
    return std::numeric_limits<double>::infinity();
  return 1./pt;
}

inline double P4BasePxPyPzE::rapidity() const
{
  const double e =this->e();
  const double pz=this->pz();
  if (e==0.0) return 0.0;
  if (e==+pz) return +std::numeric_limits<double>::infinity();
  if (e==-pz) return -std::numeric_limits<double>::infinity();
  return 0.5*std::log((e+pz)/(e-pz));
}

inline double P4BasePxPyPzE::cosTh() const
{ 
  if ( this->p() == 0 ) return 0;
  return this->pz()/this->p();
}

inline double P4BasePxPyPzE::sinTh() const
{
  if ( this->p() == 0 ) return 0;
  return this->pt()/this->p();
}

inline double P4BasePxPyPzE::tanTh() const
{
  if ( this->pz() == 0 ) return 0;
  return this->pt()/this->pz();
}

inline double P4BasePxPyPzE::cotTh() const
{ 
  if ( this->pt() == 0 ) return 0;
  return this->pz()/this->pt();
}

inline CLHEP::HepLorentzVector P4BasePxPyPzE::hlv() const
{ 
  return CLHEP::HepLorentzVector(this->px(),this->py(),this->pz(),this->e());
}

inline std::ostream& P4BasePxPyPzE::dump( std::ostream& out ) const
{
  std::stringstream s;
  s << "[px,py,pz,e] ="
    << std::right << std::scientific << std::setprecision(8) 
    << std::setw(16) << this->px()
    << std::setw(16) << this->py()
    << std::setw(16) << this->pz()
    << std::setw(16) << this->e();

  out << s.str();
  return out;
}

#endif //> FOURMOM_P4BASEPXPYPZE_H
