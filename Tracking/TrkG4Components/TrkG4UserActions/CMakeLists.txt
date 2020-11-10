# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkG4UserActions )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( TrkG4UserActions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel G4AtlasToolsLib TrkDetDescrInterfaces TrkGeometry TrkMaterialOnTrack TrkParameters CxxUtils StoreGateLib GeoPrimitives SimHelpers TrkSurfaces TrkExInterfaces TrkExUtils TrkValInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

