################################################################################
# Package: EvtGen_i
################################################################################

# Declare the package name:
atlas_subdir( EvtGen_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/GeneratorObjects
                          PRIVATE
                          Control/AthenaKernel )

# Set the version of Tauolapp to use:
set( TAUOLAPP_VERSION 1.1.6 )
set( TAUOLAPP_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/tauola++/${TAUOLAPP_VERSION}/${ATLAS_PLATFORM} )

# Set the version of Photospp to use:
set( PHOTOSPP_VERSION 3.61 )
set( PHOTOSPP_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/photos++/${PHOTOSPP_VERSION}/${ATLAS_PLATFORM} )

# Set the version of EvtGen to use:
set( EVTGEN_VERSION 1.6.0 )
set( EVTGEN_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/evtgen/${EVTGEN_VERSION}/${ATLAS_PLATFORM} )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( EvtGen )
find_package( HepMC )
find_package( HepPDT )
find_package( Photospp )
find_package( Pythia8 )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )
find_package( Tauolapp )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( EvtGen_iLib
                   src/EvtPythia.cxx
                   src/EvtTauola.cxx
                   src/EvtExternalGenFactory.cxx
                   src/EvtPHOTOS.cxx
                   src/EvtPythia6CommandConverter.cxx
                   src/EvtTauolaEngine.cxx
                   src/EvtExternalList.cxx
                   src/EvtPhotosEngine.cxx
                   src/EvtPythiaEngine.cxx
                   src/EvtInclusiveDecay.cxx
                   PUBLIC_HEADERS EvtGen_i
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EVTGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} ${HEPMC_LIBRARIES} ${EVTGEN_LIBRARIES} GaudiKernel GeneratorObjects ${ROOT_LIBRARIES} StoreGateLib SGtests GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel )

atlas_add_component( EvtGen_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EVTGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} ${HEPMC_LIBRARIES} ${EVTGEN_LIBRARIES} StoreGateLib SGtests GaudiKernel GeneratorModulesLib GeneratorObjects AthenaKernel EvtGen_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.DEC share/*.table share/*.dec share/*.pdt )

