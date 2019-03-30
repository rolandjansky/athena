################################################################################
# Package: OverlayByteStreamAlgs
################################################################################

# Declare the package name:
atlas_subdir( OverlayByteStreamAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/ByteStreamCnvSvc
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( OverlayByteStreamAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ByteStreamCnvSvcLib StoreGateLib
                     TrigSteeringEvent TrigT1Result )
