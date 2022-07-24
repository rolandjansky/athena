/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGL1CALO_VERSIONS_JFEXTOWER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JFEXTOWER_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

   /// Class describing input data of a LVL1 eFEX
   //  in the xAOD format.
 
    class jFexTower_v1 : public SG::AuxElement{
    public:

      /// Inherit all of the base class's constructors 
      using SG::AuxElement::AuxElement;

      /// @brief The pseudorapidity (\f$\eta\f$)
      float eta() const; /// getter for the global eta value (float)
      void setEta(float); /// setter for the above

      /// @brief The azimuthal angle (\f$\phi\f$)     
      float phi() const; /// getter for the global phi value (float)
      void setPhi(float); /// setter for the above

      /// get module number
      uint8_t module() const; /// getter for the module number [0-5] inclusive
      /// set module number
      void setModule(uint8_t); /// setter for the above

      /// get fpga number
      uint8_t fpga() const; /// getter for the fpga number [0-3] inclusive
      ///  set fpga number
      void setFpga(uint8_t); /// setter for the above

      /// get channel number
      uint8_t channel() const; /// getter for the channel number [0-59]
      ///  set fpga number
      void setChannel(uint8_t); /// setter for the above
      
      /// get Energy Value
      uint16_t et() const; /// getter for the energy value
      /// set Energy Value
      void setEt(uint16_t); /// setter for the above

      /// get the location where Et is placed in the data stream
      uint8_t dataID() const; /// getter for the location in data stream where Et is placed [0-15]
      /// set the location where Et is placed in the data stream
      void setDataID(uint8_t); /// setter for the above

      /// Is jTower saturated?
      char isSaturated() const; /// getter for the saturation flag of jTower
      /// set saturation flag of jTower
      void setIsSaturated(char); ///setter for the above

      void initialize(const float Eta,const float Phi);
      void initialize(const float Eta,const float Phi,
                                   const uint16_t Et,
                                   const uint8_t Module,
                                   const uint8_t Fpga,
				   const uint8_t Channel,
				   const uint8_t DataID,
				   const char IsSaturated);

    
    private:
        
  }; // class jFexTower_v1
} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexTower_v1, SG::AuxElement );
#endif // XAODTRIGL1CALO_VERSIONS_JFEXTOWER_V1_H
