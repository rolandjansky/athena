################################################################################
# Package: RecEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( RecEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Reconstruction/RecEvent
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( RecEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS RecEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib RecEvent AthenaKernel )

atlas_add_dictionary( RecEventTPCnvDict
                      RecEventTPCnv/RecEventTPCnvDict.h
                      RecEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib RecEvent AthenaKernel RecEventTPCnv )

