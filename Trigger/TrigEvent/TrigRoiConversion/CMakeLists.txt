################################################################################
# Package: TrigRoiConversion
################################################################################

# Declare the package name:
atlas_subdir( TrigRoiConversion )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Event/xAOD/xAODTrigger
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( TrigRoiConversionLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TrigRoiConversion
                   LINK_LIBRARIES AsgTools AthenaBaseComps TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES xAODTrigger GaudiKernel )

atlas_add_component( TrigRoiConversion
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools AthenaBaseComps TrigSteeringEvent xAODTrigger GaudiKernel TrigRoiConversionLib )

