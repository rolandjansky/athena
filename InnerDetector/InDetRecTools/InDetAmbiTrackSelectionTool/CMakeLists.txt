# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAmbiTrackSelectionTool )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( InDetAmbiTrackSelectionTool
   InDetAmbiTrackSelectionTool/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrkRIO_OnTrack
   TrkTrack TrkToolInterfaces AthContainers CxxUtils StoreGateLib
   InDetIdentifier InDetPrepRawData InDetRecToolInterfaces TrkDetElementBase TrkEventUtils
   TrkSurfaces TrkCaloClusterROI TrkMeasurementBase TrkPrepRawData
   TrkPseudoMeasurementOnTrack TrkTrackSummary TrkParameters PixelGeoModelLib TrkFitterInterfaces TrkValInterfaces)
