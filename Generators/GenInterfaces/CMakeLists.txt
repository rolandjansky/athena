# The name of the package:
atlas_subdir( GenInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/AtlasHepMC
   GaudiKernel )

# External dependencies:

# Component(s) in the package:
atlas_add_library( GenInterfacesLib
   GenInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS GenInterfaces
   LINK_LIBRARIES AtlasHepMCLib GaudiKernel )

atlas_add_dictionary( GenInterfacesDict
   GenInterfaces/GenInterfacesDict.h
   GenInterfaces/selection.xml
   LINK_LIBRARIES AtlasHepMCLib GenInterfacesLib )

