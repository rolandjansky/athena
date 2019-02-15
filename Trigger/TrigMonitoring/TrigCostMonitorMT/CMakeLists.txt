################################################################################
# Package: TrigCostMonitorMT
################################################################################

# Declare the package name:
atlas_subdir( TrigCostMonitorMT )

find_package(TBB)

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthViews
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Event/xAOD/xAODTrigger
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( TrigCostMonitorMT
                     src/*.cxx
                     src/components/*.cxx 
                     INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthViews TrigSteeringEvent TrigTimeAlgsLib xAODTrigger TrigConfHLTData ${TBB_LIBRARIES} )

# Install files from the package:
atlas_install_headers( TrigCostMonitorMT )
atlas_install_joboptions( share/*.py )
