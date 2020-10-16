# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaConditions )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( egammaConditions
                   src/*.cxx
                   PUBLIC_HEADERS egammaConditions
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( egammaConditionsDict
                      egammaConditions/egammaConditionsDict.h
                      egammaConditions/selection.xml
                      LINK_LIBRARIES egammaConditions )
