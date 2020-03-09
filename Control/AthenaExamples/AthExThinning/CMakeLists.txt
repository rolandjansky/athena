################################################################################
# Package: AthExThinning
################################################################################

# Declare the package name:
atlas_subdir( AthExThinning )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaPython
                          Control/AthenaServices
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthExThinningEvent
                   src_lib/AthExIParticle.cxx
                   src_lib/AthExIParticles.cxx
                   src_lib/AthExParticle.cxx
                   src_lib/AthExParticles.cxx
                   src_lib/AthExElephantino.cxx
                   src_lib/AthExDecay.cxx
                   src_lib/AthExFatObject.cxx
                   src_lib/AthExElephantino_p1.cxx
                   src_lib/AthExDecay_p1.cxx
                   src_lib/AthExParticles_p1.cxx
                   src_lib/AthExFatObject_p1.cxx
                   PUBLIC_HEADERS AthExThinning
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} )

atlas_add_component( AthExThinningAlgs
                     src_lib/CreateData.cxx
                     src_lib/WriteThinnedData.cxx
                     src_lib/ReadThinnedData.cxx
                     src_lib/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel DataModelAthenaPoolLib AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthExThinningEvent )

atlas_add_poolcnv_library( AthExThinningPoolCnv
                           src/*.cxx
                           FILES AthExThinning/AthExParticles.h AthExThinning/AthExIParticles.h AthExThinning/AthExDecay.h AthExThinning/AthExElephantino.h AthExThinning/AthExFatObject.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel DataModelAthenaPoolLib AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthExThinningEvent )

atlas_add_dictionary( AthExThinningEventDict
                      AthExThinning/AthExThinningEventDict.h
                      AthExThinning/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel DataModelAthenaPoolLib AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthExThinningEvent
                      ELEMENT_LINKS  AthExParticles AthExIParticles )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.ref )


atlas_add_test( test_thinning
                SCRIPT test/test_thinning.sh
                PROPERTIES TIMEOUT 900
                LOG_IGNORE_PATTERN ":::running" )

atlas_add_test( test_pythinning
                SCRIPT test/test_pythinning.sh
                PROPERTIES TIMEOUT 900
                LOG_IGNORE_PATTERN ":::running" )
set_tests_properties( AthExThinning_test_pythinning_ctest
                      PROPERTIES DEPENDS AthExThinning_test_thinning_ctest )
