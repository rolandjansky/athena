# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_CalibData )

# External dependencies:
find_package( CLHEP )

atlas_add_library( TRT_CalibData
                   TRT_CalibData/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_CalibData
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} TRT_ConditionsData InDetIdentifier )
