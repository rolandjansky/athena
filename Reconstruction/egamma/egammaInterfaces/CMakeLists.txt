# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaInterfaces )

# Extra libraries used by the interface library.
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs TrkParameters TrkTrack TrkEventPrimitives
      TrkNeutralParameters TrkCaloExtension egammaRecEvent )
endif()

# Component(s) in the package:
atlas_add_library( egammaInterfacesLib
   egammaInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS egammaInterfaces
   LINK_LIBRARIES GaudiKernel xAODCaloEvent xAODEgamma xAODTracking
   ${extra_libs} )

atlas_add_dictionary( egammaInterfacesDict
   egammaInterfaces/egammaInterfacesDict.h
   egammaInterfaces/selection.xml
   LINK_LIBRARIES egammaInterfacesLib )
