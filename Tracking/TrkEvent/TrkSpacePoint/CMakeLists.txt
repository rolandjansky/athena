################################################################################
# Package: TrkSpacePoint
################################################################################

# Declare the package name:
atlas_subdir( TrkSpacePoint )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          DetectorDescription/Identifier
                          Event/EventContainers
                          GaudiKernel
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData )

# Component(s) in the package:
atlas_add_library( TrkSpacePoint
                   src/*.cxx
                   PUBLIC_HEADERS TrkSpacePoint
                   LINK_LIBRARIES AthContainers Identifier GaudiKernel TrkMeasurementBase EventContainers 
                   PRIVATE_LINK_LIBRARIES TrkDetElementBase TrkSurfaces TrkEventPrimitives TrkPrepRawData )

