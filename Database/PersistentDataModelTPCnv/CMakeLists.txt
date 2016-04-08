################################################################################
# Package: PersistentDataModelTPCnv
################################################################################

# Declare the package name:
atlas_subdir( PersistentDataModelTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/PersistentDataModel
                          PRIVATE
                          Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( PersistentDataModelTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS PersistentDataModelTPCnv
                   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES PersistentDataModel
                   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils )

atlas_add_dictionary( PersistentDataModelTPCnvDict
                      PersistentDataModelTPCnv/PersistentDataModelTPCnvDict.h
                      PersistentDataModelTPCnv/selection.xml
                      INCLUDE_DIRS ${UUID_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${UUID_LIBRARIES} ${ROOT_LIBRARIES} PersistentDataModel CxxUtils PersistentDataModelTPCnv )

