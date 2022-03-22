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
                          Control/AthenaKernel 
                          Tools/PathResolver )



# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( EvtGen )
find_package( HepMC )
find_package( HepPDT )
find_package( Photospp COMPONENTS Photospp PhotosppHepMC )
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
                   src/Belle2/EvtB0toKsKK.cc
                   src/Belle2/EvtBCL.cc
                   src/Belle2/EvtBCLFF.cc
                   src/Belle2/EvtBGL.cc
                   src/Belle2/EvtBGLFF.cc
                   src/Belle2/EvtBSemiTauonic.cc
                   src/Belle2/EvtBSemiTauonic2HDMType2.cc
		   src/Belle2/EvtBSemiTauonicAmplitude.cc
		   src/Belle2/EvtBSemiTauonicHelicityAmplitudeCalculator.cc
		   src/Belle2/EvtBSemiTauonicScalarMesonAmplitude.cc
		   src/Belle2/EvtBSemiTauonicVectorMesonAmplitude.cc
		   src/Belle2/EvtEtaFullDalitz.cc
 		   src/Belle2/EvtEtaPi0Dalitz.cc
 		   src/Belle2/EvtEtaPrimeDalitz.cc
 		   src/Belle2/EvtHQET3.cc
		   src/Belle2/EvtHQET3FF.cc
 		   src/Belle2/EvtLLSW.cc
		   src/Belle2/EvtLLSWFF.cc
 		   src/Belle2/EvtPHSPBMix.cc
 		   src/Belle2/EvtYmSToYnSpipiCLEOboost.cc
                   PUBLIC_HEADERS EvtGen_i
                   DEFINITIONS -DEVTGEN_PYTHIA -DEVTGEN_PHOTOS -DEVTGEN_TAUOLA
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EVTGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} ${HEPMC_LIBRARIES} ${EVTGEN_LIBRARIES} GaudiKernel GeneratorObjects ${ROOT_LIBRARIES} StoreGateLib SGtests GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel PathResolver )

atlas_add_component( EvtGen_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EVTGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} ${HEPMC_LIBRARIES} ${EVTGEN_LIBRARIES} StoreGateLib SGtests GaudiKernel GeneratorModulesLib GeneratorObjects AthenaKernel PathResolver EvtGen_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.DEC share/file/*.table share/file/*.dec share/file/*.pdt share/file/*.dat )

set( EvtGenEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of EvtGenEnvironment.cmake" )
find_package( EvtGenEnvironment )


