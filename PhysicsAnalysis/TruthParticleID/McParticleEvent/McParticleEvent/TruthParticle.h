///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticle.h 
// Header file for class TruthParticle
// Author  : Frank Paige Created : Jan 2004
// Author  : Ketevi A. Assamagan Updated : May 2004
// Author  : Sebastien Binet Updated : Jan 2005
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHPARTICLE_H 
#define MCPARTICLEEVENT_TRUTHPARTICLE_H 

/** @class TruthParticle holds a (HepMC) Monte Carlo particle. 
 *
 * Adds isolation information.
 * Adapts the @c HepMC::GenParticle to the @c IParticle interface.
 *
 */

// STL includes
#include <vector>

// HepMC / CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/Polarization.h"
#include "AtlasHepMC/Flow.h"

// Gaudi includes

// EventKernel includes
#include "EventKernel/IParticle.h"

// FourMom includes

// ParticleEvent includes
#include "ParticleEvent/ParticleImpl.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleParamDefs.h"
#include "McParticleEvent/TruthParticleNavigation.h"
#include "McParticleEvent/TruthParticleMomentum.h"
#include "McParticleEvent/TruthParticleBase.h"
#include "McParticleEvent/TruthEtIsolations.h"

// Forward declaration
namespace Trk   { class RecVertex; }
class TruthParticleContainer;

class TruthParticle : public ParticleImpl< 
                                TruthParticleNavigation,
		                TruthParticleMomentum,
		                TruthParticleBase
		             >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // for readability and lazy people
  typedef ParticleImpl< TruthParticleNavigation,
			TruthParticleMomentum,
			TruthParticleBase
	  > TruthParticleImpl_t;

  typedef std::size_t evtIndex_t;
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor
   */
  TruthParticle();
  
  /** Copy constructor
   */
  TruthParticle( const TruthParticle& rhs );
  
  /** Constructor with parameters: 
   *  construct a @c TruthParticle from a @c HepMC::GenParticle and
   *  the @c TruthParticleContainer holding this @c TruthParticle
   */
  TruthParticle( const HepMC::GenParticle* genParticle, 
		 const TruthParticleContainer * container = 0 );

  /** Assignment operator
   */
  TruthParticle& operator=( const TruthParticle& rhs );

  /** Default constructor: 
   */
  ~TruthParticle();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

