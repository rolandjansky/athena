# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRecConditions )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( LArRecConditions
                   src/*.cxx
                   PUBLIC_HEADERS LArRecConditions
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} CaloIdentifier AthenaKernel Identifier GaudiKernel AthenaBaseComps LArElecCalib LArCablingLib
                   PRIVATE_LINK_LIBRARIES LArIdentifier )

atlas_add_dictionary( LArRecConditionsDict
                      LArRecConditions/LArRecConditionsDict.h
                      LArRecConditions/selection.xml
                      LINK_LIBRARIES LArRecConditions )

# Test(s) in the package:
atlas_add_test (test_LArBadChannelCont
                SOURCES
	            test/test_LArBadChannelCont.cxx
                LINK_LIBRARIES LArRecConditions )
