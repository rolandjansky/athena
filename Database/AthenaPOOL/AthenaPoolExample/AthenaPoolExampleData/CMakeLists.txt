# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolExampleData )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AthenaPoolExampleData
                   src/*.cxx
                   PUBLIC_HEADERS AthenaPoolExampleData
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaKernel GaudiKernel Navigation )

atlas_add_dictionary( AthenaPoolExampleDataDict
                      AthenaPoolExampleData/AthenaPoolExampleDataDict.h
                      AthenaPoolExampleData/selection.xml
                      LINK_LIBRARIES AthenaPoolExampleData
                      NAVIGABLES ExampleHitContainer )

