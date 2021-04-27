# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TruthUtils )

# External dependencies:
find_package( HEPUtils )
find_package( MCUtils )
find_package( HepMC )

# Extra include directories and libraries, based on which externals were found:
set( extra_includes )
set( extra_libs )
if( HEPUTILS_FOUND )
   list( APPEND extra_includes ${HEPUTILS_INCLUDE_DIRS} )
   list( APPEND extra_libs ${HEPUTILS_LIBRARIES} )
endif()
if( MCUTILS_FOUND )
   list( APPEND extra_includes ${MCUTILS_INCLUDE_DIRS} )
   list( APPEND extra_libs ${MCUTILS_LIBRARIES} )
endif()
if( HEPMC_FOUND )
   list( APPEND extra_includes  )
   list( APPEND extra_libs AtlasHepMCLib )
endif()

# Component(s) in the package:
atlas_add_library( TruthUtils
   TruthUtils/*.h Root/*.cxx
   PUBLIC_HEADERS TruthUtils
   INCLUDE_DIRS ${extra_includes}
   LINK_LIBRARIES ${extra_libs}
   PRIVATE_LINK_LIBRARIES CxxUtils )

# Dictionary
atlas_add_dictionary( TruthUtilsDict
                      TruthUtils/TruthUtilsDict.h
                      TruthUtils/selection.xml
                      LINK_LIBRARIES TruthUtils )

# Python module
atlas_install_python_modules( python/*.py )# POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Install helper utility
atlas_install_runtime( util/generator-weights.py )

# Install files from the package:
atlas_install_runtime( share/*.txt )

# Run tests:
atlas_add_test( WeightHelpersTest
                SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/testWeightHelpers.py
                PROPERTIES TIMEOUT 300 )
