// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_EFEXTAUROI_V1_H
#define XAODTRIGGER_VERSIONS_EFEXTAUROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 eFEX tau region of interest
   ///
   /// This class describes the properties of a tau Trigger Object (TOB)
   /// or Region of Interest (RoI) in the xAOD format.
   ///
   /// @author Nicholas Luongo <nicholas.andrew.luongo@cern.ch>
   ///
   class eFexTauRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      eFexTauRoI_v1();

      /// Initialise the object with its most important properties
      void initialize( unsigned int eFexNumber, unsigned int shelf, uint32_t word0 );
      void initialize( uint32_t word0, uint32_t word1 );

      /// Object types
      enum ObjectType {
         xTOB  = 0, ///< This object is an xTOB (2*32 bit words)
         TOB   = 1  ///< This object is a TOB (1*32 bit word)
      };

      /// The "raw" 32-bit words describing the e/gamma candidate
      uint32_t word0() const;
      uint32_t word1() const;
      /// Set the "raw" 32-bit words describing the e/gamma candidate
      void setWord0( uint32_t value );
      void setWord1( uint32_t value );

      // Shelf number (decoded from second xTOB word)
      unsigned int shelfNumber() const;

      // eFEX number (decoded from second xTOB word)
      unsigned int eFexNumber() const;

      /// TOB ET (decoded from TOB, stored for convenience)
      float    et() const; /// floating point value (MeV, TOB scale)
      void     setEt( float value); /// setter for the above

      /// Eta Coordinates (decoded from TOB, stored for convenience)
      float   eta() const; /// Floating point, full precision (0.025)
      void    setEta( float value); /// setter for the above 
      int     iEta() const;  /// getter for integer eta index (-25->24)
      unsigned int seed() const; /// Seed supercell index within central tower (0 -> 3)

      /// Phi coordinates
      float   phi() const; /// Floating point, ATLAS phi convention (-pi -> pi)
      void    setPhi( float value); /// Setter for the above
      int     iPhi() const; /// Getter for integer phi index (0-63)

      /// Getters for Topo coordinate indices
      int iEtaTopo() const;
      int iPhiTopo() const;
      
      /// Jet Discriminants
      /// Derived floating point values (not used in actual algorithm)
      float rCore() const;
      float rHad() const;

      /// Actual values used in algorithm (setters and getters)
      uint16_t rCoreNumerator() const;
      uint16_t rCoreDenominator() const;
      void  setRCoreNumerator( uint16_t value);
      void  setRCoreDenominator( uint16_t value);
      uint16_t rHadNumerator() const;
      uint16_t rHadDenominator() const;
      void  setRHadNumerator( uint16_t value);
      void  setRHadDenominator( uint16_t value);

      /// Is this one a TOB (or xTOB partner of a TOB)?
      char isTOB() const;
      void setIsTOB( char value);

      /// Return TOB word corresponding to this TOB or xTOB
      uint32_t tobWord() const;
      

      /// Data decoded from the TOB/RoI word and eFEX number

      /// Object type (TOB or xTOB)
      ObjectType type() const;


      /// FPGA number
      unsigned int fpga() const;

      /// eta index within FPGA
      unsigned int fpgaEta() const;

      /// phi index within FPGA
      unsigned int fpgaPhi() const;

      /// Cluster up/down flag (1 = up, 0 = down)
      bool upNotDown() const;

      /// Cluster ET (TOB ET scale, 100 MeV/count)
      unsigned int etTOB() const;

      /// Cluster ET (xTOB ET scale, 25 MeV/count)
      unsigned int etXTOB() const;

      /// Tau Condition 1 (rCore) results
      unsigned int rCoreThresholds() const;

      /// Tau Condition 2 (none) results
      unsigned int rHadThresholds() const;

      /// Tau Condition 3 (none) results
      unsigned int tauThreeThresholds() const;

      /// Seed = max flag. Is this really useful?
      bool seedMax() const;

      //? Bunch crossing number (last 4 bits, xTOB only)
      unsigned int bcn4() const;

   private:

      /// Constants used in converting to ATLAS units
      static const float s_tobEtScale;
      static const float s_xTobEtScale;
      static const float s_towerEtaWidth;


      /** Constants used in decoding TOB words
          For TOB word format changes these can be replaced
          by arrays in the _v2 object so that different 
          versions can be decoded by one class */

      //  Data locations within word
      static const int s_fpgaBit         = 30;
      static const int s_etaBit          = 27;
      static const int s_phiBit          = 24;
      static const int s_veto3Bit        = 22;
      static const int s_veto2Bit        = 20;
      static const int s_veto1Bit        = 18;
      static const int s_seedBit         = 16;
      static const int s_updownBit       = 15;
      static const int s_maxBit          = 14;
      static const int s_etBit           =  0;
      static const int s_etBitXTOB       =  3;
      static const int s_bcn4Bit         = 28;
      static const int s_shelfBit        = 24;
      static const int s_eFexBit         = 20;

      //  Data masks
      static const int s_fpgaMask        = 0x3;
      static const int s_etaMask         = 0x7;
      static const int s_phiMask         = 0x7;
      static const int s_veto1Mask       = 0x3;
      static const int s_veto2Mask       = 0x3;
      static const int s_veto3Mask       = 0x3;
      static const int s_seedMask        = 0x3;
      static const int s_updownMask      = 0x1;
      static const int s_maxMask         = 0x1;
      static const int s_etMask          = 0xfff;
      static const int s_etFullMask      = 0xffff;
      static const int s_bcn4Mask        = 0xf;
      static const int s_eFexMask        = 0xf;
      static const int s_shelfMask       = 0xf;

      // For decoding coordinates
      // 
      static const int s_numPhi          = 64;
      static const int s_eFexPhiMask     = 0xf;
      static const int s_eFexPhiBit      =  0;
      static const int s_eFexPhiWidth    =  8;
      static const int s_eFexPhiOffset   =  2;
      static const int s_eFexEtaMask     = 0xf;
      static const int s_eFexEtaBit      =  4;
      static const int s_eFexEtaWidth    =  16;
      static const int s_fpgaEtaWidth    =  4;
      static const int s_shelfPhiWidth   =  4;
      static const int s_minEta          = -25;

   }; // class eFexTauRoI_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::eFexTauRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_EFEXTAUROI_V1_H
