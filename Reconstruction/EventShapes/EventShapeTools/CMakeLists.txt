# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventShapeTools )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_library( EventShapeToolsLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS EventShapeTools
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} AsgTools EventShapeInterface JetRecLib JetInterface xAODEventShape AsgDataHandlesLib AnaAlgorithmLib
                   PRIVATE_LINK_LIBRARIES  JetEDM )

if( NOT XAOD_STANDALONE )
atlas_add_component( EventShapeTools
                     src/components/*.cxx
                     LINK_LIBRARIES EventShapeToolsLib AthenaBaseComps)
endif()

if( XAOD_STANDALONE )
atlas_add_dictionary( EventShapeToolsDict
                      EventShapeTools/EventShapeToolsDict.h
                      EventShapeTools/selection.xml
                      LINK_LIBRARIES EventShapeToolsLib  )
endif()
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

