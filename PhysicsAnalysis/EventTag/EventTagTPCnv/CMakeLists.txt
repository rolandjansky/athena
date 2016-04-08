################################################################################
# Package: EventTagTPCnv
################################################################################

# Declare the package name:
atlas_subdir( EventTagTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/EventTag/TagEvent
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( EventTagTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventTagTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TagEvent AthenaKernel )

atlas_add_dictionary( EventTagTPCnvDict
                      EventTagTPCnv/EventTagTPCnvDict.h
                      EventTagTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TagEvent AthenaKernel EventTagTPCnv )

