# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopConfiguration )

# External dependencies:
find_package( Boost REQUIRED COMPONENTS iostreams )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopConfiguration _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopConfiguration Root/*.cxx Root/*.h Root/*.icc
                   TopConfiguration/*.h TopConfiguration/*.icc TopConfiguration/*/*.h
                   TopConfiguration/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopConfiguration
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES}
                                  AsgMessagingLib
                                  AsgTools
                                  PATInterfaces
                                  TopDataPreparation
                                  xAODCore
                   PRIVATE_LINK_LIBRARIES PathResolver )

# Install data files from the package:
atlas_install_data( share/* )
