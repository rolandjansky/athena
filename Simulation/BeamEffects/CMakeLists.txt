################################################################################
# Package: BeamEffects
################################################################################

# Declare the package name:
atlas_subdir( BeamEffects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventInfo
                          Generator/GeneratorObjects
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Simulation/Interfaces/HepMC_Interfaces
                          Simulation/Tools/AtlasCLHEP_RandomGenerators )

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
                INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HepMC_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HepMC_LIBRARIES} GaudiKernel TestTools AthenaBaseComps CxxUtils StoreGateLib EventInfo GeneratorObjects AtlasCLHEP_RandomGenerators ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
                 )

atlas_add_component( BeamEffects
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HepMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HepMC_LIBRARIES} GaudiKernel AthenaBaseComps CxxUtils StoreGateLib EventInfo GeneratorObjects AtlasCLHEP_RandomGenerators )

# Install files from the package:
atlas_install_headers( BeamEffects )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

