# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardTransportSvc )

# External dependencies:
find_package( CLHEP )   # for Geant4
find_package( Geant4 )
find_package( ROOT COMPONENTS Tree )

atlas_add_library( ForwardTransportSvcLib
                   ForwardTransportSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS ForwardTransportSvc
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ForwardTracker GaudiKernel )

# Component(s) in the package:
atlas_add_component( ForwardTransportSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps ForwardTracker ForwardTransportSvcLib GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

