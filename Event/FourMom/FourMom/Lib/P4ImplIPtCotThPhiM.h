///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4IMPLIPTCOTTHPHIM_H
#define FOURMOM_P4IMPLIPTCOTTHPHIM_H

// STL includes
#include <cmath>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

// FourMom includes
#include "FourMom/Lib/P4BaseIPtCotThPhiM.h"
#include "FourMom/DeepCopyPointer.h"
#include "FourMom/FourMomentumError.h"

// forward declare
class P4ImplIPtCotThPhiMCnv_p1; // for persistency

/** 
   @class P4ImplIPtCotThPhiM

   P4ImplIPtCotThPhiM is a base class for classes with 4-momentum behavior, 
for which 1/Pt, cottan(tehta), phi and M are natural parameters, which is 
typically the case of some (not all!) tracking classes. 
   Any class deriving from it should implement iPt(), cotTh(), phi(), m()
    
   @author David Rousseau rousseau@lal.in2p3.fr
   @author Sebastien Binet binet@cern.ch
 */

class P4ImplIPtCotThPhiM : public P4BaseIPtCotThPhiM
{
  friend class P4ImplIPtCotThPhiMCnv_p1;
 public:

  typedef FourMomentumError< P4ImplIPtCotThPhiM>  ErrorType;

  /** Default constructor: 
   */
  P4ImplIPtCotThPhiM();

  /** Copy constructor
   */
  explicit P4ImplIPtCotThPhiM( const P4ImplIPtCotThPhiM& rhs );

  /** Assignment operator
   */
  P4ImplIPtCotThPhiM& operator=( const P4ImplIPtCotThPhiM& rhs );

  /** constructor with all data members */
  P4ImplIPtCotThPhiM( const double iPt, const double cotTh, 
		      const double phi, const double m );

  /** constructor from hlv to allow conversion from hlv */
  explicit P4ImplIPtCotThPhiM( const CLHEP::HepLorentzVector& hlv );

  /** constructor from any I4Momentum reference */
  explicit P4ImplIPtCotThPhiM( const I4Momentum& theI4M );

  /** constructor from any I4Momentum pointer */
  explicit P4ImplIPtCotThPhiM( const I4Momentum* const theI4M );

  /** virtual destructor needed by Pool */
  virtual ~P4ImplIPtCotThPhiM();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */

  double m() const;
  double phi() const;
  double iPt() const;
  double cotTh() const;

  virtual const I4MomentumError* errors() const;

  I4Momentum::Kind kind() const { return I4Momentum::P4IPTCOTTHPHIM; };

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** set inverse pT data member  */
  virtual void setIPt( const double theIPt );
  /** set cot(theta) data member  */
  virtual void setCotTh( const double theCotTh );
  /** set phi data member  */
  virtual void setPhi( const double thePhi );
  /** set m data member  */
  virtual void setM( const double theM );

  /** set the errors */
  void setErrors( const ErrorMatrixPtCotThPhiM& err);

