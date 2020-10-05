################################################################################
# Package: BeamEffects
################################################################################

# Declare the package name:
atlas_subdir( BeamEffects )

# External dependencies:
find_package( CLHEP )
find_package( GTest )
find_package( Eigen REQUIRED )
#find_package( GMock )

# Needed to suppress warnings from eigen when compiling the test.
include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})

atlas_add_test( BeamEffectsAlg_test
                SOURCES src/*.cxx test/BeamEffectsAlg_test.cxx
                INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel TestTools AthenaBaseComps StoreGateLib EventInfo GeneratorObjects HepMC_InterfacesLib BeamSpotConditionsData ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
                )

#Added test of new component accumulator syntax
atlas_add_test( BeamEffectsAlgConfig_test
                SCRIPT python/BeamEffectsAlgConfig.py
                PROPERTIES TIMEOUT 300 )

atlas_add_component( BeamEffects
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps HepMC_InterfacesLib StoreGateLib EventInfo GeneratorObjects BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
