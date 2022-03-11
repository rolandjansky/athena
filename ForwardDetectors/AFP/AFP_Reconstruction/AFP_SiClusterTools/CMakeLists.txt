# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_SiClusterTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Math GenVector )

atlas_add_library( AFP_SiClusterToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS AFP_SiClusterTools
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AFP_DigiEv AFP_Geometry AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel xAODForward )

atlas_add_component( AFP_SiClusterTools
                     src/components/*.cxx AFP_SiClusterTools/*.h
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AFP_SiClusterToolsLib )

# Install files from the package:
atlas_install_python_modules( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
