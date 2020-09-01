///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSigStateImpl.h
// Header file for class ParticleSigStateImpl
// Author  : Pier-Olivier DeViveiros
//           Inspired by Sebastien Binet's ParticleImpl
// Date    : November 2009
///////////////////////////////////////////////////////////////////
#ifndef PARTICLEEVENT_PARTICLESIGSTATEIMPL_H
#define PARTICLEEVENT_PARTICLESIGSTATEIMPL_H 1

/** @class ParticleSigStateImpl is a hub for various interface implementations
 */

// STL includes
#include <ostream>

// boost
#include <boost/any.hpp> // for navigable stuff

#include "AthLinks/ElementLink.h"

// EventKernel includes
#include "EventKernel/IParticle.h"
#include "EventKernel/ISignalState.h"

// Navigation includes
#include "Navigation/NavWrapper.h"

// ParticleEvent includes
#include "ParticleEvent/Lib/Base.h"
#include "CxxUtils/checker_macros.h"

template< class INavigable_t,
          class I4Momentum_t,
	  class IParticle_t = ParticleEvent::Base >
class ParticleSigStateImpl : public IParticle, public ISignalState
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
  typedef ParticleSigStateImpl<INavigable_t, 
			       I4Momentum_t,
			       IParticle_t>    self_type;

  typedef P4SignalState::State  state_t;
  
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:
  
  /** Default constructor
   */
  ParticleSigStateImpl( int nStates = 1 , state_t defaultState = state_t(0) );
  
  /** Copy constructor
   */
  ParticleSigStateImpl( const ParticleSigStateImpl& rhs );
  
  /** Assignment operator
   */
  ParticleSigStateImpl& operator=( const ParticleSigStateImpl& rhs );
  
  /** Default destructor:
   */
  virtual ~ParticleSigStateImpl();
  
  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////
  
  /** access to underlying base type (INavigable-like)
   */
  const navigable_type& navigableBase() const;
  
  /** access to underlying base type (I4Momentum-like)
   */
  const momentum_type& momentumBase( state_t s ) const;
  const momentum_type& momentumBase( ) const;
  
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
  
  /** We re-define here extra class routines that will 
      allow direct access to signal state kinematics 
      without changing the current state 
  */

  /** x component of momentum */
  virtual  double px() const;
  virtual  double px(state_t s) const;
  
  /** y component of momentum */
  virtual  double py() const;
  virtual  double py(state_t s) const;
  
  /** z component of momentum */
  virtual  double pz() const;
  virtual  double pz(state_t s) const;
  
  /** mass */
  virtual  double m() const;
  virtual  double m(state_t s) const;
  
  /** mass squared */
  virtual  double m2() const;
  virtual  double m2(state_t s) const;
  
  /** mass momentum magnitude */
  virtual  double p() const;
  virtual  double p(state_t s) const;
  
  /** square of momentum magnitude */
  virtual  double p2() const;
  virtual  double p2(state_t s) const;
  
  /** rapidity */
  virtual  double rapidity() const;
  virtual  double rapidity(state_t s) const;
  
  /** pseudo rapidity */
  virtual  double eta() const;
  virtual  double eta(state_t s) const;
  
  /** phi in [-pi,pi[ */
  virtual  double phi() const;
  virtual  double phi(state_t s) const;
  
  /** energy */
  virtual  double e() const;
  virtual  double e(state_t s) const;
  
  /** transverse energy defined to be e*sin(theta) */
  virtual  double et() const;
  virtual  double et(state_t s) const;
  
  /** transverse momentum */
  virtual  double pt() const;
  virtual  double pt(state_t s) const;
  
  /** inverse of transverse momentum */
  virtual  double iPt() const;
  virtual  double iPt(state_t s) const;
  
  /** cosinus phi */
  virtual  double cosPhi() const;
  virtual  double cosPhi(state_t s) const;
  
  /** sinus phi */
  virtual  double sinPhi() const;
  virtual  double sinPhi(state_t s) const;
  
  /** tan theta */
  virtual  double tanTh() const;
  virtual  double tanTh(state_t s) const;
  
  /** cosinus theta */
  virtual  double cosTh() const;
  virtual  double cosTh(state_t s) const;
  
  /** sinus theta */
  virtual  double sinTh() const;
  virtual  double sinTh(state_t s) const;
  
  /** cottan theta */
  virtual  double cotTh() const;
  virtual  double cotTh(state_t s) const;
  
  /** CLHEP HepLorentzVector */
  virtual  CLHEP::HepLorentzVector hlv() const;
  virtual  CLHEP::HepLorentzVector hlv(state_t s) const;
  
  virtual const I4MomentumError* errors() const;
  virtual const I4MomentumError* errors(state_t s) const;
  
  /// add Kind which tells what kind (P4XYZ) the underlying implementation has
  virtual I4Momentum::Kind kind() const;
  
  /// Print @c I4Momentum content
  virtual std::ostream& dump( std::ostream& out ) const;
  
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
  momentum_type& momentumBase( state_t s );
  momentum_type& momentumBase( );

  /** access to underlying base type (IParticle-like)
   */
  particle_type& particleBase();


  // Set functions (for I4Momentum)
  virtual  void set4Mom( const I4Momentum& p4 );
  virtual  void set4Mom( const I4Momentum * const p4 );
  virtual  void set4Mom( const CLHEP::HepLorentzVector& hlv );
  virtual  void set4Mom( const I4Momentum& p4, state_t s );
  virtual  void set4Mom( const I4Momentum * const p4, state_t s );
  virtual  void set4Mom( const CLHEP::HepLorentzVector& hlv, state_t s );

  // Set functions (for IParticle)
  virtual  void set_dataType( ParticleDataType::DataType x );
  virtual  void set_charge( ChargeType x );
  virtual  void set_pdgId ( PDG::pidType x );
  virtual  void set_origin( const VxContainer* theContainer, int index );
  virtual  void set_origin( const VxContainer* theContainer,
			    const Trk::VxCandidate * vertex );
  virtual  void set_origin( const ElementLink<VxContainer>& origin );

  // ISignalState methods

  /** @brief Retrieve current signal state */
  virtual state_t signalState() const;
  
  /** @brief check if signal state exists for current implementation*/
  virtual bool hasSignalState(state_t s) const;

  /** @brief check if we are at the passed state */
  virtual bool isAtSignalState(state_t s) const;

  ///////////////////////////////////////////////////////////////////
  // Protected accessors:
  ///////////////////////////////////////////////////////////////////
 protected:

  /** @brief set the current signal state */
  virtual bool setSignalState(state_t s);

  /** @brief reset the signal state */
  virtual void resetSignalState();

  /** access the vector of momentum */
  std::vector<I4Momentum_t>& momentumVector(){return m_mom;}

  /** access the number of state */
  int nStates(){return m_nStates;}
  void setNStates(int n){m_nStates = n;}

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  
  /// The navigable part
  INavigable_t m_nav;
  
  /// The 4-momentum part
  /// Eventually, perhaps we can use a set since
  /// not all intermediate Signal States will be used
  /// by every object
  std::vector<I4Momentum_t> m_mom;
  
  /// The particle-id part
  IParticle_t m_part;

  /// Number of Signal States
  int m_nStates;
  
  /// Current Signal State
  state_t m_currentState;

};

