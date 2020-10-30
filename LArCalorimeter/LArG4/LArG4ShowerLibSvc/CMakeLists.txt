# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArG4ShowerLibSvc )

# External dependencies:
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( LArG4ShowerLibSvcLib
                   LArG4ShowerLibSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS LArG4ShowerLibSvc
                   LINK_LIBRARIES GaudiKernel LArG4Code AthenaBaseComps )

# Component(s) in the package:
atlas_add_component( LArG4ShowerLibSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEANT4_LIBRARIES} LArG4ShowerLibSvcLib AthenaKernel LArG4ShowerLib PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/*.py )

