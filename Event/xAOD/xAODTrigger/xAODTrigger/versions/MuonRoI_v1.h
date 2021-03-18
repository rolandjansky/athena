// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoI_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_MUONROI_V1_H
#define XAODTRIGGER_VERSIONS_MUONROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 muon region of interest
   ///
   /// This class describes all the properties of a muon region of interest
   /// (RoI) in the xAOD format.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class MuonRoI_v1 : public SG::AuxElement {

   public:
      /// RoI source enumeration
      enum RoISource {
         Barrel, ///< The muon candidate was detected in the barrel region
         Endcap, ///< The muon candidate was detected in the endcap region
         Forward ///< The muon candidate was detected in the forward region
      };
      /// RoI hemisphere enumeration
      enum Hemisphere {
         Positive, ///< The candidate came from the +z side (side A)
         Negative  ///< The candidate came from the -z side (side C)
      };
      /// Charge sign
      enum Charge {
         Neg = 0, ///< Candidate is a mu- from a TGC sector
         Pos = 1, ///< Candidate is a mu+ from a TGC sector
         Undef = 100 //!< This is an RPC candidate
      };

      /// Default constructor
      MuonRoI_v1();

      /// Initialise the object with all its properties
      void initialize( uint32_t roiword, float eta, float phi,
                       const std::string& thrname, float thrvalue );

      /// @name Independent properties stored for the muon candidate
      /// @{

      /// The pseudorapidity (\f$\eta\f$) of the muon candidate
      float eta() const;
      /// Set the pseudorapidity of the muon candidate
      void setEta( float v );
 
      /// The azimuthal angle (\f$\phi\f$) of the muon candidate
      float phi() const;
      /// Set the azimuthal angle of the muon candidate
      void setPhi( float v );

      /// The "raw" RoI word describing the muon candidate
      uint32_t roiWord() const;
      /// Set the "raw" RoI word, describing the muon candidate
      void setRoIWord( uint32_t value );

      /// The highest threshold value (in MeV) passed by the muon candidate
      float thrValue() const;
      /// Set the threshold value passed by the muon candidate
      void setThrValue( float v );

      /// The name of the highest threshold value passed by the muon candidate
      const std::string& thrName() const;
      /// Set the name of the threshold passed by the muon candidate
      void setThrName( const std::string& value );

      /// @}

      /// @name Properties extracted from the 32-bit muon RoI word
      /// @{

      /// Get the logic number of the highest threshold this RoI passed
      int getThrNumber() const;
      /// Get the "RoI number" (position inside the sector)
      int getRoI() const;
      /// Get the full sector address
      int getSectorAddress() const;
      /// Get the sector ID number
      int getSectorID() const;
      /// Returns if the candidate had the highest p<sub>T</sub> in the sector
      bool isFirstCandidate() const;
      /// Returns if there were other muons detected in the same RoI
      bool isMoreCandInRoI() const;
      /// Returns if there were other muons detected in the same sector
      bool isMoreCandInSector() const;
      /// Returns the system that detected the muon candidate
      RoISource getSource() const;
      /// Returns the hemisphere that detected the muon candidate
      Hemisphere getHemisphere() const;
      /// Returns whether there's overlap between barrel sectors
      bool getPhiOverlap() const;
      /// Returns whether there's overlap between the barrel-EC sectors
      bool getEtaOverlap() const;
      /// Returns the charge sign of the muon candidate
      Charge getCharge() const;
      /// Returns whether or not there was a 3-station coincidence in the TGC
      bool getBW3Coincidence() const;
      /// Returns whether or not there was an inner coincidence in the TGC
      bool getInnerCoincidence() const;
      /// Returns whether or not there was a good magnetic field quality in the TGC
      bool getGoodMF() const;
      /// Returns the veto flag for the candidate
      bool isVetoed() const;
      /// Returns extra flag at end of RoI word indicating that it's in Run3 format
      bool isRun3() const;

      /// @}

   }; // class MuonRoI_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MuonRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_MUONROI_V1_H
