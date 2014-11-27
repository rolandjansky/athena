/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMCluster_v1.cxx 596561 2014-05-12 12:10:42Z krasznaa $

// System include(s):
#include <iostream>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMCluster_v1.h"

namespace xAOD {

  static const size_t SIZE =
#if __cplusplus >= 201100
     CaloSampling::CaloSample::Unknown;
#else
     CaloSampling::Unknown;
#endif // C++11

  // Constructor
  TrigEMCluster_v1::TrigEMCluster_v1() : TrigCaloCluster() {}

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, energy,
                                        setEnergy )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, et,
                                        setEt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, eta,
                                        setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, phi,
                                        setPhi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, e237,
                                        setE237 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, e277,
                                        setE277 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, fracs1,
                                        setFracs1 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, weta2,
                                        setWeta2 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, ehad1,
                                        setEhad1 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, eta1,
                                        setEta1 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, emaxs1,
                                        setEmaxs1 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, e2tsts1,
                                        setE2tsts1 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, e233,
                                        setE233 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigEMCluster_v1, float, wstot,
                                        setWstot )

  float TrigEMCluster_v1::energy(const CaloSample sampling) const {
    static Accessor< std::vector <float > > eAcc("energySample");
    const std::vector<float>& var = eAcc( *this );
    if( var.size() <= static_cast< size_t >( sampling ) ) {
       std::cerr << "xAOD::TrigEMCluster_v1::energy ERROR Invalid/unknown "
                 << "sampling (" << sampling << ") requested" << std::endl;
       return 0;
    }
    return var[ sampling ];
  }

  bool TrigEMCluster_v1::setEnergy(const CaloSample sampling,
                                   const float theEnergy) {
    static Accessor< std::vector <float > > eAcc("energySample");
    std::vector<float>& var = eAcc( *this );
    if( var.size() < SIZE ) var.resize( SIZE );
    if( var.size() <= static_cast< size_t >( sampling ) ) {
       std::cerr << "xAOD::TrigEMCluster_v1::setEnergy ERROR "
                 << "Invalid/unknown sampling (" << sampling
                 << ") requested" << std::endl;
       return false;
    }
    var[ sampling ] = theEnergy;
    return true;
  }

  AUXSTORE_OBJECT_GETTER( TrigEMCluster_v1, std::vector< float >,
                          energySample )

} // namespace xAOD
