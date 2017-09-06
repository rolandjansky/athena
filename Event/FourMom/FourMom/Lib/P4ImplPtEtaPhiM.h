///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4IMPLPTETAPHIM_H
#define FOURMOM_P4IMPLPTETAPHIM_H

// STL includes
#include <cmath>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

// FourMom includes
#include "FourMom/Lib/P4BasePtEtaPhiM.h"
#include "FourMom/DeepCopyPointer.h"
#include "FourMom/FourMomentumError.h"

// forward declare
class P4ImplPtEtaPhiMCnv_p1; //> for persistency

/** 
   @class P4ImplPtEtaPhiM

   P4ImplPtEtaPhiM is a base class for classes with 4-momentum behavior, 
for which pt, eta, phi and M are natural parameters, which is typically 
the case for a ger object.
   Any class deriving from it should implement pt(), eta(), phi(), m().
    
   @author Olga Igonkina olya@cern.ch
 */


class P4ImplPtEtaPhiM : public P4BasePtEtaPhiM
{
  friend class P4ImplPtEtaPhiMCnv_p1;

 public:

  typedef FourMomentumError< P4ImplPtEtaPhiM>  ErrorType;

  /** Default constructor: 
   */
  P4ImplPtEtaPhiM();

  /** Copy constructor
   */
  explicit P4ImplPtEtaPhiM( const P4ImplPtEtaPhiM& rhs );

  /** Assignment operator
   */
  P4ImplPtEtaPhiM& operator=( const P4ImplPtEtaPhiM& rhs );

  /** constructor with all data members */
  P4ImplPtEtaPhiM( const double pt,  const double eta, 
	     const double phi, const double m );

  /** constructor from hlv to allow conversion from hlv */
  P4ImplPtEtaPhiM( const CLHEP::HepLorentzVector& hlv );

  /** constructor from any I4Momentum reference */
  explicit P4ImplPtEtaPhiM( const I4Momentum& theI4M );

  /** constructor from any I4Momentum pointer */
  explicit P4ImplPtEtaPhiM( const I4Momentum* const theI4M );

  /** virtual destructor needed by pool */
  virtual ~P4ImplPtEtaPhiM();  

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */
  double m() const;
  double eta() const;
  double phi() const;
  double pt() const;

  virtual const I4MomentumError* errors() const;

  I4Momentum::Kind kind() const { return I4Momentum::P4PTETAPHIM; }

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** set pt data member */
  void setPt( const double thePt );
  /** set eta data member */
  void setEta( const double theEta );
  /** set phi data member */
  void setPhi( const double thePhi );
  /** set mass data member */
  void setM( const double theM );

  /** set the errors */
  void setErrors( const ErrorMatrixPtEtaPhiM& err);

