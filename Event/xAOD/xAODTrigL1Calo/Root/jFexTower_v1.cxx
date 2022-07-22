/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/jFexTower_v1.h"

namespace xAOD{  

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, float , eta , setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, float , phi , setPhi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, uint8_t , module , setModule )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, uint8_t , fpga , setFpga )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, uint8_t , channel , setChannel )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, uint16_t , et , setEt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, uint8_t , dataID , setDataID )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTower_v1, char , isSaturated , setIsSaturated )
  
  /// initialize
  void jFexTower_v1::initialize(const float Eta,const float Phi)
  {
    setEta( Eta );
    setPhi( Phi );    
  }
  
  void jFexTower_v1::initialize(const float Eta,const float Phi,
                                   const uint16_t Et,
                                   const uint8_t Module,
                                   const uint8_t Fpga,
				   const uint8_t Channel,
				   const uint8_t DataID,
				   const char IsSaturated)
  {
    setEta( Eta );
    setPhi( Phi );
    setEt( Et );
    setModule( Module );
    setFpga( Fpga );
    setChannel( Channel );
    setDataID( DataID );
    setIsSaturated( IsSaturated );
  }

} // namespace xAOD
