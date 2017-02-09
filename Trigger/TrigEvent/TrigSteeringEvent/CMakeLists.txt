# $Id: CMakeLists.txt 790888 2016-12-22 22:12:11Z ssnyder $
################################################################################
# Package: TrigSteeringEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigSteeringEvent )

# Declare the package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthContainers
      Control/AthToolSupport/AsgTools
      Trigger/TrigConfiguration/TrigConfHLTData
      DetectorDescription/RoiDescriptor
      Event/xAOD/xAODCore )
elseif( XAOD_ANALYSIS )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthContainers
      Control/AthToolSupport/AsgTools
      Control/CLIDSvc
      Control/SGTools
      DetectorDescription/RoiDescriptor
      Trigger/TrigConfiguration/TrigConfHLTData
      DetectorDescription/RoiDescriptor
      Event/xAOD/xAODCore )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthContainers
      Control/AthToolSupport/AsgTools
      Control/AthenaKernel
      Control/CLIDSvc
      Control/SGTools
      DetectorDescription/RoiDescriptor
      Event/xAOD/xAODCore
      GaudiKernel
      Trigger/TrigConfiguration/TrigConfHLTData
      PRIVATE
      Event/xAOD/xAODTrigger
      Trigger/TrigDataAccess/TrigSerializeCnvSvc
      Trigger/TrigEvent/TrigNavigation
      Trigger/TrigT1/TrigT1Interfaces )

    find_package( tdaq-common )
endif()

# External dependencies:
# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( TrigSteeringEvent
      TrigSteeringEvent/*.h Root/*.cxx
      PUBLIC_HEADERS TrigSteeringEvent
      LINK_LIBRARIES AthContainers AsgTools RoiDescriptor xAODCore
      TrigConfHLTData )
elseif( XAOD_ANALYSIS )
   atlas_add_library( TrigSteeringEvent
      TrigSteeringEvent/*.h Root/*.cxx src/*.cxx
      PUBLIC_HEADERS TrigSteeringEvent
      LINK_LIBRARIES AthContainers AsgTools AthenaKernel SGTools RoiDescriptor
      xAODCore GaudiKernel TrigConfHLTData )

   atlas_add_dictionary( TrigSteeringEventDict
      TrigSteeringEvent/TrigSteeringEventDict.h TrigSteeringEvent/selection.xml
      DATA_LINKS TrigRoiDescriptor TrigSuperRoi
      LINK_LIBRARIES TrigSteeringEvent )
else()
   atlas_add_library( TrigSteeringEvent
      TrigSteeringEvent/*.h Root/*.cxx src/*.cxx
      PUBLIC_HEADERS TrigSteeringEvent
      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthContainers AsgTools AthenaKernel
      SGTools RoiDescriptor xAODCore GaudiKernel TrigConfHLTData
      PRIVATE_LINK_LIBRARIES TrigT1Interfaces xAODTrigger TrigNavigationLib )

   atlas_add_dictionary( TrigSteeringEventDict
      TrigSteeringEvent/TrigSteeringEventDict.h TrigSteeringEvent/selection.xml
      DATA_LINKS TrigRoiDescriptor TrigSuperRoi
      LINK_LIBRARIES TrigSteeringEvent )

   atlas_add_sercnv_library( TrigSteeringEventSerCnv
      FILES TrigSteeringEvent/TrigRoiDescriptor.h
      TrigSteeringEvent/TrigSuperRoi.h 
      TrigSteeringEvent/TrigPassFlags.h
      TrigSteeringEvent/TrigPassFlagsCollection.h
      TrigSteeringEvent/TrigRoiDescriptorCollection.h
      TrigSteeringEvent/TrigSuperRoiCollection.h
      TrigSteeringEvent/TrigOperationalInfo.h
      TrigSteeringEvent/TrigOperationalInfoCollection.h
      TrigSteeringEvent/TrigPassBits.h
      TrigSteeringEvent/TrigPassBitsCollection.h
      TrigT1Interfaces/RecJetRoI.h
      TrigT1Interfaces/RecEmTauRoI.h
      TrigT1Interfaces/RecEnergyRoI.h
      TrigT1Interfaces/RecJetEtRoI.h
      TrigT1Interfaces/RecMuonRoI.h
      xAODTrigger/TrigCompositeContainer.h
      xAODTrigger/TrigCompositeAuxContainer.h
      TYPES_WITH_NAMESPACE LVL1::RecJetRoI LVL1::RecJetEtRoI LVL1::RecEmTauRoI
      LVL1::RecEnergyRoI LVL1::RecMuonRoI  xAOD::TrigCompositeContainer
      xAOD::TrigCompositeAuxContainer
      LINK_LIBRARIES TrigSteeringEvent TrigT1Interfaces xAODTrigger TrigSerializeCnvSvcLib )

   atlas_add_sercnv_library( TrigSteeringEventxAODSerCnv
      FILES xAODTrigger/TrigPassBitsContainer.h
      xAODTrigger/TrigPassBitsAuxContainer.h
      TYPES_WITH_NAMESPACE xAOD::TrigPassBits xAOD::TrigPassBitsContainer
      xAOD::TrigPassBitsAuxContainer
      CNV_PFX xAOD
      LINK_LIBRARIES xAODTrigger TrigSerializeCnvSvcLib )
endif()

# Test(s) in the package:
atlas_add_test( Enums_test
   SOURCES test/Enums_test.cxx
   LINK_LIBRARIES TrigSteeringEvent
   EXTRA_PATTERNS ".*" )

atlas_add_test( TrigPassBits_test
   SOURCES test/TrigPassBits_test.cxx
   LINK_LIBRARIES TrigSteeringEvent
   EXTRA_PATTERNS ".*" )

if( NOT XAOD_STANDALONE )
   atlas_add_test( Operators_test
      SOURCES test/Operators_test.cxx
      LINK_LIBRARIES AthenaKernel GaudiKernel TrigSteeringEvent
      EXTRA_PATTERNS ".*" )

   atlas_add_test( Truncation_test
      SOURCES test/Truncation_test.cxx
      LINK_LIBRARIES AthenaKernel GaudiKernel TrigSteeringEvent
      EXTRA_PATTERNS ".*" )

   atlas_add_test( HLTExtraData_test
      SOURCES test/HLTExtraData_test.cxx
      LINK_LIBRARIES TrigSteeringEvent
      EXTRA_PATTERNS ".*" )
endif()

