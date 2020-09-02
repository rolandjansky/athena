// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticle_v1.h 624338 2014-10-27 15:08:55Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHPARTICLE_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPARTICLE_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/ObjectType.h"

// Local include(s):
#include "xAODTruth/TruthVertexContainerFwd.h"

// ROOT include(s):
#include "Math/Vector4D.h"
#include "xAODTruth/TruthParticleContainerFwd.h"
template struct DataVector_detail::DVLEltBaseInit< xAOD::TruthParticle_v1>;

namespace xAOD {

   /// Class describing a truth particle in the MC record
   ///
   /// The xAOD truth record mimicks the structure of HepMC. Truth particles
   /// and truth vertices form a decay tree. Where each truth particle is
   /// connected to a single production and decay vertex. And each vertex is
   /// connected to one or more incoming, and one or more outgoing particles.
   ///
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
   ///
   /// $Revision: 624338 $
   /// $Date: 2014-10-27 16:08:55 +0100 (Mon, 27 Oct 2014) $
   ///
   class TruthParticle_v1 : public IParticle {

   public:
      /// Default constructor
      TruthParticle_v1();

      /// @name Functions identifying the particle in the MC record
      /// @{

      /// Set PDG ID code
      void setPdgId( int pid );
      /// PDG ID code
      int pdgId() const;
      /// Absolute PDG ID code (often useful)
      int absPdgId() const;

      /// Barcode
      /// @note Meaning of barcode may change -- be careful!
      int barcode() const;
      /// Set barcode
      void setBarcode( int value );

      /// Status code
      int status() const;
      /// Set status code
      void setStatus( int value );

      /// @}

      /// @name Links to the production and decay vertices
      /// @{

      /// Check for a production vertex on this particle
      bool hasProdVtx() const;
      /// The production vertex of this particle
      const TruthVertex_v1* prodVtx() const;
      /// The production vertex link of this particle
      const ElementLink< TruthVertexContainer >& prodVtxLink() const;
      /// Set the production vertex of the particle
      void setProdVtxLink( const ElementLink< TruthVertexContainer >& link );

      /// Check for a decay vertex on this particle
      bool hasDecayVtx() const;
      /// The decay vertex of this particle
      const TruthVertex_v1* decayVtx() const;
      /// The decay vertex link of this particle
      const ElementLink< TruthVertexContainer >& decayVtxLink() const;
      /// Set the decay vertex of the particle
      void setDecayVtxLink( const ElementLink< TruthVertexContainer >& link );

      /// @}

      /// @name Particle connection / origin / decay information
      /// @{

      /// Number of parents of this particle
      size_t nParents() const;

      /// Retrieve the i-th mother (TruthParticle) of this TruthParticle
      const TruthParticle_v1* parent( size_t i = 0 ) const;

      /// Number of children of this particle
      size_t nChildren() const;

      /// Retrieve the i-th mother (TruthParticle) of this TruthParticle
      const TruthParticle_v1* child( size_t i = 0 ) const;

      /// @todo Add mappings of e.g. isPrimary, isDecayed, isPrompt,
      /// isFromDecay, hasHadronicDecay, hasLeptonicDecay,
      /// isHadronic/LeptonicTau ...

      /// @todo Add isFirstWith, isLastWith

      /// @}

      /// @name Functions implementing the xAOD::IParticle interface
      /// @{

      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double eta() const;
      /// The azimuthal angle (\f$\phi\f$) of the particle
      virtual double phi() const;
      /// The mass of the particle
      ///
      /// Note, not necessarily = \f$E^2-p^2\f$ if not on mass shell.
      ///
      virtual double m() const;
      /// The total energy of the particle
      virtual double e() const;
      /// The true rapidity (\f$y\f$) of the particle
      virtual double rapidity() const;

      /// Definition of the 4-momentum type
      typedef IParticle::FourMom_t FourMom_t;

      /// The full 4-momentum of the particle
      ///
      /// Note that m may not be right, though, if not on mass shell. In which
      /// case <code>p4().M()</code> will be different from <code>m()</code>.
      ///
      virtual FourMom_t p4() const;

