///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolations.h 
// Header file for class TruthEtIsolations
// Author  : Sebastien Binet : April 2007
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHETISOLATIONS_H 
#define MCPARTICLEEVENT_TRUTHETISOLATIONS_H 

/** @class TruthEtIsolations is a container which holds isolation informations
 * for a given @c HepMC::GenParticle (labelled by barcode) for a given 
 * @c HepMC::GenEvent of a given @c McEventCollection
 */

// STL includes
#include <vector>
#include <iosfwd>
#include <string>
#include <stdexcept>
#include <unordered_map>

// Boost includes
#include "boost/array.hpp"

// Gaudi includes

#include "AthLinks/ElementLink.h"

// GeneratorObjects includes
#include "GeneratorObjects/McEventCollection.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleParamDefs.h"

// Forward declaration
class McEventCollection;

class TruthEtIsolations
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// A (persistent) link to a @c HepMC::GenEvent
  typedef ElementLink<McEventCollection> GenEventLink_t;

  /// An array of doubles of fixed size to modelize the Et isolations
  /// for different values of isolation radii' cones.
  typedef McAod::EtIsolations EtIsol_t;

  /// The map of barcode-to-Et isolations
  typedef std::unordered_map<int,EtIsol_t> EtIsolMap_t;

  /// The iterator over the map of barcode-to-Et-isolations
  typedef EtIsolMap_t::const_iterator const_iterator;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor
   */
  TruthEtIsolations();
  
  /** Copy constructor
   */
  TruthEtIsolations( const TruthEtIsolations& rhs );
  
  /** Constructor with parameters: 
   *  construct a @c TruthEtIsolations container from an @c ElementLink to
   *  a @c HepMC::GenEvent (hold by a @c McEventCollection)
   *  Note that no Et isolations are computed at that time.
   */
  TruthEtIsolations( const GenEventLink_t& genEvtLink );

  /** Constructor with parameters: 
   *  construct a @c TruthEtIsolations container from a @c StoreGate key to a
   *  @c McEventCollection and an index @c std::size_t to a @c HepMC::GenEvent.
   *  Note that no Et isolations are computed at that time.
   */
  TruthEtIsolations( const std::string& genEvtName,
		     const std::size_t  genEvtIdx );

  /** Assignment operator
   */
  TruthEtIsolations& operator=( const TruthEtIsolations& rhs );

  /** Default destructor: 
   */
  ~TruthEtIsolations();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /**
   * Return the name (ie: the @c StoreGate key) of the @c McEventCollection 
   * holding the @c HepMC::GenEvent
   * from which the Et isolations have been computed.
   */
  const std::string& genEventName() const;

  /**
   * Return the index in the @c McEventCollection container holding the 
   * @c HepMC::GenEvent from which the Et isolations have been computed.
   */
  std::size_t genEventIdx() const;

  /**
   * Return the number of @c HepMC::GenParticle for which Et isolations have
   * been stored into this @c TruthEtIsolations container.
   */
  std::size_t size() const;

  /** 
   * Return true if this @c TruthEtIsolations container holds Et isolations
   * for that @c HepMC::GenParticle.
   */
  bool hasEtIsol( HepMC::ConstGenParticlePtr genParticle ) const;

  /** 
   * Return true if this @c TruthEtIsolations container holds Et isolations
   * for that @c HepMC::GenParticle.
   */
  bool hasEtIsol( const int genParticleBarcode ) const;

  /** Return the transverse energies for all cone sizes of isolation, for
   *  a given @c HepMC::GenParticle's barcode
   */
  const EtIsol_t* etIsolations( HepMC::ConstGenParticlePtr genParticle ) const;

  /** Return the transverse energies for all cone sizes of isolation, for
   *  a given @c HepMC::GenParticle's barcode
   */
  const EtIsol_t* etIsolations( const int genParticleBarcode ) const;

  /** Return the transverse energy in a given cone size of isolation, for
   *  a given @c HepMC::GenParticle.
   *  Note that the size of the code must be one of the encoded sizes
   *  (see TruthEtIsolationsParamDefs.h file) and that the index is checked for
   *  out of bounds.
   */
  double etIsol( HepMC::ConstGenParticlePtr genParticle,
		 const TruthParticleParameters::ConeSize coneIdx ) const;

  /** Return the transverse energy in a given cone size of isolation, for
   *  a given @c HepMC::GenParticle's barcode
   *  Note that the size of the code must be one of the encoded sizes
   *  (see TruthEtIsolationsParamDefs.h file) and that the index is checked for
   *  out of bounds.
   */
  double etIsol( const int genParticleBarcode,
		 const TruthParticleParameters::ConeSize coneIdx ) const;

  /** Const iterator over the barcodes of the @c HepMC::GenParticle for which
   *  we have Et isolations computed
   */
  const_iterator begin() const;

  /** Const iterator over the barcodes of the @c HepMC::GenParticle for which
   *  we have Et isolations computed
   */
  const_iterator end() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  // Set functions

  /** Set the transverse energy isolation of a @c HepMC::GenParticle
   *  for a given Cone size
   */
  void setEtIsol( HepMC::ConstGenParticlePtr genParticle,
		  const TruthParticleParameters::ConeSize coneIdx,
                  const double etIsol );

  /** Set the transverse energy isolations for all the Cone sizes
   */
  void setEtIsol( HepMC::ConstGenParticlePtr genParticle,
		  const std::vector<double>& etIsols );
  
  /** Set the transverse energy isolations for all the Cone sizes
   */
  void setEtIsol( HepMC::ConstGenParticlePtr genParticle,
		  const EtIsol_t& etIsols );
  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:
  friend class TruthEtIsolationsCnv_p1;

  /// @c ElementLink to the @c HepMC::GenEvent from which the Et isolations
  /// have been (or will be) computed
  GenEventLink_t m_genEvtLink;

  /// Holds transverse energy isolations for different isolation cones
  /// The key of this map is the barcode of the @c HepMC::GenParticle
  /// for which the Et isolations have been computed
  EtIsolMap_t m_etIsolations;

  /// The default Et isolation energy which is being returned when
  /// I don't know what to return.
  /// That's ugly but for backward compatibility's sake... Next EDM :P
  static EtIsol_t m_dfltEtIsols;
}; 

/////////////////////////////////////////////////////////////////// 
// I/O functions
/////////////////////////////////////////////////////////////////// 
std::ostream& 
operator<<( std::ostream& out, const TruthEtIsolations& etIsols );

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline std::size_t TruthEtIsolations::size() const
{ return m_etIsolations.size(); }

inline bool 
TruthEtIsolations::hasEtIsol( const int bc ) const
{
  return m_etIsolations.end() != m_etIsolations.find( bc );
}

inline const TruthEtIsolations::EtIsol_t* 
TruthEtIsolations::etIsolations( const int bc ) const
{
  const EtIsolMap_t::const_iterator i = m_etIsolations.find( bc );
  return ( m_etIsolations.end() == i )
    ? 0
    : &i->second;
}
  
inline
double 
TruthEtIsolations::etIsol( const int bc,
			   const TruthParticleParameters::ConeSize idx ) const
{
  return (*etIsolations(bc))[idx];
}

inline TruthEtIsolations::const_iterator TruthEtIsolations::begin() const
{ return m_etIsolations.begin(); }

inline TruthEtIsolations::const_iterator TruthEtIsolations::end() const
{ return m_etIsolations.end(); }


#endif //> MCPARTICLEEVENT_TRUTHETISOLATIONS_H
