################################################################################
# Package: LumiBlockData
################################################################################

# Declare the package name:
atlas_subdir( LumiBlockData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LumiBlockData
                   src/*.cxx
                   PUBLIC_HEADERS LumiBlockData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel DataModel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( LumiBlockDataDict
                      LumiBlockData/LumiBlockDataDict.h
                      LumiBlockData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel DataModel SGTools LumiBlockData )

