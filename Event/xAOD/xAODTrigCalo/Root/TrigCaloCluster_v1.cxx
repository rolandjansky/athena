/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloCluster_v1.cxx 596561 2014-05-12 12:10:42Z krasznaa $

// System include(s):
#include <iostream>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloCluster_v1.h"

namespace xAOD {

  static const size_t SIZE =
#if __cplusplus >= 201100
     CaloSampling::CaloSample::Unknown;
#else
     CaloSampling::Unknown;
#endif // C++11

  // Constructor
  TrigCaloCluster_v1::TrigCaloCluster_v1() {}

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, float, rawEnergy,
                                        setRawEnergy )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, float, rawEt,
                                        setRawEt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, float, rawEta,
                                        setRawEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, float, rawPhi,
                                        setRawPhi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, long, RoIword,
                                        setRoIword )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, int, nCells,
                                        setNCells )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigCaloCluster_v1, unsigned int,
                                        clusterQuality, setClusterQuality )

  float TrigCaloCluster_v1::rawEnergy(const CaloSample sampling) const {
    static Accessor< std::vector <float > > eAcc("rawEnergySample");
    const std::vector<float>& var = eAcc( *this );
    if( var.size() <= static_cast< size_t >( sampling ) ) {
       std::cerr << "xAOD::TrigCaloCluster_v1::rawEnergy ERROR Invalid/unknown "
                 << "sampling (" << sampling << ") requested" << std::endl;
       return 0;
    }
    return var[ sampling ];
  }

  bool TrigCaloCluster_v1::setRawEnergy(const CaloSample sampling,
                                        const float theEnergy) {
    static Accessor< std::vector <float > > eAcc("rawEnergySample");
    std::vector<float>& var = eAcc( *this );
    if( var.size() < SIZE ) var.resize( SIZE );
    if( var.size() <= static_cast< size_t >( sampling ) ) {
       std::cerr << "xAOD::TrigCaloCluster_v1::setRawEnergy ERROR "
                 << "Invalid/unknown sampling (" << sampling
                 << ") requested" << std::endl;
       return false;
    }
    var[ sampling ] = theEnergy;
    return true;
  }

  AUXSTORE_OBJECT_GETTER( TrigCaloCluster_v1, std::vector< float >,
                          rawEnergySample )

} // namespace xAOD
