################################################################################
# Package: TrigPartialEventBuilding
################################################################################

# Declare the package name
atlas_subdir( TrigPartialEventBuilding )

# Declare the package's dependencies
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthViews )

# Component(s) in the package
atlas_add_library( TrigPartialEventBuildingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigPartialEventBuilding
                   LINK_LIBRARIES DecisionHandlingLib TrigSteeringEvent
                   PRIVATE AthViews )

atlas_add_component( TrigPartialEventBuilding
                     src/components/*.cxx
                     LINK_LIBRARIES TrigPartialEventBuildingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Tests
#atlas_add_test( )
