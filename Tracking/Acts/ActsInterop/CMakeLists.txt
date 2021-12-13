# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( ActsInterop )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( Boost )

# Component(s) in the package.
atlas_add_library( ActsInteropLib
   ActsInterop/*.h src/*.cxx
   PUBLIC_HEADERS ActsInterop
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ActsCore GaudiKernel
   PRIVATE_LINK_LIBRARIES InDetIdentifier InDetReadoutGeometry )

# Unpack header from Acts target to use without linking
get_target_property(Acts_INCLUDE_DIR ActsCore INTERFACE_INCLUDE_DIRECTORIES)

# Set up dictionary with special header
atlas_add_dictionary( ActsUnitDict
   ActsInterop/ActsUnitDict.h
   ActsInterop/selection.xml 
   INCLUDE_DIRS ${Acts_INCLUDE_DIR})

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( Units
   SCRIPT test/test_units.py
   POST_EXEC_SCRIPT nopost.sh )
