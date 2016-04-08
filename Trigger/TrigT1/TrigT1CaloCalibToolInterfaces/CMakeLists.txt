################################################################################
# Package: TrigT1CaloCalibToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCalibToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTrigL1Calo
                          Trigger/TrigT1/TrigT1CaloEvent )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibToolInterfaces
                   PUBLIC_HEADERS TrigT1CaloCalibToolInterfaces
                   LINK_LIBRARIES AsgTools xAODTrigL1Calo TrigT1CaloEventLib )

