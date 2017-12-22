################################################################################
# Package: DataModelAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( DataModelAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/Navigation
                          Control/RootUtils
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RootConversions )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelAthenaPoolLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelAthenaPool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils GaudiKernel RootUtilsPyROOT AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TestTools SGTools AthenaPoolUtilities RootConversions )

atlas_add_component( DataModelAthenaPool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_dictionary( DataModelAthenaPoolDictDict
                      DataModelAthenaPool/DataModelAthenaPoolDict.h
                      DataModelAthenaPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( DataLink_p2_test
                SOURCES
                test/DataLink_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( ElementLinkCnv_p1_test
                SOURCES
                test/ElementLinkCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( ElementLinkCnv_p2_test
                SOURCES
                test/ElementLinkCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( ElementLinkCnv_p3_test
                SOURCES
                test/ElementLinkCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( NavigableCnv_p1_test
                SOURCES
                test/NavigableCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( NavigableCnv_p2_test
                SOURCES
                test/NavigableCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( NavigableVectorCnv_p1_test
                SOURCES
                test/NavigableVectorCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( NavigableVectorCnv_p2_test
                SOURCES
                test/NavigableVectorCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

atlas_add_test( ElementLinkVectorCnv_p1_test
                SOURCES
                test/ElementLinkVectorCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel Navigation RootUtils RootUtilsPyROOT GaudiKernel TestTools SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions DataModelAthenaPoolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/DataModelAthenaPool_jobOptions.py )

