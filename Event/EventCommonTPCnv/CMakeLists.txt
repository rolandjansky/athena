################################################################################
# Package: EventCommonTPCnv
################################################################################

# Declare the package name:
atlas_subdir( EventCommonTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/FourMom
                          Event/NavFourMom
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( EventCommonTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventCommonTPCnv
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib FourMom NavFourMom AthenaKernel StoreGateLib SGtests GaudiKernel )

atlas_add_dictionary( EventCommonTPCnvDict
                      EventCommonTPCnv/EventCommonTPCnvDict.h
                      EventCommonTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib FourMom NavFourMom AthenaKernel StoreGateLib SGtests GaudiKernel EventCommonTPCnv )

atlas_add_dictionary( OLD_EventCommonTPCnvDict
                      EventCommonTPCnv/EventCommonTPCnvDict.h
                      EventCommonTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib FourMom NavFourMom AthenaKernel StoreGateLib SGtests GaudiKernel EventCommonTPCnv )

