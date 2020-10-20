# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackSummaryTool )

# Component(s) in the package:
atlas_add_component( TrkTrackSummaryTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkParameters TrkTrackSummary TrkToolInterfaces AtlasDetDescr Identifier TrkDetElementBase TrkGeometry TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkTrack TRT_ElectronPidToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
