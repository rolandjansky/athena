################################################################################
# Package: TrigParticleTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigParticleTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventCommonTPCnv
                          Event/FourMom
                          Reconstruction/Particle
                          Reconstruction/TrackParticleTPCnv
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigCaloEventTPCnv
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigInDetEventTPCnv
                          Trigger/TrigEvent/TrigParticle
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/SGTools
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigParticleTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigParticleTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel )

atlas_add_dictionary( TrigParticleTPCnvDict
                      TrigParticleTPCnv/TrigParticleTPCnvDict.h
                      TrigParticleTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_dictionary( OLD_TrigParticleTPCnvDict
                      TrigParticleTPCnv/TrigParticleTPCnvDict.h
                      TrigParticleTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigEFBjetCnv_p1_test
                SOURCES
                test/TrigEFBjetCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigEFBjetCnv_p2_test
                SOURCES
                test/TrigEFBjetCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigEFBphysCnv_p1_test
                SOURCES
                test/TrigEFBphysCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigEFBphysCnv_p2_test
                SOURCES
                test/TrigEFBphysCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigEFBphysCnv_p3_test
                SOURCES
                test/TrigEFBphysCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigElectronCnv_p2_test
                SOURCES
                test/TrigElectronCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigElectronCnv_p3_test
                SOURCES
                test/TrigElectronCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigL2BjetCnv_p1_test
                SOURCES
                test/TrigL2BjetCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigL2BjetCnv_p2_test
                SOURCES
                test/TrigL2BjetCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigL2BjetCnv_p3_test
                SOURCES
                test/TrigL2BjetCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigL2BphysCnv_p1_test
                SOURCES
                test/TrigL2BphysCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigL2BphysCnv_p2_test
                SOURCES
                test/TrigL2BphysCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigL2BphysCnv_p3_test
                SOURCES
                test/TrigL2BphysCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigPhotonCnv_p2_test
                SOURCES
                test/TrigPhotonCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigPhotonCnv_p3_test
                SOURCES
                test/TrigPhotonCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigTauCnv_p1_test
                SOURCES
                test/TrigTauCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigTauCnv_p2_test
                SOURCES
                test/TrigTauCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

atlas_add_test( TrigTauCnv_p3_test
                SOURCES
                test/TrigTauCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrackParticleTPCnv TrigCaloEvent TrigCaloEventTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigParticle TestTools AthenaKernel SGTools GaudiKernel TrigParticleTPCnv )

