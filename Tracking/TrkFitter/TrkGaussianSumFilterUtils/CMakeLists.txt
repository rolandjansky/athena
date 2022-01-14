# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkGaussianSumFilterUtils )

#Component(s) in the package:
atlas_add_library( TrkGaussianSumFilterUtilsLib
                   src/*.cxx
		   PUBLIC_HEADERS TrkGaussianSumFilterUtils
		   LINK_LIBRARIES EventPrimitives TrkParameters 
		                  TrkEventPrimitives TrkMeasurementBase
                      TrkGeometry
       PRIVATE_LINK_LIBRARIES CxxUtils PathResolver TrkExUtils)

#Executables for tests
atlas_add_executable( GSF_testAlignedDynArray
	test/testAlignedDynArray.cxx 
	LINK_LIBRARIES TrkGaussianSumFilterUtilsLib)

atlas_add_executable( GSF_testMergeComponents
	test/testMergeComponents.cxx
        LINK_LIBRARIES CxxUtils TrkGaussianSumFilterUtilsLib)

atlas_add_executable( GSF_testFindMode
	test/testFindMode.cxx 
        LINK_LIBRARIES CxxUtils TrkGaussianSumFilterUtilsLib)


#Tests
atlas_add_test(ut_GSF_testAlignedDynArray
	SCRIPT GSF_testAlignedDynArray)

atlas_add_test(ut_GSF_testMergeComponents
	SCRIPT GSF_testMergeComponents)

atlas_add_test(ut_GSF_testFindMode
	SCRIPT GSF_testFindMode)

