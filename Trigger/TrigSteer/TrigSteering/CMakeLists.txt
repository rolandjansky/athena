# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSteering )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigSteeringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigSteering
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthContainers AthenaBaseComps AthenaKernel AthenaMonitoringLib ByteStreamCnvSvcBaseLib EventInfo EventInfoUtils GaudiKernel L1TopoAlgorithms L1TopoCoreSim L1TopoEvent L1TopoSimulationLib TrigConfBase TrigConfHLTData TrigConfInterfaces TrigNavigationLib TrigSteeringEvent TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Result TrigTimeAlgsLib xAODEventInfo TrigInterfacesLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaMonitoringKernelLib L1TopoCommon L1TopoConfig TrigConfL1Data TrigMonitorBaseLib TrigSerializeResultLib TrigStorageDefinitions TrigT1Interfaces xAODTrigger )

atlas_add_component( TrigSteering
                     src/components/*.cxx
                     LINK_LIBRARIES TrigSteeringLib )

atlas_add_dictionary( TrigSteeringDict
                      TrigSteering/TrigSteeringDict.h
                      TrigSteering/selection.xml
                      LINK_LIBRARIES TrigSteeringLib )

# Tests in the package:
atlas_add_test( Signature_test
                SOURCES
                test/Signature_test.cxx
                LINK_LIBRARIES TestTools TrigSteeringLib
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( SteeringChain_test
                SOURCES
                test/SteeringChain_test.cxx
                LINK_LIBRARIES TestTools TrigSteeringLib
                POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/Lvl1Results.txt )
atlas_install_xmls( share/*.dtd )

