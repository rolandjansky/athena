# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbPTCnv )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( IOVDbPTCnv
                   src/*.cxx
                   PUBLIC_HEADERS IOVDbPTCnv
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} IOVDbDataModel IOVDbTPCnv
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities )

atlas_add_dictionary( IOVDbPTCnvDict
                      IOVDbPTCnv/IOVDbPTCnvDict.h
                      IOVDbPTCnv/selection.xml
                      LINK_LIBRARIES IOVDbPTCnv )
