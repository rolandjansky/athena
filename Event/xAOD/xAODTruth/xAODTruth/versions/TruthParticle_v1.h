// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPARTICLE_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPARTICLE_V1_H

#include "xAODBase/IParticle.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/ObjectType.h"

#include "xAODTruth/TruthVertexContainerFwd.h"

#include "TruthUtils/PIDUtils.h"

namespace xAOD {


  /// Class describing a truth particle in the MC record
  ///
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
  ///
  /// $Revision: 614100 $
  /// $Date: 2014-08-29 16:01:25 +0200 (Fri, 29 Aug 2014) $
  ///
  class TruthParticle_v1 : public IParticle {
  public:

    /// Default constructor
    TruthParticle_v1();


    /// @name Functions identifying the particle in the MC record
    //@{

    /// Set PDG ID code
    void setPID(int pid) { setPdgId(pid); }
    /// Ugly alias
    void setPdgId(int pid);
    /// PDG ID code
    int pid() const { return pdgId(); }
    /// Ugly alias
    int pdgId() const;
    /// Absolute PDG ID code (often useful)
    int abspid() const { return abs(pid()); }
    /// Ugly alias
    int absPdgId() const { return abspid(); }

    /// Set barcode
    void setBarcode(int);
    /// Barcode
    /// @note Meaning of barcode may change -- be careful!
    int barcode() const;

    /// Set status code
    void setStatus(int);
    /// Status code
    int status() const;

    //@}


    /// @name Links to the production and decay vertices
    //@{

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

    //@}


    /// @name Functions implementing the xAOD::IParticle interface
    //@{

    /// The transverse momentum (\f$p_T\f$) of the particle
    double pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle
    double eta() const;
    /// The absolute pseudorapidity (\f$|\eta|\f$) of the particle
    /// @todo Remove this when/if supported in xAOD::IParticle
    double abseta() const { return fabs(eta()); }
    /// The azimuthal angle (\f$\phi\f$) of the particle
    double phi() const;
    /// The mass of the particle: note, not necessarily = \f$E^2-p^2\f$ if not on mass shell
    double m() const;
    /// The total energy of the particle
    double e() const;
    /// The true rapidity (\f$y\f$) of the particle
    double rapidity() const;
    /// The true absolute rapidity (\f$|y|\f$) of the particle
    /// @todo Remove this when/if supported in xAOD::IParticle
    double absrapidity() const { return fabs(rapidity()); }

    /// Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// The full 4-momentum of the particle (note that m may not be right, though, if not on mass shell)
    const FourMom_t& p4() const;

    /// The type of the object as a simple enumeration
    Type::ObjectType type() const;

    /// Function making sure that the object is ready for persistification
    void toPersistent();

    //@}


    /// @name 4-momentum accessors in the px-py-pz-E frame
    //@{

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

    //@}



    /// @name Particle species information
    ///
    /// Just convenience methods here, since they all just forward to the utility
    /// functions operating on the PDG particle ID code.
    ///
    //@{

    /// Physical charge
    double charge() const { return MC::PID::charge(pdgId()); }
    /// 3 x the physical charge (so it can be an int for quarks)
    int threeCharge() const { return MC::PID::threeCharge(pdgId()); }

    /// Whether the particle is electrically charged
    bool isCharged() const { return MC::PID::isCharged(pdgId()); }
    /// Whether the particle is electrically neutral
    bool isNeutral() const { return MC::PID::isNeutral(pdgId()); }

    /// Whether the particle is a photon
    bool isPhoton() const { return MC::PID::isPhoton(pdgId()); }
    /// Whether the particle is a lepton
    bool isLepton() const { return MC::PID::isLepton(pdgId()); }
    /// Whether the particle is a charged lepton
    bool isChLepton() const { return MC::PID::isChLepton(pdgId()); }
    /// Whether the particle is an electron (or positron)
    bool isElectron() const { return MC::PID::isElectron(pdgId()); }
    /// Whether the particle is a muon (or antimuon)
    bool isMuon() const { return MC::PID::isMuon(pdgId()); }
    /// Whether the particle is a tau (or antitau)
    bool isTau() const { return MC::PID::isTau(pdgId()); }
    /// Whether the particle is a neutrino (or antineutrino)
    bool isNeutrino() const { return MC::PID::isNeutrino(pdgId()); }

    /// Whether the particle is a hadron
    bool isHadron() const { return MC::PID::isHadron(pdgId()); }
    /// Whether the particle is a meson
    bool isMeson() const { return MC::PID::isMeson(pdgId()); }
    /// Whether the particle is a baryon
    bool isBaryon() const { return MC::PID::isBaryon(pdgId()); }

    /// Whether the particle contains a strange quark (or antiquark)
    bool hasStrange() const { return MC::PID::isPhoton(pdgId()); }
    /// Whether the particle contains a charm quark (or antiquark)
    bool hasCharm() const { return MC::PID::isPhoton(pdgId()); }
    /// Whether the particle contains a bottom quark (or antiquark)
    bool hasBottom() const { return MC::PID::isPhoton(pdgId()); }

    /// Determine if the PID is that of a light flavour (not b or c) meson
    bool isLightMeson() const { return MC::PID::isLightMeson(pdgId()); }
    /// Determine if the PID is that of a light flavour (not b or c) baryon
    bool isLightBaryon() const { return MC::PID::isLightBaryon(pdgId()); }
    /// Determine if the PID is that of a light flavour (not b or c) hadron
    bool isLightHadron() const { return MC::PID::isLightHadron(pdgId()); }

