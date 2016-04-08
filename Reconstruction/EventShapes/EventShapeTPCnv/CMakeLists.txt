################################################################################
# Package: EventShapeTPCnv
################################################################################

# Declare the package name:
atlas_subdir( EventShapeTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Reconstruction/EventShapes/EventShapeEvent
                          PRIVATE
                          Control/DataModelAthenaPool )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventShapeTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS EventShapeTPCnv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES EventShapeEvent AthenaPoolCnvSvcLib DataModelAthenaPoolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( EventShapeTPCnvDict
                      EventShapeTPCnv/EventShapeTPCnvDict.h
                      EventShapeTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventShapeEvent DataModelAthenaPoolLib EventShapeTPCnv )

