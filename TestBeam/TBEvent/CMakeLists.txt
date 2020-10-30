# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TBEvent
                   src/*.cxx
                   PUBLIC_HEADERS TBEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthLinks AthContainers AthenaKernel GaudiKernel LArRawEvent )

atlas_add_dictionary( TBEventDict
                      TBEvent/TBEventDict.h
                      TBEvent/selection.xml
                      LINK_LIBRARIES TBEvent )