///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline
ParticleSigStateImpl<INavigable_t,
                     I4Momentum_t,
                     IParticle_t>::ParticleSigStateImpl(int nStates, state_t defaultState) :
		       IAthenaBarCode      ( ), // FIXME should be removed
		       INavigable          ( ), // FIXME should be removed
		       I4Momentum          ( ), // FIXME should be removed
		       INavigable4Momentum ( ), // FIXME should be removed
		       IParticle     ( ),
		       ISignalState  ( ),
		       m_nav         ( ),
		       m_mom         ( ),
		       m_part        ( ),
		       m_nStates     ( nStates ),
		       m_currentState( defaultState )
  
{
  // Need at least 1 Signal State!
  if( m_nStates < 1 )
    m_nStates = 1;
  // Re-size the 4-vector holder
  // Initialize it in the first SignalState
  m_mom.resize(nStates);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline ParticleSigStateImpl<INavigable_t,
                            I4Momentum_t,
                            IParticle_t>::ParticleSigStateImpl( const ParticleSigStateImpl& rhs ) :
			      IAthenaBarCode      ( rhs ), // FIXME should be removed
			      INavigable          ( rhs ), // FIXME should be removed
			      I4Momentum          ( rhs ), // FIXME should be removed
			      INavigable4Momentum ( rhs ), // FIXME should be removed
			      IParticle     ( rhs ),
			      ISignalState  ( rhs ),
			      m_nav         ( rhs.m_nav  ),
			      m_mom         ( rhs.m_mom  ),
			      m_part        ( rhs.m_part ),
			      m_nStates     ( rhs.m_nStates ),
			      m_currentState( rhs.m_currentState )
{}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline ParticleSigStateImpl<INavigable_t,I4Momentum_t,IParticle_t>&
ParticleSigStateImpl<INavigable_t,
                     I4Momentum_t,
                     IParticle_t>::operator=( const ParticleSigStateImpl& rhs )
{
  if ( this != &rhs ) {
    IAthenaBarCode::operator=(rhs);      // FIXME should be removed
    INavigable::operator=(rhs);          // FIXME should be removed
    I4Momentum::operator=(rhs);          // FIXME should be removed
    INavigable4Momentum::operator=(rhs); // FIXME should be removed
    IParticle::operator=( rhs );
    m_nav          = rhs.m_nav;
    m_mom          = rhs.m_mom;
    m_part         = rhs.m_part;
    m_nStates      = rhs.m_nStates;
    m_currentState = rhs.m_currentState;
  }
  return *this;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
//inline
ParticleSigStateImpl<INavigable_t,
                     I4Momentum_t,
                     IParticle_t>::~ParticleSigStateImpl()
{}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const INavigable_t& ParticleSigStateImpl<INavigable_t,
                                                I4Momentum_t,
                                                IParticle_t>::navigableBase() const
{
  return m_nav;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const I4Momentum_t& ParticleSigStateImpl<INavigable_t,
                                                I4Momentum_t,
                                                IParticle_t>::momentumBase(state_t s) const
{
  if(hasSignalState(s))
    return m_mom[int(s)];
  else
    return m_mom[0];
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const I4Momentum_t& ParticleSigStateImpl<INavigable_t,
                                                I4Momentum_t,
                                                IParticle_t>::momentumBase() const
{
  if(hasSignalState(m_currentState))
    return m_mom[int(m_currentState)];
  else
    return m_mom[0];
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const IParticle_t&  ParticleSigStateImpl<INavigable_t,
                                                I4Momentum_t,
                                                IParticle_t>::particleBase() const
{
  return m_part;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::fillToken( INavigationToken& thisToken ) const
{
  m_nav.fillToken( thisToken );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::fillToken( INavigationToken & thisToken,
                                                          const boost::any& weight ) const
{
  m_nav.fillToken( thisToken, weight );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::px(state_t s) const
{
  return momentumBase(s).px();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::py(state_t s) const
{
  return momentumBase(s).py();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::pz(state_t s) const
{
  return momentumBase(s).pz();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::m(state_t s) const
{
  return momentumBase(s).m();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::m2(state_t s) const
{
  return momentumBase(s).m2();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::p(state_t s) const
{
  return momentumBase(s).p();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::p2(state_t s) const
{
  return momentumBase(s).p2();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::rapidity(state_t s) const
{
  return momentumBase(s).rapidity();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::eta(state_t s) const
{
  return momentumBase(s).eta();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::phi(state_t s) const
{
  return momentumBase(s).phi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::e(state_t s) const
{
  return momentumBase(s).e();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::et(state_t s) const
{
  return momentumBase(s).et();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::pt(state_t s) const
{
  return momentumBase(s).pt();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::iPt(state_t s) const
{
  return momentumBase(s).iPt();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::cosPhi(state_t s) const
{
  return momentumBase(s).cosPhi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::sinPhi(state_t s) const
{
  return momentumBase(s).sinPhi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::tanTh(state_t s) const
{
  return momentumBase(s).tanTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::cosTh(state_t s) const
{
  return momentumBase(s).cosTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::sinTh(state_t s) const
{
  return momentumBase(s).sinTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::cotTh(state_t s) const
{
  return momentumBase(s).cotTh();
}


template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline CLHEP::HepLorentzVector ParticleSigStateImpl<INavigable_t,
                                                    I4Momentum_t,
                                                    IParticle_t>::hlv(state_t s) const
{
  return momentumBase(s).hlv();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const I4MomentumError* ParticleSigStateImpl<INavigable_t,
                                                   I4Momentum_t,
                                                   IParticle_t>::errors(state_t s) const
{
  return momentumBase(s).errors();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::px() const
{
  return momentumBase().px();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::py() const
{
  return momentumBase().py();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::pz() const
{
  return momentumBase().pz();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::m() const
{
  return momentumBase().m();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::m2() const
{
  return momentumBase().m2();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::p() const
{
  return momentumBase().p();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::p2() const
{
  return momentumBase().p2();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::rapidity() const
{
  return momentumBase().rapidity();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::eta() const
{
  return momentumBase().eta();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::phi() const
{
  return momentumBase().phi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::e() const
{
  return momentumBase().e();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::et() const
{
  return momentumBase().et();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::pt() const
{
  return momentumBase().pt();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::iPt() const
{
  return momentumBase().iPt();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::cosPhi() const
{
  return momentumBase().cosPhi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::sinPhi() const
{
  return momentumBase().sinPhi();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::tanTh() const
{
  return momentumBase().tanTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::cosTh() const
{
  return momentumBase().cosTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::sinTh() const
{
  return momentumBase().sinTh();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline double ParticleSigStateImpl<INavigable_t,
                                   I4Momentum_t,
                                   IParticle_t>::cotTh() const
{
  return momentumBase().cotTh();
}


template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline CLHEP::HepLorentzVector ParticleSigStateImpl<INavigable_t,
                                                    I4Momentum_t,
                                                    IParticle_t>::hlv() const
{
  return momentumBase().hlv();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const I4MomentumError* ParticleSigStateImpl<INavigable_t,
                                                   I4Momentum_t,
                                                   IParticle_t>::errors() const
{
  return momentumBase().errors();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline I4Momentum::Kind ParticleSigStateImpl<INavigable_t,
                                             I4Momentum_t,
                                             IParticle_t>::kind() const
{
  return momentumBase().kind();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline std::ostream& ParticleSigStateImpl<INavigable_t,
                                          I4Momentum_t,
                                          IParticle_t>::dump( std::ostream& out ) const
{
  momentumBase().dump ( out );
  out << "\n";
  m_part.dump( out );
  return out;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline ParticleDataType::DataType ParticleSigStateImpl<INavigable_t,
                                                       I4Momentum_t,
                                                       IParticle_t>::dataType() const
{
  return m_part.dataType();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const Trk::RecVertex* ParticleSigStateImpl<INavigable_t,
                                                  I4Momentum_t,
                                                  IParticle_t>::origin() const
{
  return m_part.origin();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline const ElementLink< VxContainer >& ParticleSigStateImpl<INavigable_t,
                                                              I4Momentum_t,
                                                              IParticle_t>::originLink() const
{
  return m_part.originLink();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::hasCharge()  const
{
  return m_part.hasCharge();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline ChargeType ParticleSigStateImpl<INavigable_t,
                                       I4Momentum_t,
                                       IParticle_t>::charge() const
{
  return m_part.charge();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::hasPdgId() const
{
  return m_part.hasPdgId();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline PDG::pidType ParticleSigStateImpl<INavigable_t,
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
inline AthenaBarCode_t ParticleSigStateImpl<INavigable_t,
                                            I4Momentum_t,
                                            IParticle_t>::getAthenaBarCode() const 
{
  return particleBase().getAthenaBarCodeImpl().getAthenaBarCode();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t> 
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::setAthenaBarCode(AthenaBarCode_t id) 
{
  particleBase().getAthenaBarCodeImpl().setAthenaBarCode(id);
}

//Comparing & Versioning Control
template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::hasSameAthenaBarCode(const IAthenaBarCode &bc) const
{
  return particleBase().getAthenaBarCodeImpl().hasSameAthenaBarCode(bc);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t, 
                                 I4Momentum_t,
                                 IParticle_t>::hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &bc) const 
{
  return particleBase().getAthenaBarCodeImpl().hasSameAthenaBarCodeExceptVersion(bc);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline AthenaBarCodeVersion_t ParticleSigStateImpl<INavigable_t,
                                                   I4Momentum_t,
                                                   IParticle_t>::getVersion() const 
{
  return particleBase().getAthenaBarCodeImpl().getVersion();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t> 
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::newVersion() 
{
  particleBase().getAthenaBarCodeImpl().newVersion();
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t, 
                                 IParticle_t>::setVersion(AthenaBarCodeVersion_t newversion) 
{
  particleBase().getAthenaBarCodeImpl().setVersion(newversion);
}

////////////////////////////////////////////////
// Finished Passing all interface of IAthenaBarCode
////////////////////////////////////////////////

// --------------------
// non-const interface
// --------------------

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline INavigable_t& ParticleSigStateImpl<INavigable_t,
                                          I4Momentum_t,
                                          IParticle_t>::navigableBase()
{
  return m_nav;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline I4Momentum_t& ParticleSigStateImpl<INavigable_t,
                                          I4Momentum_t,
                                          IParticle_t>::momentumBase(state_t s)
{
  if(hasSignalState(s))
    return m_mom[int(s)];
  else
    return m_mom[0];
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline I4Momentum_t& ParticleSigStateImpl<INavigable_t,
                                          I4Momentum_t,
                                          IParticle_t>::momentumBase()
{
  if(hasSignalState(m_currentState))
    return m_mom[int(m_currentState)];
  else
    return m_mom[0];
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline IParticle_t& ParticleSigStateImpl<INavigable_t,
                                         I4Momentum_t,
                                         IParticle_t>::particleBase()
{
  return m_part;
}


// setters

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set4Mom( const I4Momentum& p4, state_t s )
{
  return momentumBase(s).set4Mom( p4 );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set4Mom( const I4Momentum * const p4, state_t s )
{
  return momentumBase(s).set4Mom( p4 );
}


template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set4Mom( const CLHEP::HepLorentzVector& hlv, state_t s )
{
  return momentumBase(s).set4Mom( hlv );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set4Mom( const I4Momentum& p4 )
{
  return momentumBase().set4Mom( p4 );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set4Mom( const I4Momentum * const p4 )
{
  return momentumBase().set4Mom( p4 );
}


template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set4Mom( const CLHEP::HepLorentzVector& hlv )
{
  return momentumBase().set4Mom( hlv );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set_dataType( ParticleDataType::DataType x )
{
  m_part.set_dataType(x);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set_charge( ChargeType x )
{
  m_part.set_charge(x);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set_pdgId( PDG::pidType x )
{
  m_part.set_pdgId(x);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set_origin( const VxContainer* theContainer,
                                                           int index )
{
  m_part.set_origin(theContainer, index);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set_origin( const VxContainer* theContainer,
                                                           const Trk::VxCandidate * vertex )
{
  m_part.set_origin(theContainer, vertex);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::set_origin( const ElementLink<VxContainer>& origin )
{
  m_part.set_origin(origin);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
std::ostream& operator<< (std::ostream& out,
                          const ParticleSigStateImpl<INavigable_t,
                          I4Momentum_t,
                          IParticle_t>& p4);

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline std::ostream& operator<< (std::ostream& out,
                                 const ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>& p4)
{
  return p4.dump( out );
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline P4SignalState::State ParticleSigStateImpl<INavigable_t,
                                                 I4Momentum_t,
                                                 IParticle_t>::signalState() const
{
  return m_currentState;
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::setSignalState(state_t s)
{
  if(!hasSignalState(s))
    {
      return false;
    } else {
      m_currentState = s;
      return true;
    }
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::hasSignalState(state_t s) const
{
  // UNKNOWN is not an actual SignalState!
  if(int(s)<0)
    return false;
  
  return (int(s) < m_nStates);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline bool ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::isAtSignalState(state_t s) const
{
  return (s == m_currentState);
}

template< class INavigable_t, class I4Momentum_t, class IParticle_t>
inline void ParticleSigStateImpl<INavigable_t,
                                 I4Momentum_t,
                                 IParticle_t>::resetSignalState()
{
  m_currentState = state_t(0);
}



#endif //> PARTICLEEVENT_PARTICLEIMPL_H
