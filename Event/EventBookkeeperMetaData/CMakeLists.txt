# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventBookkeeperMetaData )

# Component(s) in the package:
atlas_add_library( EventBookkeeperMetaData
                   src/*.cxx
                   PUBLIC_HEADERS EventBookkeeperMetaData
                   LINK_LIBRARIES AthContainers AthenaKernel GaudiKernel )

atlas_add_dictionary( EventBookkeeperMetaDataDict
                      EventBookkeeperMetaData/EventBookkeeperMetaDataDict.h
                      EventBookkeeperMetaData/selection.xml
                      LINK_LIBRARIES EventBookkeeperMetaData )
