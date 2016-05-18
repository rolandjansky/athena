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
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/RootUtils
                          Control/StoreGate )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( GeneratorObjects
                   src/HepMcParticleLink.cxx
                   src/McEventCollection.cxx
                   src/HijingEventParams.cxx
                   PUBLIC_HEADERS GeneratorObjects
                   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPMC_LIBRARIES} AthContainers AthLinks SGTools xAODTruth GaudiKernel RootUtilsPyROOT StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaKernel RootUtils )

atlas_add_dictionary( GeneratorObjectsDict
                      GeneratorObjects/McEventDict.h
                      GeneratorObjects/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthContainers AthLinks SGTools xAODTruth GaudiKernel TestTools AthenaKernel RootUtils RootUtilsPyROOT StoreGateLib SGtests GeneratorObjects )

atlas_add_test( HepMcParticleLink_test
                SOURCES
                test/HepMcParticleLink_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthContainers AthLinks SGTools xAODTruth GaudiKernel TestTools AthenaKernel RootUtils RootUtilsPyROOT StoreGateLib SGtests GeneratorObjects
                EXTRA_PATTERNS "ClassIDSvc Initialized successfully|^JobOptionsSvc +INFO"
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_joboptions( share/HepMcParticleLink_test.txt )

