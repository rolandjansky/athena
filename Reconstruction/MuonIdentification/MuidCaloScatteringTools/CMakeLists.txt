################################################################################
# Package: MuidCaloScatteringTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCaloScatteringTools )

# Component(s) in the package:
atlas_add_component( MuidCaloScatteringTools
                     src/MuidCaloMaterialParam.cxx
                     src/MuidCaloTrackStateOnSurface.cxx
                     src/MuidMaterialEffectsOnTrackProvider.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuidInterfaces TrkDetDescrInterfaces TrkSurfaces TrkParameters TrkExInterfaces GeoPrimitives MuidEvent muonEvent TrkGeometry TrkMaterialOnTrack TrkTrack TrkExUtils MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_headers( MuidCaloScatteringTools )

