/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecision_v1.cxx 581968 2014-02-06 16:08:52Z krasznaa $

#include <limits.h>
#include <algorithm>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TrigDecision_v1.h"

// string2hash
#include "TrigConfHLTData/HLTUtils.h"

namespace xAOD {

   TrigDecision_v1::TrigDecision_v1()
      : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //        Implementation of the trigger configuration key accessors
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigDecision_v1, uint32_t,
                                         smk, setSMK )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the LVL1 result accessors
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigDecision_v1, char,
                                         bgCode, setBGCode )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      tav, setTAV )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      tap, setTAP )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      tbp, setTBP )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the HLT result accessors
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigDecision_v1, uint32_t,
                                         lvl2ErrorBits, setLVL2ErrorBits )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigDecision_v1, uint32_t,
                                         efErrorBits, setEFErrorBits )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigDecision_v1, bool,
                                         lvl2Truncated, setLVL2Truncated )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigDecision_v1, bool,
                                         efTruncated, setEFTruncated )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      lvl2PassedPhysics, setLVL2PassedPhysics )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      efPassedPhysics, setEFPassedPhysics )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      lvl2PassedRaw, setLVL2PassedRaw )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      efPassedRaw, setEFPassedRaw )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      lvl2PassedThrough, setLVL2PassedThrough )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      efPassedThrough, setEFPassedThrough )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      lvl2Prescaled, setLVL2Prescaled )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      efPrescaled, setEFPrescaled )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      lvl2Resurrected, setLVL2Resurrected )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigDecision_v1, std::vector< uint32_t >,
                                      efResurrected, setEFResurrected )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Additions for AthenaMT Trigger / Run 3


   const std::vector< ::TrigCompositeUtils::DecisionID >& TrigDecision_v1::chainMTPassedRaw() const {
      static Accessor< std::vector< ::TrigCompositeUtils::DecisionID > > acc( "chainMTPassedRaw" );
      return acc( *this );
   }

   void TrigDecision_v1::setChainMTPassedRaw( std::vector< ::TrigCompositeUtils::DecisionID > value ) {
      std::sort(value.begin(), value.end()); // Clone vector and enforce weak ordering to be able to binary_search
      static Accessor< std::vector< ::TrigCompositeUtils::DecisionID > > acc( "chainMTPassedRaw" );
      acc( *this ) = value;
      return;
   }

   const std::vector< ::TrigCompositeUtils::DecisionID >& TrigDecision_v1::chainMTPrescaled() const {
      static Accessor< std::vector< ::TrigCompositeUtils::DecisionID > > acc( "chainMTPrescaled" );
      return acc( *this );
   }

   void TrigDecision_v1::setChainMTPrescaled( std::vector< ::TrigCompositeUtils::DecisionID > value ) {
      std::sort(value.begin(), value.end()); // Clone vector and enforce weak ordering to be able to binary_search
      static Accessor< std::vector< ::TrigCompositeUtils::DecisionID > > acc( "chainMTPrescaled" );
      acc( *this ) = value;
      return;
   }

   const std::vector< ::TrigCompositeUtils::DecisionID >& TrigDecision_v1::chainMTRerun() const {
      static Accessor< std::vector< ::TrigCompositeUtils::DecisionID > > acc( "chainMTRerun" );
      return acc( *this );
   }

   void TrigDecision_v1::setChainMTRerun( std::vector< ::TrigCompositeUtils::DecisionID > value ) {
      std::sort(value.begin(), value.end()); // Clone vector and enforce weak ordering to be able to binary_search
      static Accessor< std::vector< ::TrigCompositeUtils::DecisionID > > acc( "chainMTRerun" );
      acc( *this ) = value;
      return;
   }

   bool TrigDecision_v1::isPassed( ::TrigCompositeUtils::DecisionID id ) const {
      return std::binary_search(chainMTPassedRaw().begin(), chainMTPassedRaw().end(), id);
   }

   bool TrigDecision_v1::isPassed( const std::string& hltChainName ) const {
      return isPassed(::TrigConf::HLTUtils::string2hash( hltChainName, "Identifier" ));
   }

   bool TrigDecision_v1::isPrescaled( ::TrigCompositeUtils::DecisionID id ) const {
      return std::binary_search(chainMTPrescaled().begin(), chainMTPrescaled().end(), id);
   }

   bool TrigDecision_v1::isPrescaled( const std::string& hltChainName ) const {
      return isPrescaled(::TrigConf::HLTUtils::string2hash( hltChainName, "Identifier" ));
   }

   bool TrigDecision_v1::isRerun( ::TrigCompositeUtils::DecisionID id ) const {
      return std::binary_search(chainMTRerun().begin(), chainMTRerun().end(), id);
   }

   bool TrigDecision_v1::isRerun( const std::string& hltChainName ) const {
      return isRerun(::TrigConf::HLTUtils::string2hash( hltChainName, "Identifier" ));
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

/// Prints space separated positions of set bits from input bitset stored in vector
void writeBits(std::ostream& s, const std::vector< uint32_t >& input) {
   for (size_t i = 0; i < input.size(); ++i) {
      const uint32_t entry = input.at(i);
      const size_t bits = CHAR_BIT * sizeof( entry ); // This is expected to be 32 bits per entry in the vector
      for (size_t bit = 0; bit < bits; ++bit) {
         // Examine a single bit and, if 1, print its numeric position within the vector of uint32's.
         if (entry & (1 << bit)) s << ((bits * i) + bit) << " ";
      }
   }
   s << std::endl;
}

std::ostream& operator<<(std::ostream& s, const xAOD::TrigDecision_v1& td) {
   s << "xAODTrigDecision Object: SMK=" << td.smk() << " BGCode=" << (size_t)td.bgCode() << std::endl;
   s << " L1 Before Prescale: ";
   writeBits(s, td.tbp());
   s << " L1 After Prescale: ";
   writeBits(s, td.tap());
   s << " L1 After Veto: ";
   writeBits(s, td.tav());
   s << " Error Bits: L2Err=" << td.lvl2ErrorBits() << " L2Truncated=" << td.lvl2Truncated();
   s << " EF/HLTErr=" << td.efErrorBits() << " EF/HLTTrucated=" << td.efTruncated() << std::endl;
   // Run 1 & 2 support
   if (td.lvl2PassedPhysics().size()) {
      s << " L2 Prescaled: ";
      writeBits(s, td.lvl2Prescaled());
      s << " L2 Passed Physics: ";
      writeBits(s, td.lvl2PassedPhysics());
      s << " L2 Passed Raw: ";
      writeBits(s, td.lvl2PassedRaw());
      s << " L2 Resurrected/Rerun: ";
      writeBits(s, td.lvl2Resurrected());
      s << " L2 Passedthrough: ";
      writeBits(s, td.lvl2PassedThrough());
   }
   const std::string level = (td.lvl2PassedPhysics().size() ? "EF" : "HLT");
   if (td.efPassedPhysics().size()) { // Run 1: EF, Run 2: HLT
      s << " " << level << " Prescaled: ";
      writeBits(s, td.efPrescaled());
      s << " " << level << " Passed Physics: ";
      writeBits(s, td.efPassedPhysics());
      s << " " << level << " Passed Raw: ";
      writeBits(s, td.efPassedRaw());
      s << " " << level << " Resurrected/Rerun: ";
      writeBits(s, td.efResurrected());
      s << " " << level << " Passedthrough: ";
      writeBits(s, td.efPassedThrough());
   } else { // Run 3
      s << " HLT Prescaled: ";
      for (::TrigCompositeUtils::DecisionID ID : td.chainMTPrescaled()) s << ID << " ";
      s << std::endl << " HLT Passed Raw: ";
      for (::TrigCompositeUtils::DecisionID ID : td.chainMTPassedRaw()) s << ID << " ";
      s << std::endl << " HLT Resurrected/Rerun: ";
      for (::TrigCompositeUtils::DecisionID ID : td.chainMTRerun()) s << ID << " ";
      s << std::endl;
   }
   return s;
}
