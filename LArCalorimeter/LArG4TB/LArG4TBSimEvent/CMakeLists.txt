# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArG4TBSimEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArG4TBSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArG4TBSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel HitManagement )

atlas_add_dictionary( LArG4TBSimEventDict
                      LArG4TBSimEvent/LArG4TBSimEventDict.h
                      LArG4TBSimEvent/selection.xml
                      LINK_LIBRARIES LArG4TBSimEvent )
