################################################################################
# Package: GeneratorObjects
################################################################################

# Declare the package name:
atlas_subdir( GeneratorObjects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/SGTools
                          Control/AthenaKernel
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Generators/AtlasHepMC
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          Control/RootUtils
                          Control/StoreGate )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( GeneratorObjects
                   src/HepMcParticleLink.cxx
                   src/McEventCollection.cxx
                   src/HijingEventParams.cxx
                   PUBLIC_HEADERS GeneratorObjects
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES  AtlasHepMCLib AthContainers AthLinks SGTools xAODTruth GaudiKernel RootUtilsPyROOT StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaKernel RootUtils )

atlas_add_dictionary( GeneratorObjectsDict
                      GeneratorObjects/McEventDict.h
                      GeneratorObjects/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}  AtlasHepMCLib AthContainers AthLinks SGTools xAODTruth GaudiKernel TestTools AthenaKernel RootUtils RootUtilsPyROOT StoreGateLib SGtests GeneratorObjects )

# Install a dummy rootmap file to suppress loading the rootmap from hepmc3,
# which defines dictionary entries for HepMC types that clash with
# what we generate here (and result in ROOT giving duplicate rootmap
# entry warnings).  See !31633.
atlas_install_generic ( share/libHepMC3rootIO.rootmap
                        DESTINATION ${CMAKE_INSTALL_LIBDIR}
                        BUILD_DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

atlas_add_test( HepMcParticleLink_test
                SOURCES test/HepMcParticleLink_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthContainers CxxUtils AthLinks SGTools xAODTruth GaudiKernel TestTools AthenaKernel RootUtils RootUtilsPyROOT StoreGateLib SGtests GeneratorObjects ${GTEST_LIBRARIES} )


atlas_add_test( CachedParticlePtr_test
                SOURCES test/CachedParticlePtr_test.cxx
                LINK_LIBRARIES GeneratorObjects AtlasHepMCLib)

