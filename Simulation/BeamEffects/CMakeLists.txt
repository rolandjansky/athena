################################################################################
# Package: BeamEffects
################################################################################

# Declare the package name:
atlas_subdir( BeamEffects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Control/AthenaBaseComps
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Control/CxxUtils
                          Generator/GeneratorObjects
                          Simulation/Interfaces/HepMC_Interfaces
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Control/StoreGate
                          AtlasTest/TestTools )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( GTest )
#find_package( GMock )

atlas_add_test( BeamEffectsAlg_test
                SOURCES src/*.cxx test/BeamEffectsAlg_test.cxx
                INCLUDE_DIRS  ${CLHEP_INCLUDE_DIRS} ${HepMC_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES  ${CLHEP_LIBRARIES} ${HepMC_LIBRARIES} GaudiKernel AthenaBaseComps AtlasCLHEP_RandomGenerators CxxUtils GeneratorObjects HepMC_Interfaces InDetBeamSpotService StoreGate TestTools ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
                 )

atlas_add_component( BeamEffects
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HepMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HepMC_LIBRARIES} GaudiKernel AthenaBaseComps AtlasCLHEP_RandomGenerators CxxUtils GeneratorObjects HepMC_Interfaces InDetBeamSpotService StoreGate )

# Install files from the package:
atlas_install_python_modules( python/*.py )


