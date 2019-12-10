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
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODTrigger xAODTracking TrigConfHLTData TrigSteeringEvent ViewAlgsLib AthContainers FourMomUtils)

atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

atlas_add_test( creatingEVTest 
                SCRIPT athena --threads=1 ViewAlgsTest/EVTest.py
                EXTRA_PATTERNS "-s EMViewsMaker|algInView|start processing" )

atlas_add_test( mergingEVTest 
		        SCRIPT athena --threads=1 -c "doMerging=True" ViewAlgsTest/EVTest.py
                EXTRA_PATTERNS "-s EMViewsMaker|algInView|start processing" )