//   /** @{ INavigable interface implementation
//    */
//   void fillToken( INavigationToken & thisToken ) const;
//   void fillToken( INavigationToken & thisToken, const boost::any& ) const;
//   /** @}
//    */

  /// Retrieve the GenParticle mother of this TruthParticle
  const HepMC::GenParticle* genMother(const std::size_t i=0) const;

  /// Retrieve the GenParticle this TruthParticle has been made from (if any)
  const HepMC::GenParticle* genParticle() const;

  /// Retrieve the i-th child (GenParticle) of this TruthParticle
  const HepMC::GenParticle* genChild( const std::size_t i ) const;

  /** @{ HepMC::GenParticle forwarding interface
   */
  
  /** Forwards the HepMC::GenParticle::status() information
   */
  int status() const;

  /** Forwards the HepMC::GenParticle::flow() information
   */
  const HepMC::Flow flow() const;

  /** Forwards the HepMC::GenParticle::flow(idx) information
   */
  int flow( int code_index ) const;

  /** Forwards the HepMC::GenParticle::polarization() information
   */
  const HepMC::Polarization polarization() const;

  /** Forwards the HepMC::GenParticle::barcode() information
   */
  int barcode() const;

  /** Forwards the HepMC::GenParticle::barcode() information
   */
  long compressedExtBarCode() const { return 0; };
  /** @}
   */

  /// Retrieve the i-th mother (TruthParticle) of this TruthParticle
  const TruthParticle * mother(const std::size_t i=0) const;

  /// Retrieve the i-th child (TruthParticle) of this TruthParticle
  const TruthParticle * child( const std::size_t i ) const;

  /** Return true if the given PDG::id to match can be found within the 
   *  parents of this TruthParticle.
   */
  bool hasMother( const PDG::pidType pdgId ) const;

  /** Return true if the given PDG::id to match can be found within the 
   *  children of this TruthParticle.
   */
  bool hasChild( const PDG::pidType pdgId ) const;

  /** Return true if the given PDG::id to match can be found within the 
   *  parents of this TruthParticle. It also fills the given vector with 
   *  the indices toward the matching particles.
   */
  bool hasMother( const PDG::pidType pdgId, std::vector<unsigned int>& indices ) const;

  /** Return true if the given PDG::id to match can be found within the 
   *  children of this TruthParticle. It also fills the given vector with 
   *  the indices toward the matching particles.
   */
  bool hasChild( const PDG::pidType pdgId, std::vector<unsigned int>& indices ) const;

  // Accessor functions
  
  /** Return true if one has filled the Et isolation infos for 
   *  this @c TruthParticle.
   */
  bool hasEtIsol() const;

  /** Return the transverse energy in a given cone size of isolation.
   *  Note that the size of the code must be one of the encoded sizes
   *  (see TruthParticleParamDefs.h file) and that the index is checked for
   *  out of bounds.
   */
  double etIsol( const TruthParticleParameters::ConeSize coneIdx ) const;

  /** Return the transverse energies for all the isolation cones
   */
  const TruthEtIsolations::EtIsol_t* etIsol() const;

  /// Number of parents of this particle
  unsigned int nParents() const;
  
  /// Number of children of this particle
  unsigned int nDecay() const;

  /// returns a Pointer to a CLHEP::LorentzVector from a HepMC::FourVector 
  CLHEP::HepLorentzVector transformVector( const HepMC::FourVector&  ) const;
  
  /// Return the 4-vec of i-th child of this particle
  CLHEP::HepLorentzVector pDecay( const std::size_t i ) const;

  /// Return the PDG-Id of the i-th child of this particle
  PDG::pidType pdgDecay( const std::size_t i ) const;

  /// conversion operator: convert a @c TruthParticle to its underlying
  /// @c HepMC::GenParticle
  operator const HepMC::GenParticle&() const;

  evtIndex_t genEventIndex() const {return m_nGenEventIdx;}

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  // Set functions

  /// Set the charge of this TruthParticle
  void setCharge( const ChargeType charge );
  
  void set4Mom( const HepMC::FourVector& hlv ) 
  { this->momentumBase().set4Mom(hlv); }

  // un-hide base-class' set4Mom
  using TruthParticleImpl_t::set4Mom;

  /// set the GenEvent index
  void setGenEventIndex( size_t index );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
  /// Fill the data members of ParticleBase from the GenParticle
  void setGenParticle( const HepMC::GenParticle* particle );



  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Pointer to the collection holding ourself.
   */
  const TruthParticleContainer* m_container;

  /** vector of barcodes of all the mothers of this particle.
   *  This will be forwarded to the container holding ourself to
   *  be able to retrieve TruthParticle pointers.
   */
  std::vector<int> m_mothers;

  /** vector of barcodes of all the children of this particle.
   *  This will be forwarded to the container holding ourself to
   *  be able to retrieve TruthParticle pointers.
   */
  std::vector<int> m_children;

  /** the index of the GenEvent this particle is in */
  evtIndex_t m_nGenEventIdx;

}; 

/////////////////////////////////////////////////////////////////// 
// I/O functions
/////////////////////////////////////////////////////////////////// 
std::ostream& operator<<( std::ostream& out, const TruthParticle& mc );

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthParticle::~TruthParticle() 
{}

inline const HepMC::GenParticle* TruthParticle::genParticle() const
{
  return this->particleBase().genParticle();
}

inline int TruthParticle::status() const
{
  return genParticle()->status();
}

inline const HepMC::Flow TruthParticle::flow() const
{
  return HepMC::flow(genParticle());
}

inline int TruthParticle::flow( int code_index ) const
{
  return HepMC::flow(genParticle(),code_index);
}

inline const HepMC::Polarization TruthParticle::polarization() const
{
  return HepMC::polarization(genParticle());
}

inline int TruthParticle::barcode() const
{
  return HepMC::barcode(genParticle());
}

inline unsigned int TruthParticle::nParents() const 
{ return m_mothers.size(); }
  
inline unsigned int TruthParticle::nDecay() const 
{ return m_children.size(); }

inline
TruthParticle::operator const HepMC::GenParticle&() const
{
  return *genParticle();
}

inline void TruthParticle::setCharge( const ChargeType charge )
{
  this->particleBase().setCharge( charge );
}

inline CLHEP::HepLorentzVector TruthParticle::transformVector( const HepMC::FourVector& fv ) const
{
    CLHEP::HepLorentzVector hlv(fv.x(),fv.y(),fv.z(),fv.t());
    return hlv;
}  

inline void TruthParticle::setGenEventIndex( size_t index ){
  m_nGenEventIdx = index;
}

  
#endif //> MCPARTICLEEVENT_TRUTHPARTICLE_H
