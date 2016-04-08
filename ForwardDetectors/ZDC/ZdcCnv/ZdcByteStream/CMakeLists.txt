################################################################################
# Package: ZdcByteStream
################################################################################

# Declare the package name:
atlas_subdir( ZdcByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/DataModel
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Event/xAOD/xAODTrigL1Calo
                          ForwardDetectors/ZDC/ZdcEvent
                          GaudiKernel
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          ForwardDetectors/ZDC/ZdcConditions
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( ZdcByteStreamLib
                   src/*.cxx
                   PUBLIC_HEADERS ZdcByteStream
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AsgTools AthenaBaseComps DataModel ByteStreamData xAODTrigL1Calo ZdcEvent GaudiKernel ByteStreamCnvSvcBaseLib ByteStreamData_test StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools ZdcConditions TrigT1Interfaces )

atlas_add_component( ZdcByteStream
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps DataModel ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test xAODTrigL1Calo ZdcEvent GaudiKernel SGTools StoreGateLib SGtests ZdcConditions TrigT1Interfaces ZdcByteStreamLib )

atlas_add_dictionary( ZdcByteStreamDict
                      ZdcByteStream/ZdcByteStreamDict.h
                      ZdcByteStream/selection.xml
                      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps DataModel ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test xAODTrigL1Calo ZdcEvent GaudiKernel SGTools StoreGateLib SGtests ZdcConditions TrigT1Interfaces ZdcByteStreamLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

