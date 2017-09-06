################################################################################
# Package: IDScanHitFilter
################################################################################

# Declare the package name:
atlas_subdir( IDScanHitFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
                          DetectorDescription/IRegionSelector )

# Component(s) in the package:
atlas_add_library( IDScanHitFilterLib
                   src/*.cxx
                   PUBLIC_HEADERS IDScanHitFilter
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigInDetEvent TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES IRegionSelector )

atlas_add_component( IDScanHitFilter
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigInDetEvent TrigSteeringEvent IRegionSelector IDScanHitFilterLib )

