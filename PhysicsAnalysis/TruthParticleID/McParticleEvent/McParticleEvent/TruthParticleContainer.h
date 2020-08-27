///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainer.h 
// Header file for class TruthParticleContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHPARTICLECONTAINER_H 
#define MCPARTICLEEVENT_TRUTHPARTICLECONTAINER_H 

// STL includes
#include <iosfwd>
#include <unordered_map>

#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"

#include "AthenaKernel/CLASS_DEF.h"

// GeneratorObjects includes
#include "GeneratorObjects/McEventCollection.h"

// NavFourMom includes
#include "NavFourMom/IParticleContainer.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleParamDefs.h"
#include "McParticleEvent/TruthEtIsolationsContainer.h"

#include "AtlasHepMC/GenEvent.h"
// Forward declaration

// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<TP> derives from DV<IP>
DATAVECTOR_BASE( TruthParticle, IParticle );

class TruthParticleContainer : public DataVector<TruthParticle>
{ 

  friend class TruthParticleCnvTool;
  /////////////////////////////////////////////////////////////////// 
  // Public members: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  // NB: This is an unsigned type, and thus represents the maximum
  // size that the allocator can hold. (modeled after std::string::npos)
  static const std::size_t NoIdx = static_cast<std::size_t>(-1);

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public:

  /// barcode to TruthParticle dictionary
  typedef std::unordered_map<long,const TruthParticle*> Map_t;
  /// 
  typedef TruthParticle::evtIndex_t          evtIndex_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainer();

  /** Copy constructor: compiler generated one is good enough
   */
  //TruthParticleContainer( const TruthParticleContainer& rhs );

  /** Assignment operator: compiler generated one is good enough
   */
  //TruthParticleContainer& operator=( const TruthParticleContainer& rhs ); 

  /** Constructor with parameters: to setup the ownership policy
   */
  TruthParticleContainer( const SG::OwnershipPolicy own );

