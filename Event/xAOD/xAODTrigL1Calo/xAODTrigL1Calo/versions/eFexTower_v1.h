/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGL1CALO_VERSIONS_EFEXTOWER_V1_H
#define XAODTRIGL1CALO_VERSIONS_EFEXTOWER_V1_H

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
 
    class eFexTower_v1 : public SG::AuxElement{
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
      uint32_t module() const; /// getter for the module number [0-23] inclusive
      /// set module number
      void setModule(uint32_t); /// setter for the above

      /// get fpga number
      uint8_t fpga() const; /// getter for the fpga number [0-3] inclusive
      ///  set fpga number
      void setFpga(uint8_t); /// setter for the above
      
      /// get Energy Counts
      const std::vector<uint16_t>& et_count() const; /// getter for the 11 energy counts
      /// set Energy Counts
      void setEt_count(const std::vector<uint16_t>&); /// setter for the above
      void setEt_count(std::vector<uint16_t>&&); /// setter for the above

      /// get em status bit
      uint32_t em_status() const; /// getter for the electromagnetic status bit
      /// set em status bit
      void setEm_status(uint32_t); /// setter for the above
      
      /// get em status bit
      uint32_t had_status() const; /// getter for hadronic status bit
      /// set em status bit
      void setHad_status(uint32_t); /// setter for the above

      void initialize(const float Eta,const float Phi);
      void initialize(const float Eta,const float Phi,
                                   const std::vector<uint16_t>& Et_count,
                                   const uint32_t Module,
                                   const uint8_t Fpga,
                                   const uint32_t Em_status,
                                   const uint32_t Had_status);
    
    private:
        
  }; // class eFexTower_v1
} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::eFexTower_v1, SG::AuxElement );
#endif // XAODTRIGL1CALO_VERSIONS_EFEXTOWER_V1_H
