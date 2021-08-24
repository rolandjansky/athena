# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Set the package's name.
atlas_subdir( AtlasHepMC )

if (HEPMC3_USE)

# Tell the user what's happening.
message( STATUS "Using HepMC3 for AtlasHepMC" )
find_package( hepmc3 COMPONENTS HepMC3 HepMC3search )

# Component(s) in the package.
atlas_add_library( AtlasHepMCLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   DEFINITIONS HEPMC3=1
   INCLUDE_DIRS  ${HEPMC3_INCLUDE_DIRS}
   LINK_LIBRARIES ${HEPMC3_HepMC3_LIBRARY}  )

atlas_add_library( AtlasHepMCfioLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   DEFINITIONS HEPMC3=1
   INCLUDE_DIRS  ${HEPMC3_INCLUDE_DIRS}
   )

atlas_add_library( AtlasHepMCsearchLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   DEFINITIONS HEPMC3=1
   INCLUDE_DIRS  ${HEPMC3_INCLUDE_DIRS}
   LINK_LIBRARIES ${HEPMC3_HepMC3search_LIBRARY}  )

else()

# Tell the user what's happening.
message( STATUS "Using HepMC for AtlasHepMC" )
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Component(s) in the package.
atlas_add_library( AtlasHepMCLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${HEPMC_HepMC_LIBRARY} )

atlas_add_library( AtlasHepMCsearchLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   )
atlas_add_library( AtlasHepMCfioLib
   AtlasHepMC/*.h
   INTERFACE
   PUBLIC_HEADERS AtlasHepMC
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${HEPMC_HepMCfio_LIBRARY} )
endif()
