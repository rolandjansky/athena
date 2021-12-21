################################################################################
# Package: egammaTrackTools
################################################################################

# Declare the package name:
atlas_subdir( egammaTrackTools )


# Component(s) in the package:
atlas_add_component( egammaTrackTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS
                     LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib CaloGeoHelpers AthenaBaseComps AthContainers
		     AtlasDetDescr IdDictDetDescr EventPrimitives xAODCaloEvent xAODEgamma xAODTracking FourMomUtils GaudiKernel BeamSpotConditionsData InDetIdentifier
		     RecoToolInterfaces TrkSurfaces TrkCaloCluster_OnTrack TrkCaloExtension TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase
		     TrkNeutralParameters TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack
		     TrkVertexOnTrack TrkExInterfaces TrkFitterInterfaces TrkVertexFitterInterfaces egammaInterfacesLib CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test(egammaTrackToolsConfigTest
               SCRIPT python -m egammaTrackTools.egammaTrackToolsConfig
               POST_EXEC_SCRIPT nopost.sh)

atlas_add_test(GSFTrackFitterConfigTest
               SCRIPT python -m egammaTrackTools.GSFTrackFitterConfig
               POST_EXEC_SCRIPT nopost.sh)
