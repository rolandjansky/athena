################################################################################
# Package: TBTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TBTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          TestBeam/TBEvent
                          PRIVATE
                          DetectorDescription/Identifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TBTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS TBTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} TBEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier )

atlas_add_dictionary( TBTPCnvDict
                      TBTPCnv/TBTPCnvDict.h
                      TBTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TBEvent Identifier TBTPCnv )

