# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAlignEvent )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrkAlignEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlignEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthContainers CxxUtils Identifier xAODTracking GaudiKernel TrkEventPrimitives TrkTrack VxVertex
                   PRIVATE_LINK_LIBRARIES TrkDetElementBase TrkSurfaces TrkCompetingRIOsOnTrack TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkRIO_OnTrack )

