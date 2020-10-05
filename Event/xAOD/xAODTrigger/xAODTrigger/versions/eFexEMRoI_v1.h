// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoI_v1.h 1 2019-06-30 12:15:18 watsona $
#ifndef XAODTRIGGER_VERSIONS_EFEXEMROI_V1_H
#define XAODTRIGGER_VERSIONS_EFEXEMROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 eFEX EM region of interest
   ///
   /// This class describes the properties of an EM Trigger Object (TOB)
   /// or Region of Interest (RoI) in the xAOD format.
   ///
   /// @author Alan Watson <Alan.Watson@cern.ch>
   ///
   /// $Revision: 1 $
   /// $Date: 2019-06-30 12:15:18 +0100 (Sun, 30 Jun 2019) $
   ///
   class eFexEMRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      eFexEMRoI_v1();

      /// Initialise the object with its most important properties
      void initialize( uint8_t eFexNumber, uint32_t Word0, uint32_t Word1 = 0 );

      /// Object types
      enum ObjectType {
         xTOB  = 0, ///< This object is an xTOB (2*32 bit words)
         TOB   = 1  ///< This object is a TOB (1*32 bit word)
      };

      /// The "raw" 32-bit words describing the e/gamma candidate
      uint32_t Word0() const;
      uint32_t Word1() const;
      /// Set the "raw" 32-bit words describing the e/gamma candidate
      void setWord0( uint32_t value );
      void setWord1( uint32_t value );

      /// The eFEX number
      uint8_t eFexNumber() const;
      /// Set the eFEX number
      void seteFexNumber( uint8_t value );

      /// TOB ET (decoded from TOB, stored for convenience)
      uint16_t tobEt() const;    /// getter for integer ET on TOB scale (100 MeV/count)
      void     setTobEt( uint16_t value); /// setter for the above
      float    et() const; /// floating point value (MeV, TOB scale)


      /// Eta Coordinates (decoded from TOB, stored for convenience)
      uint8_t iEta() const;  /// getter for integer eta index (0-49)
      void    setEta( uint8_t value); /// setter for the above 
      unsigned int seed() const; /// Seed supercell index within central tower (0 -> 3)
      float eta() const; /// Floating point, full precision (0.025)

      /// Phi coordinates
      uint8_t iPhi() const; /// Getter for integer phi index (0-63)
      void  setPhi( uint8_t value); /// Setter for the above
      float phi() const; /// Floating point, ATLAS phi convention (-pi -> pi)

      
      /// Jet Discriminants
      /// Derived floating point values (not used in actual algorithm)
      float Reta() const;
      float Rhad() const;
      float Wstot() const;

      /// Actual values used in algorithm (setters and getters)
      uint16_t RetaCore() const;
      uint16_t RetaEnv() const;
      uint16_t RhadEM() const;
      uint16_t RhadHad() const;
      uint16_t WstotNumerator() const;
      uint16_t WstotDenominator() const;
      void  setRetaCore( uint16_t value);
      void  setRetaEnv( uint16_t value);
      void  setRhadEM( uint16_t value);
      void  setRhadHad( uint16_t value);
      void  setWstotNumerator( uint16_t value);
      void  setWstotDenominator( uint16_t value);

      /// Is this one a TOB (or xTOB partner of a TOB)?
      char isTOB() const;
      void setIsTOB( char value);

      /// Pattern of thresholds passed
      uint32_t thrPattern() const;
      void     setThrPattern( uint32_t value);
   
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
      unsigned int UpNotDown() const;

      /// Cluster ET (TOB ET scale, 100 MeV/count)
      unsigned int etTOB() const;

      /// Cluster ET (xTOB ET scale, 25 MeV/count)
      unsigned int etXTOB() const;

      /// Jet Discriminant 1 (R_eta) results
      unsigned int RetaThresholds() const;

      /// Jet Discriminant 2 (R_had) results
      unsigned int RhadThresholds() const;

      /// Jet Discriminant 3 (W_stot) results
      unsigned int WstotThresholds() const;

      /// Seed = max flag. Is this really useful?
      unsigned int seedMax() const;

      //? Bunch crossing number (last 4 bits, xTOB only)
      unsigned int bcn4() const;

   private:

      /// Constants used in converting to ATLAS units
      static const float s_tobEtScale;
      static const float s_xTobEtScale;
      static const float s_towerEtaWidth;
      static const float s_minEta;


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
      // For decoding coordinates
      // 
      static const int s_numPhi          = 64;
      static const int s_eFexPhiMask     = 0xf;
      static const int s_eFexPhiBit      =  0;
      static const int s_eFexPhiWidth    =  8;
      static const int s_eFexPhiOffset   =  2;
      static const int s_eFexEtaMask     = 0xf;
      static const int s_eFexEtaBit      =  4;
      static const int s_fpgaEtaWidth    =  4;
      static const int s_EtaCOffset      =  0;
      static const int s_EtaBOffset      = 17;
      static const int s_EtaAOffset      = 33;

      /// eFEX numbering (eta, 3 locations)
      enum eFEXTypes {eFexC = 0xc, eFexB = 0xb, eFexA = 0xa};

      /// Compute eta & phi indices from TOB word
      int etaIndex() const;
      int phiIndex() const;

   }; // class eFexEMRoI_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::eFexEMRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_EFEXEMROI_V1_H
