# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigParticleTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigParticleTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigParticleTPCnv
                         LINK_LIBRARIES AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities DataModelAthenaPoolLib EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle )

atlas_add_dictionary( TrigParticleTPCnvDict
                      TrigParticleTPCnv/TrigParticleTPCnvDict.h
                      TrigParticleTPCnv/selection.xml
                      LINK_LIBRARIES TrigParticleTPCnv )

atlas_add_dictionary( OLD_TrigParticleTPCnvDict
                      TrigParticleTPCnv/TrigParticleTPCnvDict.h
                      TrigParticleTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TrigParticleTPCnv )

# Tests:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES TrigParticleTPCnv )
endfunction()

_add_test( TrigEFBjetCnv_p1_test )
_add_test( TrigEFBjetCnv_p2_test )
_add_test( TrigEFBphysCnv_p1_test )
_add_test( TrigEFBphysCnv_p2_test )
_add_test( TrigEFBphysCnv_p3_test )
_add_test( TrigElectronCnv_p2_test )
_add_test( TrigElectronCnv_p3_test )
_add_test( TrigL2BjetCnv_p1_test )
_add_test( TrigL2BjetCnv_p2_test )
_add_test( TrigL2BjetCnv_p3_test )
_add_test( TrigL2BphysCnv_p1_test )
_add_test( TrigL2BphysCnv_p2_test )
_add_test( TrigL2BphysCnv_p3_test )
_add_test( TrigPhotonCnv_p2_test )
_add_test( TrigPhotonCnv_p3_test )
_add_test( TrigTauCnv_p1_test )
_add_test( TrigTauCnv_p2_test )
_add_test( TrigTauCnv_p3_test )
