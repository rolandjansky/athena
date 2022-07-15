/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/eFexTower_v1.h"

namespace xAOD{  

  AUXSTORE_OBJECT_SETTER_AND_GETTER( eFexTower_v1 , std::vector<uint16_t> , et_count , setEt_count )
  AUXSTORE_OBJECT_MOVE( eFexTower_v1 , std::vector<uint16_t> , et_count , setEt_count )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTower_v1, float , eta , setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTower_v1, float , phi , setPhi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTower_v1, uint32_t , module , setModule )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTower_v1, uint8_t , fpga , setFpga )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTower_v1, uint32_t , em_status , setEm_status )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTower_v1, uint32_t , had_status , setHad_status )  
  
  
  /// initialize
  void eFexTower_v1::initialize(const float Eta,const float Phi)
  {
    setEta( Eta );
    setPhi( Phi );    
  }
  
  void eFexTower_v1::initialize(const float Eta,const float Phi,
                                   const std::vector<uint16_t>& Et_count,
                                   const uint32_t Module,
                                   const uint8_t Fpga,
                                   const uint32_t Em_status,
				   const uint32_t Had_status)
  {
    setEta( Eta );
    setPhi( Phi );
    setEt_count( Et_count );
    setModule( Module );
    setFpga( Fpga );
    setEm_status( Em_status );
    setHad_status( Had_status );
  }

} // namespace xAOD