      /// Base 4 Momentum type for TruthParticle
      typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > GenVecFourMom_t;

      /// The full 4-momentum of the particle : GenVector form
      GenVecFourMom_t genvecP4() const;

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;

      /// @}

      /// @name 4-momentum accessors specific to truth particles
      /// @{

      /// The absolute pseudorapidity (\f$|\eta|\f$) of the particle
      /// @todo Remove this when/if supported in xAOD::IParticle
      double abseta() const;
      /// The true absolute rapidity (\f$|y|\f$) of the particle
      /// @todo Remove this when/if supported in xAOD::IParticle
      double absrapidity() const;

      /// The x component of the particle's momentum
      float px() const;
      /// Set the x component of the particle's momentum
      void setPx( float value );

      /// The y component of the particle's momentum
      float py() const;
      /// Set the y component of the particle's momentum
      void setPy( float value );

      /// The z component of the particle's momentum
      float pz() const;
      /// Set the z component of the particle's momentum
      void setPz( float value );

      /// Set the energy of the particle
      void setE( float value );

      /// Also store the mass
      void setM( float value );

      /// @}

      /// @name Particle species information
      ///
      /// Just convenience methods here, since they all just forward to the
      /// utility functions operating on the PDG particle ID code.
      ///
      /// @{

      /// Physical charge
      double charge() const;
      /// 3 x the physical charge (so it can be an int for quarks)
      int threeCharge() const;

      /// Whether the particle is electrically charged
      bool isCharged() const;
      /// Whether the particle is electrically neutral
      bool isNeutral() const;

      /// Whether the particle is a photon
      bool isPhoton() const;
      /// Whether the particle is a lepton
      bool isLepton() const;
      /// Whether the particle is a charged lepton
      bool isChLepton() const;
      /// Whether the particle is an electron (or positron)
      bool isElectron() const;
      /// Whether the particle is a muon (or antimuon)
      bool isMuon() const;
      /// Whether the particle is a tau (or antitau)
      bool isTau() const;
      /// Whether the particle is a neutrino (or antineutrino)
      bool isNeutrino() const;

      /// Whether the particle is a hadron
      bool isHadron() const;
      /// Whether the particle is a meson
      bool isMeson() const;
      /// Whether the particle is a baryon
      bool isBaryon() const;

      /// Whether the particle contains a strange quark (or antiquark)
      bool hasStrange() const;
      /// Whether the particle contains a charm quark (or antiquark)
      bool hasCharm() const;
      /// Whether the particle contains a bottom quark (or antiquark)
      bool hasBottom() const;

      /// Determine if the PID is that of a light flavour (not b or c) meson
      bool isLightMeson() const;
      /// Determine if the PID is that of a light flavour (not b or c) baryon
      bool isLightBaryon() const;
      /// Determine if the PID is that of a light flavour (not b or c) hadron
      bool isLightHadron() const;

      /// Determine if the PID is that of a heavy flavour (b or c) meson
      bool isHeavyMeson() const;
      /// Determine if the PID is that of a heavy flavour (b or c) baryon
      bool isHeavyBaryon() const;
      /// Determine if the PID is that of a heavy flavour (b or c) hadron
      bool isHeavyHadron() const;

      /// Determine if the PID is that of a b-meson.
      bool isBottomMeson() const;
      /// Determine if the PID is that of a b-baryon.
      bool isBottomBaryon() const;
      /// Determine if the PID is that of a b-hadron.
      bool isBottomHadron() const;

      /// @brief Determine if the PID is that of a c-meson.
      ///
      /// Specifically, the _heaviest_ quark is a c: a B_c is a b-meson and NOT
      /// a c-meson. Charmonia (closed charm) are counted as c-mesons here.
      ///
      bool isCharmMeson() const;
      /// @brief Determine if the PID is that of a c-baryon.
      ///
      /// Specifically, the _heaviest_ quark is a c: a baryon containing a b & c
      /// is a b-baryon and NOT a c-baryon. To test for the simpler case, just
      /// use a combination of hasCharm() and isBaryon().
      ///
      bool isCharmBaryon() const;
      /// Determine if the PID is that of a c-hadron.
      bool isCharmHadron() const;

