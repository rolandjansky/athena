# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaRecEvent )

# Extra dependencies based on the environment:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel )
endif()

# Component(s) in the package:
atlas_add_library( egammaRecEvent
   egammaRecEvent/*.h Root/*.cxx
   PUBLIC_HEADERS egammaRecEvent
   LINK_LIBRARIES AthContainers AthLinks xAODCaloEvent xAODTracking
   ${extra_libs} )

atlas_add_dictionary( egammaRecEventDict
   egammaRecEvent/egammaRecEventDict.h
   egammaRecEvent/selection.xml
   LINK_LIBRARIES egammaRecEvent )
