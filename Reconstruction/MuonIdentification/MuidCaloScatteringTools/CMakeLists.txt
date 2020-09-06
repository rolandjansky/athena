################################################################################
# Package: MuidCaloScatteringTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCaloScatteringTools )

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

