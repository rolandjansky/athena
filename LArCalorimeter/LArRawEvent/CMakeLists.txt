# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRawEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArRawEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArRawEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel CaloIdentifier AthContainers Identifier )

atlas_add_dictionary( LArRawEventDict
                      LArRawEvent/LArRawEventDict.h
                      LArRawEvent/selection.xml
                      LINK_LIBRARIES LArRawEvent )
