################################################################################
# Package: LArCondTPCnv
################################################################################

# Declare the package name:
atlas_subdir( LArCondTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LArCalorimeter/LArRawConditions
                          PRIVATE
                          Control/AthenaKernel
                          LArCalorimeter/LArElecCalib )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( LArCondTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LArCondTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawConditions AthenaKernel )

atlas_add_dictionary( LArCondTPCnvDict
                      LArCondTPCnv/LArCondTPCnvDict.h
                      LArCondTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawConditions AthenaKernel LArCondTPCnv )

atlas_add_dictionary( OLD_LArCondTPCnvDict
                      LArCondTPCnv/LArCondTPCnvDict.h
                      LArCondTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawConditions AthenaKernel LArCondTPCnv )

