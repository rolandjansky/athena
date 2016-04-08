// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2CombinedMuon_v1.h 658542 2015-04-01 18:17:00Z ssnyder $
#ifndef XAODTRIGMUON_VERSIONS_L2COMBINEDMUON_V1_H
#define XAODTRIGMUON_VERSIONS_L2COMBINEDMUON_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <iosfwd>

// EDM include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

namespace xAOD {

   /// Class describing combined muon reconstructed in the LVL2 trigger
   ///
   /// This is a simple description of the LVL2 CB muon
   ///
   /// @author Stefano Giagu <stefano.giagu@cern.ch>
   ///
   /// V1 2014-06-24 
   ///
   class L2CombinedMuon_v1 : public IParticle {

   public:
      /// Constructor
      L2CombinedMuon_v1();

      /// @name Functions implementing the xAOD::IParticle interface
      /// @{

      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double           pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double           eta() const;
      /// The azimuthal angle (\f$\phi\f$) of the particle
      virtual double           phi() const;
      /// The invariant mass of the particle
      virtual double           m() const;
      /// The total energy of the particle
      virtual double           e() const;
      /// The true rapidity (y) of the particle
      virtual double           rapidity() const;

      /// Definition of the 4-momentum type
      typedef TLorentzVector FourMom_t;

      /// The full 4-momentum of the particle
      virtual const FourMom_t& p4() const;

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;

      /// @}

      /// @name 4-momentum setter functions
      /// @{

      /// Set the transverse momentum (\f$p_T\f$) of the muon
      void setPt( float pt );
      /// Set the pseudorapidity (\f$\eta\f$) of the muon
      void setEta( float eta );
      /// Set the azimuthal angle (\f$\phi\f$) of the muon
      void setPhi( float phi );

      /// @}

      /// @name Properties specific to the LVL2 CB muon
      /// @{

      /// get seeding muon charge
      float charge() const;
      /// set seeding muon charge
      void setCharge( float value );

      /// get algorithm strategy flag
      int strategy() const;
      /// set algorithm strategy flag
      void setStrategy( int value );

      /// get algorithm error flag 
      int errorFlag() const;
      /// set algorithm error flag
      void setErrorFlag( int value );

      /// get algorithm match flag 
      int matchFlag() const;
      /// set algorithm match flag
      void setMatchFlag( int value );

      /// get sigma combined Pt
      float sigmaPt() const;
      /// set sigma combined Pt
      void setSigmaPt( float value );

      /// get ID track used to make the CB muon
      const ElementLink< xAOD::TrackParticleContainer >& idTrackLink() const;
      /// set ID track used to make the CB muon
      void setIdTrackLink(const ElementLink< xAOD::TrackParticleContainer >& link);
      /// Get the ID track as a bare pointer
      const xAOD::TrackParticle* idTrack() const;

      /// get SA muon used to make the CB muon
      const ElementLink< xAOD::L2StandAloneMuonContainer >& muSATrackLink() const;
      /// set SA muon used to make the CB muon
      void setMuSATrackLink(const ElementLink< xAOD::L2StandAloneMuonContainer >& link);
      /// Get the SA muon as a bare pointer
      const xAOD::L2StandAloneMuon* muSATrack() const;

      /// @}

   private:
      /// Cached 4-momentum of the muon
      mutable FourMom_t m_p4;
      /// Cached state of the internal 4-momentum object
      mutable bool m_p4Cached;

   }; // class L2CombinedMuon_v1


/// Operator for printing the contents of a LVL2 muon
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2CombinedMuon_v1& mu );

} // namespace xAOD

#endif // XAODTRIGMUON_VERSIONS_L2COMBINEDMUON_V1_H
