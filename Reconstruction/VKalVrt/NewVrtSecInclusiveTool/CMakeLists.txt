# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( NewVrtSecInclusiveTool )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread TMVA )

# Component(s) in the package:
atlas_add_component( NewVrtSecInclusiveTool 
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AnalysisUtilsLib AthenaBaseComps GaudiKernel GeoPrimitives InDetBeamSpotServiceLib MVAUtils PathResolver TrkExInterfaces TrkGeometry TrkNeutralParameters TrkTrackSummary TrkVKalVrtFitterLib VxSecVertex xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
