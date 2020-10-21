# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetByteStreamErrors )

# Component(s) in the package:
atlas_add_dictionary( InDetByteStreamErrorsDict
                      InDetByteStreamErrors/InDetByteStreamErrorsDict.h
                      InDetByteStreamErrors/selection.xml
                      LINK_LIBRARIES AthContainers Identifier InDetByteStreamErrors )

atlas_add_library( InDetByteStreamErrors
                   src/*.cxx
                   PUBLIC_HEADERS InDetByteStreamErrors
                   LINK_LIBRARIES AthContainers AthenaKernel EventContainers Identifier )
