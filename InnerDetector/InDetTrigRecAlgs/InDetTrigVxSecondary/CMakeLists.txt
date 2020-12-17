# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigVxSecondary )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( InDetTrigVxSecondary
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTracking CxxUtils GaudiKernel VxSecVertex TrigInterfacesLib AthContainers GeoPrimitives xAODBase InDetRecToolInterfaces TrkParameters TrkTrack VxVertex TrigInDetEvent TrigSteeringEvent InDetBeamSpotServiceLib TrigInDetToolInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
