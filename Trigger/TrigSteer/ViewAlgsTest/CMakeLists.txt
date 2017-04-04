################################################################################
# Package: ViewAlgsTest
################################################################################

# Declare the package name:
atlas_subdir( ViewAlgsTest )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Event/FourMomUtils
                          Control/CxxUtils
                          Control/AthViews
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/L1Decoder
                          Trigger/TrigSteer/ViewAlgs )


# Component(s) in the package:
atlas_add_component( ViewAlgsTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps CxxUtils xAODTrigger xAODTracking TrigConfHLTData TrigSteeringEvent ViewAlgsLib AthContainers FourMomUtils)
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/caloRoIData.dat share/menuData.dat share/muonRoIData.dat )

atlas_install_python_modules( python/*.py )
atlas_install_xmls( data/*.xml )

