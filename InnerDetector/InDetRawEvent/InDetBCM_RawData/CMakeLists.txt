################################################################################
# Package: InDetBCM_RawData
################################################################################

# Declare the package name:
atlas_subdir( InDetBCM_RawData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetBCM_RawData
                   src/*.cxx
                   PUBLIC_HEADERS InDetBCM_RawData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( InDetBCM_RawDataDict
                      InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h
                      InDetBCM_RawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools InDetBCM_RawData )

