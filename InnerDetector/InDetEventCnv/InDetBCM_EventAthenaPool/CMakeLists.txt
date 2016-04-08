################################################################################
# Package: InDetBCM_EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( InDetBCM_EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetBCM_RawData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetBCM_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES InDetBCM_RawData/BCM_RDO_Container.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TestTools CxxUtils StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel InDetBCM_RawData )

atlas_add_dictionary( InDetBCM_EventAthenaPoolCnvDict
                      InDetBCM_EventAthenaPool/InDetBCM_EventAthenaPoolCnvDict.h
                      InDetBCM_EventAthenaPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TestTools CxxUtils StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel InDetBCM_RawData )

atlas_add_test( BCM_RawDataCnv_p0_test
                SOURCES
                test/BCM_RawDataCnv_p0_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TestTools CxxUtils StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel InDetBCM_RawData )

atlas_add_test( BCM_RDO_CollectionCnv_p0_test
                SOURCES
                test/BCM_RDO_CollectionCnv_p0_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TestTools CxxUtils StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel InDetBCM_RawData )

atlas_add_test( BCM_RDO_ContainerCnv_p0_test
                SOURCES
                test/BCM_RDO_ContainerCnv_p0_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TestTools CxxUtils StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel InDetBCM_RawData )

# Install files from the package:
atlas_install_headers( InDetBCM_EventAthenaPool )
atlas_install_joboptions( share/*.py )

