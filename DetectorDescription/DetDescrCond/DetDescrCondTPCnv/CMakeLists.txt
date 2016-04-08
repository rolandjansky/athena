################################################################################
# Package: DetDescrCondTPCnv
################################################################################

# Declare the package name:
atlas_subdir( DetDescrCondTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/RootConversions
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/Identifier
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( DetDescrCondTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS DetDescrCondTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaPoolCnvSvcLib RootConversions DetDescrConditions Identifier GaudiKernel )

atlas_add_dictionary( DetDescrCondTPCnvDict
                      DetDescrCondTPCnv/DetDescrCondTPCnvDict.h
                      DetDescrCondTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaPoolCnvSvcLib RootConversions DetDescrConditions Identifier GaudiKernel DetDescrCondTPCnv )

atlas_add_test( DetStatusMapCnv_p1_test
                SOURCES
                test/DetStatusMapCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaPoolCnvSvcLib RootConversions DetDescrConditions Identifier GaudiKernel DetDescrCondTPCnv )

atlas_add_test( AlignableTransformCnv_p1_test
                SOURCES
                test/AlignableTransformCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaPoolCnvSvcLib RootConversions DetDescrConditions Identifier GaudiKernel DetDescrCondTPCnv )

atlas_add_test( DetCondCFloatCnv_p1_test
                SOURCES
                test/DetCondCFloatCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaPoolCnvSvcLib RootConversions DetDescrConditions Identifier GaudiKernel DetDescrCondTPCnv )

atlas_add_test( DetDescrCondIdentifier_old_cnv_test
                SOURCES
                test/DetDescrCondIdentifier_old_cnv_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools AthenaPoolCnvSvcLib RootConversions DetDescrConditions Identifier GaudiKernel DetDescrCondTPCnv )

