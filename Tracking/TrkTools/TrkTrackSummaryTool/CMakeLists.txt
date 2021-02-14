# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackSummaryTool )

find_package( Boost )
find_package( ROOT )

# Component(s) in the package:
atlas_add_component( TrkTrackSummaryTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTrackSummary  TrkToolInterfaces AtlasDetDescr Identifier TrkDetElementBase TrkGeometry TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkTrack TRT_ElectronPidToolsLib )


atlas_add_test(TrackSummaryTool_test
                SOURCES test/TrackSummaryTool_test.cxx 
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}   AthenaBaseComps GaudiKernel IdDictParser StoreGateLib TrkToolInterfaces AtlasDetDescr Identifier TrkDetElementBase TrkGeometry TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkTrack TRT_ElectronPidToolsLib 
                POST_EXEC_SCRIPT "nopost.sh" )

# Install files from the package:
atlas_install_joboptions( share/*.txt )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
