# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Herwig7_i )

# External dependencies:
find_package( Boost )
find_package( Herwig3 COMPONENTS HerwigAPI )
find_package( ThePEG COMPONENTS "ThePEG" "LesHouches${CMAKE_SHARED_LIBRARY_SUFFIX}" )
find_package( GSL )
find_package( OpenLoops )
find_package( hepmc3 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Add extra flag for H7 versions >=7.2
set( _extraDefs )
list( APPEND _extraDefs -DHWVER_IS_72=1 )

atlas_add_library ( powhegHerwig
   src/PowhegLesHouchesFileReader.cxx
   Herwig7_i/PowhegLesHouchesFileReader.h
   Herwig7_i/PowhegLesHouchesFileReader.fh
   NO_PUBLIC_HEADERS
   SHARED
   INCLUDE_DIRS ${THEPEG_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
   LINK_LIBRARIES ${THEPEG_LIBRARIES} ${GSL_LIBRARIES} )

# Component(s) in the package:
if (HEPMC3_USE)
atlas_add_component( Herwig7_i
   Herwig7_i/Herwig7.h src/Herwig7.cxx src/Herwig7HepMC.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HERWIG3_INCLUDE_DIRS}
   ${THEPEG_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${OPENLOOPS_INCLUDE_DIRS} ${HEPMC3_INCLUDE_DIRS}
   DEFINITIONS -DHWVERSION=\"${HERWIG3_LCGVERSION}\"
   -DHWVER_IS_72=\"${HW3_VER_IS_72}\"
   -DHAVE_HEPMC3
   LINK_LIBRARIES ${HERWIG3_LIBRARIES}  ${Boost_LIBRARIES}
   ${THEPEG_LIBRARIES} ${GSL_LIBRARIES} ${OPENLOOPS_LIBRARIES}
   GeneratorModulesLib EventInfo GaudiKernel PathResolver AtlasHepMCLib powhegHerwig )

else()
atlas_add_component( Herwig7_i
   Herwig7_i/Herwig7.h src/Herwig7.cxx src/Herwig7HepMC.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HERWIG3_INCLUDE_DIRS}
   ${THEPEG_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${OPENLOOPS_INCLUDE_DIRS}
   DEFINITIONS -DHWVERSION=\"${HERWIG3_LCGVERSION}\"
   -DHWVER_IS_72=\"${HW3_VER_IS_72}\"
   LINK_LIBRARIES ${HERWIG3_LIBRARIES}  ${Boost_LIBRARIES}
   ${THEPEG_LIBRARIES} ${GSL_LIBRARIES} ${OPENLOOPS_LIBRARIES}
   GeneratorModulesLib EventInfo GaudiKernel PathResolver AtlasHepMCLib powhegHerwig )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.pdt share/file/*.dat )

# Set up some environment variables for Herwig.
set( Herwig7Environment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of Herwig7Environment.cmake" )
find_package( Herwig7Environment )
