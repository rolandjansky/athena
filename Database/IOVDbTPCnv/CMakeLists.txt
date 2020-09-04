# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbTPCnv )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_tpcnv_library( IOVDbTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS IOVDbTPCnv
                         INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                         LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolCnvSvcLib IOVDbDataModel
                         PRIVATE_LINK_LIBRARIES AthenaKernel AthenaPoolUtilities )

atlas_add_dictionary( IOVDbTPCnvDict
                      IOVDbTPCnv/IOVDbTPCnvDict.h
                      IOVDbTPCnv/selection.xml
                      LINK_LIBRARIES IOVDbTPCnv )

atlas_add_dictionary( OLD_IOVDbTPCnvDict
                      IOVDbTPCnv/IOVDbTPCnvDict.h
                      IOVDbTPCnv/OLD_selection.xml
                      LINK_LIBRARIES IOVDbTPCnv )
