///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4IMPLEETAPHIM_H
#define FOURMOM_P4IMPLEETAPHIM_H

// STL includes
#include <cmath>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

// FourMom includes
#include "FourMom/Lib/P4BaseEEtaPhiM.h"
#include "FourMom/FourMomentumError.h"
#include "FourMom/DeepCopyPointer.h"

// forward declare
class P4ImplEEtaPhiMCnv_p1; // for persistency
/** 
   @class P4ImplEEtaPhiM

   P4ImplEEtaPhiM is a base class for classes with 4-momentum behavior, 
for which E, eta, phi and M are natural parameters, which is typically 
the case for a calorimeter object.
   Any class deriving from it should implement e(), eta(), phi(), m().
    
   @author David Rousseau rousseau@lal.in2p3.fr
   @author Sebastien Binet binet@cern.ch
 */

class P4ImplEEtaPhiM : public P4BaseEEtaPhiM
{
  friend class P4ImplEEtaPhiMCnv_p1;

 public:

  typedef FourMomentumError< P4ImplEEtaPhiM>  ErrorType;

  /** Default constructor: 
   */
  P4ImplEEtaPhiM();

  /** Copy constructor
   */
  explicit P4ImplEEtaPhiM( const P4ImplEEtaPhiM& rhs );

  /** Assignment operator
   */
  P4ImplEEtaPhiM& operator=( const P4ImplEEtaPhiM& rhs );

  /** constructor with all data members */
  P4ImplEEtaPhiM( const double e,   const double eta, 
		  const double phi, const double m );

  /** constructor from hlv to allow conversion from hlv */
  explicit P4ImplEEtaPhiM( const CLHEP::HepLorentzVector& hlv );

  /** constructor from any I4Momentum reference */
  explicit P4ImplEEtaPhiM( const I4Momentum& theI4M );

  /** constructor from any I4Momentum pointer */
  explicit P4ImplEEtaPhiM( const I4Momentum* const theI4M );

  /** virtual destructor needed by pool */
  virtual ~P4ImplEEtaPhiM();  

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */
  double m() const;
  double eta() const;
  double phi() const;
  double e() const;

  virtual const I4MomentumError* errors() const;

  I4Momentum::Kind kind() const { return I4Momentum::P4EETAPHIM; };

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** set energy data member */
  void setE( const double theE );
  /** set eta data member */
  void setEta( const double theEta );
  /** set phi data member */
  void setPhi( const double thePhi );
  /** set mass data member */
  void setM( const double theM );

