# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkGaussianSumFilter )

#Component(s) in the package:
atlas_add_component( TrkGaussianSumFilter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives GaudiKernel
                     TrkGeometry  TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkMultiComponentStateOnSurface
                     TrkParameters TrkExInterfaces TrkExUtils TrkFitterInterfaces TrkFitterUtils TrkToolInterfaces CxxUtils PathResolver TrkDetElementBase
                     TrkCaloCluster_OnTrack TrkMeasurementBase  TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack
                     KalmanParameterUpdatorLib)

# Install files from the package:
atlas_install_runtime( Data/*.par )

#Executables for tests
atlas_add_executable( GSF_testFindMinimumIndex
	test/testFindMinimumIndex.cxx src/KLGaussianMixtureReduction.cxx
  LINK_LIBRARIES CxxUtils )

atlas_add_executable( GSF_testAlignedDynArray
	test/testAlignedDynArray.cxx)

atlas_add_executable( GSF_testMergeComponents
	test/testMergeComponents.cxx src/KLGaussianMixtureReduction.cxx
  LINK_LIBRARIES CxxUtils )

atlas_add_executable( GSF_testFindMode
	test/testFindMode.cxx src/MultiComponentStateModeCalculator.cxx
  LINK_LIBRARIES CxxUtils TrkMultiComponentStateOnSurface)


#Tests
atlas_add_test(ut_GSF_testFindMinimumIndex
	SCRIPT GSF_testFindMinimumIndex)

atlas_add_test(ut_GSF_testAlignedDynArray
	SCRIPT GSF_testAlignedDynArray)

atlas_add_test(ut_GSF_testMergeComponents
	SCRIPT GSF_testMergeComponents)

atlas_add_test(ut_GSF_testFindMode
	SCRIPT GSF_testFindMode)

