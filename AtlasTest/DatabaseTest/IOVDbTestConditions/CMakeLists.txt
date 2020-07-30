# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbTestConditions )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( IOVDbTestConditions
                   src/IOVDbTestAmdbCorrection.cxx
                   src/IOVDbTestMDTEleMap.cxx
                   PUBLIC_HEADERS IOVDbTestConditions
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel AthenaPoolUtilities )

atlas_add_dictionary( IOVDbTestConditionsDict
                      IOVDbTestConditions/IOVDbTestConditionsDict.h
                      IOVDbTestConditions/selection.xml
                      LINK_LIBRARIES IOVDbTestConditions )
