/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexSumETRoI.h"

namespace xAOD {

    jFexSumETRoI_v1::jFexSumETRoI_v1()
        : SG::AuxElement() {
    }
    void jFexSumETRoI_v1::initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, int resolution) {

        setTobWord     ( tobWord           );
        setjFexNumber  ( jFexNumber        );
        setfpgaNumber  ( fpgaNumber        );
        setTobEt_lower ( unpackEt_lower()  );
        setTobEt_upper ( unpackEt_upper()  );
        setTobSat_lower( unpackSat_lower() );
        setTobSat_upper( unpackSat_upper() );
        setResolution  ( resolution        );
        
        //include in future when xTOB in jFEX has been implemented.

        // If the object is a TOB then the isTOB should be true.
        // For xTOB default is false, but should be set if a matching TOB is found
        // if (type() == TOB) setIsTOB(1);
        // else               setIsTOB(0);

        return;
    }

    //----------------
    /// Raw data words
    //----------------

    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint32_t, tobWord   , setTobWord   )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t , jFexNumber, setjFexNumber)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t , fpgaNumber, setfpgaNumber)
    /// Only calculable externally

    /// Extracted from data words, stored for convenience
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint16_t, tobEt_lower , setTobEt_lower )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint16_t, tobEt_upper , setTobEt_upper )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t , tobSat_lower, setTobSat_lower)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t , tobSat_upper, setTobSat_upper)
   
   ///Setting the jFEX ET resolution
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, int  , tobEtScale, setResolution)    
    

    //-----------------
    /// Methods to decode data from the TOB/RoI and return to the user
    //-----------------

    //include in future when xTOB in jFEX has been implemented.

    /// TOB or xTOB?
    //jFexSumETRoI_v1::ObjectType jFexSumETRoI_v1::type() const {
    //if (Word1() == 0) return TOB;
    //else              return xTOB;
    //}

    //Raw ET_lower on TOB scale (200 MeV/count)
    uint16_t jFexSumETRoI_v1::unpackEt_lower() const {
        return (tobWord() >> s_Et_lowerBit) & s_Et_lowerMask;
    }
    
    //Raw ET_upper on TOB scale (200 MeV/count)
    uint16_t jFexSumETRoI_v1::unpackEt_upper() const {
        return (tobWord() >> s_Et_upperBit) & s_Et_upperMask;
    }

    //Return sat Et_lower flag
    uint8_t jFexSumETRoI_v1::unpackSat_lower() const {
        return (tobWord() >> s_Sat_lowerBit) & s_Sat_lowerMask;
    }
    //Return sat Et_upper flag
    uint8_t jFexSumETRoI_v1::unpackSat_upper() const {
        return (tobWord() >> s_Sat_upperBit) & s_Sat_upperMask;
    }

    /// Methods that require combining results or applying scales

    /// Et_lower in 1 MeV scale
    int jFexSumETRoI_v1::Et_lower() const {
        return tobEt_lower()*tobEtScale();
    }

    /// Et_upper in 1 MeV scale
    int jFexSumETRoI_v1::Et_upper() const {
        return tobEt_upper()*tobEtScale();
    }


} // namespace xAOD

