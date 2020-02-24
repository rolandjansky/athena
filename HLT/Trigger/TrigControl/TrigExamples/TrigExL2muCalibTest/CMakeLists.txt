################################################################################
# Package: TrigExL2muCalibTest
################################################################################

# Declare the package name:
atlas_subdir( TrigExL2muCalibTest )

# Component(s) in the package:
atlas_add_component( TrigExL2muCalibTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaInterprocess EventInfo GaudiKernel TrigNavigationLib StoreGateLib SGtests ByteStreamCnvSvcBaseLib xAODTrigger TrigSteeringEvent TrigHLTResultByteStreamLib )

# Install files from the package:
atlas_install_headers( TrigExL2muCalibTest )
atlas_install_joboptions( share/*.py )

