# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CommissionEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CommissionEvent
                   src/*.cxx
                   PUBLIC_HEADERS CommissionEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel )

atlas_add_dictionary( CommissionEventDict
                      CommissionEvent/CommissionEventDict.h
                      CommissionEvent/selection.xml
                      LINK_LIBRARIES CommissionEvent )
