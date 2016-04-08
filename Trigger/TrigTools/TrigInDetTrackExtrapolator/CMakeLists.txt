################################################################################
# Package: TrigInDetTrackExtrapolator
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetTrackExtrapolator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkFitter/TrkDistributedKalmanFilter
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( TrigInDetTrackExtrapolator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigInDetEvent TrigSteeringEvent MagFieldInterfaces TrkDistributedKalmanFilterLib TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigInDetTrackExtrapolator )
atlas_install_joboptions( share/*.py )

