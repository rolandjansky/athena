# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODInDetMeasurement )

# External dependencies:
find_package( xAODUtilities )

atlas_add_library( xAODInDetMeasurement
    xAODInDetMeasurement/*.h xAODInDetMeasurement/versions/*.h Root/*.cxx
    PUBLIC_HEADERS xAODInDetMeasurement
    LINK_LIBRARIES xAODCore GeoPrimitives xAODMeasurementBase Identifier)

atlas_add_xaod_smart_pointer_dicts(
    INPUT xAODInDetMeasurement/selection.xml
    OUTPUT _selectionFile
    CONTAINERS "xAOD::PixelClusterContainer_v1" "xAOD::StripClusterContainer_v1" )

atlas_add_dictionary( xAODInDetMeasurementDict
    xAODInDetMeasurement/xAODInDetMeasurementDict.h
    ${_selectionFile}
    LINK_LIBRARIES xAODCore xAODInDetMeasurement)

# Test(s) in the package.
atlas_add_test( xAODInDetMeasurement_PixelCluster_test
   SOURCES test/xAODInDetMeasurement_PixelCluster_test.cxx
   LINK_LIBRARIES xAODInDetMeasurement )

atlas_add_test( xAODInDetMeasurement_StripCluster_test
   SOURCES test/xAODInDetMeasurement_StripCluster_test.cxx
   LINK_LIBRARIES xAODInDetMeasurement )
