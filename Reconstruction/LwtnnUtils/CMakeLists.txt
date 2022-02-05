# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# Declare the package name:
atlas_subdir( LwtnnUtils )

# External dependencies:
find_package( lwtnn )
find_package( Eigen )

# Build a shared library:
atlas_add_library( LwtnnUtils
  LwtnnUtils/*.h                # <-- for Xcode, not needed otherwise
  src/FastGraph.cxx
  src/FastInputPreprocessor.cxx
  PUBLIC_HEADERS LwtnnUtils
  INCLUDE_DIRS ${LWTNN_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
  LINK_LIBRARIES EventPrimitives ${LWTNN_LIBRARIES} ${EIGEN_LIBRARIES} )

atlas_add_executable(
  test_lwtnn_fastgraph
  utils/test_lwtnn_fastgraph.cxx
  LINK_LIBRARIES LwtnnUtils )