  /** set all 4-mom from another I4Momentum reference  */
  virtual void set4Mom( const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */
  virtual void set4Mom( const I4Momentum * const theI4Mom );
  /** set all 4-mom from CLHEP HepLorentzVector  */
  virtual void set4Mom( const CLHEP::HepLorentzVector & theHlv );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  
  /** inverse P_T
   */
  double m_iPt;

  /** cot(theta)
   */
  double m_cotTh;

  /** Phi
   */
  double m_phi;

  /** mass
   */
  double m_m;
  
  /** error matrix pointer
   */
  DeepCopyPointer< ErrorType> m_error;

};

inline P4ImplIPtCotThPhiM::P4ImplIPtCotThPhiM() :
  P4BaseIPtCotThPhiM( ),
  m_iPt   ( 0./CLHEP::GeV ),
  m_cotTh ( 0. ),
  m_phi   ( 0. ),
  m_m     ( 0.*CLHEP::GeV ),
  m_error(0)
{}

inline P4ImplIPtCotThPhiM::P4ImplIPtCotThPhiM( const P4ImplIPtCotThPhiM& rhs ):
  P4BaseIPtCotThPhiM( ),
  m_iPt   ( rhs.m_iPt   ),
  m_cotTh ( rhs.m_cotTh ),
  m_phi   ( rhs.m_phi   ),
  m_m     ( rhs.m_m     ),
  m_error(0)
{}

inline P4ImplIPtCotThPhiM::P4ImplIPtCotThPhiM( const double iPt, 
					       const double cotTh,
					       const double phi,
					       const double m ) :
  P4BaseIPtCotThPhiM( ),
  m_iPt  ( std::fabs(iPt) ), //inverse Pt is unsigned. std::fabs for safety
  m_cotTh( cotTh ),
  m_phi  ( phi   ),
  m_m    ( m     ),
  m_error(0)
{
  // could enforce phi range convention there
  // const double twopi =2.*M_PI;
  // const double threepi=3.*M_PI;
  // m_phi=fmod(phi+threepi,twopi)-M_PI;
}

inline P4ImplIPtCotThPhiM::P4ImplIPtCotThPhiM( const CLHEP::HepLorentzVector& hlv ) :
  P4BaseIPtCotThPhiM( ),
  m_iPt   ( 1./hlv.perp() ),
  m_cotTh ( hlv.pz() / hlv.perp() ),
  m_phi   ( hlv.phi() ),
  m_m     ( hlv.m() ),
  m_error(0)
{}

inline P4ImplIPtCotThPhiM::P4ImplIPtCotThPhiM( const I4Momentum & p4 ) :
  P4BaseIPtCotThPhiM( ),
  m_iPt  ( p4.iPt()   ),
  m_cotTh( p4.cotTh() ),
  m_phi  ( p4.phi()   ),
  m_m    ( p4.m()     ),
  m_error(0)
{}

inline P4ImplIPtCotThPhiM::P4ImplIPtCotThPhiM( const I4Momentum * const p4 ) :
  P4BaseIPtCotThPhiM( ),
  m_iPt  ( p4->iPt()   ),
  m_cotTh( p4->cotTh() ),
  m_phi  ( p4->phi()   ),
  m_m    ( p4->m()     ),
  m_error(0)
{}

inline P4ImplIPtCotThPhiM::~P4ImplIPtCotThPhiM()
{}

inline double P4ImplIPtCotThPhiM::m() const
{ 
  return m_m;
}

inline double P4ImplIPtCotThPhiM::phi() const
{ 
  return m_phi;
}

inline double P4ImplIPtCotThPhiM::iPt() const
{ 
  return m_iPt;
}

inline double P4ImplIPtCotThPhiM::cotTh() const
{ 
  return m_cotTh;
}

inline const I4MomentumError* P4ImplIPtCotThPhiM::errors() const
{
  return m_error.get();
}

// setters

inline void P4ImplIPtCotThPhiM::setIPt( const double theIPt ) 
{ 
  m_iPt = theIPt;
}

inline void P4ImplIPtCotThPhiM::setCotTh( const double theCotTh )
{ 
  m_cotTh = theCotTh;
}

inline void P4ImplIPtCotThPhiM::setPhi( const double thePhi )
{ 
  m_phi = thePhi;
}

inline void P4ImplIPtCotThPhiM::setM( const double theM )
{ 
  m_m=theM;
}

inline void P4ImplIPtCotThPhiM::set4Mom( const I4Momentum & theI4Mom )
{
  m_iPt   = theI4Mom.iPt();
  m_cotTh = theI4Mom.cotTh();
  m_phi   = theI4Mom.phi();
  m_m     = theI4Mom.m();
}

inline void P4ImplIPtCotThPhiM::set4Mom( const I4Momentum * const theI4Mom )
{
  this->set4Mom(*theI4Mom);
}

inline void P4ImplIPtCotThPhiM::set4Mom( const CLHEP::HepLorentzVector & hlv )
{
  m_iPt   = 1./hlv.perp();
  m_cotTh = hlv.pz()*m_iPt;
  m_phi   = hlv.phi();
  m_m     = hlv.m();
}

inline void P4ImplIPtCotThPhiM::setErrors( const ErrorMatrixPtCotThPhiM& err)
{
  m_error = DeepCopyPointer< ErrorType>(new ErrorType( err, *this));
}


#endif // FOURMOM_P4IMPLIPTCOTTHPHIM_H
