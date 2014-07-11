///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcParticleLink_p1.h 
// Header file for class HepMcParticleLink_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_HEPMCPARTICLELINK_P1_H 
#define GENERATOROBJECTSTPCNV_HEPMCPARTICLELINK_P1_H 

// STL includes
#include <string>

// Forward declaration
class HepMcParticleLinkCnv_p1;

class HepMcParticleLink_p1
{ 
  // Make HepMcParticleLinkCnv_p1 our friend
  friend class HepMcParticleLinkCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  HepMcParticleLink_p1();

  /// Constructor with all parameters
  HepMcParticleLink_p1( //const std::string& mcEvtName,
			const unsigned short genEvtIndex,
			const unsigned long barcode );

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// name of the @c McEventCollection holding the @c HepMC::GenEvent which
  /// itself holds the @c HepMC::GenParticle we are pointing to
  /// FIXME: suboptimal, should put that into the @c DataHeader !!
  //std::string m_mcEvtName;

  /// index of the @c HepMC::GenEvent holding the @c HepMC::GenParticle we
  /// are pointing to. This is the index in the @c McEventCollection.
  unsigned short m_mcEvtIndex;

  /// barcode of the @c HepMC::GenParticle we are pointing to.
  unsigned long m_barcode;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline HepMcParticleLink_p1::HepMcParticleLink_p1() :
  //m_mcEvtName( "Unknown" ),
  m_mcEvtIndex ( 0 ),
  m_barcode    ( 0 )
{}

inline 
HepMcParticleLink_p1::HepMcParticleLink_p1( //const std::string& mcEvtName,
					    const unsigned short genEvtIndex,
					    const unsigned long barcode ) :
  //  m_mcEvtName  ( mcEvtName   ),
  m_mcEvtIndex ( genEvtIndex ),
  m_barcode    ( barcode     )
{}

#endif //> GENERATOROBJECTSTPCNV_HEPMCPARTICLELINK_P1_H