  /** Destructor: 
   */
  ~TruthParticleContainer(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Dump the content of the TruthParticleContainer
   */
  std::ostream& dump( std::ostream& out = std::cout ) const;

  /** Retrieve the @c HepMC::GenEvent this @c TruthParticleContainer
   *  is proxying
   */
  const HepMC::GenEvent* genEvent() const;

  /** Retrieve the @c HepMC::GenEvent this @c TruthParticleContainer
   *  is proxying, as a link.
   */
  const ElementLink<McEventCollection>& genEventLink() const;

  /** Retrieve the name (@c StoreGate location) of the 
   *  @c HepMC::GenEvent this @c TruthParticleContainer is proxying
   */
  const std::string& genEventName() const;

  /** Retrieve the index (within a @c McEventCollection) of the 
   *  @c HepMC::GenEvent this @c TruthParticleContainer is proxying
   */
  std::size_t genEventIdx() const;

  /** return a const pointer to an @c TruthParticle given the
   *  barcode of the @c HepMC::GenParticle it is wrapping
   */
  const TruthParticle* truthParticle( const int barcode, std::size_t genEventIdx = 0  ) const;

  /** return the index to an @c TruthParticle given the
   *  barcode of the @c HepMC::GenParticle it is wrapping
   *  return TruthParticleContainer::NoIdx if no such @c TruthParticle exists
   */
  std::size_t truthParticleIdx( const int barcode, std::size_t genEventIdx = 0  ) const;

  /**
   * tell if this @c TruthParticleContainer has been registered with a
   * @c TruthEtIsolations container
   */
  bool hasEtIsol() const;
  
  /**
   * tell if a @c TruthParticle (or a @c HepMC::GenParticle) has Et isolations
   * data associated to it.
   */
  bool hasEtIsol( const int barcode ) const;
  
  /** 
   * return the container of Truth Et isolations for a given @c TruthParticle
   * (or a @c HepMC::GenParticle)
   */
  const TruthEtIsolations::EtIsol_t* etIsolations( const int barcode ) const;

  /** 
   * return the link to truth Et isolations.
   */
  const ElementLink<TruthEtIsolationsContainer>& etIsolationsLink() const;

  /**
   * Return the Et isolation for a given particle and a given cone size
   */
  double etIsol( const int barcode,
		 const TruthParticleParameters::ConeSize coneIdx ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Setup the persistent pointer toward the @c HepMC::GenEvent
   *  this @c TruthParticleContainer is proxying.
   */
  void setGenEvent( const ElementLink<McEventCollection>& genEvent ); 

  /** Setup the persistent pointer toward the @c HepMC::GenEvent
   *  this @c TruthParticleContainer is proxying.
   */
  void setGenEvent( const McEventCollection* mcColl, 
		    const std::size_t genEventIdx,
                    IProxyDict* sg = nullptr); 

  /** Setup the persistent pointer toward the @c TruthEtIsolations
   */
  void setEtIsolations( const ElementLink<TruthEtIsolationsContainer>& etIsolations ); 

  /** Setup the persistent pointer toward the @c TruthEtIsolations
   */
  void setEtIsolations( const TruthEtIsolationsContainer* etIsolations,
			const std::size_t etIsolationsIdx,
                        IProxyDict* sg = nullptr);

  /** Setup the dictionary of barcodes-to-TruthParticle
   */
  void setParticles( const Map_t& parts );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Persistent pointer to the @c HepMC::GenEvent we are proxying.
   */
  ElementLink<McEventCollection> m_genEvent;

  /** Persistent pointer to the @c TruthEtIsolations container, holding
   *  Et isolations for @c TruthParticle or @c HepMC::GenParticle.
   *  Note that a @c TruthParticleContainer is not required to always have such
   *  a valid link.
   */
  ElementLink<TruthEtIsolationsContainer> m_etIsolations;

  /** Dictionary to ease the extraction of a @c TruthParticle (contained
   *  by this collection) from the @c HepMC::GenParticle barcode
   */
  Map_t m_particles;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

// These functions are really not needed inside the dictionary...
#ifndef __REFLEX__

inline 
TruthParticleContainer::~TruthParticleContainer()
{}

inline 
std::ostream& TruthParticleContainer::dump( std::ostream& out ) const 
{
  if ( m_genEvent.isValid() ) {
    HepMC::Print::line(out,**m_genEvent);
  }
  return out;
}

inline 
const HepMC::GenEvent* TruthParticleContainer::genEvent() const
{
  return m_genEvent.isValid() ? (*m_genEvent) : 0;
}

inline 
const ElementLink<McEventCollection>&
TruthParticleContainer::genEventLink() const
{
  return m_genEvent;
}

inline 
std::size_t TruthParticleContainer::genEventIdx() const
{
  return m_genEvent.index();
}

inline 
const std::string& TruthParticleContainer::genEventName() const
{
  return m_genEvent.dataID();
}

inline 
bool TruthParticleContainer::hasEtIsol() const
{
  return m_etIsolations.isValid();
}

inline 
bool TruthParticleContainer::hasEtIsol( const int barcode ) const
{
  return hasEtIsol() && (*m_etIsolations)->hasEtIsol( barcode );
}
 
inline const TruthEtIsolations::EtIsol_t*
TruthParticleContainer::etIsolations( const int barcode ) const
{
  return hasEtIsol( barcode ) 
    ? (*m_etIsolations)->etIsolations(barcode) 
    : 0;
}

inline const ElementLink<TruthEtIsolationsContainer>&
TruthParticleContainer::etIsolationsLink() const
{
  return m_etIsolations;
}

inline double 
TruthParticleContainer::etIsol( const int barcode,
				const TruthParticleParameters::ConeSize idx ) const
{
  return hasEtIsol( barcode ) 
    ? (*m_etIsolations)->etIsol(barcode, idx) 
    : -999.;
}

inline void 
TruthParticleContainer::setGenEvent( const ElementLink<McEventCollection>& genEvent ) 
{
  m_genEvent = genEvent;
  m_particles.clear();
}

inline void
TruthParticleContainer::setGenEvent( const McEventCollection* mcColl,
				     const std::size_t genEvtIdx,
                                     IProxyDict* sg /*= nullptr*/)
{
  if ( 0 != mcColl && mcColl->size() > genEvtIdx ) {
    m_genEvent.toIndexedElement( *mcColl, genEvtIdx, sg );
  }
}

inline void
TruthParticleContainer::setEtIsolations( const ElementLink<TruthEtIsolationsContainer>& etIsolations )
{
  m_etIsolations = etIsolations;
}

inline void
TruthParticleContainer::setEtIsolations( const TruthEtIsolationsContainer* etIsolations,
					 const std::size_t etIsolationsIdx,
                                         IProxyDict* sg /*= nullptr*/)
{
  if ( 0 != etIsolations && etIsolations->size() > etIsolationsIdx ) {
    m_etIsolations.toIndexedElement( *etIsolations, etIsolationsIdx, sg );
  }
}

inline void
TruthParticleContainer::setParticles( const TruthParticleContainer::Map_t& particles )
{
  m_particles = particles;
}

#endif // not __REFLEX__

CLASS_DEF(TruthParticleContainer, 77883132, 1)

// Tell StoreGate that TruthParticleContainer derives from DV<TP>
SG_BASE(TruthParticleContainer, DataVector<TruthParticle>);

#endif //> MCPARTICLEEVENT_TRUTHPARTICLECONTAINER_H
