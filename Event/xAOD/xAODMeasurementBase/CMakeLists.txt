# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODMeasurementBase )

# External dependencies:
find_package( xAODUtilities )

atlas_add_library( xAODMeasurementBase
    xAODMeasurementBase/*.h xAODMeasurementBase/versions/*.h Root/*.cxx
    PUBLIC_HEADERS xAODMeasurementBase
    LINK_LIBRARIES xAODCore GeoPrimitives Identifier)

atlas_add_xaod_smart_pointer_dicts(
    INPUT xAODMeasurementBase/selection.xml
    OUTPUT _selectionFile
    CONTAINERS "xAOD::UncalibratedMeasurementContainer_v1" )

atlas_add_dictionary( xAODMeasurementBaseDict
    xAODMeasurementBase/xAODMeasurementBaseDict.h
    ${_selectionFile}
    LINK_LIBRARIES xAODCore xAODMeasurementBase)
