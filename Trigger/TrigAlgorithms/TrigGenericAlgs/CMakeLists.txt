# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigGenericAlgs )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( tdaq-common COMPONENTS CTPfragment eformat )

# Component(s) in the package:
atlas_add_component( TrigGenericAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaKernel AthenaMonitoringKernelLib ByteStreamCnvSvcBaseLib CxxUtils DecisionHandlingLib EventInfo GaudiKernel IRegionSelector L1TopoConfig L1TopoRDO RoiDescriptor TrigConfHLTData TrigConfInterfaces TrigConfL1Data TrigInterfacesLib TrigL2MuonSALib TrigMuonToolInterfaces TrigNavigationLib TrigParticle TrigSteeringEvent TrigSteeringLib TrigT1Interfaces TrigT1Result xAODTrigL1Calo xAODTrigMuon xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
atlas_install_joboptions( share/*.py )

