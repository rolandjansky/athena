// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_GFEXGLOBALROI_V1_H
#define XAODTRIGGER_VERSIONS_GFEXGLOBALROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing properties of a LVL1 gFEX global Trigger Object (TOB) 
   /// in the xAOD format.
 
   class gFexGlobalRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      gFexGlobalRoI_v1();

      /// Initialise the object with its most important properties: only the word for gFEX
      void initialize( uint32_t word );

      /// Object types
      enum ObjectType {
         gNull      = 0,  /// indicaes a null value (should be accompanied by a Status=0)
         gScalar    = 1,  /// scalar values (MET, SumET))
         gMET       = 2,  /// MET components (METx, METy)
         gMHT       = 3,  /// MET hard term components (MHTx, MHTy)
         gMST       = 4   /// MET soft term components (MSTx, MSTy)
      };


      /// The "raw" 32-bit word describing the object candidate
      uint32_t word() const;
      /// Set the "raw" 32-bit words describing the object candidate
      void setWord( uint32_t value );


      /// TOB ET (decoded from TOB, stored for convenience)
      uint16_t quantityOne() const;    /// getter for integer ET on TOB scale (3.2 GeV/count)
      void     setQuantityOne( uint16_t value); /// setter for the above
      unsigned int unpackQuantityOneIndex( ) const; /// retrieves the Et index from the 32-bit word
      
      uint16_t quantityTwo() const;    /// getter for integer ET on TOB scale (3.2 GeV/count)
      void     setQuantityTwo( uint16_t value); /// setter for the above
      unsigned int unpackQuantityTwoIndex( ) const; /// retrieves the Et index from the 32-bit word

      float    METquantityOne() const; /// floating point value (GeV, TOB scale)
      float    METquantityTwo() const; /// floating point value (GeV, TOB scale)
      float    SumEt() const; /// floating point value (GeV, TOB scale)

      

      /// TOB status: set to 0 if values are null
      uint8_t statusOne() const;
      void setStatusOne( uint8_t value ) ; 
      uint8_t statusTwo() const;
      void setStatusTwo( uint8_t value ) ; 
      unsigned int unpackStatusOne( ) const; /// retrieves the Status info for Quantity 1 from the 32-bit word
      unsigned int unpackStatusTwo( ) const; /// retrieves the Status info for Quantity 2 from the 32-bit word
      /// Energy saturation in tthe global TOB values
      uint8_t saturated() const;
      void setSaturated( uint8_t value ) ; 
      unsigned int unpackSaturated( ) const; /// retrieves the energy saturation status from the 32-bit word

      int globalType () const;
      void setGlobalType ( int type) ;
      int unpackType( ) const;
      ///Identification of object type with flags
      bool isgScalar() const;
      bool isgMET() const;
      bool isgMHT() const;
      bool isgMST() const;  




   private:


      /// Constants used in converting to ATLAS units
      static const float s_tobEtScale;


      // Constants used in decoding TOB words
          

      //  Data locations within word
      static const int s_saturBit        = 31;
      static const int s_tobIDBit        = 26;
      static const int s_statusOneBit    = 25;
      static const int s_statusTwoBit    = 24;
      static const int s_quantityOneBit  = 12;
      static const int s_quantityTwoBit  = 0;

      //  Data masks
      static const int s_saturMask       = 0x1;
      static const int s_tobIDMask       = 0x1f;
      static const int s_statusOneMask   = 0x1;
      static const int s_statusTwoMask   = 0x1;
      static const int s_quantityOneMask = 0xfff;
      static const int s_quantityTwoMask = 0xfff;


   }; // class gFexGlobalRoI_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::gFexGlobalRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_GFEXGLOBALROI_V1_H

