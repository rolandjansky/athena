# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventShapeEvent )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_library( EventShapeEvent
                   src/*.cxx
                   PUBLIC_HEADERS EventShapeEvent
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} AthContainers AthenaKernel EventKernel NavFourMom )

atlas_add_dictionary( EventShapesDict
                      EventShapeEvent/EventShapeEventDict.h
                      EventShapeEvent/selection.xml
                      LINK_LIBRARIES EventShapeEvent )
