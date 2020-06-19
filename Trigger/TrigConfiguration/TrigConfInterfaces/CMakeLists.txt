# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfInterfaces )

# Extra dependencies, for non-standalone builds:
if( NOT XAOD_STANDALONE )
   set( extra_libs GaudiKernel )
endif()

# Component(s) in the package:
atlas_add_library( TrigConfInterfaces
   TrigConfInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrigConfInterfaces
   LINK_LIBRARIES AsgTools ${extra_libs} )

atlas_add_dictionary( TrigConfInterfacesDict
   TrigConfInterfaces/TrigConfInterfacesDict.h
   TrigConfInterfaces/selection.xml
   LINK_LIBRARIES TrigConfInterfaces )
