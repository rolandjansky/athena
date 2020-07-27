# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigL2MuonSA )

# External dependencies:
find_package( CLHEP )
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS MuCalDecode circ_proc )

# Component(s) in the package:
atlas_add_library( TrigL2MuonSALib
                   src/*.cxx
                   PUBLIC_HEADERS TrigL2MuonSA
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib ByteStreamCnvSvcBaseLib CscClusterizationLib GaudiKernel GeoPrimitives Identifier MdtCalibSvcLib MuonCablingData MuonCnvToolInterfacesLib MuonIdHelpersLib MuonPrepRawData MuonRDO MuonRecToolInterfaces MuonTGC_CablingLib RPC_CondCablingLib RegionSelectorLib StoreGateLib TrigInterfacesLib TrigMuonToolInterfaces TrigSteeringEvent TrigT1Interfaces TrigT1RPCRecRoiSvcLib TrigTimeAlgsLib xAODEventInfo xAODTrigMuon xAODTrigger
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GSL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaInterprocess CxxUtils EventInfo GeoModelUtilities MuonCalibEvent MuonReadoutGeometry PathResolver )

atlas_add_component( TrigL2MuonSA
                     src/components/*.cxx
                     LINK_LIBRARIES TrigL2MuonSALib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/pt_barrel.lut share/pt_barrelSP.lut share/pt_barrelSP_new.lut share/pt_barrel.mc10.lut share/pt_endcap.lut share/pt_endcap.mc10.lut share/dZ_barrel.lut share/pt_endcap_small_large.lut share/pt_endcap_run2.lut share/pt_comb_sigma.lut share/pt_comb_mean.lut)

atlas_add_test( MdtDataPreparator_test
                SCRIPT python -m TrigL2MuonSA.MdtDataPreparator_test
                PROPERTIES TIMEOUT 600
                LOG_SELECT_PATTERN "dead tube" )
