# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardRegionMgField )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( ForwardRegionMgField
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps MagFieldInterfaces PathResolver ForwardRegionGeoModelLib )

# Install files from the package:
atlas_install_runtime( share/*.dat )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
