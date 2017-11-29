################################################################################
# Package: PersistentDataModel
################################################################################

# Declare the package name:
atlas_subdir( PersistentDataModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/SGTools
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( PersistentDataModel
                   src/*.cxx
                   PUBLIC_HEADERS PersistentDataModel
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} ${ROOT_LIBRARIES} )

atlas_add_dictionary( PersistentDataModelDict
                      PersistentDataModel/PersistentDataModelDict.h
                      PersistentDataModel/selection.xml
                      INCLUDE_DIRS ${UUID_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${UUID_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} SGTools GaudiKernel PersistentDataModel )

