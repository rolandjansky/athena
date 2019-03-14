################################################################################
# Package: AtlasCollectionTools
################################################################################

# Declare the package name:
atlas_subdir( AtlasCollectionTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          DataQuality/GoodRunsLists
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc
                          TestPolicy )

# Necessary external(s):
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_library( AtlasCollectionTools
                   src/LumiMetaHandler.cxx
                   src/GlobalUniqueKey.cxx
                   src/GlobalSum.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES GoodRunsListsLib
                   PRIVATE_LINK_LIBRARIES CollectionBase CollectionUtilities PersistencySvc StorageSvc )

atlas_add_executable( AthCollAppend
                      src/CollAppend.cxx
                      LINK_LIBRARIES GoodRunsListsLib CollectionBase CollectionUtilities PersistencySvc StorageSvc AtlasCollectionTools )


# Install files from the package:
atlas_install_python_modules( python/countGuidsClient.py python/eventLookupClient.py )
atlas_install_scripts( python/listDatasets.py )
atlas_install_scripts( python/runEventLookup.py python/runGuidsCount.py python/tagExtract.py python/tagDSList.py )

