// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_NEUTRALPARTICLE_V1_H
#define XAODTRACKING_VERSIONS_NEUTRALPARTICLE_V1_H

// Athena includes
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackingPrimitives.h" 
#include "xAODTracking/VertexContainerFwd.h" 

#include <bitset>
#include <stdint.h>

#ifndef XAOD_STANDALONE
#ifndef XAOD_MANACORE
#include "TrkNeutralParameters/NeutralParameters.h"
#endif // not XAOD_MANACORE
#endif // not XAOD_STANDALONE

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {
  /// Class describing a NeutralParticle.
  ///
  /// @author Andreas Salzburger <Andreas.Salzburger@cern.ch>
  /// @nosubgrouping
  
  class NeutralParticle_v1 : public IParticle {

    public:

      /// Default constructor
      NeutralParticle_v1();
      /// Destructor
      ~NeutralParticle_v1();
      /// Copy ctor. This involves copying the entire Auxilary store, and is a slow operation which should be used sparingly.
      NeutralParticle_v1(const NeutralParticle_v1& o );
      /// Assignment operator. This can involve creating and copying an Auxilary store, and so should be used sparingly.
      NeutralParticle_v1& operator=(const NeutralParticle_v1& tp );
      
      /// @name xAOD::IParticle functions
      /// @{
      /// The transverse momentum (\f$p_T\f$) of the particle.
      virtual double           pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle.
      virtual double           eta() const;
      /// The azimuthal angle (\f$\phi\f$) of the particle.
      virtual double           phi() const;
      /// The invariant mass of the particle..
      virtual double           m() const;
      /// The total energy of the particle.
      virtual double           e() const;
      /// The true rapidity (y) of the particle.
      virtual double           rapidity() const;
      
      /// Definition of the 4-momentum type.
      typedef IParticle::FourMom_t FourMom_t;
      
      /// The full 4-momentum of the particle.
      virtual FourMom_t p4() const;

      /// Base 4 Momentum type for TrackParticle
      typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > GenVecFourMom_t;

      /// The full 4-momentum of the particle : GenVector form
      GenVecFourMom_t genvecP4() const;
      
      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;
      /// @}
      
      /// @name Defining parameters functions
      /// The 'defining parameters' are key to the concept of a NeutralParticle, and give the values for the IParticle interface
      /// ( pt(), phi(), eta() etc.).
      /// They use the Trk::NeutralPerigee coordinate system, and are defined as:
      ///  \f$( d_0, z_0, \phi, \theta, q/p )\f$.
      /// The parameters are expressed with respect to an origin (returned by vx(), vy() and vy() ), currently intended to be the 'beamspot'.
      /// This origin is expected to be the same for all track particles in a collection (and this may be be enforced).
      /// @{
      
      /// Returns the \f$d_0\f$ parameter
      float d0() const;
      /// Returns the \f$z_0\f$ parameter
      float z0() const;
      /// Returns the \f$\phi\f$ parameter, which has range \f$-\pi\f$ to \f$+\pi\f$.
      float phi0() const;
      /// Returns the \f$\theta\f$  parameter, which has range 0 to \f$\pi\f$.
      float theta() const;
      /// Returns the \f$q/p\f$  parameter
      float oneOverP() const;
      /// @brief Returns a SVector of the Perigee track parameters. 
      /// i.e. a vector of
      ///  \f$\left(\begin{array}{c}d_0\\z_0\\\phi_0\\\theta\\q/p\end{array}\right)\f$
      const DefiningParameters_t& definingParameters() const;
      /// Returns the 5x5 symmetric matrix containing the defining parameters covariance matrix.
      const ParametersCovMatrix_t& definingParametersCovMatrix() const;  
      /// Returns the vector of the covariance values - 15 elements
      const std::vector<float>& definingParametersCovMatrixVec() const;
      
      /// Set the defining parameters.     
      void setDefiningParameters(float d0, float z0, float phi0, float theta, float qOverP);
      /// Set the defining parameters covariance matrix.
      void setDefiningParametersCovMatrix(const ParametersCovMatrix_t& cov);
      /// Set the defining parameters covariance matrix using a length 15 vector.
      void setDefiningParametersCovMatrixVec(const std::vector<float>& cov);
      
      /// The x origin for the parameters.
      float vx() const;
      /// The y origin for the parameters.
      float vy() const;
      /// The z origin for the parameters.
      float vz() const;
      /// Set the origin for the parameters.
      void setParametersOrigin(float x, float y, float z);

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
      /// @brief Returns the Trk::NeutralPerigee track parameters.
      ///
      /// These are defined as:
      ///  \f$\left(\begin{array}{c}d_0\\z_0\\\phi_0\\\theta\\1/p\\\end{array}\right)\f$
      /// @note This is only available in Athena. 
      const Trk::NeutralPerigee& perigeeParameters() const;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

      // /// @brief Returns a link (which can be invalid) to the xAOD::Vertex associated with this NeutralParticle.
      // const ElementLink< VertexContainer >& vertex() const;
      // /// @brief Set the link to the vertex
      //  void setVertex(const ElementLink< VertexContainer >& vertex);

    private:
      /// Set to false if anything related to the perigee was changed
      /// (and so it needs updating);  *** NEEDS TO CHANGE FOR ATHENAMT ***
      mutable bool m_perigeeCached;

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) ) && ( ! defined(__GCCXML__) ) && !defined(__CLING__)
      /// @brief Cached NeutralPerigee, built from this object.
      /// @note This is only available in Athena.
      mutable Trk::NeutralPerigee* m_perigeeParameters;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE and not __GCCXML__

    }; // class NeutralParticle_v1

} // namespace xAOD


#endif // XAODTRACKING_VERSIONS_NEUTRALPARTICLE_V1_H
