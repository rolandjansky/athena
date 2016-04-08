################################################################################
# Package: TrkDynamicNoiseAdjustor
################################################################################

# Declare the package name:
atlas_subdir( TrkDynamicNoiseAdjustor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkFitter/TrkFitterInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkDynamicNoiseAdjustor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkFitterInterfaces EventPrimitives GaudiKernel TrkDetElementBase TrkGeometry TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParameters TrkExInterfaces TrkFitterUtils TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkDynamicNoiseAdjustor )

