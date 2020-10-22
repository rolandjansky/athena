# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExAlgs )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree RIO )
find_package( Acts COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( TrkExAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthContainers GeoPrimitives EventPrimitives GaudiKernel MagFieldInterfaces TrkSurfaces TrkParameters TrkParametersBase TrkGeometry TrkVolumes TrkEventPrimitives TrkTrack TrkExInterfaces TrkExUtils ActsGeometryInterfacesLib ActsInteropLib ActsGeometryLib ActsCore)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
