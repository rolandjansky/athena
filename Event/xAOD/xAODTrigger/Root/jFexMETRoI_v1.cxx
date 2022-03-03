/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexMETRoI.h"

namespace xAOD {

    jFexMETRoI_v1::jFexMETRoI_v1()
        : SG::AuxElement() {
    }
    void jFexMETRoI_v1::initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, int resolution) {

        setTobWord   ( tobWord     );
        setjFexNumber( jFexNumber  );
        setfpgaNumber( fpgaNumber  );
        setTobEx     ( unpackEx()  );
        setTobEy     ( unpackEy()  );
        setTobRes    ( unpackRes() );
        setTobSat    ( unpackSat() );
        setResolution( resolution ); 

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

    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint32_t, tobWord   , setTobWord   )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint8_t , jFexNumber, setjFexNumber)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint8_t , fpgaNumber, setfpgaNumber)
    /// Only calculable externally

    /// Extracted from data words, stored for convenience
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, int    , tobEx ,  setTobEx )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, int    , tobEy ,  setTobEy )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint8_t, tobRes, setTobRes )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint8_t, tobSat, setTobSat )

   ///Setting the jFEX ET resolution
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, int  , tobEtScale, setResolution)
   
    //-----------------
    /// Methods to decode data from the TOB/RoI and return to the user
    //-----------------

    //include in future when xTOB in jFEX has been implemented.

    /// TOB or xTOB?
    //jFexMETRoI_v1::ObjectType jFexMETRoI_v1::type() const {
    //if (Word1() == 0) return TOB;
    //else              return xTOB;
    //}


    //Return tobEx in a 200 MeV scale
    int jFexMETRoI_v1::unpackEx() const {
        int energy = (tobWord() >> s_Ex_Bit) & s_E_xMask; // masking from the TOB word the Ex
        bool sign = (energy >> s_signed_E) == 1 ; //checks if the value is negative looking at the MSB within the 15 bits
        if(sign) {
            energy = -(~(energy-1) & s_E_xMask); // Energy is negative, then using the two's complement method we do absolute value, mask the 15 bits and assign a minus
        }
        return energy;
    }

    //Return tobEy in a 200 MeV scale
    int jFexMETRoI_v1::unpackEy() const {
        int energy = (tobWord() >> s_Ey_Bit) & s_E_yMask; // masking from the TOB word the Ey
        bool  sign = (energy >> s_signed_E) == 1 ;//checks if the value is negative looking at the MSB within the 15 bits
        if(sign) {
            energy = -(~(energy-1) & s_E_yMask); // Energy is negative, then using the two's complement method we do absolute value, mask the 15 bits and assign a minus
        }
        return energy;
    }

    //Return tobSat upper flag
    unsigned int jFexMETRoI_v1::unpackSat() const {
        return (tobWord() >> s_SatBit) & s_SatMask;
    }

    //Return tobRes lower flag
    unsigned int jFexMETRoI_v1::unpackRes() const {
        return (tobWord() >> s_ResBit) & s_ResMask;
    }

    /// Methods that require combining results or applying scales

    /// Ex in 1 MeV scale
    int jFexMETRoI_v1::Ex() const {
        return tobEx()*tobEtScale();
    }

    /// Ey in 1 MeV scale
    int jFexMETRoI_v1::Ey() const {
        return tobEy()*tobEtScale();
    }


} // namespace xAOD

