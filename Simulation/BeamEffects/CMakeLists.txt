################################################################################
# Package: BeamEffects
################################################################################

# Declare the package name:
atlas_subdir( BeamEffects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Simulation/Interfaces/HepMC_Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( GTest )
find_package( Eigen REQUIRED )
#find_package( GMock )

# Needed to suppress warnings from eigen when compiling the test.
include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})

atlas_add_test( BeamEffectsAlg_test
                SOURCES src/*.cxx test/BeamEffectsAlg_test.cxx
                INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel TestTools AthenaBaseComps CxxUtils StoreGateLib EventInfo GeneratorObjects BeamSpotConditionsData ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
                EXTRA_PATTERNS "[0-9]+ ms"
                 )


#Added test of new component accumulator syntax
atlas_add_test( BeamEffectsAlgConfig_test
                SCRIPT python/BeamEffectsAlgConfig.py
                EXTRA_PATTERNS "^Domain"
                PROPERTIES TIMEOUT 300 )


atlas_add_component( BeamEffects
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps CxxUtils StoreGateLib EventInfo GeneratorObjects BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

