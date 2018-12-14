################################################################################
# Package: GeneratorObjectsTPCnv
################################################################################

# Declare the package name:
atlas_subdir( GeneratorObjectsTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Generators/GeneratorObjects
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthAllocators
                          Control/AthenaKernel
                          Generators/GenInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( GeneratorObjectsTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS GeneratorObjectsTPCnv
                         INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects AthAllocators AthenaKernel TestTools )

atlas_add_dictionary( GeneratorObjectsTPCnvDict
                      GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
                      GeneratorObjectsTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects AthAllocators AthenaKernel GeneratorObjectsTPCnv TestTools )

atlas_add_dictionary( OLD_GeneratorObjectsTPCnvDict
                      GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
                      GeneratorObjectsTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects AthAllocators AthenaKernel GeneratorObjectsTPCnv TestTools )

atlas_add_test( McEventCollectionCnv_p4_test
                SOURCES
                test/McEventCollectionCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects TestTools StoreGateLib SGtests GeneratorObjectsTPCnv )

atlas_add_test( McEventCollectionCnv_p5_test
                SOURCES
                test/McEventCollectionCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects TestTools StoreGateLib SGtests GeneratorObjectsTPCnv )

atlas_add_test( HepMcParticleLinkCnv_p1_test
                SOURCES
                test/HepMcParticleLinkCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects TestTools StoreGateLib SGtests GeneratorObjectsTPCnv )

atlas_add_test( HepMcParticleLinkCnv_p2_test
                SOURCES
                test/HepMcParticleLinkCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects TestTools StoreGateLib SGtests GeneratorObjectsTPCnv )
