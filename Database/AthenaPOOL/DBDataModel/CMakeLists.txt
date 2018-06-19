################################################################################
# Package: DBDataModel
################################################################################

# Declare the package name:
atlas_subdir( DBDataModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          PRIVATE
                          Database/APR/CollectionBase )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DBDataModel
                   src/*.cxx
                   PUBLIC_HEADERS DBDataModel
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers 
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CollectionBase )

atlas_add_dictionary( DBDataModelDict
                      DBDataModel/DBDataModelDict.h
                      DBDataModel/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers CollectionBase DBDataModel )

