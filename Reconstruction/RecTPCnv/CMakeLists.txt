################################################################################
# Package: RecTPCnv
################################################################################

# Declare the package name:
atlas_subdir( RecTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventCommonTPCnv
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEventTPCnv
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEventCnv/TrkEventTPCnv
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthAllocators
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/AthenaPOOL/RootConversions )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( RecTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS RecTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEventTPCnv MissingETEvent muonEvent TrkMaterialOnTrack TrkEventTPCnv TestTools AthAllocators AthenaKernel CxxUtils RootConversions )

atlas_add_dictionary( RecTPCnvDict
                      RecTPCnv/RecTPCnvDict.h
                      RecTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEventTPCnv MissingETEvent muonEvent TrkMaterialOnTrack TrkEventTPCnv TestTools AthAllocators AthenaKernel CxxUtils RootConversions RecTPCnv )

atlas_add_dictionary( OLD_RecTPCnvDict
                      RecTPCnv/RecTPCnvDict.h
                      RecTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEventTPCnv MissingETEvent muonEvent TrkMaterialOnTrack TrkEventTPCnv TestTools AthAllocators AthenaKernel CxxUtils RootConversions RecTPCnv )

atlas_add_test( CaloEnergyCnv_p2_test
                SOURCES
                test/CaloEnergyCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEventTPCnv MissingETEvent muonEvent TrkMaterialOnTrack TrkEventTPCnv TestTools AthAllocators AthenaKernel CxxUtils RootConversions RecTPCnv )

atlas_add_test( MuonCaloEnergyContainerCnv_p1_test
                SOURCES
                test/MuonCaloEnergyContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEventTPCnv MissingETEvent muonEvent TrkMaterialOnTrack TrkEventTPCnv TestTools AthAllocators AthenaKernel CxxUtils RootConversions RecTPCnv )

