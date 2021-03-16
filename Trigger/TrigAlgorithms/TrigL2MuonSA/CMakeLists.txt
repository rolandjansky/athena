# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigL2MuonSA )

# External dependencies:
find_package( GSL )

# Component(s) in the package:
atlas_add_component( TrigL2MuonSA
		     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS ${GSL_INCLUDE_DIRS}
		     LINK_LIBRARIES ${GSL_LIBRARIES} AthenaMonitoringKernelLib MdtCalibSvcLib MuonCablingData MuonCnvToolInterfacesLib MuonRecToolInterfaces TrigMuonToolInterfaces TrigSteeringEvent TrigT1Interfaces TrkExInterfaces TrigTimeAlgsLib xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/*.lut)

atlas_add_test( MdtDataPreparator_test
                SCRIPT python -m TrigL2MuonSA.MdtDataPreparator_test
                PROPERTIES TIMEOUT 600
                LOG_SELECT_PATTERN "dead tube" )
