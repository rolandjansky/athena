// Dear emacs, this is -*- c++ -*-

/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODBASE_OBJECTTYPE_H
#define XAODBASE_OBJECTTYPE_H

// System include(s):
#include <iosfwd>

namespace xAODType {

/// Type of objects that have a representation in the xAOD EDM
///
/// xAOD classes identify themselves by all of them providing a function
/// with the signature:
///
///  <code>
///  xAOD::Type::ObjectType type() const;
///  </code>
///
/// This can be used to easily identify what sort of object some generic
/// code is dealing with, avoiding doing a lot of
/// <code>dynamic_cast</code>-s instead.
///
/// Note that Doxygen doesn't allow to group enumeration variables
/// together like it does for members of a class, that's why the grouping
/// comments are not created according to the Doxygen rules.
///
enum ObjectType {

  Other = 0, ///< An object not falling into any of the other categories

  // Reconstructed particle types
  // {

  CaloCluster  = 1, ///< The object is a calorimeter cluster
  Jet          = 2, ///< The object is a jet
  ParticleFlow = 3, ///< The object is a particle-flow object

  TrackParticle   = 4, ///< The object is a charged track particle
  NeutralParticle = 5, ///< The object is a neutral particle

  Electron = 6, ///< The object is an electron
  Photon   = 7, ///< The object is a photon
  Muon     = 8, ///< The object is a muon
  Tau      = 9, ///< The object is a tau (jet)

  TrackCaloCluster = 10, ///< The object is a track-calo-cluster

  // }

  // Reconstructed non-particle types
  // {

  Vertex = 101, ///< The object is a vertex
  BTag   = 102, ///< The object is a b-tagging object

  // }

  // Truth types
  // {

  TruthParticle    = 201, ///< The object is a truth particle
  TruthVertex      = 202, ///< The object is a truth vertex
  TruthEvent       = 203, ///< The object is a truth event
  TruthPileupEvent = 204, ///< The object is a truth pileup event

  // }

  // Trigger types
  // {

  L2StandAloneMuon = 501, ///< The object is a stand-alone LVL2 muon
  L2IsoMuon        = 502, ///< The object is a LVL2 isolated muon
  L2CombinedMuon   = 503, ///< The object is a combined LVL2 muon

  TrigElectron     = 504, ///< The object is a LVL2 electron
  TrigPhoton       = 505, ///< The object is a LVL2 photon
  TrigCaloCluster  = 506, ///< The object is a LVL2 calorimeter cluster
  TrigEMCluster    = 507, ///< The object is a LVL2 EM cluster

  // }

  // Auxiliary types
  // {

  EventInfo   = 1001, ///< The object is an event information one
  EventFormat = 1002, ///< The object is an event format one

  Particle          = 1101, ///< Generic particle object, for analysis
  CompositeParticle = 1102  ///< Particle composed of other particles

    // }

}; // enum ObjectType

struct ROOT6_NamespaceAutoloadHook{};

} // namespace xAODType

namespace xAOD{
  /* 
   * This is deemed more conveninet for the clients
   * i.e 
   * xAOD::Type::ObjectType
   * so for compatibility with the above we alias 
   * xAODType to be just Type
   */
   namespace Type=xAODType;
}
 
/// Convenience operator for printing the object type in a (debug) message
std::ostream& operator<< ( std::ostream& out, xAOD::Type::ObjectType type );

 

#endif // XAODBASE_OBJECTTYPE_H
