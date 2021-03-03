# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EvtGen_i )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepPDT )
if (HEPMC3_USE)
  find_package( Photospp COMPONENTS Photospp PhotosppHepMC3 )
  find_package( EvtGen  COMPONENTS EvtGen EvtGenExternal )
  find_package( Tauolapp COMPONENTS TauolaHepMC3 )
else()
  find_package( Photospp COMPONENTS Photospp PhotosppHepMC )
  find_package( EvtGen )
  find_package( Tauolapp COMPONENTS TauolaHepMC )
endif()
find_package( Pythia8 )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
if (HEPMC3_USE)
atlas_add_library( EvtGen_iLib
                   src/EvtInclusiveDecay.cxx
                   PUBLIC_HEADERS EvtGen_i
                   DEFINITIONS -DEVTGEN_PYTHIA -DEVTGEN_PHOTOS -DEVTGEN_TAUOLA 
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS}  ${EVTGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} AtlasHepMCLib ${EVTGEN_LIBRARIES} GaudiKernel GeneratorObjects ${ROOT_LIBRARIES} StoreGateLib SGtests GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel PathResolver )
else()
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
                   DEFINITIONS -DEVTGEN_PYTHIA -DEVTGEN_PHOTOS -DEVTGEN_TAUOLA 
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS}  ${EVTGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} AtlasHepMCLib ${EVTGEN_LIBRARIES} GaudiKernel GeneratorObjects ${ROOT_LIBRARIES} StoreGateLib SGtests GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel PathResolver )
endif()

atlas_add_component( EvtGen_i
                     src/components/*.cxx
                     LINK_LIBRARIES EvtGen_iLib ) 

# Install files from the package:
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.DEC share/file/*.table share/file/*.dec share/file/*.pdt share/file/*.dat )

