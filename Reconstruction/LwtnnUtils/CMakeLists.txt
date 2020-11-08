# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# Declare the package name:
atlas_subdir( LwtnnUtils )

# External dependencies:
find_package( lwtnn )

# Build a shared library:
atlas_add_library( LwtnnUtils
  src/FastGraph.cxx
  src/FastInputPreprocessor.cxx
  PUBLIC_HEADERS LwtnnUtils
  INCLUDE_DIRS ${LWTNN_INCLUDE_DIRS}
  LINK_LIBRARIES ${LWTNN_LIBRARIES} )

