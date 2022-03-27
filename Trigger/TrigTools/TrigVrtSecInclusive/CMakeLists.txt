# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigVrtSecInclusive )

# External package dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Matrix )

# Component(s) in the package:
atlas_add_component( TrigVrtSecInclusive
                     src/*.cxx
                     src/components/*.cxx
         PUBLIC_HEADERS TrigVrtSecInclusive
         INCLUDE_DIRS   ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib xAODRootAccess GaudiKernel TrigInDetEvent TrigInDetToolInterfacesLib TrkEventPrimitives TrkParameters TrkTrack TrkVKalVrtFitterLib xAODTracking TrkLinks TrkTrackLink TrkParticleBase TrkVKalVrtCore InDetConversionFinderToolsLib PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
