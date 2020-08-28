# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbDataModel )

# Component(s) in the package:
atlas_add_library( IOVDbDataModel
                   src/*.cxx
                   PUBLIC_HEADERS IOVDbDataModel
                   LINK_LIBRARIES AthenaKernel AthenaPoolUtilities GaudiKernel RootUtils )

atlas_add_dictionary( IOVDbDataModelDict
                      IOVDbDataModel/IOVDbDataModelDict.h
                      IOVDbDataModel/selection.xml
                      LINK_LIBRARIES IOVDbDataModel )
