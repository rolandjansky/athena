# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCoolCoralClientUtils )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( InDetCoolCoralClientUtils
                   src/*.cpp
                   PUBLIC_HEADERS InDetCoolCoralClientUtils
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} CxxUtils
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} )
