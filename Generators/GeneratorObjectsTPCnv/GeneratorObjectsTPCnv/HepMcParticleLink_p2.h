///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTSTPCNV_HEPMCPARTICLELINK_P2_H
#define GENERATOROBJECTSTPCNV_HEPMCPARTICLELINK_P2_H

// STL includes
#include <string>

// Forward declaration
class HepMcParticleLinkCnv_p2;

class HepMcParticleLink_p2
{
  // Make HepMcParticleLinkCnv_p2 our friend
  friend class HepMcParticleLinkCnv_p2;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  /** Default constructor:
   */
  HepMcParticleLink_p2();

  /// Constructor with all parameters
  HepMcParticleLink_p2( //const std::string& mcEvtName,
                       const unsigned short genEvtIndex,
                       const unsigned long barcode,
                       char evtColl );

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

  // identifier for the type of McEventCollection containing the particle (HS, PU,...)
  char m_evtColl;
};

///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////

inline HepMcParticleLink_p2::HepMcParticleLink_p2() :
  //m_mcEvtName( "Unknown" ),
  m_mcEvtIndex ( 0 ),
  m_barcode    ( 0 ),
  m_evtColl    ('a')
{}

inline
HepMcParticleLink_p2::HepMcParticleLink_p2( //const std::string& mcEvtName,
                                           const unsigned short genEvtIndex,
                                           const unsigned long barcode,
                                           char evtColl) :
  //  m_mcEvtName  ( mcEvtName   ),
  m_mcEvtIndex ( genEvtIndex ),
  m_barcode    ( barcode     ),
  m_evtColl    ( evtColl     )
{}

#endif //> GENERATOROBJECTSTPCNV_HEPMCPARTICLELINK_P2_H
