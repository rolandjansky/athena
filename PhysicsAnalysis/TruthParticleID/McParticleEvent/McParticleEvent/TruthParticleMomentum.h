///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleMomentum.h 
// Header file for class TruthParticleMomentum
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHPARTICLEMOMENTUM_H 
#define MCPARTICLEEVENT_TRUTHPARTICLEMOMENTUM_H 

// STL includes
#include <iosfwd>
#include <limits>

// HepMC / CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "HepMC/SimpleVector.h"
#include "EventKernel/I4Momentum.h"

// Forward declaration
class I4MomentumError;

class TruthParticleMomentum
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleMomentum();

  /** Copy constructor: 
   */
  TruthParticleMomentum( const TruthParticleMomentum& rhs );

  /** Assignment operator: 
   */
  TruthParticleMomentum& operator=( const TruthParticleMomentum& rhs ); 

  /** Constructor with parameters: 
   */
  TruthParticleMomentum( const HepMC::FourVector& hlv );

  /** Destructor: 
   */
  virtual ~TruthParticleMomentum(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */

  double px() const;
  double py() const;
  double pz() const;
  double m()  const;
  double m2() const;
  double p()  const;
  double p2() const;
  double rapidity() const;
  double eta() const;
  double phi() const;
  double e()   const;
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

  // Truth particles never have errors
  const I4MomentumError* errors() const {return 0;}

  // underlying 4 vector is of type HepLorentzVector, so of P4PxPyPzE style
  I4Momentum::Kind kind() const { return I4Momentum::P4PXPYPZE; };


 

  /// Print @c I4Momentum content
  std::ostream& dump( std::ostream& out ) const;

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const I4Momentum * const theI4Mom ) ;

  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const I4Momentum & theI4Mom);

  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv ) ;

  /// reset the proxied CLHEP HepLorentzVector
  void set4Mom( const HepMC::FourVector& hlv );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// The @c CLHEP::LorentzVector we are proxying 
  /// (from the @c HepMC::GenParticle which is also being proxied)
  const HepMC::FourVector * m_hlv;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline double TruthParticleMomentum::px() const 
{ 
  return m_hlv ? m_hlv->px() : 0.*CLHEP::GeV;
}

inline double TruthParticleMomentum::py() const
{ 
  return m_hlv ? m_hlv->py() : 0.*CLHEP::GeV;
}

inline double TruthParticleMomentum::pz() const
{ 
  return m_hlv ? m_hlv->pz() : 0.*CLHEP::GeV;
}

inline double TruthParticleMomentum::m() const
{
  const double m2 = this->m2();
  return m2 < 0. ? -std::sqrt(-m2) : std::sqrt(m2);
}

inline double TruthParticleMomentum::m2() const
{
  const double thePx = this->px();
  const double thePy = this->py();
  const double thePz = this->pz();
  const double theE  = this->e();
  
  const double theM2= 
    theE*theE - ( thePx*thePx +
		  thePy*thePy +
		  thePz*thePz );
  
  return theM2;

}

inline double TruthParticleMomentum::p() const 
{ 
  return std::sqrt( this->p2() );
}

inline double TruthParticleMomentum::p2() const 
{ 
  const double px = this->px();
  const double py = this->py();
  const double pz = this->pz();

  return px*px + py*py + pz*pz;
}

inline double TruthParticleMomentum::rapidity() const 
{ 
  const double e =this->e();
  const double pz=this->pz();
  if (e==0.0) return 0.0;
  if (e==+pz) return +std::numeric_limits<double>::infinity();
  if (e==-pz) return -std::numeric_limits<double>::infinity();
  return 0.5*std::log((e+pz)/(e-pz));
}

inline double TruthParticleMomentum::eta() const 
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

inline double TruthParticleMomentum::phi() const 
{ 
  const double thePx = this->px();
  const double thePy = this->py();
  return thePx == 0.0 && thePy == 0.0 ? 0.0 : std::atan2(thePy,thePx);
}


inline double TruthParticleMomentum::e() const
{ 
  return m_hlv ? m_hlv->e() : 0.*CLHEP::GeV;
}

inline double TruthParticleMomentum::et() const
{
     //to be improved
  const double theE     = this->e();
  const double theSinTh = this->sinTh();
  
  return theE*theSinTh;
}

inline double TruthParticleMomentum::pt() const
{
  const double thePx = this->px();
  const double thePy = this->py();
  
  return std::sqrt(thePx*thePx+thePy*thePy);
}

inline double TruthParticleMomentum::iPt() const
{ 
  return 1./this->pt();
}

inline double TruthParticleMomentum::cosPhi() const
{ 
  return this->px()/this->pt();
}

inline double TruthParticleMomentum::sinPhi() const
{ 
  return this->py()/this->pt();
}

inline double TruthParticleMomentum::tanTh() const
{ 
  return this->pt()/this->pz();
}

inline double TruthParticleMomentum::cosTh() const
{ 
  return this->pz()/this->p();
}

inline double TruthParticleMomentum::sinTh() const
{ 
  return this->pt()/this->p();
}

inline double TruthParticleMomentum::cotTh() const
{ 
  return this->pz()/this->pt();
}

inline CLHEP::HepLorentzVector TruthParticleMomentum::hlv() const
{ 
  return CLHEP::HepLorentzVector(m_hlv->px(), m_hlv->py(),
				 m_hlv->pz(), m_hlv->e() );
}

inline void TruthParticleMomentum::set4Mom(const I4Momentum & )
{
  std::cout << " FATAL ERROR : TruthParticleMomentum::set4Mom called. Cannot change 4mom " << std::endl ;
  std::abort();

  
}

inline void TruthParticleMomentum::set4Mom(const I4Momentum * const  )
{
  std::cout << " FATAL ERROR : TruthParticleMomentum::set4Mom called. Cannot change 4mom " << std::endl ;
  std::abort();


}

inline void TruthParticleMomentum::set4Mom(const CLHEP::HepLorentzVector &  )
{
  std::cout << " FATAL ERROR : TruthParticleMomentum::set4Mom called. Cannot change 4mom " << std::endl ;
  std::abort();
}



inline void TruthParticleMomentum::set4Mom( const HepMC::FourVector& hlv )
{
  m_hlv = &hlv;
}

#endif //> MCPARTICLEEVENT_TRUTHPARTICLEMOMENTUM_H
