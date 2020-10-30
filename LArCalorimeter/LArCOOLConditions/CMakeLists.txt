# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCOOLConditions )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( LArCOOLConditions
                   src/*.cxx
                   PUBLIC_HEADERS LArCOOLConditions
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel Identifier GaudiKernel LArIdentifier LArElecCalib
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolUtilities StoreGateLib )

atlas_add_dictionary( LArCOOLConditionsDict
                      LArCOOLConditions/LArCOOLConditionsDict.h
                      LArCOOLConditions/selection.xml
                      LINK_LIBRARIES LArCOOLConditions )
