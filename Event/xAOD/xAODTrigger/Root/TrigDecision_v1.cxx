/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecision_v1.cxx 581968 2014-02-06 16:08:52Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TrigDecision_v1.h"

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
