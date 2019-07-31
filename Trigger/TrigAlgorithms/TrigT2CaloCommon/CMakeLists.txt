################################################################################
# Package: TrigT2CaloCommon
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloCommon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          Event/xAOD/xAODTrigCalo
                          ForwardDetectors/ZDC/ZdcCnv/ZdcByteStream
                          ForwardDetectors/ZDC/ZdcEvent
                          ForwardDetectors/ZDC/ZdcRec
                          GaudiKernel
                          LArCalorimeter/LArCnv/LArByteStream
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArCabling
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSvc/TileByteStream
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamData
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArBadChannelTool
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRecUtils
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigT2CaloCommonLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloCommon
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel IRegionSelector Identifier EventInfo xAODTrigCalo ZdcEvent GaudiKernel LArRecEvent TileEvent TrigSteeringEvent StoreGateLib SGtests ByteStreamCnvSvcBaseLib ZdcByteStreamLib ZdcRecLib LArByteStreamLib LArRawUtilsLib LArCablingLib TileByteStreamLib TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloUtilsLib ByteStreamData_test LArRecUtilsLib 
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities ByteStreamData LArIdentifier LArRecConditions TrigT1Interfaces )

atlas_add_component( TrigT2CaloCommon
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT2CaloCommonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( flake8
        SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
        POST_EXEC_SCRIPT nopost.sh )

# Helper to define unit tests running in a separate directory
function( _add_test name script )
   set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_${name} )
   file( REMOVE_RECURSE ${rundir} )   # cleanup to avoid interference with previous run
   file( MAKE_DIRECTORY ${rundir} )
   atlas_add_test( ${name}
      SCRIPT ${script}
      PROPERTIES TIMEOUT 1200
      PROPERTIES WORKING_DIRECTORY ${rundir} )
endfunction( _add_test )

_add_test( TestService test/test_dataaccess.sh )
_add_test( TestServiceNewJO test/test_dataaccessNewJO.sh )
