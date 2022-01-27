# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSimEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( InDetSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS InDetSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel GeneratorObjects HitManagement
                   PRIVATE_LINK_LIBRARIES GaudiKernel InDetIdentifier StoreGateLib )

atlas_add_dictionary( InDetSimEventDict
                      InDetSimEvent/InDetSimEventDict.h
                      InDetSimEvent/selection.xml
                      LINK_LIBRARIES InDetSimEvent )
