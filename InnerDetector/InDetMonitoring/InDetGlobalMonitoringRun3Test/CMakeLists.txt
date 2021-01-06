# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetGlobalMonitoringRun3Test )

# Component(s) in the package:
atlas_add_component( InDetGlobalMonitoringRun3Test
   InDetGlobalMonitoringRun3Test/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib AtlasDetDescr BeamSpotConditionsData EventPrimitives GaudiKernel InDetConditionsSummaryService InDetIdentifier InDetPrepRawData InDetTrackSelectionToolLib PixelGeoModelLib StoreGateLib TrkEventPrimitives TrkToolInterfaces TrkTrack TrkTrackSummary xAODTracking )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
