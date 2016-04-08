################################################################################
# Package: IOVDbTPCnv
################################################################################

# Declare the package name:
atlas_subdir( IOVDbTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/IOVDbDataModel
                          PRIVATE
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( IOVDbTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS IOVDbTPCnv
                         INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaPoolCnvSvcLib IOVDbDataModel AthenaKernel AthenaPoolUtilities )

atlas_add_dictionary( IOVDbTPCnvDict
                      IOVDbTPCnv/IOVDbTPCnvDict.h
                      IOVDbTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaPoolCnvSvcLib IOVDbDataModel AthenaKernel AthenaPoolUtilities IOVDbTPCnv )

atlas_add_dictionary( OLD_IOVDbTPCnvDict
                      IOVDbTPCnv/IOVDbTPCnvDict.h
                      IOVDbTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaPoolCnvSvcLib IOVDbDataModel AthenaKernel AthenaPoolUtilities IOVDbTPCnv )

