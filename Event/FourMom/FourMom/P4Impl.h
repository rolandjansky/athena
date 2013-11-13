///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4Impl.h 
// Header file for class P4Impl
// Author  : Sebastien Binet 
// Date    : August 2006
/////////////////////////////////////////////////////////////////// 
#ifndef FOURMOM_P4IMPL_H 
#define FOURMOM_P4IMPL_H 

/** @class P4Impl is a hub for various interface implementations 
 *  of @c I4Momentum
 */

// STL includes

// Gaudi includes

// DataModel includes

// EventKernel includes
#include "EventKernel/I4Momentum.h"

template< class P4Mom_t >
class P4Impl : virtual public I4Momentum,
                       public P4Mom_t
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef P4Mom_t base_type;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor
   */
  P4Impl();

  /** Copy constructor
   */
  explicit P4Impl( const P4Impl& rhs );

  /** Assignment operator
   */
  P4Impl& operator=( const P4Impl& rhs );

  /** Constructor with arguments
   */
  P4Impl( const double p1, const double p2, const double p3, const double p4 );
  explicit P4Impl( const CLHEP::HepLorentzVector& theHlv );
  explicit P4Impl( const P4Mom_t& rhs );
  explicit P4Impl( const I4Momentum  & rhs );
  explicit P4Impl( const I4Momentum  * const rhs );

  /** Default constructor: 
   */
  virtual ~P4Impl();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** access to underlying base type
   */
  const P4Mom_t& base() const;

  /** @{ I4Momentum interface forwarding
   */

  /** x component of momentum */
  double px() const;

  /** y component of momentum */
  double py() const;

  /** z component of momentum */
  double pz() const;

  /** mass */
  double m() const;

  /** mass squared */
  double m2() const;

  /** momentum magnitude */
  double p() const;

  /** square of momentum magnitude */
  double p2() const;

  /** pseudo rapidity */
  double eta() const;

  /** rapidity */
  double rapidity() const;

  /** phi in [-pi,pi[ */
  double phi() const;

  /** sin(phi) */
  double sinPhi() const;

  /** cos(phi) */
  double cosPhi() const;

  /** energy */
  double e() const;

  /** transverse energy defined to be e*sin(theta) */
  double et() const;   

  /** transverse momentum */
  double pt() const;

  /** inverse of transverse momentum */
  double iPt() const;

  /** cosinus theta */
  double cosTh() const;

  /** sinus theta */
  double sinTh() const;

  /** tan theta */
  double tanTh() const;

  /** cottan theta */
  double cotTh() const;

  /** CLHEP HepLorentzVector */
  CLHEP::HepLorentzVector hlv() const;

  virtual const I4MomentumError* errors() const;

  /** kind (what type of P4XYZ) of underlying class */
  I4Momentum::Kind kind() const;


  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const I4Momentum * const theI4Mom ) ;

  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const I4Momentum & theI4Mom) ;

  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv ) ;


  /// Print @c I4Momentum content
  std::ostream& dump( std::ostream& out ) const;

  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** access to underlying base type
   */
  P4Mom_t& base();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::P4Impl() :
  I4Momentum ( ),
  P4Mom_t    ( )
{}

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::P4Impl( const P4Impl& rhs ) :
  I4Momentum ( ),
  P4Mom_t    ( rhs.base() )
{}

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::P4Impl( const CLHEP::HepLorentzVector& theHlv ) :
  I4Momentum (     ),
  P4Mom_t    ( theHlv )
{}

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::P4Impl( const P4Mom_t& rhs ) :
  I4Momentum (     ),
  P4Mom_t    ( rhs )
{}

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::P4Impl( const I4Momentum & theI4M ) :
  I4Momentum ( ),
  P4Mom_t    ( )
{
  P4Mom_t::set4Mom(theI4M);
}

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::P4Impl( const I4Momentum * const theI4M ) :
  I4Momentum (        ),
  P4Mom_t    ( theI4M )
{}

template< class P4Mom_t>
P4Impl<P4Mom_t>& 
P4Impl<P4Mom_t>::operator=( const P4Impl& rhs )
{
  if ( this != &rhs ) {
    I4Momentum::operator=( rhs );
    P4Mom_t::operator=( rhs );
  }
  return *this;
}

template< class P4Mom_t >
inline
P4Impl<P4Mom_t>::P4Impl( const double p1, const double p2, 
			      const double p3, const double p4 ) :
  I4Momentum( ),
  P4Mom_t   ( p1, p2, p3, p4 )
{}

template< class P4Mom_t>
inline
P4Impl<P4Mom_t>::~P4Impl()
{}

template< class P4Mom_t>
inline const P4Mom_t& P4Impl<P4Mom_t>::base() const
{
  return *this;
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::px() const
{ 
  return P4Mom_t::px(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::py() const
{ 
  return P4Mom_t::py(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::pz() const
{ 
  return P4Mom_t::pz(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::m() const
{ 
  return P4Mom_t::m(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::m2() const
{ 
  return P4Mom_t::m2();
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::p() const
{ 
  return P4Mom_t::p(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::p2() const
{ 
  return P4Mom_t::p2(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::eta() const
{ 
  return P4Mom_t::eta(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::rapidity() const
{ 
  return P4Mom_t::rapidity(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::phi() const
{ 
  return P4Mom_t::phi(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::cosPhi() const
{ 
  return P4Mom_t::cosPhi(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::sinPhi() const
{ 
  return P4Mom_t::sinPhi(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::e() const
{ 
  return P4Mom_t::e(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::et() const   
{ 
  return P4Mom_t::et(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::pt() const
{ 
  return P4Mom_t::pt(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::iPt() const
{ 
  return P4Mom_t::iPt(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::cosTh() const
{ 
  return P4Mom_t::cosTh(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::sinTh() const
{ 
  return P4Mom_t::sinTh(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::tanTh() const
{ 
  return P4Mom_t::tanTh(); 
}

template< class P4Mom_t>
inline double P4Impl<P4Mom_t>::cotTh() const
{ 
  return P4Mom_t::cotTh(); 
}

template< class P4Mom_t>
inline CLHEP::HepLorentzVector P4Impl<P4Mom_t>::hlv() const
{ 
  return P4Mom_t::hlv(); 
}

template< class P4Mom_t>
inline const I4MomentumError* P4Impl<P4Mom_t>::errors() const
{
  return P4Mom_t::errors();
}

template< class P4Mom_t>
inline I4Momentum::Kind P4Impl<P4Mom_t>::kind() const
{
  return P4Mom_t::kind();
}


template< class P4Mom_t>
inline void P4Impl<P4Mom_t>::set4Mom (const I4Momentum * const theI4Mom )
{
  return P4Mom_t::set4Mom(theI4Mom);
}

template< class P4Mom_t>
inline void P4Impl<P4Mom_t>::set4Mom (const I4Momentum & theI4Mom )
{
  return
 P4Mom_t::set4Mom(theI4Mom);
}

template< class P4Mom_t>
inline void P4Impl<P4Mom_t>::set4Mom (const CLHEP::HepLorentzVector & theHlv )
{
  return P4Mom_t::set4Mom(theHlv);
}






template< class P4Mom_t>
inline std::ostream& P4Impl<P4Mom_t>::dump( std::ostream& out ) const
{
  return P4Mom_t::dump( out );
}

// non-const methods
template< class P4Mom_t>
inline P4Mom_t& P4Impl<P4Mom_t>::base()
{
  return *this;
}


#endif //> FOURMOM_P4IMPL_H
