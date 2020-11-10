# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkResidualPullCalculator )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkResidualPullCalculator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AtlasDetDescr EventPrimitives GaudiKernel TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkToolInterfaces TrkTrack )
