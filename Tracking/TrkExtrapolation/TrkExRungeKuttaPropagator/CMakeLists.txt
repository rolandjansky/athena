################################################################################
# Package: TrkExRungeKuttaPropagator
################################################################################

# Declare the package name:
atlas_subdir( TrkExRungeKuttaPropagator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkExtrapolation/TrkExUtils 
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
                          Control/CxxUtils
                          )

# Component(s) in the package:
atlas_add_component( TrkExRungeKuttaPropagator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldInterfaces TrkEventPrimitives TrkNeutralParameters 
                     TrkParameters TrkExInterfaces TrkGeometry TrkSurfaces TrkPatternParameters TrkExUtils MagFieldElements MagFieldConditions
                     CxxUtils)

# Install files from the package:
atlas_install_headers( TrkExRungeKuttaPropagator )

