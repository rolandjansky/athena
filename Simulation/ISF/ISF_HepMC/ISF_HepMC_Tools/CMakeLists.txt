################################################################################
# Package: ISF_HepMC_Tools
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Tools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# tag rootGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( ISF_HepMC_Tools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib GaudiKernel AtlasDetDescr AthenaBaseComps TruthUtils ISF_Event ISF_HepMC_Interfaces ISF_InterfacesLib PathResolver )

# Tests
atlas_add_test( GenParticleGenericFilter_test
                SOURCES test/GenParticleGenericFilter_test.cxx src/GenParticleGenericFilter.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} 
                LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} AtlasHepMCLib AthenaBaseComps ISF_HepMC_Interfaces )

# Tests
atlas_add_test( GenParticleInteractingFilter_test
                SOURCES test/GenParticleInteractingFilter_test.cxx src/GenParticleInteractingFilter.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} 
                LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} AtlasHepMCLib AthenaBaseComps TruthUtils ISF_HepMC_Interfaces )

# Tests
atlas_add_test( GenParticleLifetimeFilter_test
                SOURCES test/GenParticleLifetimeFilter_test.cxx src/GenParticleLifetimeFilter.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} 
                LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} AtlasHepMCLib AthenaBaseComps TruthUtils ISF_HepMC_Interfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

