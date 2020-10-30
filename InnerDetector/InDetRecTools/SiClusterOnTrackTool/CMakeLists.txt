################################################################################
# Package: SiClusterOnTrackTool
################################################################################

# Declare the package name:
atlas_subdir( SiClusterOnTrackTool )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( SiClusterOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GeoPrimitives GaudiKernel InDetPrepRawData InDetRIO_OnTrack TrkParameters TrkToolInterfaces StoreGateLib SGtests EventPrimitives InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SiClusterizationToolLib TrkSurfaces TrkRIO_OnTrack PixelConditionsData TrkNeuralNetworkUtilsLib TrkEventUtils SCT_ModuleDistortionsLib )

# Install files from the package:
atlas_install_headers( SiClusterOnTrackTool )

