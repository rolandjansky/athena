/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProton_v1.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Header file for AFPProton_v1 class representing a reconstructed proton measured in AFP detectors.
 * 
 */

#ifndef XAODFORWARD_VERSIONS_AFPPROTON_V1_H
#define XAODFORWARD_VERSIONS_AFPPROTON_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// IParticle include:
#include "xAODBase/IParticle.h"

// Local includes(s):
#include "xAODForward/versions/AFPTrack_v2.h"
#include "xAODForward/versions/AFPTrackContainer_v2.h"

namespace xAOD {

  /**
   * @brief Class representing a proton reconstructed in AFP.
   * 
   * This class provides information about a reconstucted proton measured in AFP detectors.
   */
  class AFPProton_v1 : public IParticle {
  public:
    /// Default constructor
    AFPProton_v1();

    typedef ElementLink< AFPTrackContainer_v2 > AFPTrackLink_t;

    /// @name IParticle functions
    /// @{
      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double           pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double           eta() const;
      /// The azimuthal angle (\f$\phi\f$) of the particle (has range \f$-\pi\f$ to \f$+\pi\f$.)
      virtual double           phi() const;
      /// The invariant mass of the particle
      virtual double           m() const;
      /// The total energy of the particle
      virtual double           e() const;
      /// The true rapidity (y) of the particle
      virtual double           rapidity() const;

      /// Definition of the 4-momentum type
      typedef IParticle::FourMom_t FourMom_t;

      /// The full 4-momentum of the particle
      virtual const FourMom_t& p4() const;

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;
    /// @}


    /// Get the x-component of the momentum
    double px() const;
    /// Get the y-component of the momentum
    double py() const;
    /// Get the z-component of the momentum
    double pz() const;

    /// Set the 4-momentum.
    void setPxPyPzE(double px, double py, double pz, double e);
    /// Set the x-component of the momentum
    void setPx(double px);
    /// Set the y-component of the momentum
    void setPy(double py);
    /// Set the z-component of the momentum
    void setPz(double pz);
    /// Set the energy.
    void setE(double e);

    /// Get the value of @f$\chi^2@f$ function
    float chi2() const;
    /// Set the value of @f$\chi^2@f$ function
    void setChi2(float chi2);

    /// Get the ATLAS side on which the proton was measured
    int side() const;
    /// Set the ATLAS side on which the proton was measured
    void setSide(int side);

    /// Get the ID of method used to reconstruct the proton
    int methodID() const;
    /// Set the ID of method used to reconstruct the proton
    void setMethodID(int methodID);

    /// Get vector of links to tracks that were used to reconstruct the proton
    const std::vector<AFPTrackLink_t>& tracks() const;
    /// Set vector of links to tracks used for proton reconstruction
    void setTracks(const std::vector<AFPTrackLink_t>& newTracksVector);
    /// Add a link to a tracks used to reconstruct the proton
    void addTrack(const AFPTrackLink_t& newTrack);

    /// Function making sure that the object is ready for persistification i.e. saving
    void toPersistent();

  private:
    /// Cached 4-momentum object
    mutable FourMom_t m_p4;
    /// Cache state of the internal 4-momentum (reset from the streamer)
    mutable bool m_p4Cached;
  
  }; // class AFPProton_v1

} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPProton_v1, SG::AuxElement );

#endif
