################################################################################
# Package: L1TopoValDataCnv
################################################################################

# Declare the package name:
atlas_subdir( L1TopoValDataCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( L1TopoValDataCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES TrigSteeringEvent AthenaBaseComps AthenaKernel xAODTrigger GaudiKernel TrigT1Result )

# Install files from the package:
atlas_install_headers( L1TopoValDataCnv )

