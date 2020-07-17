################################################################################
# Package: TrackToCalo
################################################################################

# Declare the package name:
atlas_subdir( TrackToCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          Reconstruction/RecoEvent/ParticleCaloExtension
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecTools
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
			  Tracking/TrkExtrapolation/TrkExUtils
			  InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
			  PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODMuon
			  Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTruth
                          Reconstruction/RecoTools/ParticlesInConeTools
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
			  Control/CxxUtils )

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