      /// Determine if the PID is that of a strange meson
      bool isStrangeMeson() const;
      /// Determine if the PID is that of a strange baryon
      bool isStrangeBaryon() const;
      /// Determine if the PID is that of a strange hadron
      bool isStrangeHadron() const;

      /// Check if this particle is a quark
      bool isQuark() const;
      /// Check if this particle is a parton
      bool isParton() const;
      /// Check if this particle is a top quark
      bool isTop() const;
      /// Check if this particle is a W boson
      bool isW() const;
      /// Check if this particle is a Z boson
      bool isZ() const;
      /// Check if this particle is a Higgs boson
      bool isHiggs() const;
      /// Check if this particle is a resonant state
      bool isResonance() const;
      /// Check if this is a generator specific (non-physical) particle
      bool isGenSpecific() const;
      /// Check if this is a BSM particle
      bool isBSM() const;

      /// @}

      /// @name Polarization properties (optional)
      /// @{

      /// Polarization parameter types
      enum PolParam {
         polarizationPhi   = 0, ///< Polarization in (\f$\phi\f$)
         polarizationTheta = 1  ///< Polarization in (\f$\theta\f$)
      };

      /// Accessor for polarization parameters
      ///
      /// Generic C++ code should use this function to retrieve the polarization
      /// parameters of a truth particle. Since these parameters are optional,
      /// the function is designed to tell the user whether the requested
      /// parameter could be retrieved or not.
      ///
      /// @param value The polarization parameter value read from the object
      /// @param parameter The polarization parameter that we are interested in
      /// @return <code>true</code> if the retrieval was successful,
      ///         <code>false</code> if it wasn't
      ///
      bool polarizationParameter( float& value, PolParam parameter ) const;

      /// Set method for polarization parameter values
      ///
      /// In order to keep the symmetry with the getter function, this setter
      /// communicates a possible failure in its operation through a return
      /// value. Setting a polariozation parameter is much less likely to fail
      /// than retrieving one, but in some situations it may still happen...
      ///
      /// @param value The polarization parameter value to set on the object
      /// @param parameter The polarization parameter type that we want to set
      /// @return <code>true</code> if the operation was successful, or
      ///         <code>false</code> if it wasn't
      ///
      bool setPolarizationParameter( float value, PolParam parameter );

      /// Convenience accessor for a polariozation parameter.
      ///
      /// Throws an exception if the request can't be completed. It should
      /// mostly be used in PyROOT, as the other form of this function is very
      /// hard to use there. But one needs to know for sure that the requested
      /// parameter will exist.
      ///
      /// @param parameter The polarion parameter that we are interested in
      /// @return The value of the polarization parameter requested
      ///
      float polarizationParameter( PolParam parameter ) const;

      /// Single container for full polarization information
      ///
      /// It can be used as a convenient way for accessing the polarization of
      /// the particle with a single call.
      ///
      struct Polarization {

         /// Constructor to set (invalid) defaults
         /// @todo Use C++11 inline member init when allowed
         Polarization()
         : phi( -1.0 ), theta( -1.0 ) {}

         /// Check if the stored values are valid
         bool valid() const {
            return ( ( phi > 0.0 ) && ( theta > 0.0 ) );
         }

         float phi;   ///< Polarization in (\f$\phi\f$)
         float theta; ///< Polarization in (\f$\theta\f$)

      }; // struct Polarization

      /// Retrieve a full Polarization with a single call
      ///
      /// @note May have invalid values -- use valid() to check.
      ///
      /// @return An object holding the full polarization information
      ///
      Polarization polarization() const;

      /// @}

      /// Function making sure that the object is ready for persistification
      void toPersistent();

   }; // class TruthParticle_v1

} // namespace xAOD

#endif // XAODTRUTH_VERSIONS_TRUTHPARTICLE_V1_H
