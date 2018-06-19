################################################################################
# Package: EventBookkeeperMetaData
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperMetaData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventBookkeeperMetaData
                   src/*.cxx
                   PUBLIC_HEADERS EventBookkeeperMetaData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( EventBookkeeperMetaDataDict
                      EventBookkeeperMetaData/EventBookkeeperMetaDataDict.h
                      EventBookkeeperMetaData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers GaudiKernel EventBookkeeperMetaData )

