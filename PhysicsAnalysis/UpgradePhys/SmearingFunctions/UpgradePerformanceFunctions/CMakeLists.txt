# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( UpgradePerformanceFunctions )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( UpgradePerformanceFunctions _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( UpgradePerformanceFunctions Root/*.cxx Root/*.h Root/*.icc
                   UpgradePerformanceFunctions/*.h UpgradePerformanceFunctions/*.icc UpgradePerformanceFunctions/*/*.h
                   UpgradePerformanceFunctions/*/*.icc ${_cintDictSource}
                   PUBLIC_HEADERS UpgradePerformanceFunctions
                   LINK_LIBRARIES PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( plotPerformanceFunctions
                      util/plotPerformanceFunctions.cxx
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     UpgradePerformanceFunctions )
atlas_add_executable( testPerformanceFunctions
                      util/testPerformanceFunctions.cxx
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     UpgradePerformanceFunctions )
