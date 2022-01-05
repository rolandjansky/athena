# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetVKalVxInJetTool )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Hist MathCore Tree RIO  )

# Component(s) in the package:
atlas_add_library( InDetVKalVxInJetToolLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetVKalVxInJetTool
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AnalysisUtilsLib AthenaBaseComps BeamPipeGeoModelLib CxxUtils GaudiKernel GeoPrimitives InDetRecToolInterfaces MVAUtils PathResolver PixelReadoutGeometryLib TrkNeutralParameters TrkParticleBase TrkToolInterfaces TrkTrackSummary TrkVKalVrtFitterLib TrkVertexFitterInterfaces VxSecVertex VxVertex xAODTracking xAODTruth )

atlas_add_component( InDetVKalVxInJetTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES InDetVKalVxInJetToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.txt )

# Test(s) in the package:
atlas_add_test( InDetTrkInJetType_test
                SOURCES test/InDetTrkInJetType_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventPrimitives GaudiKernel MagFieldConditions TestTools TrkVertexFitterInterfaces xAODTracking
                LOG_IGNORE_PATTERN "Booking|ClassIDSvc|weight file|create data set info" )