  /** set all 4-mom from another I4Momentum reference  */
  void set4Mom( const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */
  void set4Mom( const I4Momentum * const theI4Mom );
  /** set all 4-mom from CLHEP HepLorentzVector  */
  void set4Mom( const CLHEP::HepLorentzVector & theHlv );
  /** set the errors */
  void setErrors( const ErrorMatrixEEtaPhiM& err);

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  double m_e;
  double m_eta;
  double m_phi;
  double m_m;
  DeepCopyPointer< ErrorType> m_error;

};

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4ImplEEtaPhiM::P4ImplEEtaPhiM( ) :
  P4BaseEEtaPhiM( ),
  m_e  ( 0.*CLHEP::GeV ),
  m_eta( 0. ),
  m_phi( 0. ),
  m_m  ( 0.*CLHEP::GeV ),
  m_error(0)
{}

inline P4ImplEEtaPhiM::P4ImplEEtaPhiM( const P4ImplEEtaPhiM& rhs ) :
  P4BaseEEtaPhiM( ),
  m_e  ( rhs.m_e ),
  m_eta( rhs.m_eta ),
  m_phi( rhs.m_phi ),
  m_m  ( rhs.m_m ),
  m_error(0)
{}

inline P4ImplEEtaPhiM::P4ImplEEtaPhiM( const double e,   const double eta,
				       const double phi, const double m ) :
  P4BaseEEtaPhiM( ),
  m_e  ( e   ),
  m_eta( eta ),
  m_phi( phi ),
  m_m  ( m   ),
  m_error(0)
{ 
  // could enforce phi range convention there
  // const double twopi =2.*M_PI;
  // const double threepi=3.*M_PI;
  // m_phi=std::fmod(phi+threepi,twopi)-M_PI;
}

inline P4ImplEEtaPhiM::P4ImplEEtaPhiM( const CLHEP::HepLorentzVector& hlv ) :
  P4BaseEEtaPhiM( ),
  m_e  ( hlv.e()   ),
  m_eta( hlv.eta() ),
  m_phi( hlv.phi() ),
  m_m  ( hlv.m()   ),
  m_error(0)
{}

inline P4ImplEEtaPhiM::P4ImplEEtaPhiM( const I4Momentum& i4mom ) :
  P4BaseEEtaPhiM( ),
  m_e  ( i4mom.e()   ),
  m_eta( i4mom.eta() ),
  m_phi( i4mom.phi() ),
  m_m  ( i4mom.m()   ),
  m_error(0)
{}

inline P4ImplEEtaPhiM::P4ImplEEtaPhiM( const I4Momentum * const i4Mom ) :
  P4BaseEEtaPhiM( ),
  m_e  ( i4Mom->e()   ),
  m_eta( i4Mom->eta() ),
  m_phi( i4Mom->phi() ),
  m_m  ( i4Mom->m()   ),
  m_error(0)
{}

inline P4ImplEEtaPhiM::~P4ImplEEtaPhiM()
{
}

inline double P4ImplEEtaPhiM::m() const
{
  return m_m;
}

inline double P4ImplEEtaPhiM::eta() const
{ 
  return m_eta;
}

inline double P4ImplEEtaPhiM::phi() const
{ 
  return m_phi;
}

inline double P4ImplEEtaPhiM::e() const
{ 
  return m_e;
}

inline const I4MomentumError* P4ImplEEtaPhiM::errors() const
{
  return m_error.get();
}

inline void P4ImplEEtaPhiM::setE( const double theE )
{ 
  m_e = theE;
}

inline void P4ImplEEtaPhiM::setEta( const double theEta )
{ 
  m_eta = theEta;
}

inline void P4ImplEEtaPhiM::setPhi( const double thePhi )
{ 
  m_phi = thePhi;
}

inline void P4ImplEEtaPhiM::setM( const double theM )
{ 
  m_m = theM;
}

inline void P4ImplEEtaPhiM::set4Mom( const I4Momentum & theI4Mom )
{
  m_e   = theI4Mom.e();
  m_eta = theI4Mom.eta();
  m_phi = theI4Mom.phi();
  m_m   = theI4Mom.m();
}

inline void P4ImplEEtaPhiM::set4Mom( const I4Momentum * const theI4Mom )
{
  this->set4Mom(*theI4Mom);
}

inline void P4ImplEEtaPhiM::set4Mom( const CLHEP::HepLorentzVector & hlv )
{
  m_e   = hlv.e();
  m_eta = hlv.eta();
  //FIXME protect against negative energy
  //assert(m_e >= 0,"P4::Impl::EEtaPhiM::set4Mom cannot have negative energy");
  //assert(m_e >= 0);
  // FIXME of the FIXME in fact it is not necessary to prtoect against negative energy
  // and besides Seal assert does not work
  //  ASSERT( m_e >= 0 );
  m_phi = hlv.phi();
  m_m   = hlv.m();
}

//void P4ImplEEtaPhiM::setErrors( const FourMomentumError* err)
inline void P4ImplEEtaPhiM::setErrors( const ErrorMatrixEEtaPhiM& err)
{
  //  if (m_error != 0) delete m_error;
  //  if (err != 0) m_error = new FourMomentumError(*err);
  //  else m_error = 0;

  m_error = DeepCopyPointer< ErrorType>(new ErrorType( err, *this));
}


#endif // FOURMOM_P4IMPLEETAPHIM_H
