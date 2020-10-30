# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( GenInterfaces )

# Component(s) in the package:
atlas_add_library( GenInterfacesLib
   GenInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS GenInterfaces
   LINK_LIBRARIES AtlasHepMCLib GaudiKernel )

atlas_add_dictionary( GenInterfacesDict
   GenInterfaces/GenInterfacesDict.h
   GenInterfaces/selection.xml
   LINK_LIBRARIES GenInterfacesLib )
