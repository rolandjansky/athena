# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_SiClusterTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Math GenVector )

atlas_add_component( AFP_SiClusterTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel xAODForward )