  /** set all 4-mom from another I4Momentum reference  */
  void set4Mom( const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */
  void set4Mom( const I4Momentum * const theI4Mom );
  /** set all 4-mom from CLHEP HepLorentzVector  */
  void set4Mom( const CLHEP::HepLorentzVector & theHlv );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  double m_pt;
  double m_eta;
  double m_phi;
  double m_m;
  DeepCopyPointer< ErrorType> m_error;

};

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4ImplPtEtaPhiM::P4ImplPtEtaPhiM() :
  P4BasePtEtaPhiM( ),
  m_pt ( 0.*CLHEP::GeV ),
  m_eta( 0.     ),
  m_phi( 0.     ),
  m_m  ( 0.*CLHEP::GeV ),
  m_error(0)
{}

inline P4ImplPtEtaPhiM::P4ImplPtEtaPhiM( const P4ImplPtEtaPhiM& rhs ) :
  P4BasePtEtaPhiM( ),
  m_pt ( rhs.m_pt  ),
  m_eta( rhs.m_eta ),
  m_phi( rhs.m_phi ),
  m_m  ( rhs.m_m   ),
  m_error(0)
{}

inline P4ImplPtEtaPhiM::P4ImplPtEtaPhiM( const double pt,  const double eta,
					 const double phi, const double m ) :
  P4BasePtEtaPhiM( ),
  m_pt ( pt  ),
  m_eta( eta ),
  m_phi( phi ),
  m_m  ( m   ),
  m_error(0)
{}

inline P4ImplPtEtaPhiM::P4ImplPtEtaPhiM( const CLHEP::HepLorentzVector& hlv ) :
  P4BasePtEtaPhiM(  ),
  m_pt ( hlv.perp() ),
  m_eta( hlv.eta()  ),
  m_phi( hlv.phi()  ),
  m_m  ( hlv.m()    ),
  m_error(0)
{}

inline P4ImplPtEtaPhiM::P4ImplPtEtaPhiM( const I4Momentum& i4mom ) :
  P4BasePtEtaPhiM(   ),
  m_pt ( i4mom.pt()  ),
  m_eta( i4mom.eta() ),
  m_phi( i4mom.phi() ),
  m_m  ( i4mom.m()   ),
  m_error(0)
{}

inline P4ImplPtEtaPhiM::P4ImplPtEtaPhiM( const I4Momentum * const i4Mom ) :
  P4BasePtEtaPhiM(    ),
  m_pt ( i4Mom->pt()  ),
  m_eta( i4Mom->eta() ),
  m_phi( i4Mom->phi() ),
  m_m  ( i4Mom->m()   ),
  m_error(0)
{}

inline P4ImplPtEtaPhiM::~P4ImplPtEtaPhiM()
{}

inline double P4ImplPtEtaPhiM::m() const
{ 
  return m_m;
}

inline double P4ImplPtEtaPhiM::eta() const
{ 
  return m_eta;
}

inline double P4ImplPtEtaPhiM::phi() const
{ 
  return m_phi;
}

inline double P4ImplPtEtaPhiM::pt() const
{ 
  return m_pt;
}

inline const I4MomentumError* P4ImplPtEtaPhiM::errors() const
{
  // check if the pointer is empty before dereferencing
  return m_error.get();
}

// setters

inline void P4ImplPtEtaPhiM::setPt( const double thePt )
{ 
  m_pt = thePt;
}

inline void P4ImplPtEtaPhiM::setEta( const double theEta )
{ 
  m_eta = theEta;
}

inline void P4ImplPtEtaPhiM::setPhi( const double thePhi )
{ 
  m_phi = thePhi;
}

inline void P4ImplPtEtaPhiM::setM( const double theM )
{ 
  m_m = theM;
}

inline void P4ImplPtEtaPhiM::set4Mom( const I4Momentum& i4mom )
{
  m_pt  = i4mom.pt();
  m_eta = i4mom.eta();
  m_phi = i4mom.phi();
  m_m   = i4mom.m();
}

inline void P4ImplPtEtaPhiM::set4Mom( const I4Momentum* const theI4Mom )
{
  this->set4Mom(*theI4Mom);
}

inline void P4ImplPtEtaPhiM::set4Mom( const CLHEP::HepLorentzVector & hlv)
{
  m_pt  = hlv.perp();
  m_eta = hlv.eta();
  //FIXME protect against negative energy
  //assert(m_e >= 0,"P4ImplPtEtaPhiM::set4Mom cannot have negative energy");
  //assert(m_e >= 0);
  // FIXME of the FIXME in fact it is not necessary to prtoect against negative energy
  // and besides Seal assert does not work
  //  ASSERT( m_e >= 0 );
  m_phi = hlv.phi();
  m_m   = hlv.m();
}

inline void P4ImplPtEtaPhiM::setErrors( const ErrorMatrixPtEtaPhiM& err)
{
  m_error = DeepCopyPointer< ErrorType>(new ErrorType( err, *this));
}

#endif // FOURMOM_P4IMPLPTETAPHIM_H
