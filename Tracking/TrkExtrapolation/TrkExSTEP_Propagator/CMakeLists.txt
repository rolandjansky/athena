################################################################################
# Package: TrkExSTEP_Propagator
################################################################################

# Declare the package name:
atlas_subdir( TrkExSTEP_Propagator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkExSTEP_Propagator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives MagFieldInterfaces TrkGeometry TrkEventPrimitives TrkMaterialOnTrack TrkParameters TrkExInterfaces TrkExUtils GaudiKernel TrkDetDescrUtils TrkSurfaces TrkTrack )

# Install files from the package:
atlas_install_headers( TrkExSTEP_Propagator )

