# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AtlasCollectionTools )

# Necessary external(s):
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_library( AtlasCollectionTools
                   src/LumiMetaHandler.cxx
                   src/GlobalUniqueKey.cxx
                   src/GlobalSum.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_LINK_LIBRARIES CollectionUtilities GoodRunsListsLib )

atlas_add_executable( AthCollAppend
                      src/CollAppend.cxx
                      LINK_LIBRARIES AtlasCollectionTools CollectionUtilities )

# Install files from the package:
atlas_install_python_modules( python/countGuidsClient.py python/eventLookupClient.py )
atlas_install_scripts( python/listDatasets.py )
atlas_install_scripts( python/runEventLookup.py python/runGuidsCount.py python/tagExtract.py python/tagDSList.py )

