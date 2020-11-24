# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigTrackResidualMonitor )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( InDetTrigTrackResidualMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasDetDescr CxxUtils GaudiKernel IdDictDetDescr InDetAlignGenToolsLib InDetIdentifier InDetReadoutGeometry MagFieldInterfaces TrigInterfacesLib TrkEventPrimitives TrkExInterfaces TrkGeometry TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSurfaces TrkToolInterfaces TrkTrack TrkTrackSummary TrkValToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
