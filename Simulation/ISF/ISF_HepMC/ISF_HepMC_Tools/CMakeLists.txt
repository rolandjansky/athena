################################################################################
# Package: ISF_HepMC_Tools
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Tools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Generators/TruthUtils
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_HepMC/ISF_HepMC_Interfaces
                          Tools/PathResolver )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# tag rootGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( ISF_HepMC_Tools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps TruthUtils ISF_Event ISF_Interfaces PathResolver )

# Tests
atlas_add_test( GenParticleGenericFilter_test
                SOURCES test/GenParticleGenericFilter_test.cxx src/GenParticleGenericFilter.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps )

# Install files from the package:
atlas_install_headers( ISF_HepMC_Tools )
atlas_install_python_modules( python/*.py )

