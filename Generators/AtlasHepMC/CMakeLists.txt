# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

atlas_subdir( AtlasHepMC )

# External(s) needed by the package.
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Component(s) in the package.
atlas_add_library( AtlasHepMCLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${_HEPMC_HepMC_library} )

atlas_add_library( AtlasHepMCfioLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${_HEPMC_HepMCfio_library} )
