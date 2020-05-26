################################################################################
# Package: MuidCaloScatteringTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCaloScatteringTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExUtils
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuidCaloScatteringTools
                     src/MuidCaloMaterialParam.cxx
                     src/MuidCaloTrackStateOnSurface.cxx
                     src/MuidMaterialEffectsOnTrackProvider.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MuidInterfaces TrkDetDescrInterfaces TrkSurfaces TrkParameters TrkExInterfaces GeoPrimitives MuidEvent muonEvent TrkGeometry TrkMaterialOnTrack TrkTrack TrkExUtils MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_headers( MuidCaloScatteringTools )

