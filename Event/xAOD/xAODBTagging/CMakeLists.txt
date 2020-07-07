# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODBTagging )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODBTagging
   xAODBTagging/*.h xAODBTagging/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODBTagging
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore xAODTracking
   CxxUtils )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODBTagging/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::BTaggingContainer_v1" "xAOD::BTagVertexContainer_v1" )

atlas_add_dictionary( xAODBTaggingDict
   xAODBTagging/xAODBTaggingDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODBTagging
   EXTRA_FILES Root/dict/*.cxx )
