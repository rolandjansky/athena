# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DBDataModel )

# Component(s) in the package:
atlas_add_library( DBDataModel
                   src/*.cxx
                   PUBLIC_HEADERS DBDataModel
                   LINK_LIBRARIES AthContainers AthenaKernel
                   PRIVATE_LINK_LIBRARIES CollectionBase )

atlas_add_dictionary( DBDataModelDict
                      DBDataModel/DBDataModelDict.h
                      DBDataModel/selection.xml
                      LINK_LIBRARIES DBDataModel )
