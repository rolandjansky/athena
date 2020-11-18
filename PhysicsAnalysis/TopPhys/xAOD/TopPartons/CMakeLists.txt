# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopPartons )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Physics )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopPartons _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopPartons Root/*.cxx Root/*.h Root/*.icc
                   TopPartons/*.h TopPartons/*.icc TopPartons/*/*.h
                   TopPartons/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopPartons
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                                  AsgTools
                                  AthContainers
                                  xAODCore
                                  xAODTruth
                   PRIVATE_LINK_LIBRARIES TopConfiguration )
