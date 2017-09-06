///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplPxPyPzE.h 
// Header file for class P4::Impl::PxPyPzE
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef FOURMOM_P4IMPLPXPYPZE_H 
#define FOURMOM_P4IMPLPXPYPZE_H 

// STL includes

// HepMC / CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

// FourMom includes
#include "FourMom/Lib/P4BasePxPyPzE.h"
#include "FourMom/DeepCopyPointer.h"
#include "FourMom/FourMomentumError.h"

// Forward declaration
class P4ImplPxPyPzECnv_p1; //> for persistency

class P4ImplPxPyPzE : public P4BasePxPyPzE
{ 
  /////////////////////////////////////////////////////////////////// 
  // friends: 
  /////////////////////////////////////////////////////////////////// 
  friend class P4ImplPxPyPzECnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  typedef FourMomentumError< P4ImplPxPyPzE>  ErrorType;

  /** Default constructor: 
   */
  P4ImplPxPyPzE();

  /** Copy constructor: 
   */
  P4ImplPxPyPzE( const P4ImplPxPyPzE& rhs );

  /** Assignment operator: 
   */
  P4ImplPxPyPzE& operator=( const P4ImplPxPyPzE& rhs ); 

  /** constructor with all data members */
  P4ImplPxPyPzE( const double px, const  double py,
		 const double pz, const  double e );

  /** constructor from hlv to allow conversion from hlv */
  P4ImplPxPyPzE( const CLHEP::HepLorentzVector& hlv);

  /** constructor from any I4Momentum reference */
  explicit P4ImplPxPyPzE( const I4Momentum& theI4M );

  /** constructor from any I4Momentum pointer */
  explicit P4ImplPxPyPzE( const I4Momentum* const theI4M );

  /** Destructor: 
   */
  virtual ~P4ImplPxPyPzE(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** {@ a la @c I4Momentum -like interface
   */

  /** get px data member */
  double px() const;
  /** get py data member */
  double py() const;
  /** get pz data member */
  double pz() const;
  /** get energy data member */
  double e() const;

  virtual const I4MomentumError* errors() const;

  I4Momentum::Kind kind() const { return I4Momentum::P4PXPYPZE; };

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** set Px data member */
  void setPx(const double thePx );

  /** set Py data member */
  void setPy(const double thePy );

  /** set Pz data member */
  void setPz(const double thePz );

  /** set energy data member */
  void setE(const double theE );

  /** set all 4-mom from another I4Momentum reference  */
  void set4Mom (const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */
  void set4Mom (const I4Momentum * const theI4Mom );
  /** set all 4-mom from CLHEP::HepLorentzVector  */
  void set4Mom (const CLHEP::HepLorentzVector & theHlv );
  /** set the errors */
  void setErrors( const ErrorMatrixPxPyPzE& err);

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  double m_px;
  double m_py;
  double m_pz;
  double m_e;
  DeepCopyPointer< ErrorType> m_error;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4ImplPxPyPzE::P4ImplPxPyPzE( ) :
  P4BasePxPyPzE( ),
  m_px ( 0.*CLHEP::GeV ),
  m_py ( 0.*CLHEP::GeV ),
  m_pz ( 0.*CLHEP::GeV ),
  m_e  ( 0.*CLHEP::GeV ),
  m_error(0)
{}

inline P4ImplPxPyPzE::P4ImplPxPyPzE( const P4ImplPxPyPzE& rhs ) :
  P4BasePxPyPzE(  ),
  m_px ( rhs.m_px ),
  m_py ( rhs.m_py ),
  m_pz ( rhs.m_pz ),
  m_e  ( rhs.m_e  ),
  m_error(0)
{}

inline P4ImplPxPyPzE::P4ImplPxPyPzE( const double px, 
				     const double py, 
				     const double pz, 
				     const double e ) :
  P4BasePxPyPzE( ),
  m_px( px ),
  m_py( py ),
  m_pz( pz ),
  m_e ( e  ),
  m_error(0)
{}

inline P4ImplPxPyPzE::P4ImplPxPyPzE( const CLHEP::HepLorentzVector& hlv ) :
  P4BasePxPyPzE( ),
  m_px( hlv.px() ),
  m_py( hlv.py() ),
  m_pz( hlv.pz() ),
  m_e ( hlv.e()  ),
  m_error(0)
{}

inline P4ImplPxPyPzE::P4ImplPxPyPzE( const I4Momentum& i4Mom ) :
  P4BasePxPyPzE( ),
  m_px ( i4Mom.px() ),
  m_py ( i4Mom.py() ),
  m_pz ( i4Mom.pz() ),
  m_e  ( i4Mom.e()  ),
  m_error(0)
{}

inline P4ImplPxPyPzE::P4ImplPxPyPzE( const I4Momentum* const i4Mom ) :
  P4BasePxPyPzE( ),
  m_px ( i4Mom->px() ),
  m_py ( i4Mom->py() ),
  m_pz ( i4Mom->pz() ),
  m_e  ( i4Mom->e()  ),
  m_error(0)
{}

inline P4ImplPxPyPzE::~P4ImplPxPyPzE()
{}

inline double P4ImplPxPyPzE::px() const 
{ 
  return m_px;
}

inline double P4ImplPxPyPzE::py() const
{ 
  return m_py;
}

inline double P4ImplPxPyPzE::pz() const
{ 
  return m_pz;
}

inline double P4ImplPxPyPzE::e() const
{ 
  return m_e;
}

inline const I4MomentumError* P4ImplPxPyPzE::errors() const
{
  // check if the pointer is empty before dereferencing
  return m_error.get();
}

inline void P4ImplPxPyPzE::setPx( const double px ) 
{ 
  m_px = px;
}

inline void P4ImplPxPyPzE::setPy( const double py ) 
{ 
  m_py = py;
}

inline void P4ImplPxPyPzE::setPz( const double pz ) 
{ 
  m_pz = pz;
}

inline void P4ImplPxPyPzE::setE( const double e ) 
{ 
  m_e = e;
}

inline void P4ImplPxPyPzE::set4Mom( const I4Momentum& i4Mom )
{
  m_px = i4Mom.px();
  m_py = i4Mom.py();
  m_pz = i4Mom.pz();
  m_e  = i4Mom.e();
}

inline void P4ImplPxPyPzE::set4Mom( const I4Momentum* const i4Mom )
{
  this->set4Mom(*i4Mom);
}


inline void P4ImplPxPyPzE::set4Mom( const CLHEP::HepLorentzVector& hlv )
{
  m_px = hlv.px();
  m_py = hlv.py();
  m_pz = hlv.pz();
  m_e  = hlv.e();
}

inline void P4ImplPxPyPzE::setErrors( const ErrorMatrixPxPyPzE& err)
{
  m_error = DeepCopyPointer< ErrorType>(new ErrorType( err, *this));
}

#endif //> FOURMOM_P4IMPLPXPYPZE_H
