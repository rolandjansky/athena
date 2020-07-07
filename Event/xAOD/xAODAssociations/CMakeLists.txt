# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODAssociations )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODAssociations
   xAODAssociations/*.h Root/*.cxx
   PUBLIC_HEADERS xAODAssociations
   LINK_LIBRARIES AthContainers AthLinks xAODCaloEvent xAODCore xAODTracking )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODAssociations/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TrackParticleClusterAssociationContainer_v1" )

atlas_add_dictionary( xAODAssociationsDict
   xAODAssociations/xAODAssociationsDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODAssociations
   EXTRA_FILES Root/dict/*.cxx )
