# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EvtGen_i )

#set( PHOTOSPP_VERSION 3.61 )
#set( PHOTOSPP_ROOT
#   /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/photos++/${PHOTOSPP_VERSION}/${LCG_PLATFORM} )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( EvtGen )
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
                   PUBLIC_HEADERS EvtGen_i
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${PHOTOSPP_INCLUDE_DIRS} ${TAUOLAPP_INCLUDE_DIRS}  ${EVTGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${PHOTOSPP_LIBRARIES} ${TAUOLAPP_LIBRARIES} AtlasHepMCLib ${EVTGEN_LIBRARIES} GaudiKernel GeneratorObjects ${ROOT_LIBRARIES} StoreGateLib SGtests GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel )

atlas_add_component( EvtGen_i
                     src/components/*.cxx
                     LINK_LIBRARIES EvtGen_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.DEC share/file/*.table share/file/*.dec share/file/*.pdt share/file/*.dat )

