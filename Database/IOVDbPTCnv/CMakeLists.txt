################################################################################
# Package: IOVDbPTCnv
################################################################################

# Declare the package name:
atlas_subdir( IOVDbPTCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/IOVDbDataModel
                          Database/IOVDbTPCnv
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( IOVDbPTCnv
                   src/*.cxx
                   PUBLIC_HEADERS IOVDbPTCnv
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} IOVDbDataModel IOVDbTPCnv
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities )

atlas_add_dictionary( IOVDbPTCnvDict
                      IOVDbPTCnv/IOVDbPTCnvDict.h
                      IOVDbPTCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} IOVDbDataModel IOVDbTPCnv AthenaPoolUtilities IOVDbPTCnv )

