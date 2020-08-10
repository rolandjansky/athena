# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2CaloCommon )

# External dependencies:
find_package( tdaq-common )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( TrigT2CaloCommonLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloCommon
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloEvent CaloGeoHelpers CaloIdentifier CxxUtils GaudiKernel IRegionSelector Identifier LArByteStreamLib LArCablingLib LArRawUtilsLib LArRecEvent TileByteStreamLib TileEvent TrigInterfacesLib TrigSteeringEvent TrigTimeAlgsLib ZdcByteStreamLib ZdcEvent ZdcRecLib xAODEventInfo xAODTrigCalo
                   PRIVATE_LINK_LIBRARIES ${TBB_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaMonitoringKernelLib AthenaPoolUtilities ByteStreamCnvSvcBaseLib ByteStreamData CaloDetDescrLib CaloUtilsLib EventInfo LArBadChannelToolLib LArElecCalib LArIdentifier LArRecConditions LArRecUtilsLib LumiBlockCompsLib StoreGateLib TestTools TrigCaloEvent TrigT1Interfaces )

atlas_add_component( TrigT2CaloCommon
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT2CaloCommonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:

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
_add_test( caloOnly  test/test_t2calo_only_build.sh )
_add_test( caloRinger test/test_t2calo_ringer_only_build.sh )