    /// Determine if the PID is that of a heavy flavour (b or c) meson
    bool isHeavyMeson() const { return MC::PID::isHeavyMeson(pdgId()); }
    /// Determine if the PID is that of a heavy flavour (b or c) baryon
    bool isHeavyBaryon() const { return MC::PID::isHeavyBaryon(pdgId()); }
    /// Determine if the PID is that of a heavy flavour (b or c) hadron
    bool isHeavyHadron() const { return MC::PID::isHeavyHadron(pdgId()); }

    /// Determine if the PID is that of a b-meson.
    bool isBottomMeson() { return MC::PID::isBottomMeson(pdgId()); }
    /// Determine if the PID is that of a b-baryon.
    bool isBottomBaryon() { return MC::PID::isBottomBaryon(pdgId()); }
    /// Determine if the PID is that of a b-hadron.
    bool isBottomHadron() { return MC::PID::isBottomHadron(pdgId()); }

    /// @brief Determine if the PID is that of a c-meson.
    ///
    /// Specifically, the _heaviest_ quark is a c: a B_c is a b-meson and NOT a c-meson.
    /// Charmonia (closed charm) are counted as c-mesons here.
    bool isCharmMeson() const { return MC::PID::isCharmMeson(pdgId()); }
    /// @brief Determine if the PID is that of a c-baryon.
    ///
    /// Specifically, the _heaviest_ quark is a c: a baryon containing a b & c
    /// is a b-baryon and NOT a c-baryon. To test for the simpler case, just use
    /// a combination of hasCharm() and isBaryon().
    bool isCharmBaryon() const { return MC::PID::isCharmBaryon(pdgId()); }
    /// Determine if the PID is that of a c-hadron.
    bool isCharmHadron() const { return MC::PID::isCharmHadron(pdgId()); }

    /// Determine if the PID is that of a strange meson
    bool isStrangeMeson() const { return MC::PID::isStrangeMeson(pdgId()); }
    /// Determine if the PID is that of a strange baryon
    bool isStrangeBaryon() const { return MC::PID::isStrangeBaryon(pdgId()); }
    /// Determine if the PID is that of a strange hadron
    bool isStrangeHadron() const { return MC::PID::isStrangeHadron(pdgId()); }

    bool isQuark() const { return MC::PID::isQuark(pdgId()); }
    bool isParton() const { return MC::PID::isParton(pdgId()); }
    bool isTop() const { return MC::PID::isTop(pdgId()); }
    bool isW() const { return MC::PID::isW(pdgId()); }
    bool isZ() const { return MC::PID::isZ(pdgId()); }
    bool isHiggs() const { return MC::PID::isHiggs(pdgId()); }
    bool isResonance() const { return MC::PID::isResonance(pdgId()); }
    bool isGenSpecific() const { return MC::PID::isGenSpecific(pdgId()); }

    //@}


    /// @name Particle connection / origin / decay information
    //@{

    /// Get all the parents of this particle
    //const std::vector< ElementLink< TruthParticleContainer > >& parentLinks() const;

    /// Number of parents of this particle
    size_t nParents() const;

    /// Retrieve the i-th mother (TruthParticle) of this TruthParticle
    const TruthParticle_v1* parent(const std::size_t i=0) const;


    /// Get all the children of this particle
    //const std::vector< ElementLink< TruthParticleContainer > >& childLinks() const;

    /// Number of children of this particle
    size_t nChildren() const;

    /// Retrieve the i-th mother (TruthParticle) of this TruthParticle
    const TruthParticle_v1* child(const std::size_t i=0) const;

    /// @todo Add mappings of e.g. isPrimary, isDecayed, isPrompt, isFromDecay, hasHadronicDecay, hasLeptonicDecay, isHadronic/LeptonicTau ...

    /// @todo Add isFirstWith, isLastWith

    //@}


    /// @name Polarization properties (optional)
    //@{

    /// Polarization parameter enum
    enum PolParam {
      PHI = 0,
      THETA = 1,
    };

    /// Accessor for polarization parameters
    bool polarizationParameter(float& value, PolParam parameter) const;
    /// Set method for polarization parameter values
    void setPolarizationParameter(float value, PolParam parameter);


    /// Single container for full polarization info
    struct Polarization {
      /// Constructor to set (invalid) defaults
      /// @todo Use C++11 inline member init when allowed
      Polarization() :
        phi(-1), theta(-1) { }

      bool valid() const {
        return phi > 0 && theta > 0;
      }

      float phi;
      float theta;
    };

    // Retrieve a full Polarization with a single call
    /// @note May have invalid values -- use valid() to check.
    Polarization polarization() const {
      Polarization rtn;
      polarizationParameter(rtn.phi, PHI);
      polarizationParameter(rtn.theta, THETA);
      return rtn;
    }

    //@}



  private:

    /// Cached four momentum
    mutable FourMom_t m_p4;
    /// Flag specifying if the four-momentum is cached
    mutable bool m_p4Cached;

  }; // class TruthParticle_v1


} // namespace xAOD

#endif // XAODTRUTH_VERSIONS_TRUTHPARTICLE_V1_H
