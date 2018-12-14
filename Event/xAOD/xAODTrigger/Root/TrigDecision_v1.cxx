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


namespace xAOD {

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
   // Run 1 
   bool r1 = false;
   if (td.lvl2Prescaled().size() || td.lvl2PassedPhysics().size() || td.lvl2PassedRaw().size() 
    || td.lvl2Resurrected().size() || td.lvl2PassedThrough().size() ) {
      r1 = true;
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
   } else {
      s << " No L2 Bits" << std::endl;
   }
   const std::string level = (r1 ? " EF" : " HLT");
   if (td.efPrescaled().size() || td.efPassedPhysics().size() || td.efPassedRaw().size() 
    || td.efResurrected().size() || td.efPassedThrough().size() ) { // Run 1: EF, Runs 2, 3: HLT
      s << level << " Prescaled: ";
      writeBits(s, td.efPrescaled());
      s << level << " Passed Physics: ";
      writeBits(s, td.efPassedPhysics());
      s << level << " Passed Raw: ";
      writeBits(s, td.efPassedRaw());
      s << level << " Resurrected/Rerun: ";
      writeBits(s, td.efResurrected());
      s << level << " Passedthrough: ";
      writeBits(s, td.efPassedThrough());
   } else {
      s << " No EF/HLT Bits" << std::endl;
   }
   return s;
}


} // namespace xAOD

