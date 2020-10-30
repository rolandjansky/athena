# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArSimEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel Identifier HitManagement StoreGateLib )

atlas_add_dictionary( LArSimEventDict
                      LArSimEvent/LArSimEventDict.h
                      LArSimEvent/selection.xml
                      LINK_LIBRARIES LArSimEvent )
