################################################################################
# Package: TrigTopoEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigTopoEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Trigger/TrigEvent/TrigTopoEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigTopoEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigTopoEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib TrigTopoEvent TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigTopoEventTPCnvDict
                      TrigTopoEventTPCnv/TrigTopoEventTPCnvDict.h
                      TrigTopoEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib TrigTopoEvent TestTools AthenaKernel GaudiKernel TrigTopoEventTPCnv )

atlas_add_test( ElectronMuonTopoInfoCnv_p1_test
                SOURCES
                test/ElectronMuonTopoInfoCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolCnvSvcLib TrigTopoEvent TestTools AthenaKernel GaudiKernel TrigTopoEventTPCnv )

