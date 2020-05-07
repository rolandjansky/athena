################################################################################
# Package: TrkGaussianSumFilter
################################################################################

# Declare the package name:
atlas_subdir( TrkGaussianSumFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMultiComponentStateOnSurface
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkCaloCluster_OnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkGaussianSumFilter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES  ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives EventPrimitives GaudiKernel TrkDetDescrInterfaces TrkGeometry 
		     TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkMultiComponentStateOnSurface TrkParameters TrkExInterfaces TrkExUtils TrkFitterInterfaces 
		     TrkFitterUtils TrkToolInterfaces CxxUtils xAODEventInfo PathResolver TrkDetElementBase TrkVolumes TrkCaloCluster_OnTrack TrkMeasurementBase 
		     TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( TrkGaussianSumFilter )
atlas_install_runtime( Data/*.par )

atlas_add_executable( GSF_testFindMinimumIndex
	test/testFindMinimumIndex.cxx src/KLGaussianMixtureReduction.cxx)

atlas_add_executable( GSF_testAlignedDynArray
	test/testAlignedDynArray.cxx)

atlas_add_executable( GSF_testMergeComponents
	test/testMergeComponents.cxx src/KLGaussianMixtureReduction.cxx)

atlas_add_test(ut_GSF_testFindMinimumIndex
	SCRIPT GSF_testFindMinimumIndex)

atlas_add_test(ut_GSF_testAlignedDynArray
	SCRIPT GSF_testAlignedDynArray)

