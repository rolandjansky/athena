# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelMonitoring )

# Component(s) in the package:
atlas_add_component( PixelMonitoring
   src/*.h src/*.cxx src/components/*.cxx 
   LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib AtlasDetDescr EventPrimitives
   InDetConditionsSummaryService InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRawData
   InDetReadoutGeometry InDetTrackSelectionToolLib PixelConditionsData PixelReadoutGeometryLib
   TrkMeasurementBase TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrkTrackSummary
   MVAUtils PathResolver )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_generic( ModuleInfo/*.txt 
                    DESTINATION python/PixelMonitoring
                    EXCUTABLE )
