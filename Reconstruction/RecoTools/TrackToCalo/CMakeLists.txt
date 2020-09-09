################################################################################
# Package: TrackToCalo
################################################################################

# Declare the package name:
atlas_subdir( TrackToCalo )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrackToCaloLib
                   src/*.cxx
                   PUBLIC_HEADERS TrackToCalo
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers GeoPrimitives xAODCaloEvent GaudiKernel ParticleCaloExtension RecoToolInterfaces 
		   TrackCaloClusterRecToolsLib TrkCaloExtension TrkParametersIdentificationHelpers CaloDetDescrLib CaloUtilsLib InDetReadoutGeometry TRT_ReadoutGeometry
                   PRIVATE_LINK_LIBRARIES CaloIdentifier AthenaBaseComps AtlasDetDescr FourMomUtils xAODTracking xAODMuon xAODEgamma xAODTruth TrkSurfaces 
                   TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces CxxUtils StoreGateLib EventKernel ParticlesInConeToolsLib ITrackToVertex
                   InDetTrackSelectionToolLib )

atlas_add_component( TrackToCalo
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES GaudiKernel TrackToCaloLib InDetTrackSelectionToolLib 
                     ITrackToVertex TrkLinks VxVertex TrackVertexAssociationToolLib)

atlas_install_python_modules( python/*.py )
