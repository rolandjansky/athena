################################################################################
# Package: G4SimTPCnv
################################################################################

# Declare the package name:
atlas_subdir( G4SimTPCnv )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( G4SimTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS G4SimTPCnv
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolServicesLib AthenaKernel TrackRecordLib )

atlas_add_dictionary( G4SimTPCnvDict
                      G4SimTPCnv/G4SimTPCnvDict.h
                      G4SimTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib AthenaKernel G4SimTPCnv )

atlas_add_dictionary( OLD_G4SimTPCnvDict
                      G4SimTPCnv/G4SimTPCnvDict.h
                      G4SimTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib AthenaKernel G4SimTPCnv )

atlas_add_test( TrackRecordCnv_p1_test
                SOURCES
                test/TrackRecordCnv_p1_test.cxx
                LINK_LIBRARIES G4SimTPCnv TestTools )
