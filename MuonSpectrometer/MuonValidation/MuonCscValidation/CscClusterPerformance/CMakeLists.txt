# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CscClusterPerformance )

# External dependencies:
find_package( ROOT COMPONENTS Tree RIO Core )

# Component(s) in the package:
atlas_add_executable( csc_cluster_performance
                      src/csc_cluster_performance.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable( csc_cosmic_cluster
                      src/csc_cosmic_cluster.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )
