# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetPattRecoEvent )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrigInDetPattRecoEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetPattRecoEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives TrkParameters TrigInDetEvent TrigSteeringEvent )

