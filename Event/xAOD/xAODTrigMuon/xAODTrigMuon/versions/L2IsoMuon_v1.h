// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2IsoMuon_v1.h 631108 2014-11-26 07:47:24Z gwatts $
#ifndef XAODTRIGMUON_VERSIONS_L2ISOMUON_V1_H
#define XAODTRIGMUON_VERSIONS_L2ISOMUON_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <iosfwd>

// xAOD include(s):
#include "xAODBase/IParticle.h"

namespace xAOD {

   /// Class describing isolation info for muons reconstructed in the LVL2 trigger
   ///
   /// This is a simple description of the LVL2 isolation for muons.
   ///
   /// @author Stefano Giagu <stefano.giagu@cern.ch>
   ///
   /// V1 2014-06-24 
   ///
   class L2IsoMuon_v1 : public IParticle {

   public:
      /// Constructor
      L2IsoMuon_v1();

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

      /// @name Properties specific to the LVL2 muon isolation 
      /// @{

      /// Get the RoI ID of the seeding muon
      uint32_t roiWord() const;
      /// Set the RoI ID of the seeding muon
      void setRoiWord( uint32_t value );

      /// get seeding muon charge
      float charge() const;
      /// set seeding muon charge
      void setCharge( float value );

      /// get algorithm error flag 
      int errorFlag() const;
      /// set algorithm error flag
      void setErrorFlag( int value );

      /// get ptcone radius DeltaR<0.1
      float sumPt01() const;
      /// set ptcone radius DeltaR<0.1
      void setSumPt01( float value );

      /// get ptcone radius DeltaR<0.2
      float sumPt02() const;
      /// set ptcone radius DeltaR<0.2
      void setSumPt02( float value );

      /// get ptcone radius DeltaR<0.3
      float sumPt03() const;
      /// set ptcone radius DeltaR<0.3
      void setSumPt03( float value );

      /// get ptcone radius DeltaR<0.4
      float sumPt04() const;
      /// set ptcone radius DeltaR<0.4
      void setSumPt04( float value );

      /// get ptcone radius DeltaR<0.1
      float sumEt01() const;
      /// set ptcone radius DeltaR<0.1
      void setSumEt01( float value );

      /// get etcone radius DeltaR<0.2
      float sumEt02() const;
      /// set etcone radius DeltaR<0.2
      void setSumEt02( float value );

      /// get etcone radius DeltaR<0.3
      float sumEt03() const;
      /// set etcone radius DeltaR<0.3
      void setSumEt03( float value );

      /// get etcone radius DeltaR<0.4
      float sumEt04() const;
      /// set etcone radius DeltaR<0.4
      void setSumEt04( float value );

      /// @}

   private:
      /// Cached 4-momentum of the muon
      mutable FourMom_t m_p4;
      /// Cached state of the internal 4-momentum object
      mutable bool m_p4Cached;

   }; // class L2IsoMuon_v1

} // namespace xAOD

/// Operator for printing the contents of a LVL2 muon
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2IsoMuon_v1& mu );

#endif // XAODTRIGMUON_VERSIONS_L2ISOMUON_V1_H
