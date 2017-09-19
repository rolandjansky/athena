# The name of the package:
atlas_subdir( GenInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel )

# External dependencies:
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( GenInterfacesLib
   GenInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS GenInterfaces
   LINK_LIBRARIES ${HEPMC_LIBRARIES} GaudiKernel )

atlas_add_dictionary( GenInterfacesDict
   GenInterfaces/GenInterfacesDict.h
   GenInterfaces/selection.xml
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${HEPMC_LIBRARIES} GenInterfacesLib )

