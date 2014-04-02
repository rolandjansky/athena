///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Nav4MomWrapper.h 
// Header file for class Nav4MomWrapper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef NAVFOURMOM_NAV4MOMWRAPPER_H 
#define NAVFOURMOM_NAV4MOMWRAPPER_H 

// STL includes

// HepMC / CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

// Gaudi includes

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// Navigation includes
#include "Navigation/NavWrapper.h"

// Forward declaration

template<class Navigable_t,
	 class P4Momentum_t>
class Nav4MomWrapper : virtual public INavigable4Momentum,
		               public NavWrapper<Navigable_t>,
		               public P4Momentum_t
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef NavWrapper<Navigable_t> navigable_type;

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef P4Momentum_t momentum_type;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  Nav4MomWrapper();

  /** Copy constructor: 
   */
  Nav4MomWrapper( const Nav4MomWrapper& rhs );

  /** Assignment operator: 
   */
  Nav4MomWrapper& operator=( const Nav4MomWrapper& rhs ); 

  /** Constructor with parameters: 
   */
  Nav4MomWrapper( const double p1, const double p2, 
		  const double p3, const double p4 );
  explicit Nav4MomWrapper( const CLHEP::HepLorentzVector& hlv );
  explicit Nav4MomWrapper( const P4Momentum_t& rhs );
  explicit Nav4MomWrapper( const I4Momentum  & rhs );
  explicit Nav4MomWrapper( const I4Momentum  * const rhs );


  /** Destructor: 
   */
  virtual ~Nav4MomWrapper(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** access to underlying base type (INavigable-like)
   */
  const navigable_type& navigableBase() const;

  /** access to underlying base type (I4Momentum-like)
   */
  const momentum_type& momentumBase() const;

  /** @{ @c INavigable interface forwarding
   */

  void fillToken( INavigationToken & thisToken ) const;
  void fillToken( INavigationToken & thisToken, 
		  const boost::any& weight ) const;

  /** @}
   */

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

  /** mass squared*/
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

  /** energy */
  double e() const;

  /** transverse energy defined to be e*sin(theta) */
  double et() const;   

  /** transverse momentum */
  double pt() const;

  /** inverse of transverse momentum */
  double iPt() const;

  /** cosine phi */
  double cosPhi() const;

  /** sine phi */
  double sinPhi() const;

  /** cosinus theta */
  double cosTh() const;

  /** sinus theta */
  double sinTh() const;

  /** cottan theta */
  double cotTh() const;

  /** tan theta */
  double tanTh() const;

  /** CLHEP HepLorentzVector */
  CLHEP::HepLorentzVector hlv() const;

  virtual const I4MomentumError* errors() const;
  
  /// makes the underlying type of the implementation available (P4XYZ)
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

  /** access to underlying base type (INavigable-like)
   */
  navigable_type& navigableBase();

  /** access to underlying base type (I4Momentum-like)
   */
  momentum_type& momentumBase();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper() :
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>( ),
		 P4Momentum_t( )
{}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper( const Nav4MomWrapper& rhs ) :
		 IAthenaBarCode(rhs),
                 INavigable(rhs),
                 I4Momentum(rhs),
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>( rhs ),
		 P4Momentum_t( rhs.momentumBase() )
{}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper( const CLHEP::HepLorentzVector& hlv ) :
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>(  ),
		 P4Momentum_t(  )
{
  P4Momentum_t::set4Mom(hlv);
}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper( const I4Momentum& i4mom ) :
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>(  ),
		 P4Momentum_t( i4mom )
{}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>&
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::operator=( const Nav4MomWrapper& rhs )
{
  if ( this != &rhs ) {
    INavigable4Momentum::operator=( rhs );
    NavWrapper<Navigable_t>::operator=( rhs );
    P4Momentum_t::operator=( rhs );
  }
  return *this;
}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper( const double p1, 
					      const double p2,
					      const double p3,
					      const double p4 ) :
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>(  ),
		 P4Momentum_t( p1, p2, p3, p4 )
{}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper( const I4Momentum* const i4mom ) :
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>(  ),
		 P4Momentum_t( i4mom )
{}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::Nav4MomWrapper( const P4Momentum_t& rhs ) :
		 INavigable4Momentum( ),
		 NavWrapper<Navigable_t>(  ),
		 P4Momentum_t( rhs )
{}

template<class Navigable_t, class P4Momentum_t>
inline
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::~Nav4MomWrapper()
{}

// getters

template<class Navigable_t, class P4Momentum_t>
inline
const NavWrapper<Navigable_t>&
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::navigableBase() const
{
  return *this;
}

template<class Navigable_t, class P4Momentum_t>
inline
const P4Momentum_t&
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::momentumBase() const
{
  return *this;
}

template<class Navigable_t, class P4Momentum_t>
inline 
void 
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::fillToken( INavigationToken& thisToken ) const
{
  return NavWrapper<Navigable_t>::fillToken(thisToken);
}

template<class Navigable_t, class P4Momentum_t>
inline 
void 
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::fillToken( INavigationToken& thisToken,
					 const boost::any& weight ) const
{
  return NavWrapper<Navigable_t>::fillToken(thisToken, weight);
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::px() const
{ 
  return P4Momentum_t::px(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::py() const
{ 
  return P4Momentum_t::py(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::pz() const
{ 
  return P4Momentum_t::pz(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::m() const
{ 
  return P4Momentum_t::m(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::m2() const
{ 
  return P4Momentum_t::m2(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::p() const
{ 
  return P4Momentum_t::p(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::p2() const
{ 
  return P4Momentum_t::p2(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::eta() const
{ 
  return P4Momentum_t::eta(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::rapidity() const
{ 
  return P4Momentum_t::rapidity(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::phi() const
{ 
  return P4Momentum_t::phi(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::e() const
{ 
  return P4Momentum_t::e(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::et() const   
{ 
  return P4Momentum_t::et(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::pt() const
{ 
  return P4Momentum_t::pt(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::iPt() const
{ 
  return P4Momentum_t::iPt(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::cosPhi() const
{ 
  return P4Momentum_t::cosPhi(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::sinPhi() const
{ 
  return P4Momentum_t::sinPhi();
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::cosTh() const
{ 
  return P4Momentum_t::cosTh(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::sinTh() const
{ 
  return P4Momentum_t::sinTh(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::cotTh() const
{ 
  return P4Momentum_t::cotTh(); 
}

template<class Navigable_t, class P4Momentum_t>
inline
double
Nav4MomWrapper<Navigable_t,P4Momentum_t>::tanTh() const
{ 
  return P4Momentum_t::tanTh();
}

template<class Navigable_t, class P4Momentum_t>
inline
const I4MomentumError*
Nav4MomWrapper<Navigable_t,P4Momentum_t>::errors() const
{ 
  return P4Momentum_t::errors();
}

template<class Navigable_t, class P4Momentum_t>
inline
CLHEP::HepLorentzVector
Nav4MomWrapper<Navigable_t,P4Momentum_t>::hlv() const
{ 
  return P4Momentum_t::hlv(); 
}

template<class Navigable_t, class P4Momentum_t>
inline 
I4Momentum::Kind
Nav4MomWrapper<Navigable_t,P4Momentum_t>::kind() const
{ 
  return P4Momentum_t::kind(); 
}



template<class Navigable_t, class P4Momentum_t>
inline
void 
Nav4MomWrapper<Navigable_t,P4Momentum_t>::set4Mom (const I4Momentum * const theI4Mom )
{
  return P4Momentum_t::set4Mom(theI4Mom);
}

template<class Navigable_t, class P4Momentum_t>
inline void 
Nav4MomWrapper<Navigable_t,P4Momentum_t>::set4Mom (const I4Momentum & theI4Mom )
{
  return P4Momentum_t::set4Mom(theI4Mom);
}

template<class Navigable_t, class P4Momentum_t>
inline
void 
Nav4MomWrapper<Navigable_t,P4Momentum_t>::set4Mom (const CLHEP::HepLorentzVector & theHlv )
{
  return P4Momentum_t::set4Mom(theHlv);
}



template<class Navigable_t, class P4Momentum_t>
inline 
std::ostream& 
Nav4MomWrapper<Navigable_t,P4Momentum_t>::dump( std::ostream& out ) const
{ 
  return P4Momentum_t::dump( out ); 
}

// --------------------
// non-const interface
// --------------------

template<class Navigable_t, class P4Momentum_t>
inline
NavWrapper<Navigable_t>&
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::navigableBase()
{
  return *this;
}

template<class Navigable_t, class P4Momentum_t>
inline
P4Momentum_t&
Nav4MomWrapper<Navigable_t,
	       P4Momentum_t>::momentumBase()
{
  return *this;
}

#endif //> NAVFOURMOM_NAV4MOMWRAPPER_H
