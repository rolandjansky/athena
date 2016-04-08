################################################################################
# Package: TrkExRungeKuttaPropagator
################################################################################

# Declare the package name:
atlas_subdir( TrkExRungeKuttaPropagator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkExtrapolation/TrkExUtils )

# Component(s) in the package:
atlas_add_component( TrkExRungeKuttaPropagator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldInterfaces TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkGeometry TrkSurfaces TrkPatternParameters TrkExUtils )

# Install files from the package:
atlas_install_headers( TrkExRungeKuttaPropagator )

