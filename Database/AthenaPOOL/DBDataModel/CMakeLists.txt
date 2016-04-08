################################################################################
# Package: DBDataModel
################################################################################

# Declare the package name:
atlas_subdir( DBDataModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PRIVATE
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DBDataModel
                   src/*.cxx
                   PUBLIC_HEADERS DBDataModel
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc )

atlas_add_dictionary( DBDataModelDict
                      DBDataModel/DBDataModelDict.h
                      DBDataModel/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc DBDataModel )

