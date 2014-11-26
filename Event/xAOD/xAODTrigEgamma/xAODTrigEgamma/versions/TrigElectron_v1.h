// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectron_v1.h 631115 2014-11-26 08:34:25Z gwatts $
#ifndef XAODTRIGEGAMMA_VERSIONS_TRIGELECTRON_V1_H
#define XAODTRIGEGAMMA_VERSIONS_TRIGELECTRON_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticle.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace xAOD {

   /// Class describing an electron reconstructed in the HLT
   ///
   /// The HLT reconstructs electrons in a simplified manner before using
   /// parts of the offline software to reconstruct electrons in a manner
   /// that's very similar to what the offline reconstruction is doing.
   ///
   /// @author Fabrizio Salvatore <Fabrizio.Salvatore@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631115 $
   /// $Date: 2014-11-26 09:34:25 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigElectron_v1 : public IParticle {

   public:
      /// Default constructor
      TrigElectron_v1();

      /// @name Functions implementing the xAOD::IParticle interface
      /// @{

      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double           pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double           eta() const { return caloEta(); }
      /// The azimuthal angle (\f$\phi\f$) of the particle
      virtual double           phi() const { return caloPhi(); }
      /// The invariant mass of the particle
      virtual double           m() const { return 0.510998928; }
      /// The total energy of the particle
      virtual double           e() const;
      /// The true rapidity (y) of the particle
      virtual double           rapidity() const;

      /// The full 4-momentum of the particle
      virtual const FourMom_t& p4() const;

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const { return Type::TrigElectron; }

      /// @}

      /// @name Simple electron properties
      /// @{

      /// Get the RoI word that seeded the reconstruction of this object
      uint32_t roiWord() const;
      /// Set the RoI word that seeded the reconstruction of this object
      void setRoiWord( uint32_t roi );

      /// Get the Z position of the vertex that the electron is associated to
      float zvtx() const;
      /// Set the Z position of the vertex that the electron is associated to
      void setZvtx( float zVtx );

      /// Get the charge of the electron
      float charge() const;
      /// Set the charge of the electron
      void setCharge( float charge );

      /// Get the number of TRT hits that were used in the reconstruction
      int nTRTHits() const;
      /// Set the number of TRT hits that were used in the reconstruction
      void setNTRTHits( int nTRTHits );

      /// Get the number of high-threshold TRT hits used in the reconstruction
      int nTRTHiThresholdHits() const;
      /// Set the number of high-threshold TRT hits used in the reconstruction
      void setNTRTHiThresholdHits( int nTRTHiThresholdHits );

      /// Documentation to be added
      float rcore() const;
      /// Documentation to be added
      void setRcore( float rcore );

      /// Documentation to be added
      float eratio() const;
      /// Documentation to be added
      void setEratio( float eratio );

      /// Documentation to be added
      float etHad() const;
      /// Documentation to be added
      void setEtHad( float etHad );

      /// Documentation to be added
      float etHad1() const;
      /// Documentation to be added
      void setEtHad1( float etHad1 );

      /// Documentation to be added
      float f0() const;
      /// Documentation to be added
      void setF0( float value );

      /// Documentation to be added
      float f1() const;
      /// Documentation to be added
      void setF1( float value );

      /// Documentation to be added
      float f2() const;
      /// Documentation to be added
      void setF2( float value );

      /// Documentation to be added
      float f3() const;
      /// Documentation to be added
      void setF3( float value );

      /// @}

      /// @name Four-momentum properties
      /// @{

      /// Get the track's pseudorapidity extrapolated to the calorimeter
      float trkEtaAtCalo() const;
      /// Set the track's pseudorapidity extrapolated to the calorimeter
      void setTrkEtaAtCalo( float trEta );

      /// Get the track's azimuthal angle extrapolated to the calorimeter
      float trkPhiAtCalo() const;
      /// Set the track's azimuthal angle extrapolated to the calorimeter
      void setTrkPhiAtCalo( float trPhi );

      /// Get \f$E_T / p_T\f$ for the electron
      float etOverPt() const;
      /// Set \f$E_T / p_T\f$ for the electron
      void setEtOverPt( float etOverpt );

      /// The absolute value of the track-calo eta measurement difference
      float trkClusDeta() const;
      /// The absolute value of the track-calo phi measurement difference
      float trkClusDphi() const;

      /// Set the transverse momentum (\f$p_T\f$) of the electron
      void setPt( double pt );

      /// Pseudorapidity (\f$\eta\f$) of the electron in the calorimeter
      float caloEta() const;
      /// Set the pseudorapidity (\f$\eta\f$) of the electron in the calorimeter
      void setCaloEta( float caloEta );

      /// Azimuthal angle (\f$\phi\f$) of the electron in the calorimeter
      float caloPhi() const;
      /// Set the azimuthal angle (\f$\phi\f$) of the electron in the calorimeter
      void setCaloPhi( float caloPhi );

      /// @}

      /// @name Links to constituent objects
      /// @{

      /// Type of the EM cluster link
      typedef ElementLink< TrigEMClusterContainer > EMClusterLink_t;

      /// The associated EM cluster, as a simple pointer
      const TrigEMCluster* emCluster() const;
      /// The associated EM cluster, as an ElementLink
      const EMClusterLink_t& emClusterLink() const;
      /// Set the ElementLink pointing to the EM cluster constituent
      void setEmClusterLink( const EMClusterLink_t& value );

      /// The type of the track particle link
      typedef ElementLink< TrackParticleContainer > TrackParticleLink_t;

      /// The associated track particle, as a simple pointer
      const TrackParticle* trackParticle() const;
      /// The associated track particle, as an ElementLink
      const TrackParticleLink_t& trackParticleLink() const;
      /// Set the ElementLink pointing to the track particle constituent
      void setTrackParticleLink( const TrackParticleLink_t& value );

      /// @}

      /// Initialisation function, setting most properties of the object
      void init( uint32_t roi,
                 float trkEtaAtCalo, float trkPhiAtCalo, float etOverPt,
                 const EMClusterLink_t& clLink,
                 const TrackParticleLink_t& tpLink );

   private:
      /// Cached 4-momentum object.
      mutable FourMom_t m_p4;
      /// Cache state of the internal 4-momentum (reset from the streamer)
      mutable bool m_p4Cached;

   }; // class TrigElectron_v1

} // namespace xAOD

#endif // not XAODTRIGEGAMMA_VERSIONS_TRIGELECTRON_V1_H
