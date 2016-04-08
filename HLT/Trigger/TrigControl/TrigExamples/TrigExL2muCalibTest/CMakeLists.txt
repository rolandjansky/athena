################################################################################
# Package: TrigExL2muCalibTest
################################################################################

# Declare the package name:
atlas_subdir( TrigExL2muCalibTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/EventInfo
                          GaudiKernel
                          Trigger/TrigEvent/TrigNavigation
                          PRIVATE
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/xAOD/xAODTrigger
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigHLTResultByteStream
                          Trigger/TrigSteer/TrigInterfaces )

# Component(s) in the package:
atlas_add_component( TrigExL2muCalibTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventInfo GaudiKernel TrigNavigationLib StoreGateLib SGtests ByteStreamCnvSvcBaseLib xAODTrigger TrigSteeringEvent TrigHLTResultByteStreamLib TrigInterfacesLib )

# Install files from the package:
atlas_install_headers( TrigExL2muCalibTest )
atlas_install_joboptions( share/*.py )

