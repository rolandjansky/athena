################################################################################
# Package: TrigT1CaloByteStream
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/DataModel
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Event/EventInfo
                          ForwardDetectors/ZDC/ZdcCnv/ZdcByteStream
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloMappingToolInterfaces
                          Trigger/TrigT1/TrigT1CaloUtils
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_component( TrigT1CaloByteStream
                     src/*.cxx
                     src/xaod/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} AsgTools DataModel xAODTrigL1Calo GaudiKernel AthContainers AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test EventInfo ZdcByteStreamLib TrigT1CaloEventLib TrigT1CaloUtilsLib TrigT1Interfaces )

atlas_add_dictionary( TrigT1CaloByteStreamDict
                      TrigT1CaloByteStream/TrigT1CaloByteStreamDict.h
                      TrigT1CaloByteStream/selection.xml
                      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} AsgTools DataModel xAODTrigL1Calo GaudiKernel AthContainers AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test EventInfo ZdcByteStreamLib TrigT1CaloEventLib TrigT1CaloUtilsLib TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigT1CaloByteStream )
atlas_install_joboptions( share/*.py )

