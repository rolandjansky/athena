################################################################################
# Package: TrigFake
################################################################################

# Declare the package name:
atlas_subdir( TrigFake )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigSteering
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Generators/GeneratorObjects
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( TrigFakeLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigFake
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigInDetEvent TrigSteeringEvent TrigSteeringLib StoreGateLib SGtests TrigNavigationLib TrigInterfacesLib 
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorObjects TrigConfHLTData TrigT1Interfaces TrigT1Result  )

atlas_add_component( TrigFake
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel TrigInDetEvent TrigSteeringEvent TrigSteeringLib StoreGateLib SGtests GeneratorObjects TrigConfHLTData TrigNavigationLib TrigInterfacesLib TrigT1Interfaces TrigT1Result TrigFakeLib )

atlas_add_test( FakeRoIB_test
		SOURCES test/FakeRoIB_test.cxx
		LINK_LIBRARIES TrigFakeLib TrigT1Result )

# Install files from the package:
atlas_install_joboptions( share/jobOfragment_TrigFake.* )

