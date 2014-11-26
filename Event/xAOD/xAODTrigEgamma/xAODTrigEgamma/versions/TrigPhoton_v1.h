// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhoton_v1.h 631115 2014-11-26 08:34:25Z gwatts $
#ifndef XAODTRIGEGAMMA_VERSIONS_TRIGPHOTON_V1_H
#define XAODTRIGEGAMMA_VERSIONS_TRIGPHOTON_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticle.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

namespace xAOD {

   /// Class describing a photon reconstructed in the HLT
   ///
   /// The HLT reconstructs photons in a simplified manner before using
   /// parts of the offline software to reconstruct photons in a manner
   /// that's very similar to what the offline reconstruction is doing.
   ///
   /// @author Fabrizio Salvatore <Fabrizio.Salvatore@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631115 $
   /// $Date: 2014-11-26 09:34:25 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigPhoton_v1 : public IParticle {

   public:
      /// Default constructor
      TrigPhoton_v1();

      /// @name Functions implementing the xAOD::IParticle interface
      /// @{

      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double           pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double           eta() const { return caloEta(); }
      /// The azimuthal angle (\f$\phi\f$) of the particle
      virtual double           phi() const { return caloPhi(); }
      /// The invariant mass of the particle
      virtual double           m() const { return 0.0; }
      /// The total energy of the particle
      virtual double           e() const;
      /// The true rapidity (y) of the particle
      virtual double           rapidity() const;

      /// The full 4-momentum of the particle
      virtual const FourMom_t& p4() const;

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const { return Type::TrigPhoton; }

      /// @}

      /// @name Simple photon properties
      /// @{

      /// Get the RoI word that seeded the reconstruction of this object
      uint32_t roiWord() const;
      /// Set the RoI word that seeded the reconstruction of this object
      void setRoiWord( uint32_t roi );

      /// Documentation to be added
      float rcore() const;
      /// Documentation to be added
      void setRcore( float Reta );

      /// Documentation to be added
      float eratio() const;
      /// Documentation to be added
      void setEratio( float Eratio );

      /// Documentation to be added
      float etHad() const;
      /// Documentation to be added
      void setEtHad( float HadEt );

      /// Documentation to be added
      float etHad1() const;
      /// Documentation to be added
      void setEtHad1( float HadEt );

      /// Documentation to be added
      float f0() const;
      /// Documentation to be added
      void setF0( float f0 );

      /// Documentation to be added
      float f1() const;
      /// Documentation to be added
      void setF1( float f1 );

      /// Documentation to be added
      float f2() const;
      /// Documentation to be added
      void setF2( float f2 );

      /// Documentation to be added
      float f3() const;
      /// Documentation to be added
      void setF3( float f3 );

      /// Documentation to be added
      float fside() const;
      /// Documentation to be added
      void setFside( float fside );

      /// Documentation to be added
      float weta2() const;
      /// Documentation to be added
      void setWeta2( float Weta2 );

      /// @}

      /// @name Four-momentum properties
      /// @{

      /// Set the transverse momentum (\f$p_T\f$) of the photon
      void setPt( double pt );

      /// Pseudorapidity (\f$\eta\f$) of the photon in the calorimeter
      float caloEta() const;
      /// Set the pseudorapidity (\f$\eta\f$) of the photon in the calorimeter
      void setCaloEta( float caloEta );

      /// Azimuthal angle (\f$\phi\f$) of the photon in the calorimeter
      float caloPhi() const;
      /// Set the azimuthal angle (\f$\phi\f$) of the photon in the calorimeter
      void setCaloPhi( float caloPhi );

      /// Documentation to be added
      float dEta() const;
      /// Documentation to be added
      void setDEta( float dEta );

      /// Documentation to be added
      float dPhi() const;
      /// Documentation to be added
      void setDPhi( float dPhi );

      /// @}

      /// @name Link(s) to constituent objects
      /// @{

      /// Type of the EM cluster link
      typedef ElementLink< TrigEMClusterContainer > EMClusterLink_t;

      /// The associated EM cluster, as a simple pointer
      const TrigEMCluster* emCluster() const;
      /// The associated EM cluster, as an ElementLink
      const EMClusterLink_t& emClusterLink() const;
      /// Set the ElementLink pointing to the EM cluster constituent
      void setEmClusterLink( const EMClusterLink_t& value );

      /// @}

      /// Initialisation function, setting most properties of the object
      void init( uint32_t roi,
                 float dphi, float deta, 
                 const EMClusterLink_t& clLink );

   private:
      /// Cached 4-momentum object.
      mutable FourMom_t m_p4;
      /// Cache state of the internal 4-momentum (reset from the streamer)
      mutable bool m_p4Cached;

   }; // class TrigPhoton_v1

} // namespace xAOD

#endif // not XAODTRIGEGAMMA_VERSIONS_TRIGPHOTON_V1_H
