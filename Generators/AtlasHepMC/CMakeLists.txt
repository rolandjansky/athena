# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

atlas_subdir( AtlasHepMC )

# External(s) needed by the package.
find_package( HepMC )

# Component(s) in the package.
atlas_add_library( AtlasHepMCLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${HEPMC_LIBRARIES} )
