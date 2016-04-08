///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleImpl.h
// Header file for class ParticleImpl
// Author  : Sebastien Binet
// Date    : August 2006
///////////////////////////////////////////////////////////////////
#ifndef PARTICLEEVENT_PARTICLEIMPL_H
#define PARTICLEEVENT_PARTICLEIMPL_H 1

/** @class ParticleImpl is a hub for various interface implementations
 */

// STL includes
#include <ostream>

// boost
#include <boost/any.hpp> // for navigable stuff

#include "AthLinks/ElementLink.h"

// EventKernel includes
#include "EventKernel/IParticle.h"

// Navigation includes
#include "Navigation/NavWrapper.h"

// ParticleEvent includes
#include "ParticleEvent/Lib/Base.h"

template< class INavigable_t,
          class I4Momentum_t,
	  class IParticle_t = ParticleEvent::Base >
class ParticleImpl : public IParticle
{

  ///////////////////////////////////////////////////////////////////
  // Public typedefs:
  ///////////////////////////////////////////////////////////////////
 public:

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef INavigable_t  navigable_type;

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef I4Momentum_t  momentum_type;

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef IParticle_t   particle_type;

  /** publish the type of the base class (ie: 'traits-itize' it)
   */
  typedef ParticleImpl<INavigable_t, 
		       I4Momentum_t,
		       IParticle_t>    self_type;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  /** Default constructor
   */
  ParticleImpl();

  /** Copy constructor
   */
  ParticleImpl( const ParticleImpl& rhs );

  /** Assignment operator
   */
  ParticleImpl& operator=( const ParticleImpl& rhs );

  /** Default constructor:
   */
  virtual ~ParticleImpl();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  /** access to underlying base type (INavigable-like)
   */
  const navigable_type& navigableBase() const;

  /** access to underlying base type (I4Momentum-like)
   */
  const momentum_type& momentumBase() const;

  /** access to underlying base type (IParticle-like)
   */
  const particle_type& particleBase() const;

  /** @{ @c INavigable interface forwarding
   */
  virtual
  void fillToken( INavigationToken & thisToken ) const;
  virtual
  void fillToken( INavigationToken & thisToken,
		  const boost::any& weight ) const;

  /** @}
   */
  /** @{ @c IAthenaBarCode interface forwarding
   */

  AthenaBarCode_t getAthenaBarCode() const;
  void setAthenaBarCode(AthenaBarCode_t /*id*/);
  //Comparing & Versioning Control
  bool hasSameAthenaBarCode(const IAthenaBarCode &/*obj*/) const;
  bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &) const;
  AthenaBarCodeVersion_t getVersion() const;
  void newVersion();
  void setVersion(AthenaBarCodeVersion_t newversion);
  //	std::ostream& dump(std::ostream& out) const;

  /** @}
   */

  /** @{ I4Momentum interface forwarding
   */

  /** x component of momentum */
  virtual
  double px() const;

  /** y component of momentum */
  virtual
  double py() const;

  /** z component of momentum */
  virtual
  double pz() const;

  /** mass */
  virtual
  double m() const;

  /** mass squared */
  virtual
  double m2() const;

  /** mass momentum magnitude */
  virtual
  double p() const;

  /** square of momentum magnitude */
  virtual
  double p2() const;

  /** rapidity */
  virtual
  double rapidity() const;

  /** pseudo rapidity */
  virtual
  double eta() const;

  /** phi in [-pi,pi[ */
  virtual
  double phi() const;

  /** energy */
  virtual
  double e() const;

  /** transverse energy defined to be e*sin(theta) */
  virtual
  double et() const;

  /** transverse momentum */
  virtual
  double pt() const;

  /** inverse of transverse momentum */
  virtual
  double iPt() const;

  /** cosinus phi */
  virtual
  double cosPhi() const;

  /** sinus phi */
  virtual
  double sinPhi() const;

  /** tan theta */
  virtual
  double tanTh() const;

  /** cosinus theta */
  virtual
  double cosTh() const;

  /** sinus theta */
  virtual
  double sinTh() const;

  /** cottan theta */
  virtual
  double cotTh() const;

  /** CLHEP HepLorentzVector */
  virtual
  CLHEP::HepLorentzVector hlv() const;

  virtual const I4MomentumError* errors() const;

  /// add Kind which tells what kind (P4XYZ) the underlying implementation has
  virtual I4Momentum::Kind kind() const;

  /// Print @c I4Momentum content
  virtual
  std::ostream& dump( std::ostream& out ) const;

  /** @{ IParticle interface forwarding
   */

  /** Return enum indicating real data, fast, or full simulation
      Return Type has a DataType enum with the following values:
     {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4}  */
  virtual
  ParticleDataType::DataType dataType() const;

  /** Return a RecVertex corresponding to particle Origin  */
  virtual
  const Trk::RecVertex* origin() const;

  /** Return an ElementLink corresponding to particle's Origin */
  virtual 
  const ElementLink< VxContainer >& originLink () const ;

  /** method to check if charge information is available*/
  virtual
  bool hasCharge() const;

  /** returns charge as a typedef ChargeType
      currently Charge Type is a double for jets
      this may be changed to an int and ask jets to extend this interface  */
  virtual
  ChargeType charge() const;

  /** method to check if particle id information is available*/
  virtual
  bool hasPdgId() const;

  /** Return enum indicating particle id
      the enum file is available in Event/EventKernel/PdtPdg.h  */
  virtual
  PDG::pidType pdgId() const;

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

  /** access to underlying base type (IParticle-like)
   */
  particle_type& particleBase();


  // Set functions (for I4Momentum)
  virtual
  void set4Mom( const I4Momentum& p4 );
  virtual
  void set4Mom( const I4Momentum * const p4 );
  virtual
  void set4Mom( const CLHEP::HepLorentzVector& hlv );

  // Set functions (for IParticle)
  virtual
  void set_dataType( ParticleDataType::DataType x );
  virtual
  void set_charge( ChargeType x );
  virtual
  void set_pdgId( PDG::pidType x );
  virtual
  void set_origin( const VxContainer* theContainer, int index );
  virtual
  void set_origin( const VxContainer* theContainer,
		   const Trk::VxCandidate * vertex );
  void set_origin( const ElementLink<VxContainer>& origin );

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  /// The navigable part
  INavigable_t m_nav;

  /// The 4-momentum part
  I4Momentum_t m_mom;

  /// The particle-id part
  IParticle_t m_part;

};

