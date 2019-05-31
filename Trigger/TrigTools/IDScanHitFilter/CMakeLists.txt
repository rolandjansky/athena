################################################################################
# Package: IDScanHitFilter
################################################################################

# Declare the package name:
atlas_subdir( IDScanHitFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          DetectorDescription/IRegionSelector )

# Component(s) in the package:
atlas_add_component( IDScanHitFilter
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigInDetEvent TrigSteeringEvent IRegionSelector )