///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::ParticleImpl() :
  IAthenaBarCode      ( ), // FIXME should be removed
  INavigable          ( ), // FIXME should be removed
  I4Momentum          ( ), // FIXME should be removed
  INavigable4Momentum ( ), // FIXME should be removed
  IParticle    ( ),
  m_nav        ( ),
  m_mom        ( ),
  m_part       ( )
{}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::ParticleImpl( const ParticleImpl& rhs ) :
  IAthenaBarCode      ( rhs ), // FIXME should be removed
  INavigable          ( rhs ), // FIXME should be removed
  I4Momentum          ( rhs ), // FIXME should be removed
  INavigable4Momentum ( rhs ), // FIXME should be removed
  IParticle   ( rhs ),
  m_nav       ( rhs.m_nav  ),
  m_mom       ( rhs.m_mom  ),
  m_part      ( rhs.m_part )
{}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
ParticleImpl<INavigable_t,I4Momentum_t,IParticle_t>&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::operator=( const ParticleImpl& rhs )
{
  if ( this != &rhs ) {
    IAthenaBarCode::operator=(rhs);      // FIXME should be removed
    INavigable::operator=(rhs);          // FIXME should be removed
    I4Momentum::operator=(rhs);          // FIXME should be removed
    INavigable4Momentum::operator=(rhs); // FIXME should be removed
    IParticle::operator=( rhs );
    m_nav  = rhs.m_nav;
    m_mom  = rhs.m_mom;
    m_part = rhs.m_part;
  }
  return *this;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
//inline
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::~ParticleImpl()
{}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
const INavigable_t&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::navigableBase() const
{
  return m_nav;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
const I4Momentum_t&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::momentumBase() const
{
  return m_mom;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
const IParticle_t&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::particleBase() const
{
  return m_part;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::fillToken( INavigationToken& thisToken ) const
{
  m_nav.fillToken( thisToken );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::fillToken( INavigationToken & thisToken,
				      const boost::any& weight ) const
{
  m_nav.fillToken( thisToken, weight );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::px() const
{
  return m_mom.px();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double 
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::py() const
{
  return m_mom.py();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double 
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::pz() const
{
  return m_mom.pz();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::m() const
{
  return m_mom.m();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::m2() const
{
  return m_mom.m2();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::p() const
{
  return m_mom.p();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::p2() const
{
  return m_mom.p2();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::rapidity() const
{
  return m_mom.rapidity();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::eta() const
{
  return m_mom.eta();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::phi() const
{
  return m_mom.phi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::e() const
{
  return m_mom.e();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::et() const
{
  return m_mom.et();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::pt() const
{
  return m_mom.pt();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::iPt() const
{
  return m_mom.iPt();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::cosPhi() const
{
  return m_mom.cosPhi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::sinPhi() const
{
  return m_mom.sinPhi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::tanTh() const
{
  return m_mom.tanTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::cosTh() const
{
  return m_mom.cosTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::sinTh() const
{
  return m_mom.sinTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
double
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::cotTh() const
{
  return m_mom.cotTh();
}


template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
CLHEP::HepLorentzVector
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::hlv() const
{
  return m_mom.hlv();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
const I4MomentumError*
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::errors() const
{
  return m_mom.errors();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
I4Momentum::Kind
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::kind() const
{
  return m_mom.kind();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
std::ostream& 
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::dump( std::ostream& out ) const
{
  m_mom.dump ( out );
  out << "\n";
  m_part.dump( out );
  return out;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
ParticleDataType::DataType
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::dataType() const
{
  return m_part.dataType();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
const Trk::RecVertex*
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::origin() const
{
  return m_part.origin();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
const ElementLink< VxContainer >&
ParticleImpl<INavigable_t,
             I4Momentum_t,
             IParticle_t>::originLink() const
{
  return m_part.originLink();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
bool
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::hasCharge()  const
{
  return m_part.hasCharge();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
ChargeType
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::charge() const
{
  return m_part.charge();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
bool
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::hasPdgId() const
{
  return m_part.hasPdgId();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
PDG::pidType
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::pdgId() const
{
  return m_part.pdgId();
}

////////////////////////////////////////////////
// Passing all interface of IAthenaBarCode Here
// Through particleBase()
////////////////////////////////////////////////

template< class INavigable_t, class I4Momentum_t, class IParticle_t> 
inline
AthenaBarCode_t
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::getAthenaBarCode() const 
{
  return particleBase().getAthenaBarCodeImpl().getAthenaBarCode();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t> 
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::setAthenaBarCode(AthenaBarCode_t id) 
{
  particleBase().getAthenaBarCodeImpl().setAthenaBarCode(id);
}

//Comparing & Versioning Control
template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
bool
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::hasSameAthenaBarCode(const IAthenaBarCode &bc) const
{
  return particleBase().getAthenaBarCodeImpl().hasSameAthenaBarCode(bc);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
bool
ParticleImpl<INavigable_t, 
	     I4Momentum_t,
	     IParticle_t>::hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &bc) const 
{
  return particleBase().getAthenaBarCodeImpl().hasSameAthenaBarCodeExceptVersion(bc);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline 
AthenaBarCodeVersion_t
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::getVersion() const 
{
  return particleBase().getAthenaBarCodeImpl().getVersion();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t> 
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::newVersion() 
{
  particleBase().getAthenaBarCodeImpl().newVersion();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t, 
	     IParticle_t>::setVersion(AthenaBarCodeVersion_t newversion) 
{
  particleBase().getAthenaBarCodeImpl().setVersion(newversion);
}

// template< class INavigable_t, class I4Momentum_t, class IParticle_t>
// inline
// std::ostream& 
// ParticleImpl<INavigable_t, 
// 	     I4Momentum_t,
// 	     IParticle_t>::dump(std::ostream& out) const 
// {
//   return particleBase().getAthenaBarCodeImpl().dump(out);
// }

////////////////////////////////////////////////
// Finished Passing all interface of IAthenaBarCode
////////////////////////////////////////////////

// --------------------
// non-const interface
// --------------------

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
INavigable_t&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::navigableBase()
{
  return m_nav;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
I4Momentum_t&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::momentumBase()
{
  return m_mom;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
IParticle_t&
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::particleBase()
{
  return m_part;
}


// setters

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set4Mom( const I4Momentum& p4 )
{
  return m_mom.set4Mom( p4 );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set4Mom( const I4Momentum * const p4 )
{
  return m_mom.set4Mom( p4 );
}


template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set4Mom( const CLHEP::HepLorentzVector& hlv )
{
  return m_mom.set4Mom( hlv );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set_dataType( ParticleDataType::DataType x )
{
  m_part.set_dataType(x);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set_charge( ChargeType x )
{
  m_part.set_charge(x);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set_pdgId( PDG::pidType x )
{
  m_part.set_pdgId(x);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set_origin( const VxContainer* theContainer,
				       int index )
{
  m_part.set_origin(theContainer, index);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set_origin( const VxContainer* theContainer,
				       const Trk::VxCandidate * vertex )
{
  m_part.set_origin(theContainer, vertex);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
void
ParticleImpl<INavigable_t,
	     I4Momentum_t,
	     IParticle_t>::set_origin( const ElementLink<VxContainer>& origin )
{
  m_part.set_origin(origin);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
std::ostream&
operator<< (std::ostream& out,
	    const ParticleImpl<INavigable_t,
	                       I4Momentum_t,
			       IParticle_t>& p4);

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
std::ostream&
operator<< (std::ostream& out,
	    const ParticleImpl<INavigable_t,
	                       I4Momentum_t,
	                       IParticle_t>& p4)
{
  return p4.dump( out );
}

#endif //> PARTICLEEVENT_PARTICLEIMPL_H
